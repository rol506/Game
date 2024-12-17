cmake_minimum_required(VERSION 3.31.0 FATAL_ERROR)

set(PROJECT_NAME Game)
project(${PROJECT_NAME} VERSION 1.0)

set(${CMAKE_EXPORT_COMPILE_COMMANDS} ON)

add_executable(${PROJECT_NAME}
  src/main.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}/res)
