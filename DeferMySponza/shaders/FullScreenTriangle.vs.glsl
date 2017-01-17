#version 450

layout (location = 0)   in  vec2    position;   //!< The position of the vertex.

                        out uint    lightIndex; //!< The index of the light being processed, assume this is gl_InstanceID.
/**
    Simply outputs the position of the vertex to the screen.
*/
void main()
{
    lightIndex = gl_InstanceID;
    gl_Position = vec4 (position, 0.0, 1.0);
}