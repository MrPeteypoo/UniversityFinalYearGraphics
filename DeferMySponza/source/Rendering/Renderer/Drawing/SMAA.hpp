#pragma once

#if !defined    _RENDERING_RENDERER_DRAWING_SMAA_
#define	        _RENDERING_RENDERER_DRAWING_SMAA_

// Personal headers.
#include <Rendering/Objects/Framebuffer.hpp>
#include <Rendering/Objects/Program.hpp>
#include <Rendering/Objects/Texture.hpp>
#include <Rendering/Renderer/Geometry/FullScreenTriangleVAO.hpp>
#include <Rendering/Renderer/Programs/Shaders.hpp>


/// <summary> 
/// An encapsulation of subpixel morphological antialiasing available at https://github.com/iryoku/smaa. It can be
/// used to perform an antialiasing pass on a given 2D texture. 
/// </summary>
class SMAA final
{
    public:

        /// <summary> Specifies the quality of antialiasing to be performed. </summary>
        enum class Quality 
        {
            None,   //!< 0% of the quality. :(
            Low,    //!< 60% of the quality.
            Medium, //!< 80% of the quality.
            High,   //!< 95% of the quality.
            Ultra   //!< 99% of the quality.
        };

        SMAA() noexcept                         = default;
        SMAA (SMAA&&) noexcept                  = default;
        SMAA (const SMAA&) noexcept             = default;
        SMAA& operator= (const SMAA&) noexcept  = default;
        SMAA& operator= (SMAA&&) noexcept       = default;
        ~SMAA()                                 = default;

        /// <summary> Checkes if the object is initialised. </summary>
        bool isInitialised() const noexcept;

        /// <summary> 
        /// Builds the shaders and textures required to perform subpixel morphological antialiasing. The object won't
        /// be modified if initialisation fails.
        /// </summary>
        /// <param name="quality"> 
        /// The quality preset to use. If Quality::None is passed then the object will be cleaned and return false. 
        /// </param>
        /// <param name="width"> How many pixels wide the internal render targets should be. </param>
        /// <param name="height"> How many pixels tall the internal render targets should be. </param>
        /// <param name="startingTextureUnit"> Initial texture unit for textures, requires three units. </param>
        /// <param name="usePredication"> Will a predication texture be provided when running? </param>
        /// <returns> Whether the initialisation was successful. </returns>
        bool initialise (Quality quality, GLsizei width, GLsizei height, GLuint startingTextureUnit, 
            bool usePredication = false) noexcept;

        /// <summary> Deletes every stored object. </summary>
        void clean() noexcept;

        /// <summary> Performs subpixel morphological antialiasing on the given texture. </summary>
        /// <param name="vao"> The vao containing a full-screen triangle. </param>
        /// <param name="aliasedTexture"> The input texture to antialias. </param>
        /// <param name="predication"> A texture to be supplied for predicated thresholding. </param>
        /// <param name="output"> The framebuffer to output to, if null then the output will be the screen. </param>
        void run (const FullScreenTriangleVAO& triangle, const Texture2D& aliasedTexture, 
            const Texture2D* predication = nullptr, const Framebuffer* output = nullptr) noexcept;

    private:

        /// <summary> A framebuffer where each pass can be drawn into. </summary>
        struct RenderTarget final
        {
            Framebuffer fbo     { };    //!< The framebuffer to draw to.
            Texture2D   output  { };    //!< The texture to output data from a rendering pass to.
        };
        
        Program         m_edgeDetectionPass { };    //!< The initial edge detection pass through the scene.
        RenderTarget    m_edgeDetectionFBO  { };    //!< The render target for the edge detection pass.

        Program         m_weightingPass     { };    //!< The second rendering pass which calculates blending weightings.
        RenderTarget    m_weightingFBO      { };    //!< The render target for calculating blending weightings.

        Program         m_blendingPass      { };    //!< The final blending pass where the aliased edges are blurred.

        Texture2D       m_areaTexture       { };    //!< A precalculated texture required for blending.
        Texture2D       m_searchTexture     { };    //!< A precalculated texture required for blending.
        Texture2D       m_stencil           { };    //!< A stencil buffer used to optimise which pixels are modified.

    private:

        /// <summary> Loads the precalculated textures. </summary>
        void loadTextures (Texture2D& areaTex, Texture2D& searchTex) const noexcept;

        /// <summary> Attempts to compile the three required programs based on the given quality preset. </summary>
        bool compilePrograms (Program& edge, Program& weight, Program& blend, const Texture& areaTex, 
            const Texture& searchTex, Quality quality, GLsizei width, GLsizei height, 
            GLuint outputTextureUnit, bool usePredication) const noexcept;

        /// <summary> Allocates memory and configures each render target with the given parameters. </summary>
        bool configureRenderTargets (RenderTarget& edge, RenderTarget& weight, Texture2D& stencil,
            GLsizei width, GLsizei height) const noexcept;

        /// <summary> All textures require the same parameters which can be set using this. </summary>
        template <GLenum target>
        void setTextureParameters (TextureT<target>& texture) const noexcept;

        /// <summary> Returns shader definitions for the given parameters. </summary>
        Shader::RawSource calculateDefines (Quality quality, GLsizei width, GLsizei height, 
            bool usePredication) const noexcept;

        /// <summary> Compiles the shaders required to perform SMAA. </summary>
        /// <param name="extraDefines"> Any additional defines that are determined at run-time. </param>
        Shaders compileShaders (Shader::RawSource extraDefines) const noexcept;
};

#endif // _RENDERING_RENDERER_DRAWING_SMAA_

