#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

namespace SimpleGE
{
  static std::vector<gsl::not_null<SpriteComponent*>> ListSprites(const EntityRef& owner)
  {
    std::queue<EntityRef> queue;
    queue.push(owner);

    std::vector<gsl::not_null<SpriteComponent*>> sprites;

    while (!queue.empty())
    {
      auto node = queue.front();
      queue.pop();

      for (const auto& child : node->GetChildren())
      {
        if (child->Active())
        {
          queue.push(child);
        }

        auto* comp = child->GetComponent<SpriteComponent>();
        if (comp != nullptr)
        {
          sprites.emplace_back(comp);
        }
      }
    }

    return sprites;
  }

  void LayerComponent::Display(const CameraComponent& camera, const Timing& timing)
  {
    auto layerSprites = ListSprites(Owner().shared_from_this());
    if (layerSprites.empty())
    {
      return;
    }

    auto spriteSheet = gsl::at(layerSprites, 0)->GetSpriteSheet();
    Ensures(spriteSheet.Ready());
  }
} // namespace SimpleGE