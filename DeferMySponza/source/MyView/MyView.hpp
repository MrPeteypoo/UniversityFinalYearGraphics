#pragma once

#if !defined    _MY_VIEW_
#define         _MY_VIEW_


// STL headers.
#include <memory>
#include <unordered_map>


// Engine headers.
#include <glm/fwd.hpp>
#include <scene/scene_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>


// Personal headers.
#include <MyView/Internals/Material.hpp>
#include <MyView/Internals/Mesh.hpp>
#include <Rendering/PassConfigurator/PassConfigurator.hpp>
#include <Rendering/Renderer/Internals/GeometryBuffer.hpp>
#include <Rendering/Renderer/Internals/LightBuffer.hpp>
#include <Rendering/Uniforms/Uniforms.hpp>
#include <Utility/OpenGL/ToDelete.hpp>


// Forward declarations.
namespace tygra { class Image; }
struct Light;
struct Vertex;


/// <summary>
/// Used in creating and rendering of a scene using the Sponza graphics data.
/// </summary>
class MyView final : public tygra::WindowViewDelegate
{
    public:

        MyView()                                = default;
        MyView (MyView&& move)                  = default;
        MyView& operator= (MyView&& move)       = default;
        
        MyView (const MyView& copy)             = delete;
        MyView& operator= (const MyView& copy)  = delete;

        ~MyView();


        /// <summary> Sets the scene::Context to use for rendering. </summary>
        void setScene (scene::Context* scene) { m_scene = scene; }

        /// <summary> Causes the application to rebuild the shaders. </summary>
        void rebuildShaders();
		
    private:
		
        /// <summary> Causes the object to initialise; loading and preparing all data. </summary>
        void windowViewWillStart (tygra::Window* window) override final;

        /// <summary> Generates the VAO and buffers owned by the MyView class. </summary>
        void generateOpenGLObjects();

        /// <summary> Creates a mesh of every object in the scene and loads the data into VBOs. </summary>
        void buildMeshData();

        /// <summary> Creates a material for each materialID in the map, ready for rendering. </summary>
        void buildMaterialData();

        /// <summary> Constructs the VAO for the scene using an interleaved vertex VBO and instanced transform matrices. </summary>
        void constructVAO();

        /// <summary> This will allocate enough memory in m_uniformVBO, m_materialPool and m_matricesPool for modification at run-time. </summary>
        void allocateExtraBuffers();

        /// <summary> Prepares the material TBO and allocates storage for the texture array. </summary>
        /// <param name="textureWidth"> The width each texture should be in the array. </param>
        /// <param name="textureHeight"> The height each texture should be in the array. </param>
        /// <param name="textureCount"> The total number of textures the array can store. </param>
        void prepareTextureData (const GLsizei textureWidth, const GLsizei textureHeight, const GLsizei textureCount);

        /// <summary> Loads every given image into the 2D texture array. </summary>
        /// <param name="images"> The images to load. </param>
        void loadTexturesIntoArray (const std::vector<std::pair<std::string, tygra::Image>>& images);

        /// <summary> Obtains each group of instances for each scene::MeshId and determines the maximum number of instances we'll encounter. </summary>
        /// <returns> The highest instance count of each scene::MeshId in the scene. </returns>
        size_t highestInstanceCount() const;


        /// <summary> Causes the object to free up any resources being held. </summary>
        void windowViewDidStop (tygra::Window* window) override final;
        
        /// <summary> Deletes all Mesh and Material objects and clears the containers. </summary>
        void cleanMeshMaterials();

        /// <summary> Deletes all VAOs, VBOs, TBOs, etc. owned by the MyView class. </summary>
        void deleteOpenGLObjects();


        /// <summary> Changes the viewport, updating the aspect ratio, etc. </summary>
        void windowViewDidReset (tygra::Window* window, int width, int height) override final;

        /// <summary> Renders the given scene, the object should be initialised before calling this. </summary>
        void windowViewRender (tygra::Window* window) override final;

        void renderGeometry (const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) noexcept;

        void mapTexturesToProgram (const GLuint program) const noexcept;


        // Using declarations.
        using MaterialID = int;

        /// <summary>
        /// A simple structure which contains an ID for a VBO and a TBO, the TBO should be linked to the VBO for use in a sampler buffer.
        /// </summary>
        struct SamplerBuffer final
        {
            GLuint  vbo { 0 };  //!< The buffer to contain shader accessible information.
            GLuint  tbo { 0 };  //!< The texture buffer which points to the VBO, linking them together.
        };

        PassConfigurator                                    m_configurator      { };            //!< Used to configure the OpenGL context before rendering.
        Uniforms                                            m_uniforms          { };            //!< Stores and updates uniforms used by the scene.
        GeometryBuffer                                      m_gbuffer           { };            //!< A rendering target where the scene geometry is constructed, independant of lighting.
        LightBuffer                                         m_lbuffer           { };            //!< A rendering target where lighting is applied before being rendered on screen.

        GLuint                                              m_sceneVAO          { 0 };          //!< A Vertex Array Object for the entire scene.
        GLuint                                              m_vertexVBO         { 0 };          //!< A Vertex Buffer Object which contains the interleaved vertex data of every mesh in the scene.
        GLuint                                              m_elementVBO        { 0 };          //!< A Vertex Buffer Object with the elements data for every mesh in the scene.
        
        GLuint                                              m_uniformUBO        { 0 };          //!< A Uniform Buffer Object which contains scenes uniform data.
        
        SamplerBuffer                                       m_materials         { };            //!< A VBO & TBO pair representing information on every material in the scene.
        GLuint                                              m_textureArray      { 0 };          //!< The TEXTURE_2D_ARRAY which contains each texture in the scene.
        
        size_t                                              m_instancePoolSize  { 0 };          //!< The current size of the instance pools, useful for optimising rendering.
        SamplerBuffer                                       m_poolMaterialIDs   { };            //!< A pool of material IDs for each instance, used for accessing the instance-specific material.
        GLuint                                              m_poolTransforms    { 0 };          //!< A pool of model and PVM transformation matrices, used in instanced rendering.
        
        float                                               m_aspectRatio       { 0.f };        //!< The calculated aspect ratio of the foreground resolution for the application.

        scene::Context*                                     m_scene             { nullptr };    //!< The sponza scene containing instance and camera information.
        std::vector<std::pair<scene::MeshId, Mesh>>         m_meshes            { };            //!< A container of MeshId and Mesh pairs, used in instance-based rendering of meshes in the scene.
        std::unordered_map<scene::MaterialId, MaterialID>   m_materialIDs       { };            //!< A map containing each material used for rendering.
        GLuint quadVAO { 0 };
        Buffer quadVBO { };
};

#endif // _MY_VIEW_