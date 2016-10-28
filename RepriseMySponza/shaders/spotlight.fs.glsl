#version 450

/// The uniform buffer scene specific information.
layout (std140) uniform scene
{
    mat4    projection;     //!< The projection transform which establishes the perspective of the vertex.
    mat4    view;           //!< The view transform representing where the camera is looking.

    vec3    cameraPosition; //!< Contains the position of the camera in world space.
    vec3    ambience;       //!< The ambient lighting in the scene.
};

/// The uniform buffer containing lighting data.
layout (std140) uniform spotlight
{
    vec3	position;	    //!< The position of the light in world-space
    
    vec3    direction;      //!< The direction of the light in world-space.
    float   coneAngle;      //!< The angle of the cone.

    vec3	intensity;	    //!< The colour/intensity of the light.
    float   concentration;  //!< How concentrated the beam of a spot light is.

    float	aConstant;	    //!< The constant co-efficient for the attenutation formula.
    float	aLinear;	    //!< The linear co-efficient for the attenutation formula.
    float	aQuadratic;	    //!< The quadratic co-efficient for the attenutation formula.
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

/// Calculates the luminance attenuation value of the spot light based on the given distance. L is the surface-to-light 
/// unit direction vector. The distance of the fragment from the light is represented by dist.
/// Returns an attenuation factor ranging from 0 to 1.
float luminanceAttenuation (in const vec3 L, in const float dist);

/// Calculates the cone attenuation value of the spot light using angle information. L is the surface-to-light unit vector.
/// Returns an attenuation value factor from 0 to 1.
float coneAttenuation (in const vec3 L);

/// Calculates the diffuse and specular lighting to be applied based on the given colour. L is the surface-to-light unit 
/// direction vector. N should be the world normal direction of the surface. V is the surface-to-view unit direction
/// vector. The luminance of the lighting is represented by the lambertian value.
vec3 calculateContribution (in const vec3 L, in const vec3 N, in const vec3 V, 
    in const float lambertian, in const vec3 attenuatedIntensity);



void main()
{
    // Ensure we're using the correct colours.
    obtainMaterialProperties();

	// We're gonna need to store our contribution.
	vec3 contribution = vec3 (0.0);

    // Cache each component of the surface-to-light vector as it will be used multiple times.
    const vec3  toLight         = position - worldPosition;
    const float distanceToLight = length (toLight);

    // We need to calculate the surface-to-light direction and fragment normal.
	const vec3 L = toLight / distanceToLight;
    const vec3 N = normalize (worldNormal);

    // Check the lambertian value to check if we need to actually add anything.
    const float lambertian = max (dot (L, N), 0.0);

	if (lambertian > 0.0)
	{
        // Calcuate the attenuation factor to determine whether we are close enough to the surface.
        const float attenuation = luminanceAttenuation (L, distanceToLight) * coneAttenuation (L);

        if (attenuation > 0.0)
        {
		    // We'll need the surface-to-view unit vector to perform lighting calculations.
            const vec3 V = normalize (cameraPosition - worldPosition);

            contribution = calculateContribution (L, N, V, lambertian, intensity * attenuation);
        }
	}
    
    // Output the calculated fragment colour.
    fragmentColour = vec4 (contribution, 1.0);
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
    
    // The RGB values of the specular part is the specular colour.    
    material.specular = specularPart.rgb;
    
    // The alpha value of the specular part is the shininess value.
    material.shininess = specularPart.a;
    
    // The RGB values of the diffuse part are the diffuse colour.
    material.diffuse = diffusePart.rgb;

    // The alpha of the diffuse part represents the texture to use for the ambient map. -1 == no texture.
    if (diffusePart.a >= 0.0)
    {
        //material.texture    = texture (textures, vec3 (texturePoint, diffusePart.a)).rgb;
        //material.ambientMap = material.texture;
        const vec4 texture = texture (textures, vec3 (texturePoint, diffusePart.a));
        
        material.shininess  = (texture.r + texture.g + texture.b) / 3.0 * texture.a * 16.0;
        material.specular   = vec3 (1.0);
        material.texture    = vec3 (1.0);
        material.ambientMap = material.diffuse;
    }

    // Use the diffuse colour for the ambient map and don't apply an extra texture colour.
    else
    {
        material.texture    = vec3 (1.0);
        material.ambientMap = material.diffuse;
    }
    
    // The RGB values of the specular part is the specular colour.    
    //material.specular = specularPart.rgb;
    
    // The alpha value of the specular part is the shininess value.
    //material.shininess = specularPart.a;
}

float luminanceAttenuation (in const vec3 L, in const float dist)
{    
    // We need to construct Ci *= (pow (max {-R.L, 0}), p) / (Kc + kl * d + Kq * d * d).
    const float lighting    = max (dot (-direction, L), 0.0);
    const float numerator   = pow (lighting, concentration);
    const float denominator = aConstant + aLinear * dist + aQuadratic * dist * dist;
    
    // Return the final calculation.
    return numerator / denominator;
}

float coneAttenuation (in const vec3 L)
{
    // Cone attenuation is: fs := acos ((S.D)) > angle / 2. S = light to surface direction, D = light direction.
    const vec3  surface     = -L;
    const float lightAngle  = degrees (acos (max (dot (surface, direction), 0.0)));

    // Determine the half angle.
    const float halfAngle = coneAngle / 2.0;

    // Attenuate using smoothstep. Don't cut off at zero, maintains spotlight look.
    const float attenuation = lightAngle <= halfAngle ? smoothstep (1.0, 0.75, lightAngle / halfAngle) : 0.0;
    
    // Return the calculated attenuation factor.
    return attenuation;
}

vec3 calculateContribution (in const vec3 L, in const vec3 N, in const vec3 V,
    in const float lambertian, in const vec3 attenuatedIntensity)
{
    // Diffuse is easy, apply the texture and base colour with the given luminance.
    vec3 diffuseLighting = material.diffuse * material.texture * lambertian;

	// Only add specular lighting if the surface is shiny.
    vec3 specularLighting = vec3 (0.0);

    // Calculate the specular lighting.
    if (material.shininess > 0.0)
    {
        // We need to reflect the direction from the surface to the light for the specular calculations.
        vec3 R = reflect (L, N);
            
        // Finally use Kspecular = S * pow (-R.V, shininess) for the specular formula.
        specularLighting = material.specular * pow (max (dot (-R, V), 0), material.shininess);
    }

    return attenuatedIntensity * (diffuseLighting + specularLighting);
}
