#pragma once

#if !defined    _RENDERING_COMPOSITES_SAMPLER_BUFFER_
#define         _RENDERING_COMPOSITES_SAMPLER_BUFFER_

// Personal headers.
#include <Rendering/Objects/Buffer.hpp>
#include <Rendering/Objects/Texture.hpp>


/// <summary>
/// Contains the sampler index and physics properties, albedo and normal map of a material.
/// </summary>
struct SamplerBuffer final
{
    TextureBuffer   texture { };    //!< The texture object which is visible to shaders.
    Buffer          buffer  { };    //!< Contains the actual data which is accessed through texel fetches.
    

    SamplerBuffer()                                             = default;
    SamplerBuffer (SamplerBuffer&&)                             = default;
    SamplerBuffer& operator= (SamplerBuffer&&) noexcept         = default;
    ~SamplerBuffer()                                            = default;

    SamplerBuffer (const SamplerBuffer&) noexcept               = delete;
    SamplerBuffer& operator= (const SamplerBuffer&) noexcept    = delete;


    /// <summary> Checks whether the sampler buffer has been initialised. </summary>
    inline bool isInitialised() const noexcept { return texture.isInitialised() && buffer.isInitialised(); }


    /// <summary> 
    /// Attempts to initialise the contained texture and buffer. Successive calls will delete the old data. Upon
    /// failure the object will not be modified.
    /// </summary>
    /// <param name="unit"> The default texture unit for the texture object. </param>
    /// <param name="internalFormat"> The format of the buffers data store, e.g. GL_RGBA32F. </param>
    /// <returns> Whether the initialisation succeeded. </returns>
    bool initialise (const GLuint unit, const GLenum internalFormat) noexcept;

    /// <summary> Deletes the currently managed texture and buffer object. </summary>
    void clean() noexcept 
    { 
        texture.clean(); 
        buffer.clean(); 
    }
};

#endif // _RENDERING_COMPOSITES_SAMPLER_BUFFER_