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
};

uniform usamplerBuffer  materials;      //!< Contains every material in the scene.
uniform sampler2DArray  textures[16];   //!< An array of samplers containing different texture formats.


Material fetchMaterialProperties (const in vec2 uvCoordinates, const in int materialID)
{
    // Materials can contain up to three maps, requiring two texel fetches.
    const int texelCount    = 2;
    const int materialIndex = materialID * texelCount;

    // Now we can fetch each component.
    const uvec4 propertiesAndAlbedo = texelFetch (materials, materialIndex);
    const uvec2 normal              = texelFetch (materials, materialIndex + 1).xy;

    // Components are array-depth pairs, this allows us to retrieve the correct map.
    const vec3 properties   = texture (textures[propertiesAndAlbedo.x], vec3 (uvCoordinates, propertiesAndAlbedo.y)).xyz;
    const vec4 albedo       = texture (textures[propertiesAndAlbedo.z], vec3 (uvCoordinates, propertiesAndAlbedo.w));
    const vec3 normalMap    = texture (textures[normal.x], vec3 (uvCoordinates, normal.y)).rgb;

    // Reflectance controls the fresnel effect of a material. Here we restrict the F0 co-efficient based conductivity.
    const float dielecticReflectance = 0.16;
    const float conductorReflectance = 1.0;

    // Finally we can construct the material.
    Material mat;
    mat.roughness       = pow (max (1.0 - properties.x, 0.0), 2.0);
    mat.conductivity    = pow (properties.z, 2.0);
    mat.reflectance     = pow (properties.y, 2.0) * mix (dielecticReflectance, conductorReflectance, mat.conductivity);

    mat.transparency    = albedo.a;
    mat.albedo          = albedo.rgb;
    mat.normalMap       = normalMap;
    return mat;
}