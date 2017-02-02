#version 450

layout (location = 0)   in  vec2    position;       //!< The position of the vertex.

flat                    out uint    lightIndex;     //!< The index of the light being processed, assume this is gl_InstanceID.
                        out vec4    lightSpacePos;  //!< The position of the vertex in light space.
/**
    Simply outputs the position of the vertex to the screen.
*/
void main()
{
    lightIndex      = gl_InstanceID;
    lightSpacePos   = vec4 (position, 0.0, 1.0);
    gl_Position     = vec4 (position, 0.0, 1.0);
}