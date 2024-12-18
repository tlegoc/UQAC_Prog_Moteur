#version 450

#extension GL_GOOGLE_include_directive : require
#include "compositor.glsl"

layout(location = 0) out vec2 vTextureCoord;

void main(void) {
    vTextureCoord = textureCoord[gl_VertexIndex];
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
