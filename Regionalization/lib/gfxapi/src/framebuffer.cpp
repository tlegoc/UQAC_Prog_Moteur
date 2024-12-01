#include <gfxapi/pch/precomp.h>

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  Framebuffer::Framebuffer(const FramebufferConfig& config)
      : device(config.device), colorAttachments(config.colorAttachments.begin(), config.colorAttachments.end()),
        depthAttachment(config.depthAttachment)
  {
    const auto& vkDevice = device.GetNative();

    std::vector<vk::ImageView> attachments;
    attachments.reserve(std::size(colorAttachments) + 1);

    std::transform(colorAttachments.begin(), colorAttachments.end(), std::back_inserter(attachments),
                   [](const auto& target) { return target->GetNativeView(); });
    if (depthAttachment != nullptr)
    {
      attachments.emplace_back(depthAttachment->GetNativeView());
    }

    CreateRenderPass(config.isSwapTarget);

    vk::FramebufferCreateInfo framebufferInfo{
        .renderPass = renderPass,
        .attachmentCount = static_cast<std::uint32_t>(std::size(attachments)),
        .pAttachments = attachments.data(),
        .width = GetSize().width,
        .height = GetSize().height,
        .layers = 1,
    };

    framebuffer = vkDevice.createFramebuffer(framebufferInfo);
  }

  Framebuffer::Framebuffer(Framebuffer&& other) noexcept
      : device(other.device), framebuffer(other.framebuffer), renderPass(other.renderPass),
        colorAttachments(std::move(other.colorAttachments)), depthAttachment(other.depthAttachment)
  {
    other.framebuffer = nullptr;
    other.renderPass = nullptr;
    other.depthAttachment = nullptr;
  }

  Framebuffer::~Framebuffer()
  {
    const auto& vkDevice = device.GetNative();

    vkDevice.destroyFramebuffer(framebuffer);
    vkDevice.destroyRenderPass(renderPass);
  }

  Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
  {
    Expects(&device == &other.device);
    this->~Framebuffer();
    framebuffer = other.framebuffer;
    renderPass = other.renderPass;
    colorAttachments = std::move(other.colorAttachments);
    depthAttachment = other.depthAttachment;
    other.framebuffer = nullptr;
    other.renderPass = nullptr;
    other.depthAttachment = nullptr;
    return *this;
  }

  Size<std::uint32_t> Framebuffer::GetSize() const { return gsl::at(colorAttachments, 0)->GetSize(); }

  std::vector<vk::AttachmentDescription> Framebuffer::DescribeNativeAttachments(bool isSwapTarget) const
  {
    std::vector<vk::AttachmentDescription> descr;
    descr.reserve(std::size(colorAttachments) + 1);

    std::transform(colorAttachments.begin(), colorAttachments.end(), std::back_inserter(descr),
                   [isSwapTarget](const auto& target)
                   {
                     return vk::AttachmentDescription{
                         .format = target->GetColorFormat(),
                         .samples = vk::SampleCountFlagBits::e1,
                         .loadOp = vk::AttachmentLoadOp::eClear,
                         .storeOp = vk::AttachmentStoreOp::eStore,
                         .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
                         .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                         .initialLayout = vk::ImageLayout::eUndefined,
                         .finalLayout =
                             isSwapTarget ? vk::ImageLayout::ePresentSrcKHR : vk::ImageLayout::eShaderReadOnlyOptimal,
                     };
                   });

    if (depthAttachment != nullptr)
    {
      descr.emplace_back(vk::AttachmentDescription{
          .format = depthAttachment->GetColorFormat(),
          .samples = vk::SampleCountFlagBits::e1,
          .loadOp = vk::AttachmentLoadOp::eClear,
          .storeOp = vk::AttachmentStoreOp::eDontCare,
          .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
          .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
          .initialLayout = vk::ImageLayout::eUndefined,
          .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
      });
    }

    return descr;
  }

  void Framebuffer::CreateRenderPass(bool isSwapTarget)
  {
    const auto& vkDevice = device.GetNative();

    auto nbColorTargets = std::size(colorAttachments);

    std::vector<vk::AttachmentReference> colorAttachmentRefs;
    colorAttachmentRefs.reserve(nbColorTargets);
    for (std::uint32_t i = 0; i < nbColorTargets; i++)
    {
      colorAttachmentRefs.emplace_back(vk::AttachmentReference{
          .attachment = i,
          .layout = vk::ImageLayout::eColorAttachmentOptimal,
      });
    }
    vk::AttachmentReference depthAttachmentRef{
        .attachment = static_cast<std::uint32_t>(nbColorTargets),
        .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    vk::SubpassDescription subpass{
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
        .colorAttachmentCount = static_cast<std::uint32_t>(nbColorTargets),
        .pColorAttachments = colorAttachmentRefs.data(),
        .pDepthStencilAttachment = (depthAttachment != nullptr) ? &depthAttachmentRef : nullptr,
    };

    vk::SubpassDependency dependency{
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask =
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .dstStageMask =
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .srcAccessMask = {},
        .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
    };

    if (depthAttachment != nullptr)
    {
      dependency.dstAccessMask |= vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    }

    auto attachments{DescribeNativeAttachments(isSwapTarget)};

    vk::RenderPassCreateInfo renderPassInfo{
        .attachmentCount = static_cast<std::uint32_t>(std::size(attachments)),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };

    renderPass = vkDevice.createRenderPass(renderPassInfo);
  }
} // namespace SimpleGE::GFXAPI
