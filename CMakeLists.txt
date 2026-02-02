cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0016 NEW)

project(protobuf_parser VERSION 1.0.0 DESCRIPTION "?????? ?????? length-prefixed Protobuf ????????? ?? C++")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf REQUIRED)
find_package(GTest REQUIRED)
find_package(absl REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ./src/protocol/messages.proto )



# ?????
enable_testing()
add_executable(TestsProtobuf ./test/tests.cpp ${PROTO_SRCS} ${PROTO_HDRS}
    ./src/protobuf_parser/helpers.hpp
    ./src/protobuf_parser/DelimitedMessagesStreamParser.hpp)
target_link_libraries(TestsProtobuf ${Protobuf_LIBRARIES} GTest::GTest GTest::Main absl::log absl::log_internal_message absl::log_internal_check_op)
add_test(NAME TestsProtobuf COMMAND TestsProtobuf)

add_executable(Test_DelimitedMessagesStreamParser ./test/Test_DelimitedMessagesStreamParser.cpp ${PROTO_SRCS} ${PROTO_HDRS}
    ./src/protobuf_parser/helpers.hpp
    ./src/protobuf_parser/DelimitedMessagesStreamParser.hpp)
target_link_libraries(Test_DelimitedMessagesStreamParser ${Protobuf_LIBRARIES} GTest::GTest GTest::Main absl::log absl::log_internal_message absl::log_internal_check_op)
add_test(NAME Test_DelimitedMessagesStreamParser COMMAND Test_DelimitedMessagesStreamParser)

add_executable(Test_Helpers ./test/Test_Helpers.cpp ${PROTO_SRCS} ${PROTO_HDRS}
    ./src/protobuf_parser/helpers.hpp)
target_link_libraries(Test_Helpers ${Protobuf_LIBRARIES} GTest::GTest GTest::Main absl::log absl::log_internal_message absl::log_internal_check_op)
add_test(NAME Test_Helpers COMMAND Test_Helpers)
