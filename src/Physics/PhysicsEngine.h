#pragma once

#include <vector>
#include <memory>
#include <glm/vec2.hpp>

constexpr const double SCR_COORD_TO_WORLD_X = 640.0f;
constexpr const double SCR_COORD_TO_WORLD_Y = 480.0f;
constexpr const double SPRITE_SCALE_TO_WORLD_X = 0.078f / 50.f * SCR_COORD_TO_WORLD_X;
constexpr const double SPRITE_SCALE_TO_WORLD_Y = 0.1f / 50.f * SCR_COORD_TO_WORLD_Y;

constexpr const float G=1.0f;
constexpr const float airForce = 0.1f;

namespace RenderEngine
{
  class Sprite2D;
}

class PhysicsEngine
{
public:

  struct Rectangle
  {
    glm::vec2 position;
    glm::vec2 position2;
  };

  PhysicsEngine() = delete;

  static void init();
  static void shutdown();

  static void update(const double deltaTime);

  static void addDynamicObject(std::shared_ptr<RenderEngine::Sprite2D>& object);

  //uses mouse x and y positions between 0 and 1
  static std::shared_ptr<RenderEngine::Sprite2D> mouseRaycast(const double xpos, const double ypos);

private:

  static bool pointVsRect(const glm::vec2& p, const Rectangle& r);
  static bool RectVsRect(const Rectangle& r1, const Rectangle& r2);

  static std::vector<std::shared_ptr<RenderEngine::Sprite2D>> m_dynamicObjects;

};
