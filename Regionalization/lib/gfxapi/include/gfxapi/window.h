#pragma once

#include <gfxapi/pch/precomp.h>

namespace SimpleGE::GFXAPI
{
  enum class KeyCode;

  class Device;
  class GFXInstance;
  class SwapChain;

  struct WindowConfig
  {
    const GFXInstance& instance;
    Size<std::uint32_t> size{};
    std::string_view title{};
  };

  class Window
  {
  public:
    static std::unique_ptr<Window> Create(const WindowConfig& config) { return std::make_unique<Window>(config); }

    Window(const WindowConfig& config);
    Window(const Window&) = delete;
    Window(Window&& other) noexcept;
    ~Window();

    Window& operator=(const Window&) = delete;
    Window& operator=(Window&& other) noexcept;

    [[nodiscard]] bool IsClosed() const;
    [[nodiscard]] bool IsKeyDown(KeyCode key) const;
    void Present(SwapChain& swapChain);
    void WaitEvents() const;

    [[nodiscard]] Size<std::uint32_t> GetSize() const;

    [[nodiscard]] GLFWwindow& GetNativeWindow() const { return *gsl::not_null<GLFWwindow*>(window); }
    [[nodiscard]] const vk::SurfaceKHR& GetNativeSurface() const { return surface; }
    [[nodiscard]] bool GetAndResetFramebufferResized()
    {
      bool isSet = framebufferResized;
      framebufferResized = false;
      return isSet;
    }

  private:
    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

    const GFXInstance& instance;
    GLFWwindow* window{};
    vk::SurfaceKHR surface{};
    bool framebufferResized{};
  };
} // namespace SimpleGE::GFXAPI
