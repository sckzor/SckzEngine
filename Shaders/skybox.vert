#version 450
#extension GL_ARB_separate_shader_objects : enable

const int MAX_LIGHTS = 4;

layout(binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
}
ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in ivec4 boneIds;
layout(location = 4) in vec4 weights;

layout(location = 0) out vec3 fragTexCoord;

void main()
{
    fragTexCoord = inPosition;
    gl_Position  = ubo.proj * ubo.view * vec4(inPosition, 1.0);
    // gl_Position = vec4(fragTexCoord.xy * 2.0f + -1.0f, 0.0f, 1.0f);
}
