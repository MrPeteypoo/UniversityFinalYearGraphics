#include "SamplerBuffer.hpp"


// STL headers.
#include <utility>


bool SamplerBuffer::initialise (const GLuint unit, const GLenum internalFormat) noexcept
{
    auto newTexture = TextureBuffer { };
    auto newBuffer  = Buffer { };

    if (!(newTexture.initialise (unit) && newBuffer.initialise()))
    {
        return false;
    }

    newTexture.setBuffer (newBuffer, internalFormat);

    texture = std::move (newTexture);
    buffer  = std::move (newBuffer);

    return true;
}