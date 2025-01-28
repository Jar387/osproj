# Collect all source files in the kernel directory
file(GLOB_RECURSE LIB_SOURCES "${CMAKE_SOURCE_DIR}/lib/*.c" "${CMAKE_SOURCE_DIR}/lib/*.s")

# Create an object library for kernel sources
add_library(lib OBJECT ${LIB_SOURCES})

# Specify include directories
target_include_directories(lib PRIVATE ${CMAKE_SOURCE_DIR}/include)