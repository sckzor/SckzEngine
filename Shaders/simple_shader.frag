#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform UniformBufferObject
{
    vec3 lightColor;

    float shineDamper;
    float reflectivity;
}
ubo;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 surfaceNormal;
layout(location = 2) in vec3 toLightVector;
layout(location = 3) in vec3 toCameraVector;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 unitNormal = normalize(surfaceNormal);
    vec3 unitLightVector = normalize(toLightVector);
    vec3 unitToCameraVector = normalize(toCameraVector);

    vec3 lightDirection = -unitLightVector;

    vec3 reflectedLightDirection = reflect(lightDirection, unitNormal);

    float brightness = dot(unitNormal, unitLightVector);
    brightness = max(brightness, 0.0);

    vec3 diffuse = brightness * ubo.lightColor;

    float specularFactor = dot(reflectedLightDirection, unitToCameraVector);
    specularFactor = max(specularFactor, 0.0);
    float dampedFactor = pow(specularFactor, ubo.shineDamper);

    vec3 finalSpecular = dampedFactor * ubo.reflectivity * ubo.lightColor;

    outColor = (vec4(diffuse, 1.0) * texture(texSampler, fragTexCoord));
}

