#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>
#include <simplege/math.h>

namespace SimpleGE
{
  class PositionComponent : public Component
  {
  public:
    static constexpr auto Type = "Position";

    struct Description
    {
      float x;
      float y;
      float z;
    };

    PositionComponent(Entity& owner) : Component(owner) {}

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    [[nodiscard]] const Point<3>& LocalPosition() const { return localPosition; }
    [[nodiscard]] Point<3> WorldPosition() const;

    void Translate(const Vector<3>& delta) { localPosition += delta; }

    template<std::size_t W>
    void Clamp(const Point<W>& min, const Point<W>& max)
    {
      for (int i = 0; i < W; i++)
      {
        if (localPosition[i] < min[i])
        {
          localPosition[i] = min[i];
        }
        if (localPosition[i] > max[i])
        {
          localPosition[i] = max[i];
        }
      }
    }

  private:
    Point<3> localPosition{};
  };
} // namespace SimpleGE
