# ============================================================================
# STM32CubeMX 生成的子 CMake 文件
# 负责管理 HAL 驱动库、CMSIS 头文件路径和应用源文件。
# ============================================================================

cmake_minimum_required(VERSION 3.22)
# 启用 ASM 和 C 语言支持
enable_language(C ASM)

# ============================================================================
# STM32CubeMX 生成的预定义宏
# ============================================================================
set(MX_Defines_Syms
	USE_HAL_DRIVER      # 启用 HAL 驱动
	STM32F407xx         # 目标 MCU 型号
    $<$<CONFIG:Debug>:DEBUG>  # Debug 构建时定义 DEBUG 宏
)

# ============================================================================
# STM32CubeMX 生成的头文件搜索路径
# ============================================================================
set(MX_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc                          # 用户应用头文件
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Inc  # HAL 驱动头文件
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy  # HAL 遗留驱动头文件
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Device/ST/STM32F4xx/Include  # CMSIS 设备头文件
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Include             # CMSIS 核心头文件
)

# ============================================================================
# 应用层源文件 (用户代码)
# ============================================================================
set(MX_Application_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/main.c                     # 主程序
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f4xx_it.c             # 中断处理
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f4xx_hal_msp.c        # HAL MSP 初始化
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/sysmem.c                   # 内存管理 (sbrk)
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/syscalls.c                 # 系统调用桩
    ${CMAKE_CURRENT_SOURCE_DIR}/../../startup_stm32f407xx.s               # 启动文件 (汇编)
)

# ============================================================================
# STM32 HAL/LL 驱动源文件 (根据 CubeMX 配置选择)
# ============================================================================
set(STM32_Drivers_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/system_stm32f4xx.c                            # 系统初始化
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c   # 时钟控制
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c # Flash
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c  # GPIO
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c  # UART
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c   # I2C
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_can.c   # CAN
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c # DMA
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c   # 电源控制
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c # Cortex 内核
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c        # HAL 核心
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c   # 外部中断
)

# 中间件驱动 (本项目未使用)

# ============================================================================
# 链接目录
# ============================================================================
set(MX_LINK_DIRS

)

# ============================================================================
# 项目链接库列表
# ============================================================================
set(MX_LINK_LIBS
    STM32_Drivers                   # STM32 驱动对象库
    ${TOOLCHAIN_LINK_LIBRARIES}     # 工具链额外库 (如 libm)

)

# ============================================================================
# 构建目标定义
# ============================================================================

# stm32cubemx: 接口库 — 提供头文件路径和编译宏，不编译自身源文件
add_library(stm32cubemx INTERFACE)
target_include_directories(stm32cubemx INTERFACE ${MX_Include_Dirs})
target_compile_definitions(stm32cubemx INTERFACE ${MX_Defines_Syms})

# STM32_Drivers: 对象库 — 编译 HAL 驱动源文件为目标文件，不生成独立库文件
add_library(STM32_Drivers OBJECT)
target_sources(STM32_Drivers PRIVATE ${STM32_Drivers_Src})
target_link_libraries(STM32_Drivers PUBLIC stm32cubemx)


# 将应用源文件添加到主可执行目标
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${MX_Application_Src})

# 链接目录
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${MX_LINK_DIRS})

# 链接库
target_link_libraries(${CMAKE_PROJECT_NAME} ${MX_LINK_LIBS})

# 将 .map 文件加入 clean 目标（清理构建产物时一并删除）
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

# 验证 C 标准兼容性：生成的代码需要 C11 或更高版本
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "生成的代码需要 C11 或更高版本")
endif()
