#pragma once

#include <simplege/simplege.h>

namespace SimpleGE::Graphic
{
  template<typename IndexT>
  inline void Material::Draw(const DrawCommand<IndexT>& drawInfos)
  {
    auto& commandBuffer = drawInfos.commandBuffer;

    commandBuffer.Bind(drawInfos.pipeline, *descriptorSet);
    commandBuffer.BindVertexBuffer(drawInfos.vertexBuffer);
    commandBuffer.BindIndexBuffer(drawInfos.indexBuffer);

    commandBuffer.DrawIndexed(drawInfos.indexCount);
  }
} // namespace SimpleGE::Graphic