#include "Texture.hpp"


// STL headers.
#include <utility>


Texture::Texture (Texture&& move) noexcept
{
    *this = std::move (move);
}


Texture& Texture::operator= (Texture&& move) noexcept
{
    if (this != &move)
    {
        clean();

        m_texture       = move.m_texture;
        m_unit          = move.m_unit;
        move.m_texture  = 0U;
        move.m_unit     = 0U;
    }

    return *this;
}


void Texture::clean() noexcept
{
    if (isInitialised())
    {
        glDeleteTextures (1, &m_texture);
        m_texture   = 0U;
        m_unit      = 0U;
    }
}