#pragma once

#if !defined    _UTILITY_OPENGL_BUFFERS_
#define         _UTILITY_OPENGL_BUFFERS_

// Personal headers.
#include <Rendering/Buffers/Buffer.hpp>


namespace util
{
    /// <summary>
    /// Binds a uniform buffer object to the given program using the data provided.
    /// </summary>
    /// <param name="buffer"> The UBO containing data to bind to. </param>
    /// <param name="program"> The OpenGL program to bind the UBO to. </param>
    /// <param name="name"> The name of the block being bound, this must exist in the given program. </param>
    /// <param name="bindingPoint"> Which uniform binding on the GPU should be used. </param>
    /// <param name="offsetIntoBuffer"> How many bytes exist before the data you wish to be used by the program. </param>
    /// <param name="blockSize"> How many bytes the block should be. </param>
    /// <returns> Whether the block was successfully bound to the program. </returns>
    bool bindBlockToProgram (const Buffer& buffer, const GLuint program, const GLchar* name, const GLuint bindingPoint,
        const GLintptr offsetIntoBuffer, const GLsizeiptr blockSize) noexcept;
}

#endif // _UTILITY_OPENGL_BUFFERS_