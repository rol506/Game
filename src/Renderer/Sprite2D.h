#pragma once

#include "Texture2D.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

#include <memory>
#include <string>

#include <glm/vec2.hpp>

namespace RenderEngine
{
  class Sprite2D
  {
  public:

    Sprite2D(std::shared_ptr<Texture2D> texture, const std::string& initialSubTexture, std::shared_ptr<ShaderProgram> shader);
    ~Sprite2D();

    void render(const float layer) const;

    void setPosition(const glm::vec2& position);
    glm::vec2 getPosition() const {return m_position;}
    glm::vec2 getLastPosition() const {return m_lastPosition;}

    void setScale(const glm::vec2& scale);
    glm::vec2 getScale() const {return m_scale;}

    void setRotation(const float rotation);
    float getRotation() const {return m_rotation;}

    void setOutline(int enabled);

    Sprite2D(const Sprite2D&) = delete;
    Sprite2D& operator=(const Sprite2D&) = delete;
    Sprite2D& operator=(Sprite2D&& sprite) noexcept;
    Sprite2D(Sprite2D&& sprite) noexcept;

    glm::vec2 velocity;
    bool isGrounded = false;
    bool isStatic = false;

  private:

    std::shared_ptr<Texture2D> m_texture;
    std::shared_ptr<ShaderProgram> m_shader;

    glm::vec2 m_position;
    glm::vec2 m_lastPosition = glm::vec2(0.0f);
    glm::vec2 m_scale;
    float m_rotation;

    VertexBuffer m_vertexBuffer;
    IndexBuffer m_indexBuffer;
    VertexArray m_vertexArray;

  };
}
