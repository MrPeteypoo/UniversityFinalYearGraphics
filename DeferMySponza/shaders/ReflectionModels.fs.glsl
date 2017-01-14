#version 450

/// Contains the properties of the material to be applied to the current fragment.
struct Material
{
    float   smoothness;     //!< Effects the distribution of specular light over the surface.
    float   reflectance;    //!< Effects the fresnel effect of dieletric surfaces.
    float   conductivity;   //!< Conductive surfaces absorb incoming light, causing them to be fully specular.
    float   transparency;   //!< How transparent the surface is.
    
    vec3    albedo;         //!< The base colour of the material.
    vec3    normalMap;      //!< The normal map of the material.
} material;


// Externals.
Material fetchMaterialProperties (const in vec2 uvCoordinates, const in float materialID);


// Forward declarations.
vec3 lambertDiffuse (const in vec3 E, const in float LDotN);
vec3 orenNayarDiffuse (const in vec3 L, const in vec3 N, const in vec3 V, const in vec3 E, const in float LdotN);

vec3 blinnPhongSpecular (const in vec3 L, const in vec3 N, const in vec3 V, const in vec3 E);
vec3 microfacetSpecular (const in vec3 L, const in vec3 N, const in vec3 V, const in vec3 E, const in float LdotN);


/**
    Sets the global instance of the material data used by reflection models.
*/
void setFragmentMaterial (const in vec2 uvCoordinates, const in float materialID)
{
    // Set our global material to the value returned by the material fetcher.
    material = fetchMaterialProperties (uvCoordinates, materialID);
}


/**
    Calculates the diffuse and specular component of a light with the given parameters.
    
    Params:
        L = The surface to light direction.
        N = The surface normal of the fragment.
        V = The surface to view/eye direction.
        E = The intensity of the light.
*/
vec3 calculateReflectance (const in vec3 L, const in vec3 N, const in vec3 V, const in vec3 E)
{
    // Determine whether we can actually light the surface.
    const float lambertian = max (dot (L, N), 0.0);

    if (lambertian > 0.0)
    {
        // Support physically based and non-physically based shading techniques.
        #if defined     LAMBERT_MICROFACET

            return  lambertDiffuse (E, lambertian) +
                    microfacetSpecular (L, N, V, E, lambertian);

        #elif defined   ORENNAYAR_MICROFACET

            return  orenNayarDiffuse (L, N, V, E, lambertian) +
                    microfacetSpecular (L, N, V, E, lambertian);

        #else

            return  lambertDiffuse (E, lambertian) + 
                    blinnPhongSpecular (L, N, V, E);

        #endif
    }

    return vec3 (0.0);
}


/**
    The most basic diffuse lighting model. Lambertian diffuse causes a relatively uniform reflectance by disregarding
    surface roughness.
*/
vec3 lambertDiffuse (const in vec3 E, const in float LDotN);
{
    return E * material.albedo * LDotN;
}


/**
    An inexpensive specular model which disregards the conductivity and roughness of a surface.
*/
vec3 blinnPhongSpecular (const in vec3 L, const in vec3 N, const in vec3 V, const in vec3 E)
{
    // First we need to interpret PBS parameters for the shading. Blinn-Phong also multiplies the shininess by four.

    const float maxShininess    = 128.f;
    const float shininess       = material.conductivity * maxShininess * 4.0;
    const vec3  specularColour  = vec3 (material.smoothness);

    // We need to determine the half vector between the light and view directions.
    const vec3  H       = normalize (L, V);
    const float HDotN   = max (dot (H, N), 0.0);

    // Finally calculate the specularity of the fragment.
    return E * specularColour * pow (HDotN, shininess);
}
