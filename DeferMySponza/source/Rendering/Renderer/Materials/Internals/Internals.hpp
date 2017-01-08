#pragma once

#if !defined    _RENDERING_RENDERER_MATERIALS_INTERNAL_INTERNALS_
#define         _RENDERING_RENDERER_MATERIALS_INTERNAL_INTERNALS_

// STL headers.
#include <array>
#include <string>


// Engine headers.
#include <glm/vec3.hpp>


// Personal headers.
#include <Rendering/Composites/SamplerBuffer.hpp>
#include <Rendering/Renderer/Materials/Materials.hpp>


/// <summary>
/// Contains internal data which isn't required at run time such as the buffer containing materials and every texture
/// array.
/// </summary>
class Materials::Internals final
{
    private:

        constexpr static auto minimumDimensions = size_t { 64 };    //!< The minimum supported texture dimensions.
        constexpr static auto maximumDimensions = size_t { 2048 };  //!< The maximum supported texture dimensions.
        
        // Compile-time computation with constexpr support in VS2015 is laughable so we can't create a constexpr function to calculate this value.
        constexpr static auto supportedResolutionCount = size_t { 8 };  //!< We store an initial texture array for 1x1 textures and 7 for dimensions between 64x64 and 2048x2048.

        using Textures      = std::array<Texture2DArray, supportedResolutionCount>;
        using TextureIDs    = std::unordered_map<std::string, glm::uvec2>;
        using Counts        = std::unordered_map<size_t, std::unordered_map<size_t, size_t>>;

        static GLint maxTexture;    //!< Tracks the maximum size a texture can be on the current GPU.
        static GLint maxArrayDepth; //!< Tracks the maximum depth of 2D texture arrays on the current GPU.

    public:
    
        SamplerBuffer   materials   { };    //!< The texture buffer which provides access to materials in shaders.
        Textures        rgb         { };    //!< Contains a 2D texture array for each supported texture resolution in the RGB format.
        Textures        rgba        { };    //!< Contains a 2D texture array for each supported texture resolution in the RGBA format.
        TextureIDs      ids         { };    //!< Maps: File location -> texture unit index & array index.
        Counts          counts      { };    //!< Maps: Dimensions -> Counts -> number of textures.


        Internals() noexcept { }
        Internals (Internals&& move) noexcept;
        Internals (const Internals&) noexcept               = default;
        Internals& operator= (const Internals&) noexcept    = default;
        Internals& operator= (Internals&&) noexcept         = default;
        ~Internals()                                        = default;


        static GLint getMaxTextureSize() noexcept   { return maxTexture; }
        static GLint getMaxArrayDepth() noexcept    { return maxArrayDepth; }
        bool isInitialised() const noexcept;
        bool initialise (const GLuint startingIndex) noexcept;
        void clean() noexcept;
        void bind() const noexcept;
        void unbind() const noexcept;
        bool contains (const std::string& file) const noexcept;

        static bool areDimensionsSupported (const size_t width, const size_t height) noexcept;

        /// <summary> 
        /// Retrieves the texture unit index and texture array for the given component and dimension count. 
        /// </summary>
        std::pair<GLuint, Texture2DArray*> get (const size_t components, const size_t dimensions) noexcept;
};

#endif