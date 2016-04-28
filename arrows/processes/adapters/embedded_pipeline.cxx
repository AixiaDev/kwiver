/*ckwg +29
 * Copyright 2016 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "embedded_pipeline.h"

#include <vital/config/config_block.h>
#include <vital/vital_foreach.h>
#include <vital/logger/logger.h>

#include <sprokit/tools/pipeline_builder.h>
#include <sprokit/pipeline/modules.h>
#include <sprokit/pipeline/pipeline.h>
#include <sprokit/pipeline/datum.h>
#include <sprokit/pipeline/scheduler.h>
#include <sprokit/pipeline/scheduler_registry.h>

#include <arrows/processes/adapters/input_adapter.h>
#include <arrows/processes/adapters/input_adapter_process.h>

#include <arrows/processes/adapters/output_adapter.h>
#include <arrows/processes/adapters/output_adapter_process.h>

#include <sstream>
#include <stdexcept>


namespace {

static kwiver::vital::config_block_key_t const scheduler_block = kwiver::vital::config_block_key_t("_scheduler");

} // end


namespace kwiver {

// ----------------------------------------------------------------
class embedded_pipeline::priv
{
public:
  // -- CONSTRUCTORS --
  priv()
    : m_logger( kwiver::vital::get_logger( "embedded_pipeline" )),
      m_at_end( false ),
      m_pipeline_started( false )
  {
  }


  ~priv()
  {
    // If the pipeline has been started, wait until it has completed
    // before freeing storage. May have to do more here to deal with a
    // still-running pipeline.
    if ( m_pipeline_started )
    {
      try
      {
        m_scheduler->stop();
      }
      catch ( ... ) { }
    }
  }


// ------------------------------------------------------------------
bool connect_adapters()
{
  auto names = m_pipeline->process_names();
  bool input_connected( false );
  bool output_connected( false );

  VITAL_FOREACH( auto n, names )
  {
    auto proc = m_pipeline->process_by_name( n );
    if ( proc->type() == "input_adapter" )
    {
      m_input_adapter.connect( proc->name(), m_pipeline );
      input_connected = true;
      break;
    }
  }

  VITAL_FOREACH( auto n, names )
  {
    auto proc = m_pipeline->process_by_name( n );
    if (proc->type() == "output_adapter" )
    {
      m_output_adapter.connect( proc->name(), m_pipeline );
      output_connected = true;
      break;
    }
  }

  return (input_connected && output_connected);
}


//---------------------------
  vital::logger_handle_t m_logger;
  bool m_at_end;
  bool m_pipeline_started;

  kwiver::input_adapter m_input_adapter;
  kwiver::output_adapter m_output_adapter;

  sprokit::pipeline_t m_pipeline;
  kwiver::vital::config_block_sptr m_pipe_config;
  kwiver::vital::config_block_sptr m_scheduler_config;
  sprokit::scheduler_t m_scheduler;

}; // end class embedded_pipeline::priv


// ==================================================================
embedded_pipeline
::embedded_pipeline( std::istream& istr )
  : m_priv( new priv() )
{
  // load processes
  sprokit::load_known_modules();

  // create a pipeline
  sprokit::pipeline_builder builder;
  builder.load_pipeline( istr );

  // build pipeline
  m_priv->m_pipeline = builder.pipeline();
  m_priv->m_pipe_config = builder.config();

  if ( ! m_priv->m_pipeline)
  {
    throw std::runtime_error( "Unable to bake pipeline" );
  }

  // perform setup operation on pipeline and get it ready to run
  // This throws many exceptions
  try
  {
    m_priv->m_pipeline->setup_pipeline();
  }
  catch( sprokit::pipeline_exception const& e)
  {
    std::stringstream str;
    str << "Error setting up pipeline: " << e.what();
    throw std::runtime_error( str.str() );
  }

  if ( ! m_priv->connect_adapters() )
  {
    throw std::runtime_error( "Unable to connect to input and output adapter processes");
  }

  //
  // Setup scheduler
  //
  sprokit::scheduler_registry::type_t scheduler_type = sprokit::scheduler_registry::default_type;
  m_priv->m_scheduler_config = m_priv->m_pipe_config->subblock(scheduler_block +
                                              kwiver::vital::config_block::block_sep + scheduler_type);

  sprokit::scheduler_registry_t reg = sprokit::scheduler_registry::self();
  m_priv->m_scheduler = reg->create_scheduler(scheduler_type,
                                              m_priv->m_pipeline,
                                              m_priv->m_scheduler_config);

  if ( ! m_priv->m_scheduler)
  {
    throw std::runtime_error( "Unable to create scheduler" );
  }
}


embedded_pipeline
::~embedded_pipeline()
{
}


// ------------------------------------------------------------------
void
embedded_pipeline
::send( kwiver::adapter::adapter_data_set_t ads )
{
  m_priv->m_input_adapter.send( ads );
}


// ------------------------------------------------------------------
void
embedded_pipeline
::send_end_of_input()
{
  auto ds = kwiver::adapter::adapter_data_set::create( kwiver::adapter::adapter_data_set::end_of_input );
  this->send( ds );
}


// ------------------------------------------------------------------
  kwiver::adapter::adapter_data_set_t
embedded_pipeline
::receive()
{
  auto ads =  m_priv->m_output_adapter.receive();
  m_priv->m_at_end = ads->is_end_of_data();
  return ads;
}


// ------------------------------------------------------------------
bool
embedded_pipeline
::full() const
{
  return m_priv->m_input_adapter.full();
}


// ------------------------------------------------------------------
bool
embedded_pipeline
::empty() const
{
  return m_priv->m_output_adapter.empty();
}


// ------------------------------------------------------------------
bool
embedded_pipeline
::at_end() const
{
  return m_priv->m_at_end;
}


// ------------------------------------------------------------------
void
embedded_pipeline
::start()
{
  m_priv->m_scheduler->start();
}


// ------------------------------------------------------------------
sprokit::process::ports_t
embedded_pipeline
::input_port_names() const
{
  return m_priv->m_input_adapter.port_list();
}


// ------------------------------------------------------------------
sprokit::process::ports_t
embedded_pipeline
::output_port_names() const
{
  return m_priv->m_output_adapter.port_list();
}

} // end namespace kwiver
