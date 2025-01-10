#pragma once

#include <string>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include <glad/glad.h>

#include "ShaderProgram.h"

namespace RenderEngine
{
  class Text
  {
  public:

    Text(const std::string& text, const glm::vec2& position, const float scale, const glm::vec3& color, const std::shared_ptr<ShaderProgram> shader);
    ~Text();
    void setText(const std::string& text);

    void render() const;

    void setScale(const float scale);
    float getScale() const {return m_scale;}

    void setPosition(const glm::vec2& position);
    glm::vec2 getPosition() const {return m_position;}

    float getWidth() const;
    float getHeight() const;
    
    void setBackground(const float r, const float g, const float b, const float a=1);

  private:

    GLuint m_vao;
    GLuint m_vbo;
    glm::vec3 m_color;
    glm::vec2 m_position;
    float m_scale;
    std::string m_text;
    glm::vec4 m_backgroundColor;

    float m_width;
    float m_height;

    std::shared_ptr<ShaderProgram> m_shader;

  };
}
