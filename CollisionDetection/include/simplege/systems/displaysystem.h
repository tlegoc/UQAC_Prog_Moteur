#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/eventtrigger.h>
#include <simplege/systems/system.h>

namespace SimpleGE
{
  class CameraComponent;
  class VisualComponent;

  struct DisplaySystemImpl;

  struct DisplaySystemConfig
  {
    std::string_view title;
    std::uint32_t width{};
    std::uint32_t height{};
  };

  class DisplaySystem : public ISystem
  {
  public:
    using CameraBeginEventType = std::function<void(const CameraComponent&)>;
    using CameraEndEventType = std::function<void(const CameraComponent&)>;

    static DisplaySystem& Instance()
    {
      static DisplaySystem instance;
      return instance;
    }

    static void SetConfig(const DisplaySystemConfig& config);

    DisplaySystem(const DisplaySystem&) = delete;
    DisplaySystem(DisplaySystem&&) = delete;
    DisplaySystem& operator=(const DisplaySystem&) = delete;
    DisplaySystem& operator=(DisplaySystem&&) = delete;
    ~DisplaySystem() override;

    void Register(gsl::not_null<CameraComponent*> comp);
    void Unregister(gsl::not_null<CameraComponent*> comp);
    void Register(gsl::not_null<VisualComponent*> comp);
    void Unregister(gsl::not_null<VisualComponent*> comp);

    void Iterate(const Timing& timing) override;

    [[nodiscard]] GFXAPI::GFXInstance& GetGFXInstance();
    [[nodiscard]] GFXAPI::Window& GetWindow();
    [[nodiscard]] GFXAPI::SwapChain& GetSwapChain();

    EventTrigger<CameraBeginEventType>& GetCameraBeginEvent();
    EventTrigger<CameraEndEventType>& GetCameraEndEvent();

  private:
    DisplaySystem();

    std::unique_ptr<DisplaySystemImpl> impl;
  };
} // namespace SimpleGE