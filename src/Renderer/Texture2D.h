#pragma once

#include <glad/glad.h>

#include <glm/vec2.hpp>

#include <unordered_map>
#include <string>

namespace RenderEngine
{
  class Texture2D
  {
  public:

    struct SubTexture2D {
      glm::vec2 leftBottomUV;
      glm::vec2 rightTopUV;

      SubTexture2D(const glm::vec2& _leftBottomUV, const glm::vec2& _rightTopUV)
        : leftBottomUV(_leftBottomUV), rightTopUV(_rightTopUV)
      {}

      SubTexture2D()
        : leftBottomUV(glm::vec2(0.0f)), rightTopUV(glm::vec2(1.0f))
      {}
    };

    Texture2D(const GLuint width, const GLuint height, const unsigned char* data, const unsigned int channels=4,
        const GLenum filter=GL_LINEAR, const GLenum wrapMode=GL_CLAMP_TO_EDGE);
    ~Texture2D();

    void bind() const;
    int width() const {return m_width;};
    int height() const {return m_height;};

    void addSubTexture(const std::string& subTextureName, const glm::vec2& leftBottomUV, const glm::vec2& rightTopUV);
    const SubTexture2D& getSubTexture(const std::string& subTextureName) const;

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;
    Texture2D& operator=(Texture2D&& texture) noexcept;
    Texture2D(Texture2D&& texture) noexcept;

  private:

    GLuint m_ID;
    GLuint m_width;
    GLuint m_height;
    GLenum m_mode;

    std::unordered_map<std::string, SubTexture2D> m_subTextures;

  };
}
