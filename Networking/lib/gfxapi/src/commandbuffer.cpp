#include <gfxapi/pch/precomp.h>

#include <gfxapi/gfxapi.h>

namespace SimpleGE::GFXAPI
{
  CommandBuffer::CommandBuffer(const CommandBufferConfig& config) : queue(config.queue)
  {
    const auto& vkDevice = queue.GetDevice().GetNative();

    vk::CommandBufferAllocateInfo allocInfo{
        .commandPool = queue.GetNativeCommandPool(),
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1,
    };

    auto commandBuffers = vkDevice.allocateCommandBuffers(allocInfo);
    commandBuffer = gsl::at(commandBuffers, 0);
  }

  CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept : queue(other.queue), commandBuffer(other.commandBuffer)
  {
    other.commandBuffer = nullptr;
  }

  CommandBuffer::~CommandBuffer()
  {
    const auto& vkDevice = queue.GetDevice().GetNative();

    vkDevice.freeCommandBuffers(queue.GetNativeCommandPool(), {commandBuffer});
  }

  CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept
  {
    Expects(&queue == &other.queue);
    this->~CommandBuffer();
    commandBuffer = other.commandBuffer;
    other.commandBuffer = nullptr;
    return *this;
  }

  void CommandBuffer::BeginRenderPass(const BeginRenderPassSettings& settings)
  {
    auto clearValues = std::array{
        vk::ClearValue{
            .color = {settings.clearColor},
        },
        vk::ClearValue{
            .depthStencil = {settings.clearDepth, 0},
        },
    };

    const auto& framebuffer = settings.target;
    auto size = framebuffer.GetSize();

    vk::RenderPassBeginInfo renderPassInfo{
        .renderPass = framebuffer.GetNativeRenderPass(),
        .framebuffer = framebuffer.GetNative(),
        .renderArea =
            {
                .offset = {.x = 0, .y = 0},
                .extent = size,
            },
        .clearValueCount = std::size(clearValues),
        .pClearValues = clearValues.data(),
    };

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport{
        .x = 0.F,
        .y = 0.F,
        .width = static_cast<float>(size.width),
        .height = static_cast<float>(size.height),
        .minDepth = 0.F,
        .maxDepth = 1.F,
    };
    commandBuffer.setViewport(0, {viewport});

    vk::Rect2D scissor{
        .offset = {.x = 0, .y = 0},
        .extent = size,
    };
    commandBuffer.setScissor(0, {scissor});
  }

  void CommandBuffer::EndRenderPass() { commandBuffer.endRenderPass(); }

  void CommandBuffer::Bind(const Pipeline& pipeline, const DescriptorSet& descriptorSet)
  {
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.GetNative());
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.GetNativeLayout(), 0,
                                     {descriptorSet.GetNative()}, {});
  }

  void CommandBuffer::BindIndexBuffer(const Buffer& buffer, vk::IndexType indexType)
  {
    commandBuffer.bindIndexBuffer(buffer.GetNative(), 0, indexType);
  }

  void CommandBuffer::BindVertexBuffer(const Buffer& buffer)
  {
    commandBuffer.bindVertexBuffers(0, {buffer.GetNative()}, {0});
  }

  void CommandBuffer::Draw(std::uint32_t vertexCount, std::uint32_t instanceCount)
  {
    drawCount++;
    commandBuffer.draw(vertexCount, instanceCount, 0, 0);
  }

  void CommandBuffer::DrawIndexed(std::size_t indexCount)
  {
    drawCount++;
    commandBuffer.drawIndexed(indexCount, 1, 0, 0, 0);
  }

  void CommandBuffer::PipelineBarrier(const vk::PipelineStageFlags& sourceStage,
                                      const vk::PipelineStageFlags& destinationStage,
                                      const vk::ImageMemoryBarrier& barrier)
  {
    commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, {barrier});
  }

  void CommandBuffer::Copy(const Buffer& src, Texture& dest, const vk::BufferImageCopy& region)
  {
    commandBuffer.copyBufferToImage(src.GetNative(), dest.GetNative(), vk::ImageLayout::eTransferDstOptimal, {region});
  }

  ImmediateCommandBuffer::ImmediateCommandBuffer(const CommandBufferConfig& config) : CommandBuffer(config)
  {
    vk::CommandBufferBeginInfo beginInfo{
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
        .pInheritanceInfo = nullptr,
    };

    commandBuffer.begin(beginInfo);
  }

  ImmediateCommandBuffer::~ImmediateCommandBuffer()
  {
    commandBuffer.end();

    queue.Submit(*this);
    queue.WaitIdle();
  }
} // namespace SimpleGE::GFXAPI