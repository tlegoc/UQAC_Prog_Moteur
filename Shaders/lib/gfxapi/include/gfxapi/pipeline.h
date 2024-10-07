#pragma once

#include <gfxapi/pch/precomp.h>

namespace SimpleGE::GFXAPI
{
  class DescriptorSetLayout;
  class Framebuffer;
  class Shader;
  class SwapChain;

  struct VertexAttribute
  {
    ColorFormat format;
    std::uint32_t offset;
  };

  class VertexBindingBase
  {
  public:
    void AddAttribute(const VertexAttribute& attr) { attributes.push_back(attr); }

  protected:
    explicit VertexBindingBase(std::uint32_t stride) : stride(stride) {}

  private:
    std::uint32_t stride;
    std::vector<VertexAttribute> attributes;

    friend class VertexInputInfo;
  };

  template<typename Vertex>
  class VertexBinding : public VertexBindingBase
  {
  public:
    VertexBinding() : VertexBindingBase(sizeof(Vertex)) {}
  };

  class VertexInputInfo
  {
  public:
    void AddBinding(const VertexBindingBase& binding) { bindings.push_back(binding); }

    [[nodiscard]] std::vector<vk::VertexInputBindingDescription> GetNativeBindingDescriptions() const;
    [[nodiscard]] std::vector<vk::VertexInputAttributeDescription> GetNativeAttributeDescriptions() const;

  private:
    std::vector<VertexBindingBase> bindings;
  };

  struct PipelineConfig
  {
    const Framebuffer& framebuffer;
    const VertexInputInfo& vertexInputInfo;
    const DescriptorSetLayout& descriptorSetLayout;
    std::span<gsl::not_null<const Shader*>> shaders;
    bool blend{};
  };

  class Pipeline
  {
  public:
    static std::unique_ptr<Pipeline> Create(const PipelineConfig& config) { return std::make_unique<Pipeline>(config); }

    Pipeline(const PipelineConfig& config);
    Pipeline(const Pipeline&) = delete;
    Pipeline(Pipeline&& other) noexcept;
    ~Pipeline();

    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&& other) noexcept;

    [[nodiscard]] const vk::Pipeline& GetNative() const { return graphicsPipeline; }
    [[nodiscard]] const vk::PipelineLayout& GetNativeLayout() const { return pipelineLayout; }

  private:
    const Device& device;
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;
  };
} // namespace SimpleGE::GFXAPI