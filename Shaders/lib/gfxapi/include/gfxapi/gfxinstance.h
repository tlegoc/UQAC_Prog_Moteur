#pragma once

#include <gfxapi/pch/precomp.h>

namespace SimpleGE::GFXAPI
{
  static constexpr auto ValidationLayers = std::array{"VK_LAYER_KHRONOS_validation"};

  class Window;

  struct GFXInstanceConfig
  {
    bool enableValidation;
    std::string_view title;
  };

  struct AdapterInfos
  {
    vk::PhysicalDevice physicalDevice;
    std::string_view name;
    int score{-1};
  };

  class GFXInstance
  {
  public:
    static std::unique_ptr<GFXInstance> Create(const GFXInstanceConfig& config)
    {
      return std::make_unique<GFXInstance>(config);
    }

    GFXInstance(const GFXInstanceConfig& config);
    GFXInstance(const GFXInstance&) = delete;
    GFXInstance(GFXInstance&& other) noexcept;
    ~GFXInstance();

    GFXInstance& operator=(const GFXInstance&) = delete;
    GFXInstance& operator=(GFXInstance&& other) noexcept;

    [[nodiscard]] std::vector<AdapterInfos> ListAdapters(const Window& window) const;

    [[nodiscard]] bool IsValidationEnabled() const { return enableValidation; }

    [[nodiscard]] const vk::Instance& GetNative() const { return vkInstance; }

  private:
    vk::Instance vkInstance;
    bool enableValidation;
  };
} // namespace SimpleGE::GFXAPI
