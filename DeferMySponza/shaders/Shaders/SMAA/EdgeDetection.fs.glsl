layout (location = 0) uniform sampler2D aliasedInput;   //!< The texture containing an aliased image.
layout (location = 1) uniform sampler2D predication;    //!< An extra buffer to be used to scale the predication threshold. This can be a depth buffer.

in vec2 textureCoordinate;
in vec4 offsets[3];

out vec2 edgeDetection;

/**
    The SMAA shader needs to be included before this shader with the SMAA_INCLUDE_PS flag.
*/
void main()
{

    #if SMAA_PREDICATION
        edgeDetection = SMAAColorEdgeDetectionPS (textureCoordinate, offsets, aliasedInput, predication);
    #else
        edgeDetection = SMAAColorEdgeDetectionPS (textureCoordinate, offsets, aliasedInput);
    #endif
}