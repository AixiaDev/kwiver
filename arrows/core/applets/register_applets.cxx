/*ckwg +29
 * Copyright 2019 by Kitware, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
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

/**
 * \file
 * \brief register core applets into a plugin
 */

#include <arrows/core/applets/kwiver_algo_core_applets_export.h>
#include <vital/plugin_loader/plugin_loader.h>
#include <vital/applets/applet_registrar.h>

#include <arrows/core/applets/dump_klv.h>
#include <arrows/core/applets/render_mesh.h>
#include <arrows/core/applets/track_features.h>

namespace kwiver {
namespace arrows {
namespace core {

// ----------------------------------------------------------------------------
extern "C"
KWIVER_ALGO_CORE_APPLETS_EXPORT
void
register_factories( kwiver::vital::plugin_loader& vpm )
{
  kwiver::applet_registrar reg( vpm, "arrows.core.applets" );

  if (reg.is_module_loaded())
  {
    return;
  }

  // -- register applets --
  reg.register_tool< dump_klv >();
  reg.register_tool< render_mesh >();
  reg.register_tool< track_features >();

  reg.mark_module_as_loaded();
}

} // end namespace core
} // end namespace arrows
} // end namespace kwiver
