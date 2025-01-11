#include "Sprite2D.h"

#include "Renderer.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace RenderEngine
{ 
  Sprite2D::Sprite2D(std::shared_ptr<Texture2D> texture, const std::string& initialSubTexture, std::shared_ptr<ShaderProgram> shader)
    : m_texture(std::move(texture)), m_shader(std::move(shader)), m_position(glm::vec2(0.0f)), m_scale(glm::vec2(1.0f)), m_rotation(0.0f)
  {
    auto subTexture = m_texture->getSubTexture(std::move(initialSubTexture)); 

    GLfloat vertices[] = {
    
      // 1 - 2
      // | / |
      // 0 - 3

      // X     Y              U                            V
      0.0f, 0.0f, subTexture.leftBottomUV.x, subTexture.leftBottomUV.y,
      0.0f, 1.0f, subTexture.leftBottomUV.x, subTexture.rightTopUV.y,
      1.0f, 1.0f, subTexture.rightTopUV.x,   subTexture.rightTopUV.y,
      1.0f, 0.0f, subTexture.rightTopUV.x,   subTexture.leftBottomUV.y,
    };

    GLuint indices[] = {
      0, 1, 2,
      0, 2, 3,
    };

    m_vertexBuffer.init(vertices, sizeof(vertices));
    VertexBufferLayout layout;
    layout.reserve(2);
    layout.addElementLayoutFloat(2, false);
    layout.addElementLayoutFloat(2, false);

    m_vertexArray.addBuffer(m_vertexBuffer, layout);

    m_indexBuffer.init(indices, 6);

    m_vertexArray.unbind();
    m_vertexBuffer.unbind();
    m_indexBuffer.unbind();
  }

  Sprite2D::~Sprite2D()
  {}

  void Sprite2D::render(const float layer) const
  {
    m_shader->use();

    m_shader->setInt(0, "tex");
    glActiveTexture(GL_TEXTURE0);
    m_texture->bind();

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(m_position, 0.f));
    //model = glm::translate(model, glm::vec3(0.5f * m_scale.x, 0.5f * m_scale.y, 0.f));
    model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0.f, 0.f, 1.f));
    //model = glm::translate(model, glm::vec3(-0.5f * m_scale.x, -0.5f * m_scale.y, 0.f));
    model = glm::scale(model, glm::vec3(m_scale, 1.f));

    m_shader->setMat4(model, "modelMatrix");
    m_shader->setFloat(layer, "layer");

    RenderEngine::Renderer::draw(m_vertexArray, m_indexBuffer, *m_shader);
  }

  void Sprite2D::setPosition(const glm::vec2& position)
  {
    m_position = position;
  }

  void Sprite2D::setScale(const glm::vec2& scale)
  {
    m_scale = scale;
  }

  void Sprite2D::setRotation(const float rotation)
  {
    if (rotation >= 360)
      m_rotation = 0;
    else
      m_rotation = rotation;
  }

  Sprite2D& Sprite2D::operator=(Sprite2D&& sprite) noexcept
  {
    if (this != &sprite)
    {
      m_texture = sprite.m_texture;
      m_shader = sprite.m_shader;

      m_position = sprite.m_position;
      m_scale = sprite.m_scale;
      m_rotation = sprite.m_rotation;

      m_vertexBuffer = std::move(sprite.m_vertexBuffer);
      m_indexBuffer = std::move(sprite.m_indexBuffer);
      m_vertexArray = std::move(sprite.m_vertexArray);
    }
    return *this;
  }

  Sprite2D::Sprite2D(Sprite2D&& sprite) noexcept
  {
    m_texture = sprite.m_texture;
    m_shader = sprite.m_shader;

    m_position = sprite.m_position;
    m_scale = sprite.m_scale;
    m_rotation = sprite.m_rotation;

    m_vertexBuffer = std::move(sprite.m_vertexBuffer);
    m_indexBuffer = std::move(sprite.m_indexBuffer);
    m_vertexArray = std::move(sprite.m_vertexArray);
  }
}
