/*ckwg +29
 * Copyright 2017 by Kitware, Inc.
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

#include "motion_detector_process.h"

#include <vital/util/wall_timer.h>
#include <vital/algo/motion_detector.h>

#include <sprokit/processes/kwiver_type_traits.h>
#include <sprokit/pipeline/process_exception.h>

namespace kwiver {

create_config_trait( algo, std::string, "", "Algorithm configuration subblock" );

//----------------------------------------------------------------
// Private implementation class
class motion_detector_process::priv
{
public:
  priv();
  ~priv();

  vital::algo::motion_detector_sptr m_algo;
  kwiver::vital::wall_timer m_timer;

}; // end priv class


// ==================================================================
motion_detector_process::
motion_detector_process( kwiver::vital::config_block_sptr const& config )
  : process( config ),
    d( new motion_detector_process::priv )
{
  make_ports();
  make_config();
}


motion_detector_process::
~motion_detector_process()
{
}


// ------------------------------------------------------------------
void
motion_detector_process::
_configure()
{
  scoped_configure_instrumentation();

  vital::config_block_sptr algo_config = get_config();

  // Check config so it will give run-time diagnostic of config problems
  if ( ! vital::algo::motion_detector::check_nested_algo_configuration_using_trait( algo, algo_config ) )
  {
    throw sprokit::invalid_configuration_exception( name(), "Configuration check failed." );
  }

  vital::algo::motion_detector::set_nested_algo_configuration_using_trait( algo, algo_config, d->m_algo );

  if ( ! d->m_algo )
  {
    throw sprokit::invalid_configuration_exception( name(), "Unable to create motion detector algorithm" );
  }
}


// ------------------------------------------------------------------
void
motion_detector_process::
_step()
{
  d->m_timer.start();

  //TODO, handle case where this is optionally provided
  //auto ts = grab_from_port_using_trait( timestamp );

  auto input = grab_from_port_using_trait( image );
  kwiver::vital::image_container_sptr result;

  {
    scoped_step_instrumentation();

    kwiver::vital::timestamp ts;
    if (has_input_port_edge_using_trait( timestamp ) )
    {
      ts = grab_from_port_using_trait( timestamp );
    }

    bool reset(false);
    if (has_input_port_edge_using_trait( coordinate_system_updated ) )
    {
      reset = grab_from_port_using_trait( coordinate_system_updated );
    }

    result = d->m_algo->process_image( ts, input, reset );
  }

  push_to_port_using_trait(motion_heat_map , result );

  d->m_timer.stop();
  double elapsed_time = d->m_timer.elapsed();
  LOG_DEBUG( logger(), "Total processing time: " << elapsed_time << " seconds");
}


// ------------------------------------------------------------------
void
motion_detector_process::
make_ports()
{
  // Set up for required ports
  sprokit::process::port_flags_t required;
  sprokit::process::port_flags_t optional;
  sprokit::process::port_flags_t opt_static;

  required.insert( flag_required );
  opt_static.insert( flag_input_static );

  // -- input --
  declare_input_port_using_trait( timestamp, optional );
  declare_input_port_using_trait( image, required );
  declare_input_port_using_trait( coordinate_system_updated, optional );

  // -- output --
  declare_output_port_using_trait( motion_heat_map, required );
}


// ------------------------------------------------------------------
void
motion_detector_process::
make_config()
{
  declare_config_using_trait( algo );
}


// ================================================================
motion_detector_process::priv
::priv()
{
}


motion_detector_process::priv
::~priv()
{
}

} // end namespace kwiver
