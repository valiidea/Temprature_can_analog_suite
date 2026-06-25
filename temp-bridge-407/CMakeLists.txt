# 最低 CMake 版本要求
cmake_minimum_required(VERSION 3.22)

#
# 此文件由 STM32CubeMX 生成一次，后续转换器调用时不会重新生成。
# 用户可自由修改此文件。
#

# ============================================================================
# 编译器设置
# ============================================================================
set(CMAKE_C_STANDARD 11)           # 使用 C11 标准
set(CMAKE_C_STANDARD_REQUIRED ON)  # 强制要求 C11（不支持则报错）
set(CMAKE_C_EXTENSIONS ON)         # 启用 GNU C 扩展


# ============================================================================
# 构建类型设置
# ============================================================================
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")  # 默认 Debug 构建
endif()

# ============================================================================
# 项目基本设置
# ============================================================================
set(CMAKE_PROJECT_NAME TempBridge407)                 # 项目名称
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)                # 生成 compile_commands.json 供 clangd 等工具使用

project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# 启用 ASM 和 C 语言支持
enable_language(C ASM)

# 创建可执行文件目标
add_executable(${CMAKE_PROJECT_NAME})

# ============================================================================
# 子目录和源文件
# ============================================================================
# 引入 STM32CubeMX 生成的源文件、头文件和库
add_subdirectory(cmake/stm32cubemx)

# ============================================================================
# 链接目录、源文件、头文件路径、编译宏
# ============================================================================
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # 在此添加用户自定义库搜索路径
)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # 在此添加用户源文件
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # 在此添加用户自定义头文件路径
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # 在此添加用户自定义宏
)

# ============================================================================
# 链接库设置
# ============================================================================
# 移除对 libob.a 的错误依赖（使用 C++ 文件时可能出现）
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# 链接 STM32CubeMX 接口库及其他用户库
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # 在此添加用户自定义库
)

# ============================================================================
# 烧录文件生成
# ============================================================================
# 构建完成后生成常用烧录产物：
#   - .elf: 调试/符号文件
#   - .hex: STM32CubeProgrammer、ST-LINK Utility 常用烧录文件
#   - .bin: 裸二进制固件
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex
            $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
            $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary
            $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
            $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/${CMAKE_PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    COMMENT "Generating ${CMAKE_PROJECT_NAME}.hex and ${CMAKE_PROJECT_NAME}.bin"
)
