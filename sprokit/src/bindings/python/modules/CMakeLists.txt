project(sprokit_python_modules)

set(modules_python_srcs
  registration.cxx)

set(modules_python_private_headers
  modules-config.h
  )

set(python_noarch FALSE)
kwiver_create_python_init(sprokit/modules)
set(python_noarch TRUE)
kwiver_create_python_init(sprokit/modules)
set(python_noarch FALSE)

kwiver_private_header_group(${modules_python_private_headers})

if(UNIX)
  add_definitions( -DSPROKIT_LOAD_PYLIB_SYM )
endif()

kwiver_add_plugin(modules_python
  SUBDIR          ${kwiver_plugin_process_subdir}
  SOURCES         ${modules_python_srcs}
                  ${modules_python_private_headers}
  PRIVATE         sprokit_pipeline
                  sprokit_python_util
                  kwiversys
                  ${${pybind11_library}}
                  ${PYTHON_LIBRARIES}
                  )


# Add definition to tell registration.cxx the name of the python_library it was
# compiled with. It will attempt to load symbols from this library dynamically.
get_filename_component(PYTHON_LIBRARY_NAME "${PYTHON_LIBRARY}" NAME)
target_compile_definitions(modules_python PRIVATE "-DPYTHON_LIBRARY=${PYTHON_LIBRARY_NAME}")


sprokit_add_python_module("${CMAKE_CURRENT_SOURCE_DIR}/modules.py"
  sprokit/modules
  modules)

execute_process(
  COMMAND           "${PYTHON_EXECUTABLE}" -c
                    "import pkg_resources; pkg_resources.require('straight.plugin >= 1.3.1')"
  WORKING_DIRECTORY "${sprokit_source_dir}"
  RESULT_VARIABLE   straight_return
  OUTPUT_QUIET
  ERROR_QUIET)

set(default_have_straight OFF)
if (NOT straight_return)
  set(default_have_straight ON)
endif ()

option(SPROKIT_ASSUME_STRAIGHT_PLUGIN "Assume the straight.plugin Python module exists (needs >= 1.3.1)" "${default_have_straight}")
if (NOT SPROKIT_ASSUME_STRAIGHT_PLUGIN)
  sprokit_add_python_module("${CMAKE_CURRENT_SOURCE_DIR}/loaders.py"
    sprokit/modules
    loaders)
endif()
