cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)

project(hokacc CXX)

find_package(fmt)
find_package(spdlog)

add_executable(
    ${PROJECT_NAME}
    src/main.cpp
)

target_compile_options(${PROJECT_NAME} PUBLIC "-W" "-Wall" "-Wextra")
target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_FMT_EXTERNAL)
target_link_libraries(${PROJECT_NAME} fmt::fmt)
target_link_libraries(${PROJECT_NAME} spdlog::spdlog)
