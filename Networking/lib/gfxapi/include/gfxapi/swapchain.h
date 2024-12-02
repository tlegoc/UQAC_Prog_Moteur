#pragma once

#include <gfxapi/pch/precomp.h>

#include <gfxapi/device.h>

namespace SimpleGE::GFXAPI
{
  class CommandBuffer;
  class Framebuffer;
  class Texture;
  class Window;

  struct SwapChainConfig
  {
    const Device& device{Device::GetMain()};
    const Window& window;
  };

  class SwapChain
  {
  public:
    static constexpr auto MaxFramesInFlight = 2;

    static std::unique_ptr<SwapChain> Create(const SwapChainConfig& config)
    {
      return std::make_unique<SwapChain>(config);
    }

    SwapChain(const SwapChainConfig& config);
    SwapChain(const SwapChain&) = delete;
    SwapChain(SwapChain&& other) noexcept;
    ~SwapChain();

    SwapChain& operator=(const SwapChain&) = delete;
    SwapChain& operator=(SwapChain&& other) noexcept;

    [[nodiscard]] std::tuple<CommandBuffer&, Framebuffer&> BeginFrame();
    void EndFrame(CommandBuffer& commandBuffer);

    [[nodiscard]] const Device& GetDevice() const { return device; }
    [[nodiscard]] ColorFormat GetColorFormat() const { return swapChainImageFormat; }
    [[nodiscard]] Size<std::uint32_t> GetSize() const { return swapChainExtent; }
    [[nodiscard]] const Framebuffer& GetFramebuffer(gsl::index index) const;

  private:
    void CreateSwapchain();
    void CreateDepthResources();
    void CreateFramebuffers();
    void CreateCommandBuffers();
    void CreateSyncObjects();
    void RecreateSwapChain();
    void Cleanup();

    const Device& device;
    const Window& window;
    vk::SwapchainKHR swapChain;
    std::vector<Texture> swapChainImages;
    std::unique_ptr<GFXAPI::Texture> depthBuffer{};
    std::vector<Framebuffer> swapChainFramebuffers;
    ColorFormat swapChainImageFormat;
    Size<std::uint32_t> swapChainExtent;

    std::vector<CommandBuffer> commandBuffers;
    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> inFlightFences;
    std::uint32_t imageIndex{};
    gsl::index currentFrame{};
  };
} // namespace SimpleGE::GFXAPI
