#include "PhysicsEngine.h"

#include "../Renderer/Sprite2D.h"

std::vector<std::shared_ptr<RenderEngine::Sprite2D>> PhysicsEngine::m_dynamicObjects;

bool PhysicsEngine::pointVsRect(const glm::vec2& p, const Rectangle& r)
{
  return (p.x >= r.position.x && p.x < r.position2.x && p.y >= r.position.y && p.y < r.position2.y);
}

bool PhysicsEngine::RectVsRect(const Rectangle& r1, const Rectangle& r2)
{
  return (r1.position.x < r2.position2.x && r1.position2.x >= r2.position.x && r1.position.y < r2.position2.y && r1.position2.y >= r2.position.y);
}

void PhysicsEngine::init()
{

}

void PhysicsEngine::shutdown()
{
  m_dynamicObjects.clear();
}

void PhysicsEngine::update(const double deltaTime)
{
  for (std::shared_ptr<RenderEngine::Sprite2D> obj1 : m_dynamicObjects)
  {
    for (std::shared_ptr<RenderEngine::Sprite2D> obj2 : m_dynamicObjects)
    {
      if (obj1 == obj2)
      {
        continue;
      }

      if (RectVsRect(
        {obj1->getTargetPosition(), glm::vec2(obj1->getTargetPosition().x + (obj1->getScale().x * SPRITE_SCALE_TO_WORLD_X),
        obj1->getPosition().y + (obj1->getScale().y * SPRITE_SCALE_TO_WORLD_Y))},
        {obj2->getPosition(), glm::vec2(obj2->getPosition().x + (obj2->getScale().x * SPRITE_SCALE_TO_WORLD_X),
        obj2->getPosition().y + (obj2->getScale().y * SPRITE_SCALE_TO_WORLD_Y))}
      ))
      {
        obj1->setTargetPosition(obj1->getPosition());
        //std::cout << "collision!\n";
      } else {
        obj1->setPosition(obj1->getTargetPosition());
      }
    }
  }
}

std::shared_ptr<RenderEngine::Sprite2D> PhysicsEngine::mouseRaycast(const double xpos, const double ypos)
{
  for (std::shared_ptr<RenderEngine::Sprite2D> obj : m_dynamicObjects)
  {
    if (pointVsRect(glm::vec2(xpos * SCR_COORD_TO_WORLD_X, ypos * SCR_COORD_TO_WORLD_Y),
      {obj->getPosition(), glm::vec2(obj->getPosition().x + (obj->getScale().x * SPRITE_SCALE_TO_WORLD_X),
        obj->getPosition().y + (obj->getScale().y * SPRITE_SCALE_TO_WORLD_Y))}
    ))
    {
      return obj;
    }
  }
  return nullptr;
}

void PhysicsEngine::addDynamicObject(std::shared_ptr<RenderEngine::Sprite2D>& object)
{
  m_dynamicObjects.push_back(object);
}

