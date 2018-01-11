#
# CMake file for the vital tools
#

set( sources
  register_tools.cxx

  config_explorer.cxx
  )

set( headers
  config_explorer.h
  )

###
# Add our plugin path to the default list
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

###
# Add path to applets so plugin loader can find them
kwiver_make_module_path( ${KWIVER_BINARY_DIR} modules/applets )
kwiver_add_module_path( ${kwiver_module_path_result} )

###
#
kwiver_add_library( kwiver_tools_applet
               kwiver_applet.h
               kwiver_applet.cxx
)

###
#
kwiver_add_executable( kwiver
           kwiver_tool_runner.cxx
)

target_link_libraries( kwiver
  PRIVATE        vital
                 vital_vpm
                 vital_logger
                 kwiversys
                 kwiver_tools_applet
)

###
# Add applet for config
kwiver_add_plugin( kwiver_tools
  SUBDIR       modules/applets
  SOURCES      ${sources} ${headers}
  PRIVATE      vital
               vital_vpm
               #vital_config
               vital_logger
               kwiversys
               kwiver_tools_applet
)
