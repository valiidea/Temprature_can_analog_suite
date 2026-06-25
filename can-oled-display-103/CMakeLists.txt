cmake_minimum_required(VERSION 3.22)

project(CanOledDisplay103 C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/ld/STM32F103C8Tx_FLASH.ld)
set(TARGET_NAME ${PROJECT_NAME}.elf)

add_executable(${TARGET_NAME}
    src/main.c
    src/startup_stm32f103c8tx.c
)

target_compile_options(${TARGET_NAME} PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -ffreestanding
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wextra
    -Wpedantic
)

target_link_options(${TARGET_NAME} PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -T${LINKER_SCRIPT}
    -nostdlib
    -Wl,--gc-sections
    -Wl,-Map=${PROJECT_NAME}.map
)

set_target_properties(${TARGET_NAME} PROPERTIES
    LINK_DEPENDS ${LINKER_SCRIPT}
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${TARGET_NAME}>
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${TARGET_NAME}> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET_NAME}> ${PROJECT_NAME}.bin
    COMMENT "Generating firmware images"
)
