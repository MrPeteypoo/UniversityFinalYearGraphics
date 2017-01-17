#pragma once

#if !defined    _RENDERING_RENDERER_GEOMETRY_
#define         _RENDERING_RENDERER_GEOMETRY_

// STL headers.
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>


// Engine headers.
#include <scene/scene_fwd.hpp>


// Personal headers.
#include <Rendering/Composites/DrawCommands.hpp>
#include <Rendering/Objects/Buffer.hpp>
#include <Rendering/Renderer/Geometry/Mesh.hpp>
#include <Rendering/Renderer/Geometry/FullScreenTriangleVAO.hpp>
#include <Rendering/Renderer/Geometry/SceneVAO.hpp>
#include <Rendering/Renderer/Geometry/LightingVAO.hpp>


// Forward declarations.
class Materials;


/// <summary>
/// Contains every piece of geometry in the scene. Static batching is supported with static instances having their
/// transforms permanently stored in the transforms buffer.
/// </summary>
class Geometry final
{
    public:

        // Aliases.
        using DrawCommands = MultiDrawCommands<Buffer>;

    public:

        Geometry() noexcept;
        Geometry (Geometry&&) noexcept              = default;
        Geometry& operator= (Geometry&&) noexcept   = default;
        ~Geometry()                                 = default;

        Geometry (const Geometry&)                  = delete;
        Geometry& operator= (const Geometry&)       = delete;


        /// <summary> Maps the given mesh ID to a stored mesh. </summary>
        /// <param name="id"> The scene ID of the mesh to retrieve data for. </param>
        const Mesh& operator[] (const scene::MeshId id) const noexcept;

        /// <summary> Checks whether the object is initialised or not. </summary>
        bool isInitialised() const noexcept;

        /// <summary> Retrieves a map, containing every constructed mesh with an associated ID. </summary>
        const std::unordered_map<scene::MeshId, Mesh>& getMeshes() const noexcept;
        
        /// <summary> Gets the vertex array object containing scene geometric data. </summary>
        inline const SceneVAO& getSceneVAO() const noexcept                     { return m_scene; }
        
        /// <summary> Gets the vertex array object containing scene geometric data. </summary>
        inline SceneVAO& getSceneVAO() noexcept                                 { return m_scene; }

        /// <summary> Gets the vertex array object containing light volume data. </summary>
        inline const FullScreenTriangleVAO& getTriangleVAO() const noexcept     { return m_triangle; }

        /// <summary> Gets the vertex array object containing light volume data. </summary>
        inline const LightingVAO& getLightingVAO() const noexcept               { return m_lighting; }

        /// <summary> Retrieves the command buffer filled with commands to draw static geometry. </summary>
        inline const DrawCommands& getStaticGeometryCommands() const noexcept   { return m_drawCommands; }

        /// <summary> Gets the mesh data required to draw a quad. </summary>
        inline const Mesh& getQuad() const noexcept                             { return m_quad; }

        /// <summary> Gets the mesh data required to draw a sphere. </summary>
        inline const Mesh& getSphere() const noexcept                           { return m_sphere; }

        /// <summary> Gets the mesh data required to draw a cone. </summary>
        inline const Mesh& getCone() const noexcept                             { return m_cone; }


