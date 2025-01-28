# Collect all source files in the kernel directory
file(GLOB_RECURSE DRIVERS_SOURCES "${CMAKE_SOURCE_DIR}/drivers/*.c" "${CMAKE_SOURCE_DIR}/drivers/*.s")

# Create an object library for kernel sources
add_library(drivers OBJECT ${DRIVERS_SOURCES})

# Specify include directories
target_include_directories(drivers PRIVATE ${CMAKE_SOURCE_DIR}/include)