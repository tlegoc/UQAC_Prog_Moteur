#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

namespace SimpleGE
{
  void Entity::AddChild(std::string_view name, const EntityRef& child)
  {
    Expects(child->parent.lock() == nullptr);
    children.push_back(child);
    childrenByName[std::string{name}] = child;
    child->parent = weak_from_this();
  }

  void Entity::RemoveChild(EntityRef child)
  {
    Expects(child->GetParent().get() == this);
    children.erase(std::remove(children.begin(), children.end(), child), children.end());
    child->parent.reset();
    EntitySystem::Remove(child);
  }

  gsl::not_null<Component*> Entity::AddComponent(std::string_view type, const nlohmann::json& desc,
                                                 std::vector<Component::SetupResult>& pendingSetup)
  {
    auto result = components.emplace(type, Component::Create(type, *this));
    auto iterator = result.first;
    gsl::not_null<Component*> comp = iterator->second.get();

    pendingSetup.emplace_back(comp->Setup(desc));

    return comp;
  }

  void Entity::Clear()
  {
    for (const auto& c : children)
    {
      EntitySystem::Remove(c);
    }

    childrenByName.clear();
    children.clear();
    components.clear();
  }
} // namespace SimpleGE