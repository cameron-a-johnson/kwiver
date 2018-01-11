/*ckwg +29
 * Copyright 2018 by Kitware, Inc.
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

#include "kwiver_tools_export.h"

#include <vital/plugin_loader/plugin_loader.h>
#include <vital/plugin_loader/plugin_manager.h>
#include <vital/plugin_loader/plugin_factory.h>

#include "config_explorer.h"

// ============================================================================
extern "C"
KWIVER_TOOLS_EXPORT
void
register_factories( kwiver::vital::plugin_loader& vpm )
{
  static auto const module_name = kwiver::vital::plugin_manager::module_t( "kwiver_tool_group" );

  if ( vpm.is_module_loaded( module_name ) )
  {
    return;
  }

  auto fact = vpm.ADD_APPLET( kwiver::tools::config_explorer );
  fact->add_attribute( kwiver::vital::plugin_factory::PLUGIN_NAME, "config_explorer" )
    .add_attribute( kwiver::vital::plugin_factory::PLUGIN_MODULE_NAME, module_name )
    .add_attribute( kwiver::vital::plugin_factory::PLUGIN_CATEGORY, "applet" )
    .add_attribute( kwiver::vital::plugin_factory::PLUGIN_DESCRIPTION,
                    "Explore configuration loading process.\n\n"
                    "This program assists in debugging config loading problems. It loads a "
                    "configuration and displays the contents or displays the search path." )
    .add_attribute( kwiver::vital::plugin_factory::PLUGIN_VERSION, "1.0" )
    ;
}
