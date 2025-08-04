cmake_minimum_required(VERSION 3.1)
project(LogSys)

# 启用编译数据库（即使没有代码）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 显式设置编译模式（Debug/Release）
set(CMAKE_BUILD_TYPE Debug)  # 关键！生成调试符号

# 设置 C++11 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 必须使用 C++11
# 找到源文件
file(GLOB EXAMPLE ${CMAKE_CURRENT_SOURCE_DIR}/example/*.cc)
add_executable(example ${EXAMPLE})
target_include_directories(example PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
file(GLOB BENCH ${CMAKE_CURRENT_SOURCE_DIR}/bench/*.cc)
add_executable(bench ${BENCH})
target_include_directories(bench PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)


