#pragma once

#if !defined    _RENDERING_RENDERER_GEOMETRY_BUFFER_
#define         _RENDERING_RENDERER_GEOMETRY_BUFFER_

// Personal headers.
#include <Rendering/Objects/Framebuffer.hpp>
#include <Rendering/Objects/Texture.hpp>


/// <summary>
/// A geometry buffer contains the position, normal, material and depth-stencil of every objects drawn to the buffer.
/// Each component of the Gbuffer can be attached to
/// </summary>
class GeometryBuffer final
{
    public:
        
        constexpr static GLuint positionLocation    { 0 };  //!< The shader layout location for position data.
        constexpr static GLuint normalLocation      { 1 };  //!< The shader layout location for normal data.
        constexpr static GLuint materialLocation    { 2 };  //!< The shader layout location for material data.
        constexpr static GLuint depthLocation       { 3 };  //!< The shader layout location for depth/stencil data.

    public:

        GeometryBuffer() noexcept                                   = default;
        GeometryBuffer (GeometryBuffer&& move) noexcept             = default;
        GeometryBuffer& operator= (GeometryBuffer&& move) noexcept  = default;

        GeometryBuffer (const GeometryBuffer&)                      = delete;
        GeometryBuffer& operator= (const GeometryBuffer&)           = delete;
        
        ~GeometryBuffer()                                           = default;


        /// <summary> Check if the Gbuffer has been initialised and is ready to be used. </summary>
        bool isInitialised() const noexcept;
        
        /// <summary> Gets the drawable framebuffer object, representing the Gbuffer. </summary>
        inline const Framebuffer& getFramebuffer() const noexcept               { return m_fbo; }
        
        /// <summary> Gets the texture containing position data. </summary>
        inline const TextureRectangle& getPositionTexture() const noexcept      { return m_positions; }
        
        /// <summary> Gets the texture containing normal data. </summary>
        inline const TextureRectangle& getNormalTexture() const noexcept        { return m_normals; }
        
        /// <summary> Gets the texture containing material data. </summary>
        inline const TextureRectangle& getMaterialTexture() const noexcept      { return m_materials; }
        
        /// <summary> Gets the texture containing depth and stencil data. </summary>
        inline const TextureRectangle& getDepthStencilTexture() const noexcept  { return m_depthStencil; }


        /// <summary> 
        /// Attempt to construct the Gbuffer and prepare it for usage. Successive calls will wipe the Gbuffer.
        /// Upon failure the object will not be changed.
        /// </summary>
        /// <param name="width"> How many pixels wide the Gbuffer should be. </param>
        /// <param name="height"> How many pixels tall the Gbuffer should be. </param>
        /// <param name="startingTextureUnit"> The initial index to apply to stored textures. </param>
        /// <returns> Whether the Gbuffer was successfully created or not. </returns>
        bool initialise (const GLsizei width, const GLsizei height, const GLuint startingTextureUnit) noexcept;

        /// <summary> Deletes the Gbuffer, freeing memory to the GPU. </summary>
        void clean() noexcept;

    private:

        Framebuffer         m_fbo           { }; //!< The drawable framebuffer.
        TextureRectangle    m_positions     { }; //!< Contains the 3D position of the every drawn object.
        TextureRectangle    m_normals       { }; //!< Contains the 3D normal from drawn surfaces.
        TextureRectangle    m_materials     { }; //!< Contains the material ID and texture co-ords for drawn objects.
        TextureRectangle    m_depthStencil  { }; //!< The depth & stencil value of every pixel.
};

#endif // _RENDERING_RENDERER_GEOMETRY_BUFFER_