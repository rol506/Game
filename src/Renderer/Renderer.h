#pragma once

#include <string>

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "ShaderProgram.h"

#include <glm/vec2.hpp>

namespace RenderEngine
{
  class Renderer
  {
  public:
    Renderer() = delete;

    static void setDepthTest(const bool enable);
    static void clear();
    static void setClearColor(const float r, const float g, const float b, const float a);
    static void setViewport(const unsigned int width, const unsigned int height, const unsigned int leftOffset=0, const unsigned int bottomOffset=0);
    static void draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const ShaderProgram& shader);

    static std::string getRendererStr();
    static std::string getVersionStr();
  };
}
