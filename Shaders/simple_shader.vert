#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;

    vec3 lightPosition;
}
ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 surfaceNormal;
layout(location = 2) out vec3 toLightVector;
layout(location = 3) out vec3 toCameraVector;

void main()
{
    vec4 worldPosition = ubo.model * vec4(inPosition, 1.0);

    gl_Position  = ubo.proj * ubo.view * worldPosition;
    fragTexCoord = inTexCoord;

    surfaceNormal = (ubo.model * vec4(inNormal, 0.0)).xyz;
    toLightVector = ubo.lightPosition - worldPosition.xyz;

    // TODO: change this so that the camera position is uploaded in the UBO
    toCameraVector = (inverse(ubo.view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPosition.xyz;
}
