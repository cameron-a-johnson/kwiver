project(kwiver_serialize_protbuf_tests)

include(kwiver-test-setup)

set( test_libraries vital vital_vpm vital_algo vital_util kwiver_serialize_protobuf )

##############################
# PROTOBUF tests
##############################

kwiver_discover_gtests(serialize-protobuf serialize         LIBRARIES ${test_libraries})
