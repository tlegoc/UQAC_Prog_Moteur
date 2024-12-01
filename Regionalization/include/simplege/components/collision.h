#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>

namespace SimpleGE
{
  class ColliderComponent;

  class CollisionComponent : public virtual Component
  {
  public:
    virtual void OnCollision(const ColliderComponent& other) = 0;
  };
} // namespace SimpleGE
