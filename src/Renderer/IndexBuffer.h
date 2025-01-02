#pragma once

#include <glad/glad.h>

namespace RenderEngine
{
  class IndexBuffer
  {
  public:

    IndexBuffer();
    ~IndexBuffer();

    void init(const void* data, const unsigned int count);
    void update(const void* data, const unsigned int count);
    void bind() const;
    void unbind() const;

    unsigned int getCount() const {return m_elementsCount;}

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;
    IndexBuffer& operator=(IndexBuffer&& buffer) noexcept;
    IndexBuffer(IndexBuffer&& buffer) noexcept;

  private:

    GLuint m_ID;
    GLuint m_elementsCount;

  };
}
