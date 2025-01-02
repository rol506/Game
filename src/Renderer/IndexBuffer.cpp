#include "IndexBuffer.h"

namespace RenderEngine
{
  IndexBuffer::IndexBuffer()
    : m_ID(0), m_elementsCount(0)
  {}

  IndexBuffer::~IndexBuffer()
  {
    glDeleteBuffers(1, &m_ID);
  }

  void IndexBuffer::init(const void* data, const unsigned int count)
  {
    m_elementsCount = count;
    glGenBuffers(1, &m_ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW);
  }

  void IndexBuffer::update(const void* data, const unsigned int count)
  {
    m_elementsCount = count;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(GLuint), data);
  }

  void IndexBuffer::bind() const
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
  }

  void IndexBuffer::unbind() const
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  IndexBuffer& IndexBuffer::operator=(IndexBuffer&& buffer) noexcept
  {
    if (this != &buffer)
    {
      glDeleteBuffers(1, &m_ID);
      m_ID = buffer.m_ID;
      buffer.m_ID = 0;
      m_elementsCount = buffer.m_elementsCount;
    }
    return *this;
  }

  IndexBuffer::IndexBuffer(IndexBuffer&& buffer) noexcept
  {
    m_ID = buffer.m_ID;
    buffer.m_ID = 0;
    m_elementsCount = buffer.m_elementsCount;  
  }
}
