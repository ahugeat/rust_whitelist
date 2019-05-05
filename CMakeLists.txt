cmake_minimum_required(VERSION 3.0)

project(rust_whitelist)

if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

# CXXFLAGS
set(CMAKE_CXX_FLAGS                 "-Wall -Wextra -std=c++14 -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG           "-O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL      "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE         "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "-O2 -g")

# Set the dependencies
find_package(Boost REQUIRED COMPONENTS system thread)

# Create executable
add_executable(rust_whitelist
    src/rust-whitelist.cc
    src/local/FileMonitor.cc
    src/local/InotifyWrapper.cc
    src/local/RCon.cc
    src/local/RconClient.cc
)

target_include_directories(rust_whitelist
    PRIVATE
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(rust_whitelist
    ${Boost_LIBRARIES}
)
