# ============================================================================
# STARM Clang 交叉编译工具链配置文件
# 工具链前缀: starm- (基于 LLVM/Clang 的 ARM 嵌入式工具链)
# 目标: ARM Cortex-M4 + FPU (硬浮点 ABI)
# ============================================================================

set(CMAKE_SYSTEM_NAME               Generic)     # 裸机/无操作系统
set(CMAKE_SYSTEM_PROCESSOR          arm)         # ARM 架构

set(CMAKE_C_COMPILER_ID Clang)                   # 标识为 Clang 工具链
set(CMAKE_CXX_COMPILER_ID Clang)

# 工具链前缀 — starm- 必须在 PATH 环境变量中
set(TOOLCHAIN_PREFIX                starm-)

set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}clang)
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})    # 汇编器复用 C 编译器
set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}clang++)
set(CMAKE_LINKER                    ${TOOLCHAIN_PREFIX}clang)
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ============================================================================
# 工具链配置模式选择
#   "STARM_HYBRID"   : 混合模式 — starm-clang 编译/汇编 + GNU 链接器
#   "STARM_NEWLIB"   : starm-clang + NEWLIB C 库
#   "STARM_PICOLIBC" : starm-clang + PICOLIBC C 库 (当前使用)
# ============================================================================
set(STARM_TOOLCHAIN_CONFIG "STARM_PICOLIBC")

if(STARM_TOOLCHAIN_CONFIG STREQUAL "STARM_HYBRID")
  # 混合模式需要指定 multi-lib 配置和 GCC 工具链根目录
  set(TOOLCHAIN_MULTILIBS "--multi-lib-config=\"$ENV{CLANG_GCC_CMSIS_COMPILER}/multilib.gnu_tools_for_stm32.yaml\" --gcc-toolchain=\"$ENV{GCC_TOOLCHAIN_ROOT}/..\"")
elseif (STARM_TOOLCHAIN_CONFIG STREQUAL "STARM_NEWLIB")
  set(TOOLCHAIN_MULTILIBS "--config=newlib.cfg")
endif()

# ============================================================================
# MCU 特定编译标志
# ============================================================================
# -mcpu=cortex-m4       : 目标 CPU 为 Cortex-M4
# -mfpu=fpv4-sp-d16     : 单精度 FPU
# -mfloat-abi=hard      : 硬浮点 ABI
set(TARGET_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard ${TOOLCHAIN_MULTILIBS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MP")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fdata-sections -ffunction-sections -ftls-model=local-exec -fstack-usage")
# -ftls-model=local-exec : 线程局部存储使用 local-exec 模型（裸机环境下最高效）
# -fstack-usage          : 生成栈使用信息文件

# Debug: 优化调试体验 + 完整调试信息; Release: 最小体积 + 去除调试信息
set(CMAKE_C_FLAGS_DEBUG "-Og -g3")
set(CMAKE_C_FLAGS_RELEASE "-Oz -g0")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-Oz -g0")

# C++ 禁用 RTTI、异常和线程安全静态变量以减小体积
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

# ============================================================================
# 链接器标志 (根据工具链配置模式选择不同的 C 运行时)
# ============================================================================
set(CMAKE_EXE_LINKER_FLAGS "${TARGET_FLAGS}")

if (STARM_TOOLCHAIN_CONFIG STREQUAL "STARM_HYBRID")
  # 混合模式: 使用 GNU 链接器 + nano 规格文件
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --gcc-specs=nano.specs")
  set(TOOLCHAIN_LINK_LIBRARIES "m")
elseif(STARM_TOOLCHAIN_CONFIG STREQUAL "STARM_NEWLIB")
  # NEWLIB 模式: 链接 crt0-nosys (无系统调用)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lcrt0-nosys")
elseif(STARM_TOOLCHAIN_CONFIG STREQUAL "STARM_PICOLIBC")
  # PICOLIBC 模式: 链接 crt0-hosted (托管启动) + 禁用 RELRO
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lcrt0-hosted -z norelro")

endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T \"${CMAKE_SOURCE_DIR}/STM32F407XX_FLASH.ld\"")
# -T <script>               : 使用自定义链接脚本
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")
# -Wl,-Map=<file>           : 生成链接映射文件
# -Wl,--gc-sections         : 移除未引用的段以减小体积
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack")
# -z noexecstack            : 标记栈不可执行 (安全加固)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-memory-usage ")
# -Wl,--print-memory-usage  : 链接后打印内存使用摘要
