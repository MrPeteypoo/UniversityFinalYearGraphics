#include "Geometry.hpp"


// STL headers.
#include <algorithm>
#include <iostream>


// Engine headers.
#include <scene/GeometryBuilder.hpp>
#include <scene/Mesh.hpp>
#include <tsl/shapes.hpp>


// Personal headers.
#include <Rendering/Renderer/Geometry/Internals/Vertex.hpp>
#include <Rendering/Renderer/Materials/Materials.hpp>
#include <Utility/Scene.hpp>
#include <Utility/TSL.hpp>


Geometry::Geometry() noexcept
{
    try
    {
        m_pimpl = std::make_unique<Internals>();
    }

    catch (const std::exception& e)
    {
        std::cerr << "Geometry::Geometry() couldn't allocate memory for internal data: " << e.what() << std::endl;
    }
}


bool Geometry::isInitialised() const noexcept
{
    return m_scene.vao.isInitialised() && m_drawCommands.isInitialised() && m_lighting.vao.isInitialised() && 
        m_pimpl->isInitialised();
}


void Geometry::clean() noexcept
{
     m_scene.vao.clean();
     m_drawCommands.clean();
     m_lighting.vao.clean();
     m_pimpl->clean();

     m_quad = m_sphere = m_cone = Mesh { };
}


void Geometry::buildMeshData (Internals& internals) const noexcept
{
    // Begin to construct the scene. We take a copy of the meshes data so we can sort it.
    auto meshes = scene::GeometryBuilder().getAllMeshes();

    // Ensure the meshes are sorted in order of their ID.
    std::sort (std::begin (meshes), std::end (meshes), 
        [] (const auto& a, const auto& b) { return a.getId() < b.getId(); });

    // We'll need a temporary vectors to store the vertex and element data. 
    auto vertices       = std::vector<Vertex> { };
    auto elements       = std::vector<GLuint> { };
    auto vertexCount    = size_t { 0 };
    auto elementCount   = size_t { 0 };

    // Start by allocating enough memory in each container to store the scene.
    util::calculateSceneSize (meshes, vertexCount, elementCount);
    internals.sceneMeshes.reserve (meshes.size());
    vertices.reserve (vertexCount);
    elements.reserve (elementCount);

    // Iterate through each mesh adding the vertices, elements and mapping to their corresponding container.
    auto mesh           = Mesh { };
    auto vertexIndex    = GLintptr { 0 };
    auto elementOffset  = size_t { 0 };
    
	for (const auto& sceneMesh : meshes)
    {
        // Retrieve the required mesh data.
        const auto meshVertices     = util::assembleVertices (sceneMesh);
        const auto& meshElements    = sceneMesh.getElementArray();
        
        // Set the mesh parameters, the element offset must be a pointer type.
        mesh.verticesIndex  = vertexIndex;
        mesh.elementsOffset = (void*) elementOffset;
        mesh.elementCount   = static_cast<GLsizei> (meshElements.size());

        // Now we can add the mesh to the map and the vertices/elements to the vectors.
        internals.sceneMeshes[sceneMesh.getId()] = mesh;
        vertices.insert (std::end (vertices), std::begin (meshVertices), std::end (meshVertices));
        elements.insert (std::end (elements), std::begin (meshElements), std::end (meshElements));

        // The vertexIndex needs an actual index value whereas elementOffset needs to be in bytes.
        vertexIndex     += meshVertices.size();
        elementOffset   += meshElements.size() * sizeof (GLuint);
    }

    // Now we can fill the mesh and element buffer. We will leave them with no access flags so they can be static.
    internals.buffers[internals.sceneVerticesIndex].immutablyFillWith (vertices);
    internals.buffers[internals.sceneElementsIndex].immutablyFillWith (elements);
}


void Geometry::buildLighting (Internals& internals, Mesh& quad, Mesh& sphere, Mesh& cone) const noexcept
{
    // Light volumes only contain a position but all shapes will be stored in the same buffer like scene meshes.
    auto vertices = std::vector<glm::vec3> { };
    auto elements = std::vector<GLuint> { };

    // Reserve 512KiB for shape vertices and elements, this will likely be more than enough.
    constexpr auto reservation = 256'000;
    vertices.reserve (reservation / sizeof (glm::vec3));
    elements.reserve (reservation / sizeof (GLuint));

    // Quads are very simple shapes.
    constexpr auto quadVertices = std::array<glm::vec3, 4> 
    { 
        glm::vec3 { -1, -1, 0 }, glm::vec3 { 1, -1, 0 },
        glm::vec3 { -1,  1, 0 }, glm::vec3 { 1,  1, 0 }
    };
    constexpr auto quadElements = std::array<GLuint, 6> { 0, 1, 2, 1, 3, 2 };

    // Add the quad to the vectors.
    vertices.insert (std::end (vertices), std::begin (quadVertices), std::end (quadVertices));
    elements.insert (std::end (elements), std::begin (quadElements), std::end (quadElements));

    // Now add the sphere and cone.
    util::addTSLMeshData (vertices, elements, tsl::createSpherePtr (1.f, 12));
    util::addTSLMeshData (vertices, elements, tsl::createConePtr (1.f, 1.f, 12));

    // Finally fill the GPU buffers.
    internals.buffers[internals.lightVerticesIndex].immutablyFillWith (vertices);
    internals.buffers[internals.lightElementsIndex].immutablyFillWith (elements);
}


void Geometry::fillStaticBuffers (Internals& internals, Buffer& drawCommands, const Materials& materials,
            const std::map<scene::MeshId, std::vector<scene::Instance>>& staticInstances) const noexcept
{
    
}