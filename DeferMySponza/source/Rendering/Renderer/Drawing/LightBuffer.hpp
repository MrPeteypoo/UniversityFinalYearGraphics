#pragma once

#if !defined    _RENDERING_RENDERER_LIGHT_BUFFER_
#define         _RENDERING_RENDERER_LIGHT_BUFFER_

// Personal headers.
#include <Rendering/Objects/Framebuffer.hpp>
#include <Rendering/Objects/Renderbuffer.hpp>


/// <summary>
/// Contains a framebuffer with a renderbuffer and depth-stencil texture attached. Used for the application of lighting
/// to a scene by using data stored in a geometry buffer.
/// </summary>
class LightBuffer final
{
    public:

        LightBuffer() noexcept                                  = default;
        LightBuffer (LightBuffer&& move) noexcept               = default;
        LightBuffer& operator= (LightBuffer&& move) noexcept    = default;

        LightBuffer (const LightBuffer&)                        = delete;
        LightBuffer& operator= (const LightBuffer&)             = delete;
        
        ~LightBuffer()                                          = default;


        /// <summary> Check if the Lbuffer has been initialised and is ready to be used. </summary>
        bool isInitialised() const noexcept { return m_fbo.isInitialised() && m_colour.isInitialised(); }
        
        /// <summary> Gets the drawable framebuffer object, representing the Lbuffer. </summary>
        inline const Framebuffer& getFramebuffer() const noexcept   { return m_fbo; }
        
        /// <summary> Gets the renderbuffer containing colour data. </summary>
        inline const Renderbuffer& getColourBuffer() const noexcept { return m_colour; }


        /// <summary> 
        /// Attemots to initialise the light buffer with the given format and attaches the given texture as
        /// a depth-stencil attachment. Successive calls will re-initialise the object. Upon failure the
        /// object will not be changed.
        /// </summary>
        /// <param name="depthStencilTexture"> The texture to be attached as a depth-stencil attachment. </param>
        /// <param name="internalFormat"> The data format of the renderbuffer, e.g. GL_RGB8. </param>
        /// <param name="width"> How many pixels wide the Gbuffer should be. </param>
        /// <param name="height"> How many pixels tall the Gbuffer should be. </param>
        /// <returns> Whether the Gbuffer was successfully created or not. </returns>
        bool initialise (const Texture& depthStencilTexture, GLenum internalFormat, 
            GLsizei width, GLsizei height, GLsizei samples = 0) noexcept;

        /// <summary> Deletes the Gbuffer, freeing memory to the GPU. </summary>
        void clean() noexcept;

    private:

        Framebuffer     m_fbo       { };    //!< The drawable framebuffer.
        Renderbuffer    m_colour    { };    //!< The renderbuffer to apply colour onto.
};

#endif // _RENDERING_RENDERER_LIGHT_BUFFER_