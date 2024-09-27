#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE::Graphic
{
  class BaseShaderProgram;

  struct BaseMaterialBuilder
  {
    BaseMaterialBuilder(const BaseShaderProgram* shaderProgram) : shaderProgram(shaderProgram) {}

    const BaseShaderProgram* shaderProgram;
    GFXAPI::UniformAttachmentInfo uniformAttachmentInfo;
  };

  template<typename ShaderProgram>
  struct MaterialBuilder : public BaseMaterialBuilder
  {
    MaterialBuilder() : BaseMaterialBuilder(nullptr) {}
    MaterialBuilder(const BaseShaderProgram& shaderProgram) : BaseMaterialBuilder(&shaderProgram) {}

    void Attach(ShaderProgram::Binding bindingId, const GFXAPI::Sampler& sampler,
                const GFXAPI::Texture* texture = nullptr)
    {
      uniformAttachmentInfo.Attach(static_cast<std::uint32_t>(bindingId), sampler, texture);
    }

    void Attach(ShaderProgram::Binding bindingId, const GFXAPI::Buffer& buffer)
    {
      uniformAttachmentInfo.Attach(static_cast<std::uint32_t>(bindingId), buffer);
    }
  };

  template<typename IndexT>
  struct DrawCommand
  {
    GFXAPI::CommandBuffer& commandBuffer;
    const GFXAPI::Pipeline& pipeline;
    const GFXAPI::Buffer& vertexBuffer;
    const GFXAPI::TypedBuffer<IndexT>& indexBuffer;
    std::size_t indexCount;
  };

  struct DrawDirect
  {
    GFXAPI::CommandBuffer& commandBuffer;
    const GFXAPI::Pipeline& pipeline;
    std::uint32_t vertexCount;
    std::uint32_t instanceCount;
  };

  class Material
  {
  public:
    static std::unique_ptr<Material> Create(const BaseMaterialBuilder& config)
    {
      return std::make_unique<Material>(config);
    }

    Material(const BaseMaterialBuilder& config);

    template<typename IndexT>
    inline void Draw(const DrawCommand<IndexT>& drawInfos);

    void Draw(const DrawDirect& drawInfos);

    void Update(const BaseMaterialBuilder& config);

    [[nodiscard]] const BaseShaderProgram& GetShaderProgram() const { return shaderProgram; }

  private:
    const BaseShaderProgram& shaderProgram;
    std::unique_ptr<GFXAPI::DescriptorSet> descriptorSet;
  };
} // namespace SimpleGE::Graphic