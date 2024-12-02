#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

namespace SimpleGE
{
  void Component::Register(std::string_view type, const CreateFnType& createFn)
  {
    componentRegistry.emplace(type, createFn);
  }

  std::unique_ptr<Component> Component::Create(std::string_view type, Entity& owner)
  {
    Expects(componentRegistry.find((std::string) type) != componentRegistry.end());
    const auto& createFn = componentRegistry.at((std::string) type);
    return createFn(owner);
  }

  [[nodiscard]] Component* Component::FindComponent(std::string_view name)
  {
    const auto sep = name.find('.');
    auto entName = name.substr(0, sep);
    auto compName = name.substr(sep + 1);

    auto entity = Scene::FindObject(entName);
    if (entity == nullptr)
    {
      return {};
    }

    return entity->GetComponent(compName);
  }
} // namespace SimpleGE