#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D texSampler1;
layout(binding = 1) uniform sampler2D texSampler2;


layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;


void main() 
{
    outColor = texture(texSampler1, fragTexCoord);
}

