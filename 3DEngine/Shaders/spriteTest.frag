#version 460

in vec2 TexCoords; 

layout (location = 0) out vec4 frag_color;

uniform sampler2D uTexture;
uniform vec3 textColor;

void main()
{
    vec4 sampled = texture(uTexture, TexCoords);

    if (sampled.g < 0.05) {
        discard;
    }

    frag_color = vec4(textColor, sampled.g);
}