#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  class Entity;

  class Component
  {
  public:
    using CreateFnType = std::function<std::unique_ptr<Component>(Entity&)>;

    struct SetupStep
    {
      std::function<bool()> ready = []() { return true; };
      std::function<void()> execute = []() {};
    };
    struct SetupResult
    {
      SetupResult(gsl::not_null<Component*> comp, std::initializer_list<SetupStep> steps) : comp(comp), steps(steps)
      {
        if (IsDone())
        {
          comp->SetReady();
        }
      }

      SetupResult(SetupResult&& other) noexcept
          : comp(other.comp), steps(std::move(other.steps)), current(other.current)
      {
        other.current = 0;
      }

      ~SetupResult() { Ensures(IsDone()); }

      void Append(std::initializer_list<SetupStep> nextSteps)
      {
        std::copy(nextSteps.begin(), nextSteps.end(), std::back_inserter(steps));
      }

      [[nodiscard]] bool IsDone() const { return current == std::size(steps); }

      [[nodiscard]] bool Ready() const { return steps[current].ready(); }

      void Execute()
      {
        for (; !IsDone() && Ready(); current++)
        {
          steps[current].execute();
        }
        if (IsDone())
        {
          comp->SetReady();
        }
      }

    private:
      gsl::not_null<Component*> comp;
      std::vector<SetupStep> steps;
      int current{};
    };

    [[nodiscard]] static bool Resolve(std::vector<SetupResult>& pending)
    {
      while (!pending.empty())
      {
        std::vector<SetupResult> incomplete;
        incomplete.reserve(std::size(pending));
        bool anyReady{};
        for (auto& item : pending)
        {
          if (item.IsDone())
          {
            continue;
          }
          if (item.Ready())
          {
            anyReady = true;
            item.Execute();
          }
          if (!item.IsDone())
          {
            incomplete.emplace_back(std::move(item));
          }
        }
        pending = std::move(incomplete);
        if (!anyReady)
        {
          break;
        }
      }
      return pending.empty();
    }

    template<typename T>
    static void Register()
    {
      Register(T::Type, [](Entity& owner) { return std::make_unique<T>(owner); });
    }

    static std::unique_ptr<Component> Create(std::string_view type, Entity& owner);

    template<typename T>
    [[nodiscard]] static T* FindComponent(std::string_view name)
    {
      return dynamic_cast<T*>(FindComponent(name));
    }

    [[nodiscard]] static Component* FindComponent(std::string_view name);

    Component(Entity& owner) : owner(owner) {}
    virtual ~Component() = default;

    [[nodiscard]] virtual SetupResult Setup(const nlohmann::json& descr) { return {this, {}}; }

    [[nodiscard]] bool Enabled() const { return enabled; }
    [[nodiscard]] Entity& Owner() const { return owner; }
    [[nodiscard]] bool Ready() const { return ready; }

    void SetEnabled(bool enabled)
    {
      if (enabled)
      {
        Enable();
      }
      else
      {
        Disable();
      }
    }

    void Enable()
    {
      if (!enabled)
      {
        enabled = true;
        OnEnabled();
      }
    }
    void Disable()
    {
      if (enabled)
      {
        enabled = false;
        OnDisabled();
      }
    }

  protected:
    virtual void OnEnabled() {}
    virtual void OnDisabled() {}

  private:
    static void Register(std::string_view type, const CreateFnType& createFn);

    void SetReady() { ready = true; }

    static inline std::unordered_map<std::string, Component::CreateFnType> componentRegistry;

    bool enabled{true};
    bool ready{};
    Entity& owner;
  };

  template<typename T>
  struct ComponentReference
  {
  public:
    ComponentReference() = default;

    ComponentReference(T& ref) : resolved(&ref) {}

    ComponentReference(std::string_view name) : name(name) {}

    [[nodiscard]] bool IsEmpty() const { return resolved == nullptr && name.empty(); }

    T* Resolve() const
    {
      if (!resolved)
      {
        resolved = Component::FindComponent<T>(name);
      }
      return resolved;
    }

    bool Ready() const
    {
      const auto* target = Resolve();
      return target != nullptr && target->Ready();
    }

    ComponentReference& operator=(std::string_view name)
    {
      resolved = nullptr;
      this->name = name;
      return *this;
    }

    gsl::not_null<T&> operator*() const { return *Resolve(); }

    T* operator->() const { return Resolve(); }

    template<typename U>
    bool operator==(U other) const
    {
      return Resolve() == other;
    }

    template<typename U>
    bool operator!=(U other) const
    {
      return Resolve() != other;
    }

  private:
    std::string name;
    mutable T* resolved{};
  };

  template<typename T>
  static void from_json(const nlohmann::json& j, ComponentReference<T>& ref)
  {
    ref = j.get<std::string>();
  }
} // namespace SimpleGE