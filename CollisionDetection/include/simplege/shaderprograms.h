#pragma once

#include <simplege/pch/precomp.h>

#include <simplege/graphics/shaderprograms/deformation.h>
#include <simplege/graphics/shaderprograms/renderquad.h>
#include <simplege/graphics/shaderprograms/sprite.h>

namespace SimpleGE
{
  static inline void RegisterGenericShaderPrograms()
  {
    using namespace Graphic;

    BaseShaderProgram::Register<DeformationShaderProgram>();
    BaseShaderProgram::Register<RenderQuadShaderProgram>();
    BaseShaderProgram::Register<SpriteShaderProgram>();
  }
} // namespace SimpleGE