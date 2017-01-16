#version 450

/// A universal light which casts light on all objects.
struct DirectionalLight
{
    vec3 direction; //!< The direction of the light.
    vec3 intensity; //!< The colour and brightness of the light.
};

/// An area light which uniformly distributes light within a range.
struct PointLight
{
    vec3    position;   //!< The world location of the light.
    float   range;      //!< The maximum range of the light.

    vec3    intensity;  //!< The colour and brightness of the light.
    float   aConstant;  //!< The constant co-efficient for the attenuation formula.
    
    float   aLinear;    //!< The linear co-efficient for the attenuation formula.
    float   aQuadratic; //!< The quadratic co-efficient for the attenuation formula.
};

/// A constricted area light which distributes light like a cone.
struct Spotlight
{
    vec3    position;       //!< The world location of the light.
    float   coneAngle;      //!< The angle of the cone in degrees.
    
    vec3    direction;      //!< The direction of the light.
    float   range;          //!< The maximum range of the light.

    vec3    intensity;      //!< The colour and brightness of the light.
    float   concentration;  //!< Effects how focused the light is and how it distributes away from the centre.
    float   aConstant;      //!< The constant co-efficient for the attenuation formula.
    
    float   aLinear;        //!< The linear co-efficient for the attenuation formula.
    float   aQuadratic;     //!< The quadratic co-efficient for the attenuation formula.
};

layout (std140) uniform DirectionalLights
{
    #define DirectionalLightsMax 341
    
    uint                count;                          //!< How many lights exist in the scene.
    DirectionalLight    lights[DirectionalLightsMax];   //!< A collection of light data.
} directionalLights;

layout (std140) uniform PointLights
{
    #define PointLightsMax 292
    
    uint        count;                  //!< How many lights exist in the scene.
    PointLight  lights[PointLightsMax]; //!< A collection of light data.
} pointLights;

layout (std140) uniform Spotlights
{
    #define SpotlightsMax 215
    
    uint        count;                  //!< How many lights exist in the scene.
    Spotlight   lights[SpotlightsMax];  //!< A collection of light data.
} spotlights;


// Externals.
vec3 calculateReflectance (const in vec3 L, const in vec3 N, const in vec3 V, const in vec3 E);


/**
    Calculates the lighting contribution of a directional light at the given index.
*/
vec3 directionalLightContribution (const in uint index, const in vec3 normal, const in vec3 view)
{
    // Directional lights don't need attenuation.
    const DirectionalLight light = directionalLights.lights[index];
    const vec3 L = -light.direction.xyz;
    const vec3 E = light.intensity.xyz;

    return calculateReflectance (L, normal, view, E);
}


/**
    Calculates the lighting contribution of a point light at the given index.
*/
vec3 pointLightContribution (const in uint index, const in vec3 position, const in vec3 normal, const in vec3 view)
{
    // Point lights use uniform attenuation.
    const PointLight light = pointLights.lights[index];

    // We'll need the distance and direction from the light to the surface for attenuation.
    const vec3  bigL    = light.position - position;
    const float dist    = length (bigL);
    const vec3  L       = bigL / dist;

    // Point light attenuation formula is: 1 / (Kc + Kl * d + Kq * d * d).
    const float attenuation = light.range >= dist ? 
        1.0 / (light.aConstant + light.aLinear * dist + light.aQuadratic * dist * dist) :
        0.0;

    // Scale the intensity accordingly.
    const vec3 E = light.intensity * attenuation;
    
    return calculateReflectance (L, normal, view, E);
}


/**
    Calculates the lighting contribution of a spotlight at the given index.
*/
vec3 spotlightContribution (const in uint index, const in vec3 position, const in vec3 normal, const in vec3 view)
{
    // Spotlights require a special luminance attenuation and cone attenuation.
    const Spotlight light = spotlights.lights[index];

    // We'll need the distance and direction from the light to the surface for attenuation.
    const vec3  bigL    = light.position - position;
    const float dist    = length (bigL);
    const vec3  L       = bigL / dist;
    const vec3  R       = light.direction;
    const float p       = light.concentration;

    // Luminance attenuation formula is: pow (max {-R.L, 0}), p) / (Kc + kl * d + Kq * d * d).
    const float luminance = light.range >= dist ? 
        pow (max (dot (-R, L), 0.0), p) / (light.aConstant + light.aLinear * dist + light.aQuadratic * dist * dist) :
        0.0;

    // Cone attenuation is: acos ((-L.R)) > angle / 2. Attenuate using smoothstep.
    const float lightAngle  = degrees (acos (max (dot (-L, R), 0.0)));
    const float halfAngle   = light.coneAngle / 2.0;
    const float coneCutOff  = lightAngle <= halfAngle ? smoothstep (1.0, 0.75, lightAngle / halfAngle) : 0.0;

    // Scale the intensity accordingly.
    const vec3 E = light.intensity * luminance * coneCutOff;
    
    return calculateReflectance (L, normal, view, E);
}


/**
    Calculates the lighting contribution of every directional light in the scene.
*/
vec3 directionalLightContributions (const in vec3 normal, const in vec3 view)
{
    vec3 lighting = vec3 (0.0);

    for (uint i = 0; i < directionalLights.count; ++i)
    {
        lighting += directionalLightContribution (i, normal, view);
    }

    return lighting;
}


/**
    Calculates the lighting contribution of every point light in the scene.
*/
vec3 pointLightContributions (const in vec3 position, const in vec3 normal, const in vec3 view)
{
    vec3 lighting = vec3 (0.0);

    for (uint i = 0; i < pointLights.count; ++i)
    {
        lighting += pointLightContribution (i, position, normal, view);
    }

    return lighting;
}


/**
    Calculates the lighting contribution of every spotlight in the scene.
*/
vec3 spotlightContributions (const in vec3 position, const in vec3 normal, const in vec3 view)
{
    vec3 lighting = vec3 (0.0);

    for (uint i = 0; i < spotlights.count; ++i)
    {
        lighting += spotlightContribution (i, position, normal, view);
    }

    return lighting;
}