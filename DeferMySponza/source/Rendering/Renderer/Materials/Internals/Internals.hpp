#pragma once

#if !defined    _RENDERING_RENDERER_MATERIALS_INTERNAL_INTERNALS_
#define         _RENDERING_RENDERER_MATERIALS_INTERNAL_INTERNALS_

// STL headers.
#include <array>
#include <string>


// Engine headers.
#include <glm/vec3.hpp>


// Personal headers.
#include <Rendering/Renderer/Materials/Materials.hpp>


/// <summary>
/// Contains internal data which isn't required at run time such as the buffer containing materials and every texture
/// array.
/// </summary>
class Materials::Internals final
{
    private:

        constexpr static auto minimumDimensions = size_t { 32 };    //!< The minimum supported texture dimensions.
        constexpr static auto maximumDimensions = size_t { 2048 };  //!< The maximum supported texture dimensions.
        
        // Compile-time computation with constexpr support in VS2015 is laughable so we can't create a constexpr function to calculate this value.
        constexpr static auto supportedResolutionCount = size_t { 8 };

        using Textures      = std::array<Texture2DArray, supportedResolutionCount>;
        using TextureIDs    = std::unordered_map<std::string, glm::uvec3>;

    public:
    
        SamplerBuffer   materials   { };    //!< The texture buffer which provides access to materials in shaders.
        Textures        rgb         { };    //!< Contains a 2D texture array for each supported texture resolution in the RGB format.
        Textures        rgba        { };    //!< Contains a 2D texture array for each supported texture resolution in the RGBA format.
        TextureIDs      ids         { };    //!< Maps file locations to a vector containing number of components, sampler index and array depth values for materials.


        Internals() noexcept { }
        Internals (Internals&& move) noexcept;
        Internals (const Internals&) noexcept               = default;
        Internals& operator= (const Internals&) noexcept    = default;
        Internals& operator= (Internals&&) noexcept         = default;
        ~Internals()                                        = default;


        bool isInitialised() const noexcept;
        bool initialise (const GLuint startingIndex) noexcept;
        void clean() noexcept;
        void bind() const noexcept;
        void unbind() const noexcept;
        Texture& get (const GLint resolution, const size_t components) noexcept;
        bool contains (const std::string& file) const noexcept;
};

#endif