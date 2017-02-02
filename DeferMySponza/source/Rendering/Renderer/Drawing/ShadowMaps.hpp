#pragma once

#if !defined    _RENDERING_RENDERER_DRAWING_SHADOW_MAPS_
#define	        _RENDERING_RENDERER_DRAWING_SHADOW_MAPS_

// Personal headers.
#include <unordered_map>
#include <vector>


// Engine headers
#include <glm/fwd.hpp>
#include <scene/scene_fwd.hpp>


// Personal headers.
#include <Rendering/Composites/PersistentMappedBuffer.hpp>
#include <Rendering/Objects/Framebuffer.hpp>
#include <Rendering/Objects/Texture.hpp>
#include <Rendering/Renderer/Uniforms/Blocks/FullBlock.hpp>


/// <summary> 
/// Stores and produces shadow maps for spotlights.
/// </summary>
class ShadowMaps final
{
    public:

        ShadowMaps() noexcept { };
        ShadowMaps (ShadowMaps&&) noexcept;
        ShadowMaps (const ShadowMaps&) noexcept             = default;
        ShadowMaps& operator= (const ShadowMaps&) noexcept  = default;
        ShadowMaps& operator= (ShadowMaps&&) noexcept       = default;
        ~ShadowMaps()                                       = default;


        /// <summary> Gets the shadow map index for a shader for the given light ID. </summary>
        /// <returns> An index value if valid, else -1. </returns>
        GLint operator[] (const scene::LightId lightID) const noexcept;

        /// <summary> Returns the texture unit used for shadow maps. </summary>
        auto getShadowMapTextureUnit() const noexcept   { return m_maps.getDesiredTextureUnit(); }

        /// <summary> Gets the 2D array containing shadow maps. </summary>
        const Texture& getShadowMaps() const noexcept   { return m_maps; };
        
        /// <summary> Gets the resolution of the shadow maps. </summary>
        GLsizei getResolution() const noexcept          { return m_res; };

        /// <summary> Checkes if the object is initialised. </summary>
        bool isInitialised() const noexcept;


        /// <summary> 
        /// Prepares the object for storing shadow maps for the given spotlights. Successive calls will only modify 
        /// the object if initialisation succeeds. 
        /// </summary>
        /// <param name="spotlights"> A collection of spotlights to produce shadow maps for. </param>
        /// <param name="textureUnit"> The texture unit to use for storing shadow maps. </param>
        /// <returns> Whether initialisation was successful. </returns>
        bool initialise (const std::vector<scene::SpotLight>& spotlights, const GLuint textureUnit) noexcept;

        /// <summary> Deletes every stored object. </summary>
        void clean() noexcept;


        /// <summary> Sets the light projection-view transforms for shadow mapping. </summary>
        /// <param name="scene"> The scene context containing light data for the current frame. </param>
        /// <param name="block"> A pointer to the start of the data to write to. </param>
        /// <param name="start"> A starting offset, used for returning the correct range. </param>
        /// <returns> The range of data which has been modified. </returns>
        ModifiedRange setUniforms (const scene::Context* scene, FullBlock<glm::mat4>* block, GLsizeiptr start) const noexcept;

        /// <summary> 
        /// Generates shadow maps based on the given render function. This will change the value of the uniform at 
        /// location 0, this should be the index of the view matrix to apply.
        /// </summary>
        /// <param name="clearDepth"> Whether the depth buffer should be cleared before rendering. </param>
        /// <param name="renderFunction"> A function to call which will render objects. </param>
        template <typename RenderFunc>
        void generateMaps (const bool clearDepth, const RenderFunc& renderFunction) noexcept;

    private:

        constexpr static auto maxResolution = 2048; //!< The maximum resolution of the shadow maps.

        using Spotlights    = std::vector<scene::LightId>;
        using MapIDs        = std::unordered_map<scene::LightId, GLint>;

        Framebuffer     m_fbo       { };    //!< A framebuffer containing a depth attachment to render with.
        Texture2DArray  m_maps      { };    //!< Contains every shadow map in the scene.
        Spotlights      m_lights    { };    //!< Contains every shadow-casting light in the scene.
        MapIDs          m_ids       { };    //!< Maps LightIDs to an index in the maps sampler for the shadow map.
        GLsizei         m_res       { 0 };  //!< The resolution of the shadow maps.
};


template <typename RenderFunc>
void ShadowMaps::generateMaps (const bool clearDepth, const RenderFunc& renderFunction) noexcept
{
    // We must go through each light, setting the index of the view matrix to use and render the scene.
    const auto fbo      = FramebufferBinder<GL_FRAMEBUFFER> { m_fbo };
    const auto mapCount = static_cast<GLint> (m_lights.size());
    glViewport (0, 0, m_res, m_res);

    for (GLint i { 0 }; i < mapCount; ++i)
    {
        // Ensure we render into the correct shadow map.
        m_fbo.attachTextureLayer (m_maps, GL_DEPTH_ATTACHMENT, i, false);
        assert (m_fbo.complete());

        // Clear the buffer if necesssary.
        if (clearDepth)
        {
            glClear (GL_DEPTH_BUFFER_BIT);
        }

        // Finally set the index of the view transform and render the scene.
        glUniform1i (0, i);
        renderFunction();
    }
}

#endif // _RENDERING_RENDERER_DRAWING_SHADOW_MAPS_

