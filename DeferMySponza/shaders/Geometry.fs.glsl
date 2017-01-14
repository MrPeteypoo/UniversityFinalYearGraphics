#version 450

                        in  vec3    worldPosition;  //!< The fragments position vector in world space.
                        in  vec3    worldNormal;    //!< The fragments normal vector in world space.
                        in  vec2    texturePoint;   //!< The interpolated co-ordinate to use for the texture sampler.
flat                    in  uint    materialID;     //!< The material ID of the current instance.

layout (location = 0)   out vec3    position;       //!< The position of the fragment in the Gbuffer.
layout (location = 1)   out vec3    normal;         //!< The normal of the fragment in the Gbuffer.
layout (location = 2)   out vec3    materialInfo;   //!< The texture co-ordinates and material ID of the fragment in the Gbuffer.


/**
    Simply outputs the position, normal and material properties of the current fragment.
*/
void main()
{
    // Position maps perfectly.
    position = worldPosition;

    // Normals need to be normalised.
    normal = normalize (worldNormal);

    // For materials, the XY channels are texture co-ordintes and Z is the material ID.
    materialInfo = vec3 (texturePoint, materialID);
}