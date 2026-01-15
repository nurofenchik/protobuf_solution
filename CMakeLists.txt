cmake_minimum_required(VERSION 3.15)
project(DelimitedProtobufParser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===================== Protobuf + Abseil =====================
find_package(Protobuf REQUIRED)
find_package(absl REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})  # messages.pb.h

# ===================== Protobuf generation =====================
set(PROTO_FILES ${CMAKE_SOURCE_DIR}/messages.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# ===================== Main executable =====================
add_executable(parser_demo
    parser_demo.cpp
    ${PROTO_SRCS}
)

target_link_libraries(parser_demo
    ${Protobuf_LIBRARIES}
    absl::log
    absl::check
    absl::strings
    absl::base
    pthread
)

# ===================== Google Test =====================
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ===================== Tests =====================
enable_testing()

add_executable(parser_tests
    tests.cpp
    ${PROTO_SRCS}
)

target_link_libraries(parser_tests
    gtest_main
    ${Protobuf_LIBRARIES}
    absl::log
    absl::check
    absl::strings
    absl::base
    pthread
)

include(GoogleTest)
gtest_discover_tests(parser_tests)
