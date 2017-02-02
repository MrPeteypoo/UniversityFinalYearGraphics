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

layout (std140) uniform Scene
{
    mat4    projection;     //!< The projection transform which establishes the perspective of the vertex.
    mat4    view;           //!< The view transform representing where the camera is looking.

    vec3    camera;         //!< Contains the position of the camera in world space.
    int     shadowMapRes;   //!< How many pixels wide/tall the shadow maps are.
    vec3    ambience;       //!< The ambient lighting in the scene.
} scene;

        in  vec3    worldPosition;  //!< The fragments position vector in world space.
        in  vec3    worldNormal;    //!< The fragments normal vector in world space.
        in  vec2    texturePoint;   //!< The interpolated co-ordinate to use for the texture sampler.
flat    in  int     materialID;     //!< Used in fetching instance-specific data from the uniforms.

        out vec4    fragmentColour; //!< The calculated colour of the fragment.


/// External functions.
void setFragmentMaterial (const in vec2 uvCoordinates, const in int materialID);
vec3 directionalLightContributions (const in vec3 normal, const in vec3 view);
vec3 pointLightContributions (const in vec3 position, const in vec3 normal, const in vec3 view);
vec3 spotlightContributions (const in vec3 position, const in vec3 normal, const in vec3 view);


/**
    Iterates through each light, calculating its contribution to the current fragment.
*/
void main()
{
    // Calculate the required lighting components.
    const vec3 q = worldPosition;
    const vec3 n = normalize (worldNormal);
    const vec3 v = normalize (scene.camera - q);

    // Retrieve the material properties and use it for lighting calculations.
    setFragmentMaterial (texturePoint, materialID);

    // Accumulate the contribution of every light.
    const vec3 lighting =   directionalLightContributions (n, v) +
                            pointLightContributions (q, n, v) +
                            spotlightContributions (q, n, v);
    
    // Put the equation together and we get...
    const vec3 colour = scene.ambience + lighting;
    
    // Output the calculated fragment colour.
    fragmentColour = vec4 (colour, material.transparency);
}