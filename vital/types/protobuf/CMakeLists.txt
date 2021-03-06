#
# CMake file for building protobuf libraries.
#

find_package( Protobuf REQUIRED )

set( proto_files
  bounding_box.proto
  detected_object_type.proto
  detected_object.proto
  detected_object_set.proto
  timestamp.proto
  image.proto
  string.proto
  )

include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Need to add this include file to the compile because of a defficiency in the protobuf
# compiler. The compiler has the option to insert an export macro into class definitions
# but has no way of defining that macro. as of libprotoc 2.6.1 this is the accepted way
# of defining the export symbols.
add_compile_options( -include "${CMAKE_CURRENT_BINARY_DIR}/vital_types_protobuf_export.h" )

PROTOBUF_GENERATE_CPP( cpp_srcs cpp_hdrs EXPORT_MACRO VITAL_TYPES_PROTOBUF_EXPORT ${proto_files} )

kwiver_add_library( vital_types_protobuf
  ${cpp_hdrs} ${cpp_srcs} )

target_link_libraries( vital_types_protobuf
  PUBLIC       vital
               ${PROTOBUF_LIBRARIES}
  )
