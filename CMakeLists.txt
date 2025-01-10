cmake_minimum_required(VERSION 3.31.0 FATAL_ERROR)

set(PROJECT_NAME Game)
project(${PROJECT_NAME} VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
  src/main.cpp

  src/Renderer/ShaderProgram.cpp
  src/Renderer/ShaderProgram.h
  src/Renderer/Texture2D.cpp
  src/Renderer/Texture2D.h
  src/Renderer/VertexBuffer.cpp
  src/Renderer/VertexBuffer.h
  src/Renderer/VertexBufferLayout.cpp
  src/Renderer/VertexBufferLayout.h
  src/Renderer/IndexBuffer.cpp
  src/Renderer/IndexBuffer.h
  src/Renderer/VertexArray.cpp
  src/Renderer/VertexArray.h
  src/Renderer/Renderer.cpp
  src/Renderer/Renderer.h
  src/Renderer/Sprite2D.cpp
  src/Renderer/Sprite2D.h
  src/Renderer/Text.cpp
  src/Renderer/Text.h

  src/Resources/ResourceManager.cpp
  src/Resources/ResourceManager.h

  src/System/Socket.cpp
  src/System/Socket.h
)


if (UNIX)
  add_executable(TestSocket
    src/server.c
  )

  set_target_properties(TestSocket PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif(UNIX)

add_subdirectory(external/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

add_subdirectory(external/glad)
target_link_libraries(${PROJECT_NAME} glad)

add_subdirectory(external/glm)
target_link_libraries(${PROJECT_NAME} glm)

add_subdirectory(external/freetype)
target_link_libraries(${PROJECT_NAME} freetype)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)
