cmake_minimum_required(VERSION 3.9)

project(LevelDBShell VERSION 1.0)

# Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(third_party/leveldb)
add_subdirectory(third_party/argparse)

# Create the executable
add_executable(leveldb-shell 
        src/main.cc
        src/LevelDBShell.cc
)

target_include_directories(leveldb-shell 
            PRIVATE ${CMAKE_SOURCE_DIR}/include
            PRIVATE ${argparse_INCLUDE_DIRS}
            PRIVATE /include
)

# Link against LevelDB and argparse
target_link_libraries(leveldb-shell leveldb argparse)

# Configure version header
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/config/version.h.in"  # Input template file
    "${CMAKE_BINARY_DIR}/cmake/config/version.h"     # Output file
)

# Add the directory containing the generated version.h to the include path
target_include_directories(leveldb-shell PRIVATE "${CMAKE_BINARY_DIR}/cmake/config")
