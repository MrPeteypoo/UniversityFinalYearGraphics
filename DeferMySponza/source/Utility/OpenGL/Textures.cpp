#include "Textures.hpp"


namespace util
{
    GLenum internalFormat (const size_t components) noexcept
    {
        switch (components)
        {
            case 1:
                return GL_R8;

            case 2:
                return GL_RG8;

            case 3:
                return GL_RGB8;

            case 4:
                return GL_RGBA8;
        }

        return 0;
    }
}