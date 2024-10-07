#version 450

/* Rendu du jeu */
layout(binding = 0) uniform sampler2D uSampler;

/* Texture de déformation en rouge et vert */
layout(binding = 1) uniform sampler2D uDeformation;

/* Texture pour contrôler l'intensité de la déformation */
layout(binding = 2) uniform sampler2D uIntensity;

layout(binding = 3) uniform uSettings {
    /* Interval de temps multiplié par la vitesse depuis l'activation du composant */
    float uTime;

    /* Échelle de la déformation */
    float uScale;
};

/* Coordonnées UV du fragment */
layout(location = 0) in vec2 vTextureCoord;

/* Couleur de sortie */
layout(location = 0) out vec4 fragColor;

void main(void) {
    float intensity = texture(uIntensity, vec2(uTime, 0.5)).r * uScale;
    vec2 deformation = texture(uDeformation, vTextureCoord * sin(uTime)).rg * intensity;

    fragColor = texture(uSampler, vTextureCoord + deformation);
    fragColor.gb *= 0.5;
}
