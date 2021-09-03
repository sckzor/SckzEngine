#version 450
#extension GL_ARB_separate_shader_objects : enable

const int MAX_LIGHTS = 4;
const int MAX_BONES  = 20;

layout(binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;

    vec3 lightPosition[MAX_LIGHTS];

    float refractiveIndexRatio;
}
ubo;

layout(binding = 1) uniform BoneUniformBuffer { mat4 bones[MAX_BONES]; }
bubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 boneDataA;
layout(location = 4) in vec4 boneDataB;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 surfaceNormal;
layout(location = 2) out vec3 toLightVector[MAX_LIGHTS];
layout(location = 6) out vec3 toCameraVector;
layout(location = 7) out vec3 camLocation;
layout(location = 8) out vec3 reflectedVector;
layout(location = 9) out vec3 refractedVector;

void main()
{
    mat4 boneTransform = bubo.bones[uint(boneDataA.x)] * boneDataA.z + bubo.bones[uint(boneDataA.y)] * boneDataA.w
                       + bubo.bones[uint(boneDataB.x)] * boneDataB.z + bubo.bones[uint(boneDataB.y)] * boneDataB.w;

    vec4 worldPosition = ubo.model * /*boneTransform **/ vec4(inPosition, 1.0);

    gl_Position  = ubo.proj * ubo.view * worldPosition;
    fragTexCoord = inTexCoord;

    surfaceNormal = (ubo.model * vec4(inNormal, 0.0)).xyz;

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        toLightVector[i] = ubo.lightPosition[i] - worldPosition.xyz;
    }

    // TODO: change this so that the camera position is uploaded in the UBO
    camLocation = (inverse(ubo.view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;

    toCameraVector  = camLocation - worldPosition.xyz;
    reflectedVector = reflect(normalize(toCameraVector), normalize(inNormal));
    refractedVector = refract(normalize(toCameraVector), normalize(inNormal), ubo.refractiveIndexRatio);
}
