#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform samplerCube texSampler;

layout(location = 0) in vec3 fragTexCoord;

layout(location = 0) out vec4 outColor;


void main() 
{
    vec3 color = texture(texSampler, fragTexCoord).rgb;
    outColor = vec4(color, 1.0);
}

