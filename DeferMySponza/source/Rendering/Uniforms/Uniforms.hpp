#pragma once

#if !defined    _RENDERING_UNIFORMS_
#define         _RENDERING_UNIFORMS_

// Engine headers.
#include <glm/mat4x4.hpp>
#include <scene/scene_fwd.hpp>


// Personal headers.
#include <Rendering/Objects/Buffer.hpp>
#include <Rendering/Uniforms/Internals/UniformBlocks.hpp>


// Forward declarations.
struct Programs;


/// <summary>
/// Contains and manages the uniform buffer objects used by all the programs used by the renderer.
/// </summary>
class Uniforms final
{
    public:

        /// <summary>
        /// Cannot be noexcept due to the underlying usage of glm types.
        /// </summary>
        Uniforms()                                  = default;
    
        Uniforms (Uniforms&&) noexcept              = default;
        Uniforms& operator= (Uniforms&&) noexcept   = default;

        Uniforms (const Uniforms&)                  = delete;
        Uniforms& operator= (const Uniforms&)       = delete;

        ~Uniforms()                                 = default;


        bool initialise() noexcept;

        void clean() noexcept;


        
        bool bindToProgram (const GLuint program) const noexcept;

        void updateScene (const scene::Context* const scene, const glm::mat4& projection, const glm::mat4& view) noexcept;

        void updateDirectionalLight (const scene::DirectionalLight& light) noexcept;

        void updatePointLight (const scene::PointLight& light) noexcept;

        void updateSpotlight (const scene::SpotLight& light) noexcept;

        void updateBuffer() noexcept;

    private:

        Buffer          m_ubo   { };    //!< The uniform buffer object containing all uniform block data.
        UniformBlocks   m_data  { };    //!< Contains the raw uniform data that will be updated multiple times per frame.
};

#endif // _RENDERING_UNIFORMS_
