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
};

/*layout (std140) uniform Samplers
{
    const uint      size = 16;                  //!< How many samplers can be contained.
    samplerBuffer   materials;                  //!< Contains every material in the scene.
    sampler2DArray  array[textureArrayCount];   //!< An array of samplers containing different texture formats.
} samplers;*/


Material fetchMaterialProperties (const in vec2 uvCoordinates, const in float materialID)
{
    Material mat;
    mat.smoothness = 1.0;
    mat.reflectance = 1.0;
    mat.conductivity = 0.0;
    mat.transparency = 1.0;
    mat.albedo = vec3 (1.0);
    mat.normalMap = vec3 (0.0);
    return mat;
}