#include "VertexBufferLayout.h"

namespace RenderEngine
{
  VertexBufferLayout::VertexBufferLayout()
    : m_stride(0)
  {}

  VertexBufferLayout::~VertexBufferLayout()
  {
    m_layoutElements.clear();
  }

  void VertexBufferLayout::reserve(const size_t count)
  {
    m_layoutElements.reserve(count);
  }

  void VertexBufferLayout::addElementLayoutFloat(const unsigned int count, const bool normalized)
  {
    m_layoutElements.push_back({static_cast<GLint>(count), GL_FLOAT, normalized, count * static_cast<unsigned int>(sizeof(GLfloat))});
    m_stride += m_layoutElements.back().size;
  }
}
