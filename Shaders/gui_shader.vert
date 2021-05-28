#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject
{
    vec4 location;
    vec4 scale;
}
ubo;

const int MAX_LIGHTS = 4;

layout(location = 0) out vec2 fragTexCoord;


void main()
{
    fragTexCoord = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(fragTexCoord.x * ubo.scale.x - ubo.scale.x / 2 + ubo.location.x, 
                       fragTexCoord.y * ubo.scale.y - ubo.scale.y / 2 + ubo.location.y, 
                       0.0f, 
                       1.0f);
}
