# Collect all source files in the kernel directory
file(GLOB_RECURSE FS_SOURCES "${CMAKE_SOURCE_DIR}/fs/*.c" "${CMAKE_SOURCE_DIR}/fs/*.s")

# Create an object library for kernel sources
add_library(fs OBJECT ${FS_SOURCES})

# Specify include directories
target_include_directories(fs PRIVATE ${CMAKE_SOURCE_DIR}/include)