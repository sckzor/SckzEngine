
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    const float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    // (0.454054, 0.3891892, 0.2432432, 0.108108, 0.032432)

    vec2 tex_offset = 1.0 / textureSize(texSampler, 0);                  // gets size of single texel
    vec3 result     = texture(texSampler, fragTexCoord).rgb * weight[0]; // current fragment's contribution

    for (int i = 1; i < 5; ++i)
    {
        result += texture(texSampler, fragTexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        result += texture(texSampler, fragTexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
    }

    outColor = vec4(result, 1.0);
}
