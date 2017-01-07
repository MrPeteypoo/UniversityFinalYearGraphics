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
    auto crap = Texture2D { };
    crap.allocateImmutableStorage (GL_RGBA32F, 0, 0);
    //crap.setBuffer (newBuffer, internalFormat);

    texture = std::move (newTexture);
    buffer  = std::move (newBuffer);

    return true;
}