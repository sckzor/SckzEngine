#version 450
#extension GL_ARB_separate_shader_objects : enable

const int MAX_LIGHTS = 2;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform UniformBufferObject
{
    vec3 lightColor[MAX_LIGHTS];
    vec3 attenuation[MAX_LIGHTS];

    float shineDamper;
    float reflectivity;
}
ubo;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 surfaceNormal;
layout(location = 2) in vec3 toLightVector[MAX_LIGHTS];
layout(location = 6) in vec3 toCameraVector;

layout(location = 0) out vec4 outColor;


void main() 
{
    vec3 unitNormal = normalize(surfaceNormal);
    vec3 unitToCameraVector = normalize(toCameraVector);

    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    for(int i = 0; i < MAX_LIGHTS; i++)
    {
        float dist = length(toLightVector[i]);
        float attFactor = ubo.attenuation[i].x + (ubo.attenuation[i].y * dist) + (ubo.attenuation[i].z * dist * dist);
        
        vec3 unitLightVector = normalize(toLightVector[i]);

        float brightness = dot(unitNormal, unitLightVector);
        brightness = max(brightness, 0.0);
        totalDiffuse = totalDiffuse + (brightness * ubo.lightColor[i]);
        totalDiffuse = totalDiffuse / attFactor;

        vec3 lightDirection = -unitLightVector;
        vec3 reflectedLightDirection = reflect(lightDirection, unitNormal);
        float specularFactor = dot(reflectedLightDirection, unitToCameraVector);
        specularFactor = max(specularFactor, 0.0);
        float dampedFactor = pow(specularFactor, ubo.shineDamper);
        totalSpecular = totalSpecular + (dampedFactor * ubo.reflectivity * ubo.lightColor[i]);
    }
    totalDiffuse = max(totalDiffuse, 0.2);

    outColor = vec4(totalDiffuse, 1.0) * texture(texSampler, fragTexCoord);
}