        /// <summary> 
        /// Constructs geometry from scene::GeometryBuilder class as well and building the required shapes to perform
        /// deferred lighting. Along with this, VAOs within the scene are built and static object optimisation is
        /// performed by creating instancing buffers for static objects and by creating draw calls for indirect
        /// rendering. Successive calls will not change the object unless initialisation is successful.
        /// </summary>
        /// <param name="materials"> The object containing material information. </param>
        /// <param name="staticInstances"> Contains every static instance which will be loaded into memory. </param> 
        /// <param name="dynamicMaterialIDs"> The buffer to use for the material IDs of dynamic objects. </param>
        /// <param name="dynamicTransforms"> The buffer to use for the model transforms of dynamic objects. </param>
        /// <param name="lightingTransforms"> The buffer to use for the model transforms of light volumes. </param>
        /// <returns> Whether initialisation was successful or not. </returns>
        template <size_t MaterialIDPartitions, size_t TransformPartitions, size_t LightingPartitions>
        bool initialise (const Materials& materials, 
            const std::map<scene::MeshId, std::vector<scene::Instance>>& staticInstances,
            const PersistentMappedBuffer<MaterialIDPartitions>& dynamicMaterialIDs, 
            const PersistentMappedBuffer<TransformPartitions>& dynamicTransforms,
            const PersistentMappedBuffer<LightingPartitions>& lightingTransforms) noexcept;

        /// <summary> Destroys every stored object and returns to a clean state. </summary>
        void clean() noexcept;

    private:

        struct Internals;
        using Pimpl = std::unique_ptr<Internals>;

        SceneVAO                m_scene         { };    //!< Used for drawing all scene geometry.
        DrawCommands            m_drawCommands  { };    //!< Contains the drawing commands to indirectly render every static object in the scene.

        FullScreenTriangleVAO   m_triangle      { };    //!< An oversized triangle vertex array which can be used to apply post-processing.
        
        LightingVAO             m_lighting      { };    //!< Used for applying light using quads, spheres and cones.
        Mesh                    m_quad          { };    //!< The mesh data required for drawing a full-screen quad.
        Mesh                    m_sphere        { };    //!< The mesh data required for drawing a sphere.
        Mesh                    m_cone          { };    //!< The mesh data required for drawing a cone.

        Pimpl                   m_internals     { };    //!< Stores less important internal data.

    private:

        /// <summary> Configures the given vertex array objects for storing scene and lighting geometry. </summary>
        /// <param name="scene"> The VAO to use for scene geometry. </param>
        /// <param name="triangle"> The VAO to use for oversized triangles. </param>
        /// <param name="lighting"> The VAO to use for lighting. </param>
        /// <param name="internals"> The object containing static buffers that need to be attached. </param>
        /// <param name="dynamicMaterialIDs"> The PMB containing material IDs for dynamic object instances. </param>
        /// <param name="dynamicTransforms"> The PMB containing model transforms for dynamic object instances. </param>
        /// <param name="lightingTransforms"> The PMB containing transforms for all lighting instances. </param>
        template <typename MaterialIDPMB, typename TransformPMB, typename LightingPMB>
        void configureVAOs (SceneVAO& scene, FullScreenTriangleVAO& triangle, LightingVAO& lighting, 
            const Internals& internals, const MaterialIDPMB& dynamicMaterialIDs, const TransformPMB& dynamicTransforms, 
            const LightingPMB& lightingTransforms) const noexcept;

        /// <summary> 
        /// Fills the mesh vertex and elements data in the given Internals object with data retrieved contained by
        /// scene::GeometryBuilder. Data will be stored by the GPU in scene::MeshId order.
        /// </summary>
        /// <param name="internals"> Where the data should be stored. </param>
        void buildMeshData (Internals& internals) const noexcept;

        /// <summary> Constructs an oversized full-screen triangle, useful for full-screen shading. </summary>
        void buildFullScreenTriangle (Internals& internals) const noexcept;

        /// <summary> 
        /// Fills the light volume vertex and element data in the given Internals object with valid data for the
        /// rendering of global lighting, point lighting and spotlighting.
        /// </summary>
        /// <param name="internals"> Where the data should be stored. </param>
        /// <param name="quad"> The Mesh object to contain information required to draw the quad. </param>
        /// <param name="sphere"> The Mesh object to contain information required to draw the sphere. </param>
        /// <param name="cone"> The Mesh object to contain information required to draw the cone. </param>
        void buildLighting (Internals& internals, Mesh& quad, Mesh& sphere, Mesh& cone) const noexcept;

