#version 330 core

// global data struct
struct GlobalData {
    float ka;    // Ambient term
    float kd;    // Diffuse term
    float ks;    // Specular term
};

// material struct
struct Material {
    vec4 cAmbient;     // Ambient term
    vec4 cDiffuse;     // Diffuse term
    vec4 cSpecular;    // Specular term
    float shininess;   // Specular exponent
};

// light struct
struct Light {
    int lightType;   // 0 - point, 1 - directional, 2 - spot

    vec4 color;      // light colour
    vec3 function;   // Attenuation function

    vec4 pos;        // Not applicable to directional lights
    vec4 dir;        // Not applicable to point lights

    float spotPenumbra;  // Only applicable to spot lights
    float spotAngle;     // Only applicable to spot lights
};

// vertex and norm in world space (for phong)
in vec3 worldSpacePos;
in vec3 worldSpaceNorm;

// uniforms
uniform GlobalData globalData;
uniform int numLights;
uniform Light lights[16];
uniform Material objMaterial;

uniform vec3 cameraPos;

// colour
out vec4 fragColour;

// helper for clamping vector colours to [0,1] colour values
vec4 clampToColour(vec4 illumination) {
    // min(max(I, 0), 1). clamps to range [0, 1]
    vec4 colour = illumination;
    colour.x = min(max(colour.x, 0.0), 1.0);
    colour.y = min(max(colour.y, 0.0), 1.0);
    colour.z = min(max(colour.z, 0.0), 1.0);

    return colour;
}

// helper for calculating the falloff coefficient for spot lights
float spotFalloff(float angleToDir, float innerAngle, float outerAngle) {
    if (angleToDir > outerAngle) {
        return 0.0;
    } else if (angleToDir > innerAngle) {
        float frac = (angleToDir - innerAngle) / (outerAngle - innerAngle);
        return 1.0 - (-2.0*pow(frac, 3) + 3.0*pow(frac, 2));
    } else {
        return 1.0;
    }
}

// helper for calculating attenuation factor
float getFatt(Light light, vec4 dirToLight) {
    float distToLight = length(dirToLight);
    switch(light.lightType) {
    case 0: // point
        return min(1.0, 1.0 / dot(light.function, vec3(1.0, distToLight, pow(distToLight, 2))));
        break;

    case 1: // directional
        return 1.0;
        break;

    case 2: // spot
        float fatt = min(1.0, 1.0 / dot(light.function, vec3(1.0, distToLight, pow(distToLight, 2))));
        float angleToDir = acos(dot(normalize(-dirToLight), normalize(light.dir)));
        return fatt * spotFalloff(angleToDir, light.spotAngle - light.spotPenumbra, light.spotAngle);
        break;

    default: // not a supported light type or some error
        return 0;
        break;
    }
}

// helper for calculating the direction from the object position to the light source
vec4 getDirToLight(Light light, vec3 worldSpacePos) {
    switch(light.lightType) {
    case 0: // point
        return light.pos - vec4(worldSpacePos, 1.0);
        break;

    case 1: // directional
        return -normalize(light.dir);
        break;

    case 2: // spot
        return light.pos - vec4(worldSpacePos, 1.0);
        break;

    default: // not a supported light type or some error
        return vec4(0.0);
        break;
    }
}

void main() {
    vec4 normalizedNorm = vec4(normalize(worldSpaceNorm), 0.0);

    // AMBIENT
    vec4 illumination = globalData.ka * objMaterial.cAmbient;

    // iterate through lights
    for (int i = 0; i < numLights; i++) {
        Light light = lights[i];

        vec4 dirToLight = getDirToLight(light, worldSpacePos);
        vec4 commonTerm = getFatt(light, dirToLight) * light.color;

        // DIFFUSE
        float normDot = dot(normalize(dirToLight), normalizedNorm);

        if (normDot > 0) {
            illumination += commonTerm * globalData.kd * objMaterial.cDiffuse
                            * normDot;
        }

        // SPECULAR
        vec4 reflectedLight = normalize(reflect(-normalize(dirToLight), normalizedNorm));
        vec4 dirToCamera = vec4(normalize(cameraPos - worldSpacePos), 0.0);
        float reflectDot = dot(reflectedLight, dirToCamera);

        if (normDot > 0 && reflectDot > 0) {
            illumination += commonTerm * globalData.ks * objMaterial.cSpecular
                            * pow(reflectDot, objMaterial.shininess);
        }
    }

    // clamp to [0,1]
    fragColour = clampToColour(illumination);
}

