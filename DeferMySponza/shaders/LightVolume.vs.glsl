#version 450

/// The uniform buffer scene specific information.
layout (std140) uniform Scene
{
    mat4    projection;         //!< The projection transform which establishes the perspective of the vertex.
    mat4    view;               //!< The view transform representing where the camera is looking.

    vec3    cameraPosition;     //!< Contains the position of the camera in world space.
    vec3    ambience;           //!< The ambient lighting in the scene.
} scene;

layout (location = 0)   in  vec3    position;   //!< The local position of the current vertex.
layout (location = 1)   in  mat4x3  model;      //!< The model transform representing the position and rotation of the object in world space.

flat                    out uint    lightIndex; //!< The instance ID maps directly to the index of the light.


/**
    Applies transformations to the vertex position and sets the light index.
*/
void main()
{
    // Handle the light index.
    lightIndex = gl_InstanceID;

    // We need the position with a homogeneous value and we need to create the PVM transform.
    const vec4 homogeneousPosition  = vec4 (position, 1.0);
    const mat4 projectionViewModel  = scene.projection * scene.view * mat4 (model);

    // Place the vertex in the correct position on-screen.
    gl_Position = projectionViewModel * homogeneousPosition;
}