        /// <summary> 
        /// Fills the static instancing and draw command buffers with data to draw every static object in the scene.
        /// </summary>
        /// <param name="internals"> Where the static buffers are stored. </param>
        /// <param name="drawCommands"> Where the list of indirect draw commands should be stored. </param>
        /// <param name="materials"> Material information for the material ID buffer. </param>
        /// <param name="instances"> Each instance that will be added to the static buffers. </param>
        void fillStaticBuffers (Internals& internals, DrawCommands& drawCommands, const Materials& materials,
            const std::map<scene::MeshId, std::vector<scene::Instance>>& instances) const noexcept;
};


// Personal headers.
#include <Rendering/Renderer/Geometry/Internals/Internals.hpp>


template <size_t MaterialIDPartitions, size_t TransformPartitions, size_t LightingPartitions>
bool Geometry::initialise (const Materials& materials, 
    const std::map<scene::MeshId, std::vector<scene::Instance>>& staticInstances,
    const PersistentMappedBuffer<MaterialIDPartitions>& dynamicMaterialIDs,
    const PersistentMappedBuffer<TransformPartitions>& dynamicTransforms,
    const PersistentMappedBuffer<LightingPartitions>& lightingTransforms) noexcept
{
    // We need to create replacement objects to initialise.
    auto scene          = SceneVAO { };
    auto drawCommands   = DrawCommands { };
    auto triangle       = FullScreenTriangleVAO { };
    auto lighting       = LightingVAO { };
    auto quad           = Mesh { };
    auto sphere         = Mesh { };
    auto cone           = Mesh { };
    auto internals      = std::make_unique<Internals>();

    // Initialise each object.
    if (!(scene.vao.initialise() && drawCommands.buffer.initialise() && triangle.vao.initialise() && 
        lighting.vao.initialise() && internals->initialise()))
    {
        return false;
    }

    // Start by configuring the VAOs.
    configureVAOs (scene, triangle, lighting, *internals, dynamicMaterialIDs, dynamicTransforms, lightingTransforms);

    // Construct the required geometry.
    buildMeshData (*internals);
    buildFullScreenTriangle (*internals);
    buildLighting (*internals, quad, sphere, cone);

    // Allow for static batching by filling the static buffers with instance information and draw commands.
    fillStaticBuffers (*internals, drawCommands, materials, staticInstances);

    // Finally we can make use of the successfully created data.
    m_scene         = std::move (scene);
    m_drawCommands  = std::move (drawCommands);
    m_triangle      = std::move (triangle);
    m_lighting      = std::move (lighting);
    m_quad          = std::move (quad);
    m_sphere        = std::move (sphere);
    m_cone          = std::move (cone);
    m_internals     = std::move (internals);

    return true;
}


template <typename MaterialIDPMB, typename TransformPMB, typename LightingPMB>
void Geometry::configureVAOs (SceneVAO& scene, FullScreenTriangleVAO& triangle, LightingVAO& lighting, 
    const Internals& internals, const MaterialIDPMB& dynamicMaterialIDs, const TransformPMB& dynamicTransforms, 
    const LightingPMB& lightingTransforms) const noexcept
{
    scene.attachVertexBuffers (
        internals.buffers[Internals::sceneVerticesIndex],
        internals.buffers[Internals::sceneElementsIndex],
        internals.buffers[Internals::transformsIndex],
        internals.buffers[Internals::materialIDsIndex],
        dynamicMaterialIDs,
        dynamicTransforms
    );

    triangle.attachVertexBuffers (
        internals.buffers[Internals::triangleVerticesIndex]
    );

    lighting.attachVertexBuffers (
        internals.buffers[Internals::lightVerticesIndex],
        internals.buffers[Internals::lightElementsIndex],
        lightingTransforms
    );

    scene.configureAttributes();
    triangle.configureAttributes();
    lighting.configureAttributes();
}

#endif // _RENDERING_RENDERER_GEOMETRY_