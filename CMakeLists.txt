cmake_minimum_required(VERSION 3.10)
project(QuickJSHelloWorld)

set(CMAKE_CXX_STANDARD 17)

# Add the QuickJS include directory
include_directories(/usr/local/include/quickjs)

# Add the QuickJS library directory
link_directories(/usr/local/lib/quickjs)

# Find QuickJS library
find_library(QUICKJS_LIBRARY quickjs PATHS /usr/local/lib/quickjs NO_DEFAULT_PATH)

if(NOT QUICKJS_LIBRARY)
    message(FATAL_ERROR "QuickJS library not found. Make sure it's installed in /usr/local/lib/quickjs")
endif()

# Add executable
add_executable(hello_world main.cpp)
add_executable(complex_test complex_test.cpp)

# Link QuickJS library
target_link_libraries(hello_world ${QUICKJS_LIBRARY})



# Set the path to your V8 directory
set(V8_DIR /home/bash/v8/v8)

include_directories(${V8_DIR}/include)
link_directories(${V8_DIR}/out.gn/x64.release.sample/obj/)

add_executable(v8_hello_world v8_hello_world.cpp)

target_link_libraries(v8_hello_world
        v8_monolith
        v8_libbase
        v8_libplatform
        dl
        pthread
)

target_compile_definitions(v8_hello_world PRIVATE
        V8_COMPRESS_POINTERS
        V8_ENABLE_SANDBOX
)

target_compile_options(v8_hello_world PRIVATE -fno-rtti)