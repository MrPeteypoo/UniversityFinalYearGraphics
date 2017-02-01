layout (location = 0) uniform sampler2D aliasedInput;           //!< The texture containing an aliased image.
layout (location = 1) uniform sampler2D blendWeightingResult;   //!< The results of the previous pass.

in vec2 textureCoordinate;
in vec4 offset;

out vec4 aliasedFragment;

/**
    The SMAA shader needs to be included before this shader with the SMAA_INCLUDE_PS flag.
*/
void main()
{
    // Perform 1x SMAA by passing 0 as the subsample indices parameter.
    aliasedFragment = SMAANeighborhoodBlendingPS (textureCoordinate, offset, aliasedInput, blendWeightingResult);
}