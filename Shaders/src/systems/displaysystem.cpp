#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

namespace SimpleGE
{
#ifdef NDEBUG
  static constexpr auto EnableValidation = false;
#else
  static constexpr auto EnableValidation = true;
#endif

  class DisplaySystemImpl
  {
  public:
    static void SetConfig(const DisplaySystemConfig& config) { DisplaySystemImpl::config = config; }

    DisplaySystemImpl()
        : gfxInstance(GFXAPI::GFXInstance::Create({
              .enableValidation = EnableValidation,
              .title = config.title,
          }))
    {
      window = GFXAPI::Window::Create({
          .instance = *gfxInstance,
          .size =
              {
                  .width = config.width,
                  .height = config.height,
              },
          .title = config.title,
      });
      auto adapterInfos = gfxInstance->ListAdapters(*window);
      Expects(!adapterInfos.empty());
      adapter = GFXAPI::Adapter::Create({*gfxInstance, adapterInfos.front()});
      device = GFXAPI::Device::Create({
          .adapter = *adapter,
          .window = *window,
      });
      GFXAPI::Device::SetMain(device.get());
      swapChain = GFXAPI::SwapChain::Create({.window = *window});
    }

    ~DisplaySystemImpl()
    {
      Graphic::BaseShaderProgram::Shutdown();
      GFXAPI::Device::SetMain(nullptr);
      Expects(cameras.empty());
      Expects(visuals.empty());
    }

    void Register(gsl::not_null<CameraComponent*> comp) { cameras.insert(comp); }

    void Unregister(gsl::not_null<CameraComponent*> comp) { cameras.erase(comp); }

    void Register(gsl::not_null<VisualComponent*> comp) { visuals.insert(comp); }

    void Unregister(gsl::not_null<VisualComponent*> comp) { visuals.erase(comp); }

    void Iterate(const Timing& timing)
    {
      std::vector<gsl::not_null<VisualComponent*>> sortedVisuals(visuals.begin(), visuals.end());
      std::sort(sortedVisuals.begin(), sortedVisuals.end(),
                [](gsl::not_null<VisualComponent*> a, gsl::not_null<VisualComponent*> b)
                {
                  return a->Owner().GetComponent<PositionComponent>()->WorldPosition()[2] <
                         b->Owner().GetComponent<PositionComponent>()->WorldPosition()[2];
                });

      for (const auto& cameraComp : cameras)
      {
        if (cameraComp->Enabled() && cameraComp->Owner().Active())
        {
          cameraComp->BeginRender(timing);
          cameraBeginEvent.Trigger(*cameraComp);

          for (const auto& visualComp : sortedVisuals)
          {
            if (visualComp->Enabled() && visualComp->Owner().Active())
            {
              visualComp->Display(*cameraComp, timing);
            }
          }

          cameraEndEvent.Trigger(*cameraComp);
          cameraComp->EndRender(timing);
        }
      }
    }

    [[nodiscard]] GFXAPI::GFXInstance& GetGFXInstance()
    {
      Expects(gfxInstance);
      return *gfxInstance;
    }

    [[nodiscard]] GFXAPI::Window& GetWindow()
    {
      Expects(window);
      return *window;
    }

    [[nodiscard]] GFXAPI::SwapChain& GetSwapChain()
    {
      Expects(swapChain);
      return *swapChain;
    }

    EventTrigger<DisplaySystem::CameraBeginEventType>& GetCameraBeginEvent() { return cameraBeginEvent; }
    EventTrigger<DisplaySystem::CameraEndEventType>& GetCameraEndEvent() { return cameraEndEvent; }

  private:
    static inline DisplaySystemConfig config{
        .title = "SimpleGE",
    };

    std::unique_ptr<GFXAPI::GFXInstance> gfxInstance;
    std::unique_ptr<GFXAPI::Window> window;
    std::unique_ptr<GFXAPI::Adapter> adapter;
    std::unique_ptr<GFXAPI::Device> device;
    std::unique_ptr<GFXAPI::SwapChain> swapChain;

    std::unordered_set<gsl::not_null<CameraComponent*>> cameras;
    std::unordered_set<gsl::not_null<VisualComponent*>> visuals;

    EventTrigger<DisplaySystem::CameraBeginEventType> cameraBeginEvent;
    EventTrigger<DisplaySystem::CameraEndEventType> cameraEndEvent;
  };

  void DisplaySystem::SetConfig(const DisplaySystemConfig& config) { DisplaySystemImpl::SetConfig(config); }

  DisplaySystem::DisplaySystem() : impl(std::make_unique<DisplaySystemImpl>()) {}

  DisplaySystem::~DisplaySystem() = default;

  void DisplaySystem::Register(gsl::not_null<CameraComponent*> comp) { impl->Register(comp); }

  void DisplaySystem::Unregister(gsl::not_null<CameraComponent*> comp) { impl->Unregister(comp); }

  void DisplaySystem::Register(gsl::not_null<VisualComponent*> comp) { impl->Register(comp); }

  void DisplaySystem::Unregister(gsl::not_null<VisualComponent*> comp) { impl->Unregister(comp); }

  void DisplaySystem::Iterate(const Timing& timing) { impl->Iterate(timing); }

  GFXAPI::GFXInstance& DisplaySystem::GetGFXInstance() { return impl->GetGFXInstance(); }

  GFXAPI::Window& DisplaySystem::GetWindow() { return impl->GetWindow(); }

  GFXAPI::SwapChain& DisplaySystem::GetSwapChain() { return impl->GetSwapChain(); }

  EventTrigger<DisplaySystem::CameraBeginEventType>& DisplaySystem::GetCameraBeginEvent()
  {
    return impl->GetCameraBeginEvent();
  }
  EventTrigger<DisplaySystem::CameraEndEventType>& DisplaySystem::GetCameraEndEvent()
  {
    return impl->GetCameraEndEvent();
  }

} // namespace SimpleGE