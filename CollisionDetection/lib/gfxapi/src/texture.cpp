#include <gfxapi/pch/precomp.h>

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  std::unique_ptr<Texture> Texture::Create(TextureConfig config, const std::span<const ColorU8>& pixels)
  {
    const auto& vkDevice = config.device.GetNative();

    TypedBuffer<ColorU8> stagingBuffer({
        .device = config.device,
        .count = std::size(pixels),
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
        .properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
    });

    {
      auto bufferPixels = stagingBuffer.Map({.count = std::size(pixels)});
      std::copy(pixels.begin(), pixels.end(), bufferPixels.Get().begin());
    }

    config.usage |= vk::ImageUsageFlagBits::eTransferDst;

    auto texture = Create(config);

    texture->Transition(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    stagingBuffer.CopyTo(*texture, config.size.width, config.size.height);
    texture->Transition(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    return texture;
  }

  Texture::Texture(const TextureConfig& config)
      : device(config.device), format(config.format), type(Type::Standard), size(config.size)
  {
    const auto& vkDevice = device.GetNative();

    vk::ImageCreateInfo imageInfo{
        .flags = {},
        .imageType = vk::ImageType::e2D,
        .format = format,
        .extent =
            {
                .width = size.width,
                .height = size.height,
                .depth = 1,
            },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = vk::SampleCountFlagBits::e1,
        .tiling = config.tiling,
        .usage = config.usage,
        .sharingMode = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    image = vkDevice.createImage(imageInfo);

    vk::MemoryRequirements memRequirements = vkDevice.getImageMemoryRequirements(image);

    vk::MemoryAllocateInfo allocInfo{
        .allocationSize = memRequirements.size,
        .memoryTypeIndex =
            Private::FindMemoryType(device.GetAdapter(), memRequirements.memoryTypeBits, config.properties),
    };

    imageMemory = vkDevice.allocateMemory(allocInfo);

    vkDevice.bindImageMemory(image, imageMemory, 0);

    CreateImageView(config.aspectFlags);

    if (config.usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
    {
      Transition(vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    }
  }

  Texture::Texture(const SwapTextureConfig& config)
      : device(config.device), image(config.image), format(config.format), type(Type::SwapChain), size(config.size)
  {
    CreateImageView(config.aspectFlags);
  }

  Texture::Texture(Texture&& other) noexcept
      : device(other.device), type(other.type), image(other.image), imageView(other.imageView),
        imageMemory(other.imageMemory), format(other.format), size(other.size)
  {
    other.image = nullptr;
    other.imageView = nullptr;
    other.imageMemory = nullptr;
  }

  void Texture::Destroy()
  {
    const auto& vkDevice = device.GetNative();
    vkDevice.destroyImageView(imageView);
    imageView = nullptr;

    if (type != Type::SwapChain)
    {
      vkDevice.destroyImage(image);
      image = nullptr;
      vkDevice.freeMemory(imageMemory);
      imageMemory = nullptr;
    }
  }

  Texture& Texture::operator=(Texture&& other) noexcept
  {
    Expects(&device == &other.device);
    Destroy();
    type = other.type;
    image = other.image;
    imageView = other.imageView;
    imageMemory = other.imageMemory;
    format = other.format;
    size = other.size;
    other.image = nullptr;
    other.imageView = nullptr;
    other.imageMemory = nullptr;
    return *this;
  }

  void Texture::CreateImageView(vk::ImageAspectFlags aspectFlags)
  {
    const auto& vkDevice = device.GetNative();
    vk::ImageViewCreateInfo createInfo{
        .image = image,
        .viewType = vk::ImageViewType::e2D,
        .format = format,
        .subresourceRange =
            {
                .aspectMask = aspectFlags,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };

    imageView = vkDevice.createImageView(createInfo);
  }

  static bool HasStencilComponent(ColorFormat format)
  {
    return format == ColorFormat::eD32SfloatS8Uint || format == ColorFormat::eD24UnormS8Uint;
  }

  void Texture::Transition(vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
  {
    auto commandBuffer = ImmediateCommandBuffer({device.GetGraphicsQueue()});

    vk::ImageMemoryBarrier barrier{
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange =
            {
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
      if (HasStencilComponent(format))
      {
        barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
      }
    }
    else
    {
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    }

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
      sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
      destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
      barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      sourceStage = vk::PipelineStageFlagBits::eTransfer;
      destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
      barrier.srcAccessMask = {};
      barrier.dstAccessMask =
          vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
      sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
      destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    }
    else
    {
      throw std::runtime_error("Unsupported layout transition!");
    }

    commandBuffer.PipelineBarrier(sourceStage, destinationStage, barrier);
  }

  Sampler::Sampler(const SamplerConfig& config) : device(config.device)
  {
    const auto& vkDevice = device.GetNative();

    auto addressMode = config.clamp ? vk::SamplerAddressMode::eClampToEdge : vk::SamplerAddressMode::eRepeat;

    vk::SamplerCreateInfo samplerInfo{
        .magFilter = vk::Filter::eNearest,
        .minFilter = vk::Filter::eNearest,
        .mipmapMode = vk::SamplerMipmapMode::eNearest,
        .addressModeU = addressMode,
        .addressModeV = addressMode,
        .addressModeW = addressMode,
        .mipLodBias = 0.F,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 1.F,
        .compareEnable = VK_FALSE,
        .compareOp = vk::CompareOp::eAlways,
        .minLod = 0.F,
        .maxLod = 0.F,
        .borderColor = vk::BorderColor::eIntOpaqueBlack,
        .unnormalizedCoordinates = VK_FALSE,
    };

    sampler = vkDevice.createSampler(samplerInfo);
  }

  Sampler::Sampler(Sampler&& other) noexcept : device(other.device), sampler(other.sampler) { other.sampler = nullptr; }

  Sampler::~Sampler()
  {
    const auto& vkDevice = device.GetNative();

    vkDevice.destroySampler(sampler);
  }

  Sampler& Sampler::operator=(Sampler&& other) noexcept
  {
    Expects(&device == &other.device);
    this->~Sampler();
    sampler = other.sampler;
    other.sampler = nullptr;
    return *this;
  }

} // namespace SimpleGE::GFXAPI
