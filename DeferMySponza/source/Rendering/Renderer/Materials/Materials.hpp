#pragma once

#if !defined    _RENDERING_RENDERER_MATERIALS_
#define         _RENDERING_RENDERER_MATERIALS_

// STL headers.
#include <memory>
#include <unordered_map>


// Engine headers.
#include <scene/scene_fwd.hpp>


// Personal headers.
#include <Rendering/Composites/SamplerBuffer.hpp>
#include <Rendering/Objects/Texture.hpp>


// Forward declarations.
using MaterialID = GLuint;


/// <summary>
/// Contains every piece of geometry in the scene. Static batching is supported with static instances having their
/// transforms permanently stored in the transforms buffer.
/// </summary>
class Materials final
{
    public:

        Materials() noexcept;
        Materials (Materials&&) noexcept;
        Materials& operator= (Materials&&) noexcept = default;
        ~Materials()                                = default;

        Materials (const Materials&)                = delete;
        Materials& operator= (const Materials&)     = delete;


        /// <summary> Gets the material ID associated with the scene ID given. </summary>
        /// <param name="sceneID"> The scene::MaterialId to retrieve the MaterialID for. </param>
        /// <returns> The material ID if successful, the maximum value if not found. </returns> 
        MaterialID operator[] (const scene::MaterialId sceneID) const noexcept;


        /// <summary> 
        /// Constructs every material in the scene, including loading every texture and mapping scene::MaterialId 
        /// values to built-in values. Successive calls will not change the object unless initialisation is successful.
        /// </summary>
        /// <param name="scene"> Contains every material in the scene. </param>
        /// <param name="startingTextureUnit"> The initial index to apply to stored textures. </param>
        /// <returns> Whether initialisation was successful or not. </returns>
        bool initialise (const scene::Context* const scene, const GLuint startingTextureUnit) noexcept;

        /// <summary> Destroys every stored object and returns to a clean state. </summary>
        void clean() noexcept;


        /// <summary> Binds every texture unit with its associated texture. </summary>
        void bindTextures() const noexcept;

        /// <summary> Unbind every texture, leaving their associated texture units in a clean state. </summary>
        void unbindTextures() const noexcept;

    private:

        class Internals;

        using MaterialIDs   = std::unordered_map<scene::MaterialId, MaterialID>;
        using Pimpl         = std::unique_ptr<Internals>;

        MaterialIDs     m_materialIDs   { };    //!< Maps scene material IDs to stored GPU material IDs.
        Pimpl           m_internals     { };    //!< A pointer to internal managed data.
};

#endif // _RENDERING_RENDERER_MATERIALS_