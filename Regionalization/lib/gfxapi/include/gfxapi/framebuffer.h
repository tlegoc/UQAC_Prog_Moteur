#pragma once

#include <gfxapi/pch/precomp.h>

#include <gfxapi/device.h>

namespace SimpleGE::GFXAPI
{
  class Texture;

  struct FramebufferConfig
  {
    const Device& device{Device::GetMain()};
    std::span<gsl::not_null<Texture*>> colorAttachments;
    Texture* depthAttachment{};
    bool isSwapTarget{};
  };

  class Framebuffer
  {
  public:
    static std::unique_ptr<Framebuffer> Create(const FramebufferConfig& config)
    {
      return std::make_unique<Framebuffer>(config);
    }

    Framebuffer(const FramebufferConfig& config);
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&& other) noexcept;
    ~Framebuffer();

    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&& other) noexcept;

    [[nodiscard]] const Device& GetDevice() const { return device; }
    [[nodiscard]] const std::vector<gsl::not_null<Texture*>>& GetColorAttachments() const { return colorAttachments; }
    [[nodiscard]] const Texture* GetDepthAttachment() const { return depthAttachment; }
    [[nodiscard]] Size<std::uint32_t> GetSize() const;

    [[nodiscard]] const vk::Framebuffer& GetNative() const { return framebuffer; }
    [[nodiscard]] const vk::RenderPass& GetNativeRenderPass() const { return renderPass; }

  private:
    [[nodiscard]] std::vector<vk::AttachmentDescription> DescribeNativeAttachments(bool isSwapTarget) const;
    void CreateRenderPass(bool isSwapTarget);

    const Device& device;
    vk::Framebuffer framebuffer{};
    vk::RenderPass renderPass{};
    std::vector<gsl::not_null<Texture*>> colorAttachments;
    Texture* depthAttachment{};

    friend class Pipeline;
  };
} // namespace SimpleGE::GFXAPI