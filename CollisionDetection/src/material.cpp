#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

namespace SimpleGE::Graphic
{
  Material::Material(const BaseMaterialBuilder& config) : shaderProgram(*gsl::make_not_null(config.shaderProgram))
  {
    descriptorSet = GFXAPI::DescriptorSet::Create({
        .descriptorSetLayout = shaderProgram.GetDescriptorSetLayout(),
        .uniformAttachmentInfo = config.uniformAttachmentInfo,
    });
  }

  void Material::Draw(const DrawDirect& drawInfos)
  {
    auto& commandBuffer = drawInfos.commandBuffer;

    commandBuffer.Bind(drawInfos.pipeline, *descriptorSet);
    commandBuffer.Draw(drawInfos.vertexCount, drawInfos.instanceCount);
  }

  void Material::Update(const BaseMaterialBuilder& config) { descriptorSet->Update(config.uniformAttachmentInfo); }
} // namespace SimpleGE::Graphic
