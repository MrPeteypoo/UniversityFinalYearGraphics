#version 450

layout (std140) uniform LightViews
{
    #define LightViewMax 25
    
    uint    count;                      //!< How many transforms exist.
    mat4    transforms[LightViewMax];   //!< A collection of light view transforms.
} lightViews;


layout (location = 0)   uniform int     viewIndex;  //!< The index of the light view transform to use.

layout (location = 0)   in      vec3    position;   //!< The local position of the current vertex.
layout (location = 4)   in      mat4x3  model;      //!< The model transform representing the position and rotation of the object in world space.


/**
    Transforms the vertex position into light space for a depth pass.
*/
void main()
{
    // We need the position with a homogeneous value and we need to create the PVM transform.
    const vec4 homogeneousPosition  = vec4 (position, 1.0);
    const mat4 projectionViewModel  = lightViews.transforms[viewIndex] * mat4 (model);

    // Place the vertex in the correct position on-screen.
    gl_Position = projectionViewModel * homogeneousPosition;
}