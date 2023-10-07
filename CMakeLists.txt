cmake_minimum_required(VERSION 3.22)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
project(cloneos C)
enable_language(ASM)

set(CMAKE_SYSTEM_NAME Generic)

#set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_FIND_ROOT_PATH "/home/marco/Scrivania/cross/")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(BAREMETAL_ARM_TOOLCHAIN_PATH /home/marco/Scrivania/cross/bin/)

set(CMAKE_AR                        ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-ar${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_ASM_COMPILER              ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-gcc${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_C_COMPILER                ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-gcc${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_LINKER                    ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-ld${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_OBJCOPY                   ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-objcopy${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_SIZE                      ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-size${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_STRIP                     ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-strip${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")

set(LINKER_SCRIPT_DIR "${CMAKE_SOURCE_DIR}/config/linker")
set(LINKER_SCRIPT "${LINKER_SCRIPT_DIR}/linker.ld")

set(CMAKE_C_FLAGS "-mcpu=cortex-a8")

set(CMAKE_EXE_LINKER_FLAGS "-Xlinker -L${LINKER_SCRIPT_DIR} \
                  --specs=nosys.specs \
                  -nostdlib  \
                  -T${LINKER_SCRIPT} \
                  -Wl,-Map=output.map \
                  -Wl,--stats \
                  -Wl,--gc-sections"
)

# flags -fdata-sections -ffunction-sections?
set (CMAKE_ASM_FLAGS "")

set(INCLUDES ${PROJECT_SOURCE_DIR} 
"config/" 
"arch/" 
"arch/arm/cortex_a/"
) 

include_directories(${INCLUDES})

add_library(c 
    lib/libc/minimal/string/strlen.c
)

target_compile_options(c PUBLIC
$<$<COMPILE_LANGUAGE:C>:${CMAKE_C_FLAGS}>)

target_link_libraries(c libgcc.a)

add_executable(edOS
    arch/arm/cortex-a/reset.S

    kernel/source/idle.c
    kernel/source/fatal.c

    lib/os/assert.c
)

target_link_libraries(edOS c)

target_compile_options(edOS PUBLIC
$<$<COMPILE_LANGUAGE:C>:${CMAKE_C_FLAGS}>
$<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp -mcpu=cortex-a8 -mfloat-abi=hard> )

