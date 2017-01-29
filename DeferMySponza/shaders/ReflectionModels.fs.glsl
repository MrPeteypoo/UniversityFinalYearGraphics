#version 450

/// Contains the properties of the material to be applied to the current fragment.
struct Material
{
    float   roughness;      //!< Effects the distribution of specular light over the surface.
    float   reflectance;    //!< Effects the fresnel effect of dieletric surfaces.
    float   conductivity;   //!< Conductive surfaces absorb incoming light, causing them to be fully specular.
    float   transparency;   //!< How transparent the surface is.
    
    vec3    albedo;         //!< The base colour of the material.
    vec3    normalMap;      //!< The normal map of the material.
} material;


// Globals.
const float pi              = 3.14159;
const float maxShininess    = 128.f;


// Externals.
Material fetchMaterialProperties (const in vec2 uvCoordinates, const in int materialID);


// Forward declarations.
vec3 lambertDiffuse (const in float lDotN);
vec3 blinnPhongSpecular (const in vec3 l, const in vec3 n, const in vec3 v);

vec3 disneyDiffuse (const in float hDotV, const in float vDotN, const in float lDotN);
vec3 microfacetSpecular (const in vec3 l, const in vec3 n, const in vec3 h,
    const in float hDotV, const in float lDotN, const in float vDotN);
    
vec3 fresnelReflectance (const in vec3 albedo, const in float lDotH);
float geometricAttenuation (const in float lDotH, const in float hDotV);
float distribution (const in float hDotN);

vec3 halfVector (const in vec3 l, const in vec3 v);


/**
    Sets the global instance of the material data used by reflection models.
*/
void setFragmentMaterial (const in vec2 uvCoordinates, const in int materialID)
{
    // Set our global material to the value returned by the material fetcher.
    material = fetchMaterialProperties (uvCoordinates, materialID);
}


/**
    Calculates the diffuse and specular component of a light with the given parameters.
    
    Params:
        l = The surface to light direction.
        n = The surface normal of the fragment.
        v = The surface to view/eye direction.
        e = The intensity of the light.
*/
vec3 calculateReflectance (const in vec3 l, const in vec3 n, const in vec3 v, const in vec3 e)
{
    // Determine whether we can actually light the surface.
    const float lDotN = max (dot (l, n), 0.0);

    if (lDotN <= 0.0)
    {
        return vec3 (0.0);
    }

    // Support physically based and non-physically based shading techniques.
    #ifdef _PHYSICALLY_BASED_SHADING_       
        
        // Conductive surfaces absorb light so no diffuse reflection occurs.
        const float diffuseContribution = 1.0 - material.conductivity;

        // We need the half vector for reflectance calculations.
        const vec3  h       = halfVector (l, v);
        const float hDotV   = max (dot (h, v), 0.0);
        const float vDotN   = max (dot (v, n), 0.0001);

        // Calculate and scale diffuse and specular reflectance.
        return e * (disneyDiffuse (hDotV, vDotN, lDotN) * diffuseContribution +
                    microfacetSpecular (l, n, h, hDotV, lDotN, vDotN));

    #else
        
        return e * (lambertDiffuse (lDotN) + blinnPhongSpecular (l, n, v));
        /*return e * (lambertDiffuse (lDotN) * diffuseContribution +
                    microfacetSpecular (l, n, h, hDotV, lDotN, vDotN));*/

    #endif
}


/**
    The most basic diffuse lighting model. Lambertian diffuse causes a relatively uniform reflectance by disregarding
    surface roughness.
*/
vec3 lambertDiffuse (const in float lDotN)
{
    return material.albedo * lDotN;
}


/**
    An inexpensive specular model which disregards the conductivity and roughness of a surface.
*/
vec3 blinnPhongSpecular (const in vec3 l, const in vec3 n, const in vec3 v)
{
    // First we need to interpret PBS parameters for the shading. Blinn-Phong also multiplies the shininess by four.
    const float shininess       = material.conductivity * maxShininess * 4.0;
    const vec3  specularColour  = vec3 (1.0 - material.roughness);

    // We need to determine the half vector between the light and view directions.
    const vec3  h       = halfVector (l, v);
    const float hDotN   = max (dot (h, n), 0.0);

    // Finally calculate the specularity of the fragment.
    return shininess > 0.0 ? specularColour * pow (hDotN, shininess) : vec3 (0.0);
}


