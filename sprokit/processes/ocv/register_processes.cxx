/*ckwg +29
 * Copyright 2015-2016 by Kitware, Inc.
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

#include <sprokit/processes/ocv/kwiver_processes_ocv_export.h>
#include <sprokit/pipeline/process_factory.h>
#include <vital/plugin_loader/plugin_loader.h>

// -- list processes to register --
#include "blend_images_process.h"
#include "crop_image_process.h"
#include "draw_detected_object_boxes_process.h"
#include "cvt_color_process.h"
#include "image_object_classifier_process.h"
#include "image_viewer_process.h"
#include "stabilization_inspection_image_process.h"
#include "show_mask_on_image_process.h"

// ----------------------------------------------------------------
/*! \brief Regsiter processes
 *
 */
extern "C"
KWIVER_PROCESSES_OCV_EXPORT
void
register_factories( kwiver::vital::plugin_loader& vpm )
{
  static auto const module_name = kwiver::vital::plugin_manager::module_t( "kwiver_processes_ocv" );

  if ( sprokit::is_process_module_loaded( vpm, module_name ) )
  {
    return;
  }

  // ----------------------------------------------------------------

  auto fact = vpm.ADD_PROCESS( kwiver::image_viewer_process );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_NAME, "image_viewer" );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME, module_name );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_DESCRIPTION, "Display input image and delay" );
  fact->add_attribute( kwiver::vital::plugin_factory::PLUGIN_VERSION, "1.0" );

  fact = vpm.ADD_PROCESS( kwiver::draw_detected_object_boxes_process );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_NAME,  "draw_detected_object_boxes" );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME, module_name );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_DESCRIPTION,
                        "Draw detected object boxes on images." );
  fact->add_attribute( kwiver::vital::plugin_factory::PLUGIN_VERSION, "1.0" );

  fact = vpm.ADD_PROCESS( kwiver::crop_image_process );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_NAME,  "crop_image" );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME, module_name );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_DESCRIPTION,
                        "Crops image to bbox." );
  fact->add_attribute( kwiver::vital::plugin_factory::PLUGIN_VERSION, "1.0" );
  
  fact = vpm.ADD_PROCESS( kwiver::cvt_color_process );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_NAME,  "cvt_color" );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME, module_name );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_DESCRIPTION,
                        "Converts an image from one color space to another." );
  fact->add_attribute( kwiver::vital::plugin_factory::PLUGIN_VERSION, "1.0" );
  
  fact = vpm.ADD_PROCESS( kwiver::image_object_classifier_process );
  fact->add_attribute( kwiver::vital::plugin_factory::PLUGIN_NAME, "image_object_classifier" );
  fact->add_attribute( kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME, module_name );
  fact->add_attribute( kwiver::vital::plugin_factory::PLUGIN_DESCRIPTION,
                       "Apply selected image object detector algorithm to sub-images "
                       "from incoming images according to the bounding boxes contained "
                       "within the provided input detected_object_set." );
  fact->add_attribute( kwiver::vital::plugin_factory::PLUGIN_VERSION, "1.0" );
  
  fact = vpm.ADD_PROCESS( kwiver::stabilization_inspection_image_process );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_NAME,  "stabilization_inspection_image" );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME, module_name );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_DESCRIPTION,
                        "Processes image for stabilization quality analysis." );
  fact->add_attribute( kwiver::vital::plugin_factory::PLUGIN_VERSION, "1.0" );
  
  fact = vpm.ADD_PROCESS( kwiver::show_mask_on_image_process );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_NAME,  "show_mask_on_image" );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME, module_name );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_DESCRIPTION,
                        "Apply color to an image in regions where a mask is non-zero." );
  fact->add_attribute( kwiver::vital::plugin_factory::PLUGIN_VERSION, "1.0" );
  
  fact = vpm.ADD_PROCESS( kwiver::blend_images_process );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_NAME,  "blend_images" );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME, module_name );
  fact->add_attribute(  kwiver::vital::plugin_factory::PLUGIN_DESCRIPTION,
                        "Blend two images together into one RGB image." );
  fact->add_attribute( kwiver::vital::plugin_factory::PLUGIN_VERSION, "1.0" );

// - - - - - - - - - - - - - - - - - - - - - - -
  sprokit::mark_process_module_as_loaded( vpm, module_name );
}
