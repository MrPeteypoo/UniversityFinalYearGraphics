layout (location = 0) uniform sampler2D aliasedInput; //!< The texture containing an aliased image.

in vec2 textureCoordinate;
in vec4 offsets[3];

out vec2 edgeDetection;

/**
    The SMAA shader needs to be included before this shader with the SMAA_INCLUDE_PS flag.
*/
void main()
{
    edgeDetection = SMAALumaEdgeDetectionPS (textureCoordinate, offsets, aliasedInput);
}