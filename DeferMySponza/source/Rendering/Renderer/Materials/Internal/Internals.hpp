#pragma once

#if !defined    _RENDERING_RENDERER_MATERIALS_INTERNAL_INTERNALS_
#define         _RENDERING_RENDERER_MATERIALS_INTERNAL_INTERNALS_

// STL headers.
#include <array>
#include <string>


// Engine headers.
#include <glm/vec2.hpp>


// Personal headers.
#include <Rendering/Renderer/Materials/Materials.hpp>


/// <summary>
/// Contains internal data which isn't required at run time.
/// </summary>
struct Materials::Internals final
{
    constexpr static auto supportedResolutionCount = 6; //!< How many different image resolutions are supported.
    using Textures      = std::array<Texture2DArray, supportedResolutionCount>;
    using TextureIDs    = std::unordered_map<std::string, glm::uvec3>;


    Textures    rgb     { };    //!< Contains a 2D texture array for each supported texture resolution in the RGB format.
    Textures    rgba    { };    //!< Contains a 2D texture array for each supported texture resolution in the RGBA format.
    TextureIDs  ids     { };    //!< Maps file locations to a vector containing number of components, sampler index and array depth values for materials.


    Internals() noexcept { }
    Internals (Internals&& move) noexcept { *this = std::move (move); }
    Internals (const Internals&) noexcept               = default;
    Internals& operator= (const Internals&) noexcept    = default;
    Internals& operator= (Internals&&) noexcept         = default;
    ~Internals()                                        = default;


    bool isInitialised() const noexcept 
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

    bool initialise (const GLuint startingIndex) noexcept
    {
        for (size_t i { 0 }; i < supportedResolutionCount; ++i)
        {
            if (!rgb[i].initialise (startingIndex + i) || 
                !rgba[i].initialise (startingIndex + supportedResolutionCount + i))
            {
                return false;
            }
        }

        return true;
    }

    void clean() noexcept
    {
        for (size_t i { 0 }; i < supportedResolutionCount; ++i)
        {
            rgb[i].clean();
            rgba[i].clean();
        }
    }

    void bind() const noexcept
    {
        for (size_t i { 0 }; i < supportedResolutionCount; ++i)
        {
            const auto& rgbTexture = rgb[i];
            glBindTextureUnit (rgbTexture.getDesiredTextureUnit(), rgbTexture.getID());
                
            const auto& rgbaTexture = rgba[i];
            glBindTextureUnit (rgbTexture.getDesiredTextureUnit(), rgbTexture.getID());
        }
    }

    void unbind() const noexcept
    {
        const auto start = rgb.front().getDesiredTextureUnit();
        for (size_t i { 0 }; i < supportedResolutionCount * 2; ++i)
        {
            glBindTextureUnit (start + i, 0);
        }
    }

    Texture& get (const GLint resolution, const size_t components) noexcept
    {

        switch (resolution)
        {
        
        }
    }

    bool contains (const std::string& file) const noexcept
    {
        return ids.find (file) != std::end (ids);
    }
};

#endif