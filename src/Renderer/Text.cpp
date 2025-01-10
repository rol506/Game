#include "Text.h"

#include "../Resources/ResourceManager.h"

namespace RenderEngine
{
  Text::Text(const std::string& text, const glm::vec2& position, const float scale, const glm::vec3& color, const std::shared_ptr<ShaderProgram> shader)
    : m_text(text), m_position(position), m_color(color), m_shader(std::move(shader)),
    m_scale(scale), m_width(0), m_height(0)
  {
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    auto charset = ResourceManager::getCharset();

    for (auto c = m_text.begin(); c != m_text.end(); c++)
    {
      Character ch = charset->at(*c);

      float w = ch.sizeX * m_scale;
      float h = ch.sizeY * m_scale;

      m_width += w;
      if (h > m_height) m_height = h;
    }
  }
  
  Text::~Text()
  {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
  }

  void Text::render() const
  {
    int x = m_position.x;
    int y = m_position.y;

    m_shader->use();
    m_shader->setVec3(m_color, "textColor");
    m_shader->setVec4(m_backgroundColor, "backgroundColor");
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_vao);

    auto charset = ResourceManager::getCharset();

    std::string::const_iterator c;
    for (c = m_text.begin(); c != m_text.end(); c++) 
    {
        Character ch = charset->at(*c);

        float xpos = x + ch.bearingX * m_scale;
        float ypos = y - (ch.sizeY - ch.bearingY) * m_scale;

        float w = ch.sizeX * m_scale;
        float h = ch.sizeY * m_scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };

        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.advance >> 6) * m_scale;
        x += 1;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0); 
  }

  void Text::setScale(const float scale)
  {
    m_scale = scale;
  }

  float Text::getWidth() const
  {
    return m_width;
  }

  float Text::getHeight() const
  {
    return m_height * 2.f;
  }
    

  void Text::setBackground(const float r, const float g, const float b, const float a)
  {
    m_backgroundColor = glm::vec4(r, g, b, a); 
  }

  void Text::setText(const std::string& text)
  {
    m_text = text;
    m_width = 0;
    m_height = 0;
    auto charset = ResourceManager::getCharset();

    for (auto c = m_text.begin(); c != m_text.end(); c++)
    {
      Character ch = charset->at(*c);

      float w = ch.sizeX * m_scale;
      float h = ch.sizeY * m_scale;

      m_width += w;
      if (h > m_height) m_height = h;
    }
  }

  void Text::setPosition(const glm::vec2& position)
  {
    m_position = position;
  }
}
