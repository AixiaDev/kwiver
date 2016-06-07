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

#include "object_detector_process.h"

#include <vital/algorithm_plugin_manager.h>

#include <vital/algo/image_object_detector.h>

#include <sstream>
#include <iostream>

#include <sprokit/processes/kwiver_type_traits.h>

#include <sprokit/pipeline/process_exception.h>

namespace kwiver
{

//----------------------------------------------------------------
// Private implementation class
class object_detector_process::priv
{
public:
  priv();
  ~priv();

   vital::algo::image_object_detector_sptr m_detector;

}; // end priv class


// ==================================================================
object_detector_process
::object_detector_process( kwiver::vital::config_block_sptr const& config )
: process( config ),
  d( new object_detector_process::priv )
{
  // Attach our logger name to process logger
  attach_logger( kwiver::vital::get_logger( name() ) ); // could use a better approach
  kwiver::vital::algorithm_plugin_manager::load_plugins_once();
  make_ports();
  make_config();
}

object_detector_process::~object_detector_process()
{
}

void object_detector_process::_configure()
{
  vital::config_block_sptr algo_config = get_config();

  vital::algo::image_object_detector::set_nested_algo_configuration( "object_detector", algo_config, d->m_detector );
  if ( ! d->m_detector )
  {
    throw sprokit::invalid_configuration_exception( name(), "Unable to create detector" );
  }

  vital::algo::image_object_detector::get_nested_algo_configuration( "object_detector", algo_config, d->m_detector );

  // Check config so it will give run-time diagnostic of config problems
  if ( ! vital::algo::image_object_detector::check_nested_algo_configuration("object_detector", algo_config ) )
  {
    throw sprokit::invalid_configuration_exception( name(), "Configuration check failed." );
  }
}

void object_detector_process::_step()
{
  vital::image_container_sptr input = grab_from_port_using_trait(image);
  vital::detected_object_set_sptr result = d->m_detector->detect(input);
  push_to_port_using_trait( detected_object_set, result );
}

void object_detector_process::make_ports()
{
  // Set up for required ports
  sprokit::process::port_flags_t required;
  sprokit::process::port_flags_t optional;

  required.insert( flag_required );

  // -- input --
  declare_input_port_using_trait(image, required);

  // -- output --
  declare_output_port_using_trait(detected_object_set, optional);
}

void object_detector_process::make_config()
{

}

// ================================================================
object_detector_process::priv
::priv()
{

}


object_detector_process::priv
::~priv()
{
}

}
