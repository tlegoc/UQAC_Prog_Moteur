#pragma once

#include <gfxapi/pch/precomp.h>

namespace SimpleGE::GFXAPI
{
  class Buffer;
  class DescriptorSet;
  class Framebuffer;
  class Pipeline;
  class Queue;
  class Texture;

  template<typename T>
  class TypedBuffer;

  struct CommandBufferConfig
  {
    const Queue& queue;
  };

  struct BeginRenderPassSettings
  {
    const Framebuffer& target;
    ColorFloat clearColor{0.F, 0.F, 0.F, 0.F};
    float clearDepth{1.F};
  };

  class CommandBuffer
  {
  public:
    [[nodiscard]] static std::uint32_t GetAndResetDrawCount()
    {
      auto count = drawCount;
      drawCount = 0;
      return count;
    }

    CommandBuffer(const CommandBufferConfig& config);
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer(CommandBuffer&& other) noexcept;
    ~CommandBuffer();

    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&& other) noexcept;

    void BeginRenderPass(const BeginRenderPassSettings& settings);
    void EndRenderPass();

    void Bind(const Pipeline& pipeline, const DescriptorSet& descriptorSet);

    template<typename T>
    inline void BindIndexBuffer(const TypedBuffer<T>& buffer);

    void BindVertexBuffer(const Buffer& buffer);
    void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount);
    void DrawIndexed(std::size_t indexCount);

    void PipelineBarrier(const vk::PipelineStageFlags& sourceStage, const vk::PipelineStageFlags& destinationStage,
                         const vk::ImageMemoryBarrier& barrier);
    void Copy(const Buffer& src, Texture& dest, const vk::BufferImageCopy& region);

    [[nodiscard]] const vk::CommandBuffer& GetNative() const { return commandBuffer; }

  protected:
    const Queue& queue;
    vk::CommandBuffer commandBuffer;

  private:
    void BindIndexBuffer(const Buffer& buffer, vk::IndexType indexType);

    static inline std::uint32_t drawCount{};
  };

  class ImmediateCommandBuffer : public CommandBuffer
  {
  public:
    ImmediateCommandBuffer(const CommandBufferConfig& config);
    ~ImmediateCommandBuffer();
  };
} // namespace SimpleGE::GFXAPI