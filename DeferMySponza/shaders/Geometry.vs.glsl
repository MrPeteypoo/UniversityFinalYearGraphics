#version 450

/// The uniform buffer scene specific information.
layout (std140) uniform Scene
{
    mat4 projection;    //!< The projection transform which establishes the perspective of the vertex.
    mat4 view;          //!< The view transform representing where the camera is looking.

    vec3 camera;        //!< Contains the position of the camera in world space.
    vec3 ambience;      //!< The ambient lighting in the scene.
} scene;

layout (location = 0)   in  vec3    position;       //!< The local position of the current vertex.
layout (location = 1)   in  vec3    normal;         //!< The local normal vector of the current vertex.
layout (location = 2)   in  vec2    uv;             //!< The texture co-ordinates for the vertex, used for mapping a texture to the object.
layout (location = 3)   in  int     matID;          //!< The material ID of the instance being drawn.
layout (location = 4)   in  mat4x3  model;          //!< The model transform representing the position and rotation of the object in world space.

                        out vec3    worldPosition;  //!< The world position to be interpolated for the fragment shader.
                        out vec3    worldNormal;    //!< The world normal to be interpolated for the fragment shader.
                        out vec2    texturePoint;   //!< The texture co-ordinate for the fragment to use for texture mapping.
flat                    out int     materialID;     //!< Allows the fragment shader to fetch the correct material data.


/**
    Applies transformations to the vertex position to place it in the scene and outputs data to the fragment shader. 
*/
void main()
{
    // We need the position with a homogeneous value and we need to create the PVM transform.
    const vec4 homogeneousPosition  = vec4 (position, 1.0);
    const mat4 projectionViewModel  = scene.projection * scene.view * mat4 (model);

    // Set the outputs first.
    worldPosition   = model * homogeneousPosition;
    worldNormal     = mat3 (model) * normal;
    texturePoint    = uv;
    materialID      = matID;

    // Place the vertex in the correct position on-screen.
    gl_Position = projectionViewModel * homogeneousPosition;
}