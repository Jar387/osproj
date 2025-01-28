cmake_minimum_required(VERSION 3.10)
project(osproj)

ENABLE_LANGUAGE(ASM)

# Set the toolchain for cross-compilation
set(CMAKE_C_COMPILER "i686-linux-gnu-gcc-5")
set(CMAKE_ASM_COMPILER "i686-linux-gnu-as")
set(CMAKE_LINKER "i686-linux-gnu-gcc-5")
set(CMAKE_AR "i686-linux-gnu-ar")

# Set compiler flags
set(CMAKE_C_FLAGS "-std=gnu99 -finline-functions -ffreestanding -I${CMAKE_SOURCE_DIR}/include -g -O3")
set(CMAKE_EXE_LINKER_FLAGS "-T${CMAKE_SOURCE_DIR}/link.ld -ffreestanding -nostdlib")

# Add subdirectories
add_subdirectory(kernel)
add_subdirectory(drivers)
add_subdirectory(mm)
add_subdirectory(lib)
add_subdirectory(fs)

# Define the final output
set(LIBOBJS 
    $<TARGET_OBJECTS:kernel> 
    $<TARGET_OBJECTS:drivers>
    $<TARGET_OBJECTS:mm>
    $<TARGET_OBJECTS:lib>
    $<TARGET_OBJECTS:fs>
)


add_executable(kernel.out ${LIBOBJS})
set_target_properties(kernel.out PROPERTIES LINKER_LANGUAGE C)

# Custom build commands for running and debugging
add_custom_target(run
    COMMAND cp kernel.out mnt/
    COMMAND sync
    COMMAND qemu-system-i386 -hda disk.img -no-reboot -no-shutdown -cpu host -enable-kvm
    DEPENDS kernel.out
)

add_custom_target(debug
    COMMAND cp kernel.out mnt/
    COMMAND sync
    COMMAND bochs
    DEPENDS kernel.out
)

add_custom_target(mount
    COMMAND losetup /dev/loop0 disk.img -o 1048576
    COMMAND mount /dev/loop0 mnt
)

add_custom_target(umount
    COMMAND umount /dev/loop0
    COMMAND losetup -d /dev/loop0
)

add_custom_target(indent
    COMMAND ${CMAKE_SOURCE_DIR}/indent.sh
)

add_custom_target(ln
    COMMAND ln -s ../disk.img disk.img
    COMMAND ln -s ../mnt mnt
    COMMAND ln -s ../bochsrc bochsrc
)
