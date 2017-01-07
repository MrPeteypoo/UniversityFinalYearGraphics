#include "Internals.hpp"


Materials::Internals::Internals (Internals&& move) noexcept
{
    *this = std::move (move); 
}


bool Materials::Internals::isInitialised() const noexcept 
{
    for (size_t i { 0 }; i < supportedResolutionCount; ++i)
    {
        if (!rgb[i].isInitialised() || !rgba[i].isInitialised())
        {
            return false;
        }
    }

    return true;
}

bool Materials::Internals::initialise (const GLuint startingIndex) noexcept
{
    for (GLuint i { 0 }; i < supportedResolutionCount; ++i)
    {
        if (!rgb[i].initialise (startingIndex + i) || 
            !rgba[i].initialise (startingIndex + supportedResolutionCount + i))
        {
            return false;
        }
    }

    return true;
}

void Materials::Internals::clean() noexcept
{
    for (GLuint i { 0 }; i < supportedResolutionCount; ++i)
    {
        rgb[i].clean();
        rgba[i].clean();
    }
}

void Materials::Internals::bind() const noexcept
{
    for (size_t i { 0 }; i < supportedResolutionCount; ++i)
    {
        const auto& rgbTexture = rgb[i];
        glBindTextureUnit (rgbTexture.getDesiredTextureUnit(), rgbTexture.getID());
                
        const auto& rgbaTexture = rgba[i];
        glBindTextureUnit (rgbTexture.getDesiredTextureUnit(), rgbTexture.getID());
    }
}

void Materials::Internals::unbind() const noexcept
{
    const auto start = rgb.front().getDesiredTextureUnit();
    for (GLuint i { 0 }; i < supportedResolutionCount * 2; ++i)
    {
        glBindTextureUnit (start + i, 0);
    }
}

Texture& Materials::Internals::get (const GLint resolution, const size_t components) noexcept
{
    auto index = size_t { 0 };

    for (auto size = minimumDimensions; size <= resolution; size * 2, ++index) { }

    switch (components)
    {
        case 3:
            return rgb[index];

        case 4:
            return rgba[index];
    }

    return rgb[0];
}

bool Materials::Internals::contains (const std::string& file) const noexcept
{
    return ids.find (file) != std::end (ids);
}