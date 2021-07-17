#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;


void main() 
{
    vec3 color = texture(texSampler, fragTexCoord).rgb;
    outColor = vec4(0.0, 0.0, 1.0, 1.0);
}

