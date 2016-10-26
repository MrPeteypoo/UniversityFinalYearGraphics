#pragma once

#if !defined    _RENDERING_UNIFORMS_
#define         _RENDERING_UNIFORMS_

// Engine headers.
#include <scene/scene_fwd.hpp>


// Personal headers.
#include <Rendering/Buffers/Buffer.hpp>
#include <Rendering/Uniforms/UniformBlocks.hpp>


// Forward declarations.
struct Programs;


/// <summary>
/// Contains and manages the uniform buffer objects used by all the programs used by the renderer.
/// </summary>
class Uniforms final
{
    public:

        Uniforms (const Uniforms& uniforms)             = delete;
        Uniforms& operator= (const Uniforms& uniforms)  = delete;



        bool initialise (const Programs& programs) noexcept;

        void clean() noexcept;

        void update (const scene::Context* const context) noexcept;

    private:

        Buffer          m_ubo   { };    //!< The uniform buffer object containing all uniform block data.
        UniformBlocks   m_data  { };    //!< Contains the raw uniform data that will be updated multiple times per frame.

        bool bindToProgram (const GLuint program) const noexcept;
};

#endif // _RENDERING_UNIFORMS_
