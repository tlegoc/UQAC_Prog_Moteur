#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

using json = nlohmann::json;

#ifdef WIN32
extern "C"
{
  __declspec(dllexport) DWORD NvOptimusEnablement = 1;
  __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace SimpleGE
{
  static void from_json(const json& j, CameraComponent::Description& desc)
  {
    j.at("title").get_to(desc.title);
    j.at("viewHeight").get_to(desc.viewHeight);
    j.at("viewWidth").get_to(desc.viewWidth);
    j.at("color").get_to(desc.color);
    j.at("height").get_to(desc.height);
    j.at("near").get_to(desc.nearClip);
    j.at("far").get_to(desc.farClip);
    j.at("compositors").get_to(desc.compositors);
  }

  CameraComponent::CameraComponent(Entity& owner) : Component(owner) { DisplaySystem::Instance().Register(this); }

  CameraComponent::~CameraComponent() { DisplaySystem::Instance().Unregister(this); }

  Component::SetupResult CameraComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult CameraComponent::Setup(const Description& descr)
  {
    current = this;

    viewSize = {
        .width = descr.viewWidth,
        .height = descr.viewHeight,
    };
    height = descr.height;
    nearClip = descr.nearClip;
    farClip = descr.farClip;
    clearColor = descr.color;

    compositors = std::move(descr.compositors);

    colorTarget = GFXAPI::Texture::Create({
        .size = viewSize,
        .usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
    });
    depthTarget = GFXAPI::Texture::Create(GFXAPI::DepthTextureConfig{
        .size = viewSize,
    });
    auto colorTargets = std::array{gsl::make_not_null(colorTarget.get())};
    rttFramebuffer = GFXAPI::Framebuffer::Create({
        .colorAttachments = colorTargets,
        .depthAttachment = depthTarget.get(),
    });

    return {this, {}};
  }

  void CameraComponent::BeginRender(const Timing& timing)
  {
    current = this;
    auto [commandBufferRef, framebufferRef] = DisplaySystem::Instance().GetSwapChain().BeginFrame();
    commandBuffer = &commandBufferRef;
    framebuffer = &framebufferRef;

    commandBuffer->BeginRenderPass({
        .target = *rttFramebuffer,
        .clearColor = clearColor,
    });
  }

  void CameraComponent::EndRender(const Timing& timing)
  {
    auto& window = DisplaySystem::Instance().GetWindow();
    auto& swapChain = DisplaySystem::Instance().GetSwapChain();

    commandBuffer->EndRenderPass();

    auto* currentFramebuffer = rttFramebuffer.get();
    for (auto& comp : compositors)
    {
      if (comp->Enabled())
      {
        currentFramebuffer = comp->Compose(*this, *currentFramebuffer);
      }
    }

    swapChain.EndFrame(*commandBuffer);
    commandBuffer = nullptr;
    framebuffer = nullptr;

    window.Present(swapChain);

    if (window.IsClosed())
    {
      Game::Close();
    }
  }

  glm::mat4 CameraComponent::Projection() const
  {
    gsl::not_null<PositionComponent*> posComp = Owner().GetComponent<PositionComponent>();
    auto position = posComp->WorldPosition();
    float x = position[0];
    float y = position[1];
    float z = position[2];
    float ratio = float(viewSize.width) / float(viewSize.height);
    float width = ratio * height;

    return glm::ortho(x - width, x + width, -y - height, -y + height, z + nearClip, z + farClip);
  }
} // namespace SimpleGE
