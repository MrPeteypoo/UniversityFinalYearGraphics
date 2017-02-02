#version 450

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
    int     viewIndex;      //!< Indicates which view transform to use, if this is -1 then the light doesn't cast a shadow.
};

layout (std140) uniform Spotlights
{
    #define SpotlightsMax 25
    
    uint        count;                  //!< How many lights exist in the scene.
    Spotlight   lights[SpotlightsMax];  //!< A collection of light data.
} spotlights;

layout (std140) uniform LightViews
{
    #define LightViewMax 25
    
    uint    count;                      //!< How many transforms exist.
    mat4    transforms[LightViewMax];   //!< A collection of light view transforms.
} lightViews;

layout (std140) uniform Scene
{
    mat4    projection;     //!< The projection transform which establishes the perspective of the vertex.
    mat4    view;           //!< The view transform representing where the camera is looking.

    vec3    camera;         //!< Contains the position of the camera in world space.
    int     shadowMapRes;   //!< How many pixels wide/tall the shadow maps are.
    vec3    ambience;       //!< The ambient lighting in the scene.
} scene;

layout (location = 0)   in  vec3    position;       //!< The local position of the current vertex.
layout (location = 1)   in  mat4x3  model;          //!< The model transform representing the position and rotation of the object in world space.

flat                    out uint    lightIndex;     //!< The instance ID maps directly to the index of the light.


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