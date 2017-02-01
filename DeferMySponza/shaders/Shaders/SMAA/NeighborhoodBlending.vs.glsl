layout (location = 0) in vec2 position; //!< The position of the current vertex.

out vec2 textureCoordinate;
out vec4 offset;

/**
    The SMAA shader needs to be included before this shader with the SMAA_INCLUDE_VS flag.
*/
void main()
{
    textureCoordinate = position * 0.5 + 0.5;
    SMAANeighborhoodBlendingVS (textureCoordinate, offset);
    gl_Position = vec4 (position, 1.0, 1.0);
}