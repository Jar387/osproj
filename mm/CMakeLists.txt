# Collect all source files in the kernel directory
file(GLOB_RECURSE MM_SOURCES "${CMAKE_SOURCE_DIR}/mm/*.c" "${CMAKE_SOURCE_DIR}/mm/*.s")

# Create an object library for kernel sources
add_library(mm OBJECT ${MM_SOURCES})

# Specify include directories
target_include_directories(mm PRIVATE ${CMAKE_SOURCE_DIR}/include)