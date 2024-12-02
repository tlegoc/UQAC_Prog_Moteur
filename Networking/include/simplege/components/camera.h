#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/components/component.h>
#include <simplege/math.h>
#include <simplege/timing.h>

namespace SimpleGE
{
  namespace GFXAPI
  {
    class Window;
  }

  class CompositorComponent;

  class CameraComponent : public Component
  {
  public:
    static constexpr auto Type = "Camera";

    struct Description
    {
      std::string title;
      std::uint32_t viewHeight;
      std::uint32_t viewWidth;
      GFXAPI::ColorFloat color;
      float height;
      float nearClip;
      float farClip;
      std::vector<ComponentReference<CompositorComponent>> compositors;
    };

    static CameraComponent* Current() { return current; }

    CameraComponent(Entity& owner);
    ~CameraComponent() override;

    void BeginRender(const Timing& timing);
    void EndRender(const Timing& timing);
    [[nodiscard]] GFXAPI::CommandBuffer& GetCurrentCommandBuffer() const { return *gsl::make_not_null(commandBuffer); }
    [[nodiscard]] const GFXAPI::Framebuffer& GetCurrentFramebuffer() const { return *gsl::make_not_null(framebuffer); }
    [[nodiscard]] const GFXAPI::Framebuffer& GetRTTFramebuffer() const
    {
      return *gsl::make_not_null(rttFramebuffer.get());
    }

    SetupResult Setup(const nlohmann::json& descr) override;
    SetupResult Setup(const Description& descr);

    [[nodiscard]] glm::mat4 Projection() const;

    [[nodiscard]] GFXAPI::Size<std::uint32_t> ViewSize() const { return viewSize; }

    [[nodiscard]] const GFXAPI::ColorFloat& GetClearColor() const { return clearColor; }

  private:
    static inline CameraComponent* current;

    std::unique_ptr<GFXAPI::Texture> colorTarget;
    std::unique_ptr<GFXAPI::Texture> depthTarget;
    std::unique_ptr<GFXAPI::Framebuffer> rttFramebuffer;
    GFXAPI::Framebuffer* framebuffer{};
    GFXAPI::CommandBuffer* commandBuffer{};
    GFXAPI::Size<std::uint32_t> viewSize{};
    float height{};
    float nearClip{};
    float farClip{};
    GFXAPI::ColorFloat clearColor{};
    std::vector<ComponentReference<CompositorComponent>> compositors;
  };
} // namespace SimpleGE
