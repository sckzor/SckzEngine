#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject
{
    vec4 location;
    vec4 scale;
    vec4 rotation;
}
ubo;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out float currentStage;
layout(location = 2) out float stages;

void main()
{
    fragTexCoord = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);

    float centerX = (ubo.rotation.x + 1) / 2;
    float centerY = (ubo.rotation.y + 1) / 2;

    float rotatedX
        = cos(ubo.rotation.z) * (fragTexCoord.x - centerX) - sin(ubo.rotation.z) * (fragTexCoord.y - centerY) + centerX;

    float rotatedY
        = sin(ubo.rotation.z) * (fragTexCoord.x - centerX) + cos(ubo.rotation.z) * (fragTexCoord.y - centerY) + centerY;

    gl_Position = vec4(rotatedX * ubo.scale.x - ubo.scale.x / 2 + ubo.location.x,
                       rotatedY * ubo.scale.y - ubo.scale.y / 2 + ubo.location.y,
                       0.0f,
                       1.0f);

    float originalY = fragTexCoord.y;
    // fragTexCoord.y /= ubo.scale.z;
    fragTexCoord.y = (originalY / ubo.scale.z) + (1 / (ubo.scale.w + 1));
    currentStage   = ubo.scale.w;
    stages         = ubo.scale.z;
}
