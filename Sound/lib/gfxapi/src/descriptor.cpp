#include <gfxapi/pch/precomp.h>

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  void UniformInfo::AddBinding(const UniformBinding& binding)
  {
    AddBinding(UniformBindingBase{
        .bindingId = binding.bindingId,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .stageFlags = Private::GetVkShaderType(binding.shader),
    });
  }

  void UniformInfo::AddBinding(const SamplerBinding& binding)
  {
    AddBinding(UniformBindingBase{
        .bindingId = binding.bindingId,
        .descriptorType = vk::DescriptorType::eCombinedImageSampler,
        .stageFlags = Private::GetVkShaderType(binding.shader),
    });
  }

  DescriptorPool::DescriptorPool(const DescriptorPoolConfig& config) : device(config.device)
  {
    const auto& vkDevice = device.GetNative();

    constexpr auto DescriptorCount = 100;

    auto poolSizes = std::array{
        vk::DescriptorPoolSize{
            .type = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = DescriptorCount,
        },
        vk::DescriptorPoolSize{
            .type = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = DescriptorCount,
        },
    };

    vk::DescriptorPoolCreateInfo poolInfo{
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = DescriptorCount,
        .poolSizeCount = std::size(poolSizes),
        .pPoolSizes = poolSizes.data(),
    };

    descriptorPool = vkDevice.createDescriptorPool(poolInfo);
  }

  DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
      : device(other.device), descriptorPool(other.descriptorPool)
  {
    other.descriptorPool = nullptr;
  }

  DescriptorPool::~DescriptorPool()
  {
    const auto& vkDevice = device.GetNative();

    vkDevice.destroyDescriptorPool(descriptorPool);
  }

  DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other) noexcept
  {
    Expects(&device == &other.device);
    this->~DescriptorPool();
    descriptorPool = other.descriptorPool;
    other.descriptorPool = nullptr;
    return *this;
  }

  DescriptorSetLayout::DescriptorSetLayout(const DescriptorSetLayoutConfig& config) : device(config.device)
  {
    const auto& vkDevice = device.GetNative();

    const auto& bindingDescr = config.uniformInfo.bindings;

    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    bindings.reserve(std::size(bindingDescr));

    std::transform(bindingDescr.begin(), bindingDescr.end(), std::back_inserter(bindings),
                   [](const auto& binding)
                   {
                     return vk::DescriptorSetLayoutBinding{
                         .binding = binding.bindingId,
                         .descriptorType = binding.descriptorType,
                         .descriptorCount = 1,
                         .stageFlags = binding.stageFlags,
                         .pImmutableSamplers = nullptr,
                     };
                   });

    vk::DescriptorSetLayoutCreateInfo layoutInfo{
        .bindingCount = static_cast<std::uint32_t>(std::size(bindings)),
        .pBindings = bindings.data(),
    };

    descriptorSetLayout = vkDevice.createDescriptorSetLayout({layoutInfo});
  }

  DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
      : device(other.device), descriptorSetLayout(other.descriptorSetLayout)
  {
    other.descriptorSetLayout = nullptr;
  }

  DescriptorSetLayout::~DescriptorSetLayout()
  {
    const auto& vkDevice = device.GetNative();

    vkDevice.destroyDescriptorSetLayout(descriptorSetLayout);
  }

  DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& other) noexcept
  {
    Expects(&device == &other.device);
    this->~DescriptorSetLayout();
    descriptorSetLayout = other.descriptorSetLayout;
    other.descriptorSetLayout = nullptr;
    return *this;
  }

  DescriptorSet::DescriptorSet(const DescriptorSetConfig& config) : descriptorSetLayout(config.descriptorSetLayout)
  {
    const auto& device = descriptorSetLayout.GetDevice();
    const auto& vkDevice = device.GetNative();

    vk::DescriptorSetAllocateInfo allocInfo{
        .descriptorPool = device.GetDescriptorPool().GetNative(),
        .descriptorSetCount = 1,
        .pSetLayouts = &config.descriptorSetLayout.GetNative(),
    };

    descriptorSet = vkDevice.allocateDescriptorSets(allocInfo).front();

    Update(config.uniformAttachmentInfo);
  }

  DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept
      : descriptorSetLayout(other.descriptorSetLayout), descriptorSet(other.descriptorSet)
  {
    other.descriptorSet = nullptr;
  }

  DescriptorSet::~DescriptorSet()
  {
    const auto& device = descriptorSetLayout.GetDevice();
    const auto& vkDevice = device.GetNative();

    vkDevice.freeDescriptorSets(device.GetDescriptorPool().GetNative(), {descriptorSet});
  }

  DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other) noexcept
  {
    Expects(&descriptorSetLayout == &other.descriptorSetLayout);
    this->~DescriptorSet();
    descriptorSet = other.descriptorSet;
    other.descriptorSet = nullptr;
    return *this;
  }

  void DescriptorSet::Update(const UniformAttachmentInfo& uniformAttachmentInfo)
  {
    const auto& device = descriptorSetLayout.GetDevice();
    const auto& vkDevice = device.GetNative();

    const auto& samplers = uniformAttachmentInfo.samplers;
    const auto& buffers = uniformAttachmentInfo.buffers;

    std::vector<vk::DescriptorImageInfo> imageInfos;
    imageInfos.reserve(std::size(samplers));

    std::vector<vk::DescriptorBufferInfo> bufferInfos;
    bufferInfos.reserve(std::size(buffers));

    std::vector<vk::WriteDescriptorSet> descriptorWrites;
    descriptorWrites.reserve(std::size(buffers) + std::size(samplers));

    std::transform(samplers.begin(), samplers.end(), std::back_inserter(descriptorWrites),
                   [this, &imageInfos](const auto& attachment)
                   {
                     auto& imageInfo = imageInfos.emplace_back(vk::DescriptorImageInfo{
                         .sampler = attachment.sampler.GetNative(),
                         .imageView = (attachment.texture != nullptr) ? attachment.texture->GetNativeView() : nullptr,
                         .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
                     });

                     return vk::WriteDescriptorSet{
                         .dstSet = descriptorSet,
                         .dstBinding = attachment.bindingId,
                         .dstArrayElement = 0,
                         .descriptorCount = 1,
                         .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                         .pImageInfo = &imageInfo,
                         .pBufferInfo = nullptr,
                         .pTexelBufferView = nullptr,
                     };
                   });
    auto toRemove = std::remove_if(descriptorWrites.begin(), descriptorWrites.end(),
                                   [](const auto& descriptor) { return !descriptor.pImageInfo->imageView; });
    descriptorWrites.erase(toRemove, descriptorWrites.end());

    std::transform(buffers.begin(), buffers.end(), std::back_inserter(descriptorWrites),
                   [this, &bufferInfos](const auto& attachment)
                   {
                     auto& bufferInfo = bufferInfos.emplace_back(vk::DescriptorBufferInfo{
                         .buffer = attachment.buffer.GetNative(),
                         .offset = 0,
                         .range = VK_WHOLE_SIZE,
                     });

                     return vk::WriteDescriptorSet{
                         .dstSet = descriptorSet,
                         .dstBinding = attachment.bindingId,
                         .dstArrayElement = 0,
                         .descriptorCount = 1,
                         .descriptorType = vk::DescriptorType::eUniformBuffer,
                         .pImageInfo = nullptr,
                         .pBufferInfo = &bufferInfo,
                         .pTexelBufferView = nullptr,
                     };
                   });

    vkDevice.updateDescriptorSets(descriptorWrites, {});
  }
} // namespace SimpleGE::GFXAPI