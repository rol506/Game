#include "Texture2D.h"

namespace RenderEngine
{

  Texture2D::Texture2D(const GLuint width, const GLuint height, const unsigned char* data, const unsigned int channels, const GLenum filter, const GLenum wrapMode)
    : m_width(width), m_height(height)
  {
    switch(channels)
    {
      case 3:
        m_mode = GL_RGB;
        break;
      case 4:
        m_mode = GL_RGBA;
        break;
      default:
        m_mode = GL_RGBA;
        break;
    }

    glGenTextures(1, &m_ID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, m_mode, m_width, m_height, 0, m_mode, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  Texture2D::~Texture2D()
  {
    glDeleteTextures(1, &m_ID);
  }

  void Texture2D::bind() const
  { 
    glBindTexture(GL_TEXTURE_2D, m_ID);
  }

  void Texture2D::addSubTexture(const std::string& subTextureName, const glm::vec2& leftBottomUV, const glm::vec2& rightTopUV)
  {
    m_subTextures.emplace(std::move(subTextureName), SubTexture2D(leftBottomUV, rightTopUV)); 
  }

  const Texture2D::SubTexture2D& Texture2D::getSubTexture(const std::string& subTextureName) const
  {
    auto it = m_subTextures.find(subTextureName);
    if (it != m_subTextures.end())
    {
      return it->second;
    }

    const static SubTexture2D defaultSubTexture;
    return defaultSubTexture;
  }

  Texture2D& Texture2D::operator=(Texture2D&& texture) noexcept
  {
    if (this != &texture)
    {
      glDeleteTextures(1, &m_ID);
      m_ID = texture.m_ID;
      texture.m_ID = 0;
      m_width = texture.m_width;
      m_height = texture.m_height;
      m_mode = texture.m_mode;
    }

    return *this;
  }

  Texture2D::Texture2D(Texture2D&& texture) noexcept
  {
    m_ID = texture.m_ID;
    texture.m_ID = 0;
    m_width = texture.m_width;
    m_height = texture.m_height;
    m_mode = texture.m_mode; 
  } 
}
