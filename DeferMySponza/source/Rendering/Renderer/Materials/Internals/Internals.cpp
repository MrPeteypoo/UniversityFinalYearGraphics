#include "Internals.hpp"


GLuint Materials::Internals::maxTexture = 0;
GLuint Materials::Internals::maxArrayDepth = 0;


Materials::Internals::Internals (Internals&& move) noexcept
{
    *this = std::move (move); 
}


bool Materials::Internals::isInitialised() const noexcept 
{
    if (!materials.isInitialised())
    {
        return false;
    }

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
    if (!materials.initialise (startingIndex, GL_RGBA32UI))
    {
        return false;
    }

    const auto start = startingIndex + 1;

    for (GLuint i { 0 }; i < supportedResolutionCount; ++i)
    {
        if (!rgb[i].initialise (start + i ) || 
            !rgba[i].initialise (start + supportedResolutionCount + i))
        {
            return false;
        }
    }

    auto integer = GLint { };
    glGetIntegerv (GL_MAX_TEXTURE_SIZE, &integer);
    maxTexture = static_cast<GLuint> (integer);
    
    glGetIntegerv (GL_MAX_ARRAY_TEXTURE_LAYERS, &integer);
    maxArrayDepth = static_cast<GLuint> (integer);
    return true;
}


void Materials::Internals::clean() noexcept
{
    materials.clean();

    for (GLuint i { 0 }; i < supportedResolutionCount; ++i)
    {
        rgb[i].clean();
        rgba[i].clean();
    }
}


void Materials::Internals::bind() const noexcept
{
    glBindTextureUnit (materials.texture.getDesiredTextureUnit(), materials.texture.getID());

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
    constexpr auto arrays   = GLsizei { supportedResolutionCount * 2 };
    constexpr auto extra    = GLsizei { 1 };
    constexpr auto count    = arrays + extra;

    glBindTextures (materials.texture.getDesiredTextureUnit(), count, nullptr);
}


bool Materials::Internals::contains (const std::string& file) const noexcept
{
    return ids.find (file) != std::end (ids);
}


bool Materials::Internals::areDimensionsSupported (const size_t width, const size_t height) noexcept
{
    // Ensure more than zero, width is equal to height, is power of two and is supported range.
    return  width > 0 &&
            width == height &&
            (width & (width - 1)) == 0 &&
            (width == 1 || (width >= minimumDimensions && width <= maximumDimensions && width <= maxTexture));
}


std::pair<GLuint, Texture2DArray*> Materials::Internals::get (const size_t components, const size_t dimensions) noexcept
{
    auto index = size_t { 0 };

    for (auto size = minimumDimensions; size <= dimensions; size * 2, ++index) { }

    switch (components)
    {
        case 3:
            return { static_cast<GLuint> (index), &rgb[index] };

        case 4:
            return { static_cast<GLuint> (supportedResolutionCount + index), &rgba[index] };
    }

    return { 0, nullptr };
}