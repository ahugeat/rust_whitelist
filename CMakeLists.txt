cmake_minimum_required(VERSION 3.0)
project(whitelist_watcher)

# CXXFLAGS
set(CMAKE_CXX_FLAGS                 "-Wall -Wextra -std=c++11 -O2")

# Create executable
add_executable(whitelist_watcher
    src/whitelist-watcher.cc
    src/local/FileMonitor.cc
	src/local/RCon.cc
)

