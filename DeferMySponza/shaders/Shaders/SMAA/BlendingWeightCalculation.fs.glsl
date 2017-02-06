layout (location = 0) uniform sampler2D edgeDetectionResult;    //!< The results of the previous pass.
layout (location = 1) uniform sampler2D areaTexture;            //!< The precalculated area texture.
layout (location = 2) uniform sampler2D searchTexture;          //!< The precalculated search texture.

in vec2 textureCoordinate;
in vec2 pixelCoordinate;
in vec4 offsets[3];

out vec4 blendingWeight;

/**
    The SMAA shader needs to be included before this shader with the SMAA_INCLUDE_PS flag.
*/
void main()
{
    // Perform 1x SMAA by passing 0 as the subsample indices parameter.
    blendingWeight = SMAABlendingWeightCalculationPS (textureCoordinate, pixelCoordinate, offsets,
        edgeDetectionResult, areaTexture, searchTexture, vec4 (0.0));
}