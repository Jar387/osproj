# Collect all source files in the kernel directory
file(GLOB_RECURSE KERNEL_SOURCES "${CMAKE_SOURCE_DIR}/kernel/*.c" "${CMAKE_SOURCE_DIR}/kernel/*.s")

# Create an object library for kernel sources
add_library(kernel OBJECT ${KERNEL_SOURCES})

# Specify include directories
target_include_directories(kernel PRIVATE ${CMAKE_SOURCE_DIR}/include)