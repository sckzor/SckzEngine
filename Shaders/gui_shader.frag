#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform UniformBufferObject
{    
    vec4 dummy;
}
ubo;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;


void main() 
{
    outColor = texture(texSampler, fragTexCoord);
    if(fragTexCoord.x > 1.0 || fragTexCoord.y > 1.0)
    {
        discard;
    }
}

