#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec2 fragTexCoordNext;
layout(location = 2) in vec2 mixData;
layout(location = 3) in vec2 maxTexCoord;

layout(location = 0) out vec4 outColor;


void main() 
{
    if( fragTexCoord.x > maxTexCoord.x || fragTexCoord.y > maxTexCoord.y  || mixData.x > mixData.y )
    {
        discard;
    }
    
    outColor = mix(texture(texSampler, fragTexCoord), texture(texSampler, fragTexCoordNext), mod(mixData.x, 1));
}