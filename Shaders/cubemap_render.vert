#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_multiview : enable

const int MAX_LIGHTS = 4;
const int CUBEMAP_SIDES = 6;

layout(binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view[CUBEMAP_SIDES];
    mat4 proj;
}
ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

void main()
{
    vec4 worldPosition = ubo.model * vec4(inPosition, 1.0);

    gl_Position  = ubo.proj * ubo.view[gl_ViewIndex] * worldPosition;
    fragTexCoord = inTexCoord;
}
