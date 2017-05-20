cmake_minimum_required(VERSION 3.0)
project(rust_whitelist)

# CXXFLAGS
set(CMAKE_CXX_FLAGS                 "-Wall -Wextra -std=c++11 -O2")

# Create executable
add_executable(rust_whitelist
    src/rust-whitelist.cc
    src/local/FileMonitor.cc
    src/local/InotifyWrapper.cc
	src/local/RCon.cc
)

