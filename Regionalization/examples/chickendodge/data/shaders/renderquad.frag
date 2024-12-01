#version 450

layout(binding = 0) uniform sampler2D uSampler;
layout(location = 0) in vec2 vTextureCoord;
layout(location = 0) out vec4 fragColor;

void main(void) {
    fragColor = texture(uSampler, vTextureCoord);
}
