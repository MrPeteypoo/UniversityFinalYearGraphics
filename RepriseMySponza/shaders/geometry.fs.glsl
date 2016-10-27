#version 450

/// The uniform buffer scene specific information.
layout (std140) uniform scene
{
    mat4    projection;     //!< The projection transform which establishes the perspective of the vertex.
    mat4    view;           //!< The view transform representing where the camera is looking.

    vec3    cameraPosition; //!< Contains the position of the camera in world space.
    vec3    ambience;       //!< The ambient lighting in the scene.
};

        uniform sampler2DArray  textures;       //!< The array of textures in the scene.
        uniform samplerBuffer   materials;      //!< A texture buffer filled with the required diffuse and specular properties for the material.
        uniform isamplerBuffer  materialIDs;    //!< A buffer containing the ID of the material for the instance to fetch from the materials buffer.

        in      vec3            worldPosition;  //!< The fragments position vector in world space.
        in      vec3            worldNormal;    //!< The fragments normal vector in world space.
        in      vec2            texturePoint;   //!< The interpolated co-ordinate to use for the texture sampler.
flat    in      int             instanceID;     //!< Used in fetching instance-specific data from the uniforms.

        out     vec4            fragmentColour; //!< The computed output colour of this particular pixel;


/// Contains the properties of the material to be applied to the current fragment.
struct Material
{
    vec3 ambientMap;    //!< The ambient colour to apply to the fragment.
    vec3 texture;       //!< The texture colour, if any to apply.
    vec3 diffuse;       //!< The standard diffuse colour.
    vec3 specular;      //!< The specular colour.
    float shininess;    //!< How shiny the surface is.
} material;


/// Updates the ambient, diffuse and specular colours from the materialTBO for this fragment.
void obtainMaterialProperties();


void main()
{
    // Ensure we're using the correct colours.
    obtainMaterialProperties();

    // Shade using ambient lighting.
    vec3 lighting = ambience * material.ambientMap;
    
    // Output the calculated fragment colour.
    fragmentColour = vec4 (lighting, 1.0);
}


int obtainMaterialID()
{
    /// This really isn't a nice way to do this, in future I will avoid this and just make the material ID an instance-specific 
    /// vertex attribute. However the alternative method is used here which features a samplerBuffer object and we then obtain
    /// the correct attribute using texelFetch().

    // Each instance is allocated 4-bytes of data for the material ID. We calculate the row by dividing the instance ID
    // by 4 and then the column by calculating the remainder.
    const ivec4 idRow = texelFetch (materialIDs, instanceID / 4);

    switch (instanceID % 4)
    {
        case 0:
            return idRow.r;

        case 1:
            return idRow.g;

        case 2:
            return idRow.b;

        case 3:
            return idRow.a;
    }
}

void obtainMaterialProperties()
{
    // We can use the instance ID to reconstruct the diffuse and specular colours from the RGBA material buffer.
    const int materialID = obtainMaterialID();

    // Each material is allocated 16 bytes of data for the diffuse colour and 16 bytes for the specular colour.
    const vec4 diffusePart  = texelFetch (materials, materialID);
    const vec4 specularPart = texelFetch (materials, materialID + 1);
    
    // The RGB values of the diffuse part are the diffuse colour.
    material.diffuse = diffusePart.rgb;

    // The alpha of the diffuse part represents the texture to use for the ambient map. -1 == no texture.
    if (diffusePart.a >= 0.0)
    {
        material.texture    = texture (textures, vec3 (texturePoint, diffusePart.a)).rgb;
        material.ambientMap = material.texture;
    }

    // Use the diffuse colour for the ambient map and don't apply an extra texture colour.
    else
    {
        material.texture    = vec3 (1.0);
        material.ambientMap = material.diffuse;
    }
    
    // The RGB values of the specular part is the specular colour.    
    material.specular = specularPart.rgb;
    
    // The alpha value of the specular part is the shininess value.
    material.shininess = specularPart.a;
}