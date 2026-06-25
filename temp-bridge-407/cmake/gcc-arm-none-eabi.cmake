# ============================================================================
# GCC ARM 交叉编译工具链配置文件
# 工具链前缀: arm-none-eabi- (需在 PATH 环境变量中)
# 目标: ARM Cortex-M4 + FPU (硬浮点 ABI)
# ============================================================================

set(CMAKE_SYSTEM_NAME               Generic)     # 裸机/无操作系统
set(CMAKE_SYSTEM_PROCESSOR          arm)         # ARM 架构

set(CMAKE_C_COMPILER_ID GNU)                     # 标识为 GCC 工具链
set(CMAKE_CXX_COMPILER_ID GNU)

# Prefer native arm64 GNU Arm toolchains on Apple Silicon. The STM32CubeCLT
# 1.21.0 GNU toolchain installed under /opt/ST is x86_64 on this machine and
# crashes before compilation, so do not let PATH pick it accidentally.
set(ARM_NONE_EABI_TOOLCHAIN_DIR "" CACHE PATH "Directory containing arm-none-eabi-gcc")

set(_ARM_NONE_EABI_HINTS)
if(ARM_NONE_EABI_TOOLCHAIN_DIR)
    list(APPEND _ARM_NONE_EABI_HINTS "${ARM_NONE_EABI_TOOLCHAIN_DIR}")
endif()
list(APPEND _ARM_NONE_EABI_HINTS
    "${CMAKE_SOURCE_DIR}/.tools/arm-gnu-toolchain/bin"
    "${CMAKE_SOURCE_DIR}/.tools/arm-gnu-toolchain/15.2.rel1/arm-none-eabi/bin"
    "$ENV{HOME}/Library/Application Support/stm32cube/bundles/gnu-tools-for-stm32/14.3.1+st.2/bin"
    "$ENV{HOME}/Applications/ArmGNUToolchain/15.2.rel1/arm-none-eabi/bin"
    "/Applications/ArmGNUToolchain/15.2.rel1/arm-none-eabi/bin"
)

find_program(ARM_NONE_EABI_GCC
    NAMES arm-none-eabi-gcc
    HINTS ${_ARM_NONE_EABI_HINTS}
    NO_DEFAULT_PATH
)

if(NOT ARM_NONE_EABI_GCC)
    find_program(ARM_NONE_EABI_GCC NAMES arm-none-eabi-gcc REQUIRED)
endif()

get_filename_component(ARM_NONE_EABI_BIN_DIR "${ARM_NONE_EABI_GCC}" DIRECTORY)

set(CMAKE_C_COMPILER                "${ARM_NONE_EABI_BIN_DIR}/arm-none-eabi-gcc")
set(CMAKE_ASM_COMPILER              "${CMAKE_C_COMPILER}")    # 汇编器复用 C 编译器
set(CMAKE_CXX_COMPILER              "${ARM_NONE_EABI_BIN_DIR}/arm-none-eabi-g++")
set(CMAKE_LINKER                    "${ARM_NONE_EABI_BIN_DIR}/arm-none-eabi-g++") # 链接器复用 C++ 编译器
set(CMAKE_AR                        "${ARM_NONE_EABI_BIN_DIR}/arm-none-eabi-ar")
set(CMAKE_RANLIB                    "${ARM_NONE_EABI_BIN_DIR}/arm-none-eabi-ranlib")
set(CMAKE_OBJCOPY                   "${ARM_NONE_EABI_BIN_DIR}/arm-none-eabi-objcopy")
set(CMAKE_SIZE                      "${ARM_NONE_EABI_BIN_DIR}/arm-none-eabi-size")

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

# 交叉编译时不能运行生成的程序，设置为静态库类型以通过 CMake 编译测试
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ============================================================================
# MCU 特定编译标志
# ============================================================================
# -mcpu=cortex-m4       : 目标 CPU 为 Cortex-M4
# -mfpu=fpv4-sp-d16     : 单精度 FPU
# -mfloat-abi=hard      : 硬浮点 ABI (使用 FPU 寄存器传参)
set(TARGET_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard ")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")  # 汇编文件经 C 预处理器处理
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fdata-sections -ffunction-sections -fstack-usage")
# -Wall               : 启用常用编译警告
# -fdata-sections     : 每个数据项放入独立段（配合 --gc-sections 去除未用数据）
# -ffunction-sections : 每个函数放入独立段（配合 --gc-sections 去除未用函数）
# -fstack-usage       : 生成 .su 栈使用信息文件

# 圈复杂度选项仅在 STM32CubeIDE 中有效，大多数 GCC 工具链不支持，保持禁用
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcyclomatic-complexity")

# Debug: 无优化 + 完整调试信息; Release: 体积优化 + 去除调试信息
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os -g0")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -g0")

# C++ 禁用 RTTI、异常和线程安全静态变量以减小体积
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

# ============================================================================
# 链接器标志
# ============================================================================
set(CMAKE_EXE_LINKER_FLAGS "${TARGET_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T \"${CMAKE_SOURCE_DIR}/STM32F407XX_FLASH.ld\"")
# -T <script>               : 使用自定义链接脚本
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nano.specs")
# --specs=nano.specs        : 使用 newlib-nano 精简 C 库
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")
# -Wl,-Map=<file>           : 生成链接映射文件
# -Wl,--gc-sections         : 移除未引用的代码段和数据段
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-memory-usage")
# -Wl,--print-memory-usage  : 链接后打印内存使用情况

# 链接数学库 (libm)
set(TOOLCHAIN_LINK_LIBRARIES "m")
