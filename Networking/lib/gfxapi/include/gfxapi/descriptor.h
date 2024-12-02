#pragma once

#include <gfxapi/pch/precomp.h>

#include <gfxapi/device.h>

namespace SimpleGE::GFXAPI
{
  class Buffer;
  class Device;
  class Sampler;
  class Texture;

  struct DescriptorPoolConfig
  {
    const Device& device;
  };

  struct UniformBinding
  {
    std::uint32_t bindingId;
    ShaderType shader{ShaderType::Vertex};
  };

  struct SamplerBinding
  {
    std::uint32_t bindingId;
    ShaderType shader{ShaderType::Fragment};
  };

  class UniformInfo
  {
  public:
    void AddBinding(const UniformBinding& binding);
    void AddBinding(const SamplerBinding& binding);

    [[nodiscard]] std::size_t CountBindings() const { return std::size(bindings); }

  private:
    struct UniformBindingBase
    {
      std::uint32_t bindingId;
      vk::DescriptorType descriptorType;
      vk::ShaderStageFlags stageFlags;
    };

    void AddBinding(const UniformBindingBase& binding) { bindings.push_back(binding); }

    std::vector<UniformBindingBase> bindings;

    friend class DescriptorSetLayout;
  };

  class UniformAttachmentInfo
  {
  public:
    void Attach(std::uint32_t bindingId, const Sampler& sampler, const Texture* texture = nullptr)
    {
      samplers.emplace_back(SamplerAttachment{bindingId, sampler, texture});
    }

    void Attach(std::uint32_t bindingId, const Buffer& buffer)
    {
      buffers.emplace_back(BufferAttachment{bindingId, buffer});
    }

  private:
    struct SamplerAttachment
    {
      std::uint32_t bindingId;
      const Sampler& sampler;
      const Texture* texture{};
    };

    struct BufferAttachment
    {
      std::uint32_t bindingId;
      const Buffer& buffer;
    };

    std::vector<SamplerAttachment> samplers;
    std::vector<BufferAttachment> buffers;

    friend class DescriptorSet;
  };

  class DescriptorPool
  {
  public:
    static std::unique_ptr<DescriptorPool> Create(const DescriptorPoolConfig& config)
    {
      return std::make_unique<DescriptorPool>(config);
    }

    DescriptorPool(const DescriptorPoolConfig& config);
    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool(DescriptorPool&& other) noexcept;
    ~DescriptorPool();

    DescriptorPool& operator=(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&& other) noexcept;

    [[nodiscard]] const vk::DescriptorPool& GetNative() const { return descriptorPool; }

  private:
    const Device& device;
    vk::DescriptorPool descriptorPool;
  };

  struct DescriptorSetLayoutConfig
  {
    const Device& device{Device::GetMain()};
    const UniformInfo& uniformInfo;
  };

  class DescriptorSetLayout
  {
  public:
    static std::unique_ptr<DescriptorSetLayout> Create(const DescriptorSetLayoutConfig& config)
    {
      return std::make_unique<DescriptorSetLayout>(config);
    }

    DescriptorSetLayout(const DescriptorSetLayoutConfig& config);
    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
    ~DescriptorSetLayout();

    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept;

    [[nodiscard]] const Device& GetDevice() const { return device; }
    [[nodiscard]] const vk::DescriptorSetLayout& GetNative() const { return descriptorSetLayout; }

  private:
    const Device& device;
    vk::DescriptorSetLayout descriptorSetLayout;
  };

  struct DescriptorSetConfig
  {
    const DescriptorSetLayout& descriptorSetLayout;
    const UniformAttachmentInfo& uniformAttachmentInfo;
  };

  class DescriptorSet
  {
  public:
    static std::unique_ptr<DescriptorSet> Create(const DescriptorSetConfig& config)
    {
      return std::make_unique<DescriptorSet>(config);
    }

    DescriptorSet(const DescriptorSetConfig& config);
    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet(DescriptorSet&& other) noexcept;
    ~DescriptorSet();

    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&& other) noexcept;

    void Update(const UniformAttachmentInfo& uniformAttachmentInfo);

    [[nodiscard]] const vk::DescriptorSet& GetNative() const { return descriptorSet; }

  private:
    const DescriptorSetLayout& descriptorSetLayout;
    vk::DescriptorSet descriptorSet;
  };
} // namespace SimpleGE::GFXAPI