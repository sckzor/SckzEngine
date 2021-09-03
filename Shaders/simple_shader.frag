#version 450
#extension GL_ARB_separate_shader_objects : enable

const int MAX_LIGHTS = 4;

const float CEL_SHADING_LEVELS = -1; // Use -1 to disable

layout(binding = 2) uniform sampler2D texSampler; // Normal color texture.
layout(binding = 3) uniform sampler2D norSampler; // Red is X, Green is Y, Blue is Z (as per normal (no pun intended))
layout(binding = 4) uniform sampler2D speSampler; // Red Channel is Specular Map, Green is glow map
layout(binding = 5) uniform samplerCube cubeMap;  // Red Channel is Specular Map, Green is glow map

layout(binding = 6) uniform UniformBufferObject
{
    vec4 lightColor[MAX_LIGHTS];
    vec4 attenuation[MAX_LIGHTS];
    vec4 direction[MAX_LIGHTS];
    vec4 cutoffs[MAX_LIGHTS];
    // Use Vec4 instead of vec3 for alignment, it takes up the same amount of bytes as aligning it properly and
    // this is easier

    vec4 extras; // Reflectivity is xy, Cut off and outer cut off are zw

    float reflectRefractFactor;
}
ubo;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 surfaceNormal;
layout(location = 2) in vec3 toLightVector[MAX_LIGHTS];
layout(location = 6) in vec3 toCameraVector;
layout(location = 7) in vec3 camLocation;
layout(location = 8) in vec3 reflectedVector;
layout(location = 9) in vec3 refractedVector;

layout(location = 0) out vec4 outColor;

void main()
{
    ivec2 norSize = textureSize(norSampler, 0);
    vec3  unitNormal;
    if (norSize.x == 1 && norSize.y == 1)
    {
        unitNormal = normalize(surfaceNormal); // Use the data provided in the model
    }
    else
    {
        unitNormal = normalize(texture(norSampler, fragTexCoord).rgb); // Use the data provieded in the Normal map
    }

    vec3 unitToCameraVector = normalize(toCameraVector);

    vec3 totalDiffuse  = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        float dist      = length(toLightVector[i]);
        float attFactor = ubo.attenuation[i].x + (ubo.attenuation[i].y * dist) + (ubo.attenuation[i].z * dist * dist);

        vec3 unitLightVector = normalize(toLightVector[i]);

        float brightness = dot(unitNormal, unitLightVector);
        brightness       = max(brightness, 0.0);

        if (CEL_SHADING_LEVELS > 0)
        {
            float level = floor(brightness * CEL_SHADING_LEVELS);
            brightness  = level / CEL_SHADING_LEVELS;
        }

        vec3  lightDirection          = -unitLightVector;
        vec3  reflectedLightDirection = reflect(lightDirection, unitNormal);
        float specularFactor          = dot(reflectedLightDirection, unitToCameraVector);
        specularFactor                = max(specularFactor, 0.0);
        float dampedFactor            = pow(specularFactor, ubo.extras.y);

        if (CEL_SHADING_LEVELS > 0)
        {
            float level  = floor(dampedFactor * CEL_SHADING_LEVELS);
            dampedFactor = level / CEL_SHADING_LEVELS;
        }

        float intensity = 1;
        if (ubo.direction[i].w > 0.5)
        {
            float theta   = dot(lightDirection, -normalize(ubo.direction[i].xyz));
            float epsilon = (ubo.cutoffs[i].x - ubo.cutoffs[i].y);
            intensity     = clamp(-(theta - ubo.cutoffs[i].y) / epsilon, 0.0, 1.0);
        }

        totalSpecular = totalSpecular + (dampedFactor * ubo.extras.x * ubo.lightColor[i].xyz) / attFactor * intensity;
        totalDiffuse  = totalDiffuse + (brightness * ubo.lightColor[i].xyz) * intensity;
    }
    totalDiffuse = max(totalDiffuse, 0.2);

    ivec2 speSize = textureSize(speSampler, 0);
    if (speSize.x != 1 && speSize.y != 1)
    {
        vec4 specMapColor = texture(speSampler, fragTexCoord);
        totalSpecular *= max(specMapColor.r, 0.2);
        // Set the lowest that the total Specular can be so that the model always has some specular light on it.
        if (specMapColor.g > 0.5)
        {
            totalDiffuse = vec3(1.0); // With a diffuse value of 1 models will look like they are glowing!
        }
    }

    outColor = vec4(totalDiffuse, 1.0) * texture(texSampler, fragTexCoord) + vec4(totalSpecular, 1.0);

    ivec2 cubeSize = textureSize(cubeMap, 0);
    if (ubo.reflectRefractFactor != 0)
    {
        vec4 reflectedColor = texture(cubeMap, reflectedVector);
        vec4 refractedColor = texture(cubeMap, refractedVector);
        vec4 cubeMapColor   = mix(reflectedColor, refractedColor, 0.5);
        outColor            = mix(outColor, cubeMapColor, ubo.reflectRefractFactor);
    }
}
