#include "SamplerBuffer.hpp"


// STL headers.
#include <utility>


bool SamplerBuffer::initialise (const GLuint unit) noexcept
{
    auto newTexture = TextureBuffer { };
    auto newBuffer  = Buffer { };

    if (!(newTexture.initialise (unit) && newBuffer.initialise()))
    {
        return false;
    }

    texture = std::move (newTexture);
    buffer  = std::move (newBuffer);

    return true;
}


void SamplerBuffer::specifyBufferFormat (const GLenum internalFormat) noexcept
{
    texture.setBuffer (buffer, internalFormat);
}