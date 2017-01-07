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


        /// <summary> 
        /// Constructs geometry from scene::GeometryBuilder class as well and building the required shapes to perform
        /// deferred lighting. Along with this, VAOs within the scene are built and static object optimisation is
        /// performed by creating instancing buffers for static objects and by creating draw calls for indirect
        /// rendering. Successive calls will not change the object unless initialisation is successful.
        /// </summary>
        /// <param name="scene"> Contains every material in the scene. </param>
        /// <returns> Whether initialisation was successful or not. </returns>
        bool initialise (const scene::Context* const scene) noexcept;

        /// <summary> Destroys every stored object and returns to a clean state. </summary>
        void clean() noexcept;

    private:

        struct Internals;

        using MaterialIDs   = std::unordered_map<scene::MaterialId, MaterialID>;
        using Pimpl         = std::unique_ptr<Internals>;

        MaterialIDs     m_materialIDs   { };    //!< Maps scene material IDs to stored GPU material IDs.
        SamplerBuffer   m_materials     { };    //!< The texture buffer which provides access to materials in shaders.
        Pimpl           m_internals     { };    //!< A pointer to internal managed data.
        
        
};

#endif // _RENDERING_RENDERER_MATERIALS_