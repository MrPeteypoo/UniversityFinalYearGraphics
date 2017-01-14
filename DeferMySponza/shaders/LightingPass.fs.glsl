#version 450

layout (std140) uniform GBuffer
{
    uniform sampler2DRect positions;   //!< Contains the world position of objects at every pixel.
    uniform sampler2DRect normals;     //!< Contains the world normal of objects at every pixel.
    uniform sampler2DRect materials;   //!< Contains the texture co-ordinate and material ID of objects at every pixel.
} gbuffer;

layout (std140) uniform Scene
{
    mat4 projection;    //!< The projection transform which establishes the perspective of the vertex.
    mat4 view;          //!< The view transform representing where the camera is looking.

    vec3 camera;        //!< Contains the position of the camera in world space.
    vec3 ambience;      //!< The ambient lighting in the scene.
} scene;

flat    in  uint    lightIndex;     //!< The index of the light volume being rendered. 
        out vec3    reflectedLight; //!< The light contribution of the lighting pass.


// External functions.
void setFragmentMaterial (const in vec2 uvCoordinates, const in float materialID);
vec3 directionalLightContributions (const in vec3 normal, const in vec3 view);
vec3 pointLightContribution (const in uint index, const in vec3 position, const in vec3 normal, const in vec3 view);
vec3 spotlightContribution (const in uint index, const in vec3 position, const in vec3 normal, const in vec3 view);


// Forward declarations.
subroutine vec3 LightingPass (const in vec3 position, const in vec3 normal);
vec3 viewDirection (const in vec3 position);


// Subroutines.
layout (location = 0) subroutine uniform LightingPass lightingPass; //!< Determines whether global, point or spot lighting calculations will occur.


/**
    Performs a lighting pass on a deferred shaded scene.
*/
void main()
{
    // Retrieve the location properties of the current fragment.
    const vec3 Q = texelFetch (gbuffer.positions, gl_FragCoord.rg).rgb;
    const vec3 N = texelFetch (gbuffer.normals, gl_FragCoord.rg).rgb;

    // The UV components are texture co-ordinates and the third component is a material ID.
    const vec3 material = texelFetch (gbuffer.materials, gl_FragCoord.rg).rgb;
    setFragmentMaterial (gbufferMat.xy, gbufferMat.z);
    
    // Apply lighting.
    reflectedLight = lightingPass (Q, N);
}


/**
    Calculates the ambient and directional light contributions.
*/
layout (index = 0) subroutine (LightingPass)
vec3 globalLightPass (const in vec3 position, const in vec3 normal)
{
    // Calculate the direction from the fragment to the viewer.
    return scene.ambience + directionalLightContributions (N, viewDirection (position));
}


/**
    Calculates the lighting contribution of a point light.
*/
layout (index = 1) subroutine (LightingPass)
vec3 pointLightPass (const in vec3 position, const in vec3 normal)
{
    return pointLightContribution (lightIndex, position, normal, viewDirection (position));
}


/**
    Calculates the lighting contribution of a point light.
*/
layout (index = 2) subroutine (LightingPass)
vec3 spotlightPass (const in vec3 position, const in vec3 normal)
{
    return spotlightContribution (lightIndex, position, normal, viewDirection (position));
}


/** 
    Calculates the direction from the surface to the viewer.
*/
vec3 viewDirection (const in vec3 position)
{
    return normalize (scene.camera - position);
}