/**
    A state-of-the-art diffuse model from Disney as presented here (slide 17): 
    http://blog.selfshadow.com/publications/s2016-shading-course/hoffman/s2016_pbs_recent_advances_v2.pdf
*/
vec3 disneyDiffuse (const in float hDotV, const in float vDotN, const in float lDotN)
{
    // The base colour is simply the albedo.
    const vec3 baseColour = material.albedo;

    // We need to apply a linear correction to stop the reflectance being higher than the incoming illumination.
    // http://blog.selfshadow.com/publications/s2014-shading-course/frostbite/s2014_pbs_frostbite_slides.pdf
    const float correction = mix (1.0, 0.66, material.roughness);

    // We need the angles of incidence to calculate fresnel and retro-reflection weightings.
    const float angleD = hDotV;
    const float angleV = vDotN;

    // Calculate fresnel weightings FL and FV.
    const float fresnelL = pow (1.0 - lDotN, 5.0);
    const float fresnelV = pow (1.0 - vDotN, 5.0);

    // Now we need a retro-reflection weighting RR.
    const float roughReflection = 2.0 * material.roughness * pow (hDotV, 2.0);

    // We can start putting the formula together now. 
    // fLambert = baseColour / PI.
    const vec3 lambert = baseColour / pi;

    // fretro-reflection: fLambert * RR * (FL + FV + FL * FV * (RR - 1)).
    const vec3 retroReflection = lambert * roughReflection * 
        (fresnelL + fresnelV + fresnelL * fresnelV * (roughReflection - 1.0));
    
    // fd = fLambert * (1 - 0.5 * FL) * (1 - 0.5 * FV) + fretro-reflection.
    return (lambert * (1.0 - 0.5 * fresnelL) * (1.0 - 0.5 * fresnelV) + retroReflection) * correction;
}


/**
    Using a similar structure to the Cook-Torrance model, calculates the specular lighting of the fragment using
    fresnel, geometry and distribution components:
    http://blog.selfshadow.com/publications/s2016-shading-course/hoffman/s2016_pbs_recent_advances_v2.pdf
*/
vec3 microfacetSpecular (const in vec3 l, const in vec3 n, const in vec3 h,
    const in float hDotV, const in float lDotN, const in float vDotN)
{
    // Conductive materials reflect using their albedo, everything else reflects using white.
    const vec3 albedo = mix (vec3 (material.reflectance), material.albedo, material.conductivity);

    // Perform the required dot products.
    const float lDotH = max (dot (l, h), 0.0);
    const float hDotN = max (dot (h, n), 0.0);

    // Calculate the three attenuation components.
    const vec3  f = fresnelReflectance (albedo, lDotH);
    //const float g = geometricAttenuation (lDotH, hDotV);
    const float g = geometricAttenuation (lDotN, vDotN);
    const float d = distribution (hDotN);
    //const vec3  f = albedo;
    //const float g = 1.0;
    //const float d = 1.0;

    // Calculate the denominator.
    const float denom = 4.0 * lDotN * vDotN;

    // Return the specular effect.
    return (f * g * d) / denom;
}


/**
    Calcualtes the fresnel effect for specular lighting based on Schlick's approximation.
*/
vec3 fresnelReflectance (const in vec3 albedo, const in float lDotH)
{
    // F(0) = F0 + (1 - F0) * pow (1 - cos(0), 5).
    return albedo + (1.0 - albedo) * pow (1.0 - lDotH, 5.0);
}


/**
    Calculates an attenuation factor representing self-shadowing based on the Height-Correlated Smith:
    http://jcgt.org/published/0003/02/03/
*/
float geometricAttenuation (const in float lDotH, const in float hDotV)
{
    const float heavisideLDotH = lDotH > 0.0 ? 1.0 : 0.0;
    const float heavisideHDotV = hDotV > 0.0 ? 1.0 : 0.0;

    return (heavisideLDotH * heavisideHDotV) / (1.0 + material.roughness + material.roughness);
}


/**
    Calculate a distribution attenuation factor using either GGX, Blinn-Phong or Beckmann:
    http://blog.selfshadow.com/publications/s2013-shading-course/rad/s2013_pbs_rad_notes.pdf
    http://blog.selfshadow.com/publications/s2013-shading-course/lazarov/s2013_pbs_black_ops_2_notes.pdf
*/
float distribution (const in float hDotN)
{
    const float hDotNSqr = hDotN * hDotN;
    const float roughSqr = material.roughness * material.roughness;

    // GGX.
    //return material.roughness / (pi * pow (hDotNSqr * (material.roughness - 1.0) + 1.0, 2.0));

    // Blinn-Phong.
    //return ((material.roughness + 2.0) / 8.0) * pow (hDotN, material.roughness * 128.f);

    // Beckmann.
    const float tanNumerator    = 1.0 - hDotNSqr;
    const float tanDenominator  = hDotNSqr * roughSqr;
    const float tangent         = tanNumerator / tanDenominator;

    const float exponential = exp (-tangent);
    const float denominator = pi * roughSqr * (hDotNSqr * hDotNSqr);
    return exponential / denominator;
}


/**
    Calculates the vector half way between the given surface-to-light and surface-to-viewer directions.
*/
vec3 halfVector (const in vec3 l, const in vec3 v)
{
    return normalize (l + v);
}