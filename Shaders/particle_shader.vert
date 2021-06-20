#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform StaticUniformBufferObject
{
    mat4 view;
    mat4 proj;
}
Subo;

layout(binding = 1) uniform PerParticleUniformBufferObject
{
    mat4 model;
    vec4 stageData;
}
PPubo;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec2 fragTexCoordNext;
layout(location = 2) out vec2 mixData;
layout(location = 3) out vec2 maxTexCoord;


void main()
{
    fragTexCoord = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);

    vec4 worldPosition = PPubo.model * vec4(fragTexCoord + -1.0f, 0.0f, 1.0f);

    gl_Position  = Subo.proj * Subo.view * worldPosition;

    float fragTexX = fragTexCoord.x;
    float fragTexY = fragTexCoord.y;

    fragTexCoord.x = fragTexX / PPubo.stageData.x + 
                     (1 / PPubo.stageData.x) * 
                     floor(mod(PPubo.stageData.z, PPubo.stageData.x));

    fragTexCoord.y = fragTexY / PPubo.stageData.y + 
                     (1 / PPubo.stageData.y) * 
                     floor(PPubo.stageData.z / PPubo.stageData.y);

    fragTexCoordNext.x = fragTexX / PPubo.stageData.x + 
                         (1 / PPubo.stageData.x) * 
                         floor(mod((1 + PPubo.stageData.z), PPubo.stageData.x));
                     
    fragTexCoordNext.y = fragTexY / PPubo.stageData.y + 
                         (1 / PPubo.stageData.y) * 
                         floor((1 + PPubo.stageData.z) / PPubo.stageData.y);

    mixData.x = PPubo.stageData.z;
    mixData.y = PPubo.stageData.w;

    maxTexCoord.x = (1 / PPubo.stageData.x) + (1 / PPubo.stageData.x) * floor(mod(PPubo.stageData.z, PPubo.stageData.x));
    maxTexCoord.y = (1 / PPubo.stageData.y) + (1 / PPubo.stageData.y) * floor(PPubo.stageData.z / PPubo.stageData.y);

}
