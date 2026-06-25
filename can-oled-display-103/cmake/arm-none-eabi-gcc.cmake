set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(ARM_TOOLCHAIN_HINTS
    /opt/homebrew/bin
    /usr/local/bin
)

find_program(ARM_NONE_EABI_GCC arm-none-eabi-gcc
    HINTS ${ARM_TOOLCHAIN_HINTS}
    NO_DEFAULT_PATH
)
find_program(ARM_NONE_EABI_OBJCOPY arm-none-eabi-objcopy
    HINTS ${ARM_TOOLCHAIN_HINTS}
    NO_DEFAULT_PATH
)
find_program(ARM_NONE_EABI_SIZE arm-none-eabi-size
    HINTS ${ARM_TOOLCHAIN_HINTS}
    NO_DEFAULT_PATH
)

find_program(ARM_NONE_EABI_GCC arm-none-eabi-gcc REQUIRED)
find_program(ARM_NONE_EABI_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(ARM_NONE_EABI_SIZE arm-none-eabi-size REQUIRED)

set(CMAKE_C_COMPILER ${ARM_NONE_EABI_GCC})
set(CMAKE_ASM_COMPILER ${ARM_NONE_EABI_GCC})
set(CMAKE_OBJCOPY ${ARM_NONE_EABI_OBJCOPY})
set(CMAKE_SIZE ${ARM_NONE_EABI_SIZE})
