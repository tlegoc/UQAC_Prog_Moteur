#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/collider.h>
#include <simplege/entity.h>
#include <simplege/systems/system.h>

namespace SimpleGE
{
  class BVHNode
  {
  public:
    BVHNode* tl;
    BVHNode* tr;
    BVHNode* bl;
    BVHNode* br;
    std::unordered_set<gsl::not_null<ColliderComponent*>> colliders;
    glm::vec2 min, max;

    void Insert(gsl::not_null<ColliderComponent*> comp)
    {

    };

    void Remove(gsl::not_null<ColliderComponent*> comp)
    {

    };
  };

  class PhysicSystem : public ISystem
  {
  public:
    static PhysicSystem& Instance()
    {
      static PhysicSystem instance;
      return instance;
    }

    PhysicSystem(const PhysicSystem&) = delete;
    PhysicSystem(PhysicSystem&&) = delete;
    PhysicSystem& operator=(const PhysicSystem&) = delete;
    PhysicSystem& operator=(PhysicSystem&&) = delete;

    ~PhysicSystem() override { Expects(colliders.empty()); }

    void Register(gsl::not_null<ColliderComponent*> comp) { colliders.insert(comp); }

    void Unregister(gsl::not_null<ColliderComponent*> comp) { colliders.erase(comp); }

    void Iterate(const Timing& timing) override
    {
      std::vector<gsl::not_null<ColliderComponent*>> collidersVec;
      collidersVec.reserve(std::size(colliders));
      for (auto c : colliders)
      {
        if (c->Enabled() && c->Owner().Active())
        {
          collidersVec.push_back(c);
        }
      }
      std::vector<std::pair<gsl::not_null<ColliderComponent*>, gsl::not_null<ColliderComponent*>>> collisions;

      for (int i = 0; i < std::size(collidersVec); i++)
      {
        auto c1 = collidersVec[i];

        for (int j = i + 1; j < std::size(collidersVec); j++)
        {
          auto c2 = collidersVec[j];

          if ((bool) (c1->GetMask() & c2->GetMask()) && c1->Collides(*c2))
          {
            collisions.emplace_back(c1, c2);
          }
        }
      }

      for (auto col : collisions)
      {
        col.first->OnCollision(*col.second);
        col.second->OnCollision(*col.first);
      }
    }

  private:
    PhysicSystem() = default;

    std::unordered_set<gsl::not_null<ColliderComponent*>> colliders;
  };
} // namespace SimpleGE