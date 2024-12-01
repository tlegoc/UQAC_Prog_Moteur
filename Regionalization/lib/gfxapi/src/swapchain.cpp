#include <gfxapi/pch/precomp.h>

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  [[nodiscard]] static vk::SurfaceFormatKHR
  ChooseSwapSurfaceFormat(std::span<const vk::SurfaceFormatKHR> availableFormats)
  {
    for (const auto& availableFormat : availableFormats)
    {
      if (availableFormat.format == ColorFormat::eR8G8B8A8Unorm &&
          availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
      {
        return availableFormat;
      }
    }
    return gsl::at(availableFormats, 0);
  }

  [[nodiscard]] static vk::PresentModeKHR
  ChooseSwapPresentMode(std::span<const vk::PresentModeKHR> availablePresentModes)
  {
    if (std::find(availablePresentModes.begin(), availablePresentModes.end(), vk::PresentModeKHR::eMailbox) !=
        availablePresentModes.end())
    {
      return vk::PresentModeKHR::eMailbox;
    }

    return vk::PresentModeKHR::eFifo;
  }

  [[nodiscard]] static vk::Extent2D ChooseSwapExtent(const Window& window,
                                                     const vk::SurfaceCapabilitiesKHR& capabilities)
  {
    if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max())
    {
      return capabilities.currentExtent;
    }

    vk::Extent2D actualExtent = window.GetSize();

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
  }

  SwapChain::SwapChain(const SwapChainConfig& config) : device(config.device), window(config.window)
  {
    CreateSwapchain();
    CreateDepthResources();
    CreateFramebuffers();
    CreateCommandBuffers();
    CreateSyncObjects();
  }

  void SwapChain::CreateSwapchain()
  {
    const auto& vkDevice = device.GetNative();

    SwapChainSupportDetails swapChainSupport = device.GetAdapter().QuerySwapChainSupport(window);
    auto surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    auto presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    auto extent = ChooseSwapExtent(window, swapChainSupport.capabilities);

    std::uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
      imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo{
        .surface = window.GetNativeSurface(),
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .preTransform = swapChainSupport.capabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = nullptr,
    };

    QueueFamilyIndices indices = device.GetAdapter().FindQueueFamilies(window);
    auto queueFamilyIndices = std::array{indices.GetGraphicsFamily(), indices.GetPresentFamily()};

    if (indices.GetGraphicsFamily() != indices.GetPresentFamily())
    {
      createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
      createInfo.queueFamilyIndexCount = std::size(queueFamilyIndices);
      createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    else
    {
      createInfo.imageSharingMode = vk::SharingMode::eExclusive;
      createInfo.queueFamilyIndexCount = 0;
      createInfo.pQueueFamilyIndices = nullptr;
    }

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = {extent.width, extent.height};

    swapChain = vkDevice.createSwapchainKHR(createInfo);

    auto swapChainNativeImages = vkDevice.getSwapchainImagesKHR(swapChain);
    std::transform(swapChainNativeImages.begin(), swapChainNativeImages.end(), std::back_inserter(swapChainImages),
                   [this](vk::Image& image)
                   {
                     return Texture(SwapTextureConfig{
                         .device = device,
                         .size = swapChainExtent,
                         .image = image,
                         .format = swapChainImageFormat,
                         .aspectFlags = vk::ImageAspectFlagBits::eColor,
                     });
                   });
  }

  void SwapChain::CreateDepthResources()
  {
    depthBuffer = Texture::Create(DepthTextureConfig{
        .size = swapChainExtent,
    });
  }

  void SwapChain::CreateFramebuffers()
  {
    std::transform(swapChainImages.begin(), swapChainImages.end(), std::back_inserter(swapChainFramebuffers),
                   [this](auto& target)
                   {
                     auto colorAttachment = std::array{gsl::make_not_null(&target)};
                     return Framebuffer({
                         .colorAttachments = colorAttachment,
                         .depthAttachment = depthBuffer.get(),
                         .isSwapTarget = true,
                     });
                   });
  }

  SwapChain::SwapChain(SwapChain&& other) noexcept
      : device(other.device), window(other.window), swapChain(other.swapChain),
        swapChainImages(std::move(other.swapChainImages)), depthBuffer(std::move(other.depthBuffer)),
        swapChainFramebuffers(std::move(other.swapChainFramebuffers)), swapChainImageFormat(other.swapChainImageFormat),
        swapChainExtent(other.swapChainExtent), commandBuffers(std::move(other.commandBuffers)),
        imageAvailableSemaphores(std::move(other.imageAvailableSemaphores)),
        renderFinishedSemaphores(std::move(other.renderFinishedSemaphores)),
        inFlightFences(std::move(other.inFlightFences)), imageIndex(other.imageIndex), currentFrame(other.currentFrame)
  {
    other.swapChain = nullptr;
  }

  SwapChain::~SwapChain()
  {
    Cleanup();

    const auto& vkDevice = device.GetNative();

    for (gsl::index i = 0; i < MaxFramesInFlight; i++)
    {
      vkDevice.destroySemaphore(gsl::at(imageAvailableSemaphores, i));
      vkDevice.destroySemaphore(gsl::at(renderFinishedSemaphores, i));
      vkDevice.destroyFence(gsl::at(inFlightFences, i));
    }
  }

  void SwapChain::Cleanup()
  {
    const auto& vkDevice = device.GetNative();

    depthBuffer.reset();
    swapChainFramebuffers.clear();
    swapChainImages.clear();
    vkDevice.destroySwapchainKHR(swapChain);
  }

  SwapChain& SwapChain::operator=(SwapChain&& other) noexcept
  {
    Expects(&device == &other.device);
    Expects(&window == &other.window);
    swapChain = other.swapChain;
    swapChainImages = std::move(other.swapChainImages);
    depthBuffer = std::move(other.depthBuffer);
    swapChainFramebuffers = std::move(other.swapChainFramebuffers);
    swapChainImageFormat = other.swapChainImageFormat;
    swapChainExtent = other.swapChainExtent;
    commandBuffers = std::move(other.commandBuffers);
    imageAvailableSemaphores = std::move(other.imageAvailableSemaphores);
    renderFinishedSemaphores = std::move(other.renderFinishedSemaphores);
    inFlightFences = std::move(other.inFlightFences);
    imageIndex = other.imageIndex;
    currentFrame = other.currentFrame;
    other.swapChain = nullptr;
    return *this;
  }

  std::tuple<CommandBuffer&, Framebuffer&> SwapChain::BeginFrame()
  {
    const auto& vkDevice = device.GetNative();

    auto& inFlightFence = gsl::at(inFlightFences, currentFrame);

    std::ignore = vkDevice.waitForFences({inFlightFence}, VK_TRUE, UINT64_MAX);
    vk::Result result{};

    try
    {
      std::tie(result, imageIndex) =
          vkDevice.acquireNextImageKHR(swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], nullptr);
    }
    catch (vk::SystemError const& e)
    {
      result = static_cast<vk::Result>(e.code().value());
    }

    if (result == vk::Result::eErrorOutOfDateKHR)
    {
      RecreateSwapChain();
      return BeginFrame();
    }
    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
    {
      throw std::runtime_error(fmt::format("Failed to acquire swapchain image. Error {}", static_cast<int>(result)));
    }

    vkDevice.resetFences({inFlightFence});

    auto& commandBuffer = gsl::at(commandBuffers, currentFrame);
    auto& framebuffer = gsl::at(swapChainFramebuffers, imageIndex);

    const auto& vkCommandBuffer = commandBuffer.GetNative();

    vkCommandBuffer.reset({});

    vk::CommandBufferBeginInfo beginInfo{
        .flags = {},
    };

    vkCommandBuffer.begin(beginInfo);

    return std::tie(commandBuffer, framebuffer);
  }

  void SwapChain::EndFrame(CommandBuffer& commandBuffer)
  {
    Expects(&commandBuffer == &gsl::at(commandBuffers, currentFrame));
    const auto& vkCommandBuffer = commandBuffer.GetNative();

    vkCommandBuffer.end();

    auto waitSemaphores = std::array{gsl::at(imageAvailableSemaphores, currentFrame)};
    auto waitStages = std::array{vk::PipelineStageFlags{vk::PipelineStageFlagBits::eColorAttachmentOutput}};
    auto signalSemaphores = std::array{gsl::at(renderFinishedSemaphores, currentFrame)};

    vk::SubmitInfo submitInfo{
        .waitSemaphoreCount = std::size(waitSemaphores),
        .pWaitSemaphores = waitSemaphores.data(),
        .pWaitDstStageMask = waitStages.data(),
        .commandBufferCount = 1,
        .pCommandBuffers = &vkCommandBuffer,
        .signalSemaphoreCount = std::size(signalSemaphores),
        .pSignalSemaphores = signalSemaphores.data(),
    };

    device.GetGraphicsQueue().GetNative().submit({submitInfo}, gsl::at(inFlightFences, currentFrame));

    auto swapChains = std::array{swapChain};

    vk::PresentInfoKHR presentInfo{
        .waitSemaphoreCount = std::size(signalSemaphores),
        .pWaitSemaphores = signalSemaphores.data(),
        .swapchainCount = std::size(swapChains),
        .pSwapchains = swapChains.data(),
        .pImageIndices = &imageIndex,
        .pResults = nullptr,
    };

    vk::Result result{};

    try
    {
      result = device.GetPresentQueue().GetNative().presentKHR(presentInfo);
    }
    catch (vk::SystemError const& e)
    {
      result = static_cast<vk::Result>(e.code().value());
    }

    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR ||
        const_cast<Window&>(window).GetAndResetFramebufferResized())
    {
      RecreateSwapChain();
    }
    else if (result != vk::Result::eSuccess)
    {
      throw std::runtime_error("Failed to present swapchain image");
    }

    const auto& vkDevice = device.GetNative();
    vkDevice.waitIdle();

    currentFrame = (currentFrame + 1) % MaxFramesInFlight;
  }

  const Framebuffer& SwapChain::GetFramebuffer(gsl::index index) const { return gsl::at(swapChainFramebuffers, index); }

  void SwapChain::CreateCommandBuffers()
  {
    commandBuffers.reserve(MaxFramesInFlight);
    const auto& graphicsQueue = device.GetGraphicsQueue();

    for (std::size_t i = 0; i < MaxFramesInFlight; i++)
    {
      commandBuffers.emplace_back(CommandBufferConfig{.queue = graphicsQueue});
    }
  }

  void SwapChain::CreateSyncObjects()
  {
    const auto& vkDevice = device.GetNative();

    imageAvailableSemaphores.reserve(MaxFramesInFlight);
    renderFinishedSemaphores.reserve(MaxFramesInFlight);
    inFlightFences.reserve(MaxFramesInFlight);

    vk::SemaphoreCreateInfo semaphoreInfo{};
    vk::FenceCreateInfo fenceInfo{
        .flags = vk::FenceCreateFlagBits::eSignaled,
    };

    for (std::size_t i = 0; i < MaxFramesInFlight; i++)
    {
      imageAvailableSemaphores.emplace_back(vkDevice.createSemaphore(semaphoreInfo));
      renderFinishedSemaphores.emplace_back(vkDevice.createSemaphore(semaphoreInfo));
      inFlightFences.emplace_back(vkDevice.createFence(fenceInfo));
    }
  }

  void SwapChain::RecreateSwapChain()
  {
    const auto& vkDevice = device.GetNative();

    auto size = window.GetSize();
    while (size.width == 0 || size.height == 0)
    {
      size = window.GetSize();
      window.WaitEvents();
    }

    vkDevice.waitIdle();

    Cleanup();

    CreateSwapchain();
    CreateDepthResources();
    CreateFramebuffers();
  }
} // namespace SimpleGE::GFXAPI