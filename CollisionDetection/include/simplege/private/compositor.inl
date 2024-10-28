#pragma once

#include <simplege/simplege.h>

namespace SimpleGE
{
  template<typename CompositorType>
  inline Component::SetupResult CompositorComponent::Setup(const Description& descr)
  {
    const auto& shaderProgram = Graphic::BaseShaderProgram::GetShaderProgram(descr.shaderProgram);
    Graphic::MaterialBuilder<typename CompositorType::ShaderProgram> materialBuilder{shaderProgram};
    dynamic_cast<CompositorType*>(this)->FillMaterialBuilder(materialBuilder);
    auto material = Graphic::Material::Create(materialBuilder);

    return {this, {}};
  }
} // namespace SimpleGE