#include "Geometry.hpp"


// STL headers.
#include <algorithm>
#include <iostream>


// Engine headers.
#include <scene/GeometryBuilder.hpp>
#include <scene/Mesh.hpp>


// Personal headers.
#include <Rendering/Renderer/Geometry/Internals/Vertex.hpp>
#include <Utility/Scene.hpp>


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

    // Iterate through each mesh adding them to the mesh container.
    auto vertexIndex    = GLintptr { 0 };
    void* elementOffset = nullptr;
    
	for (size_t i { 0 }; i < meshes.size(); ++i)
    {
        // Cache the required mesh data.
        const auto& sceneMesh   = meshes[i];
        const auto& elements    = sceneMesh.getElementArray();
        auto&       localMesh   = m_meshes[i].second;
        
        // Assign the local mesh an ID.
        m_meshes[i].first = sceneMesh.getId();

        localMesh.verticesIndex     = vertexIndex;
        localMesh.elementsOffset    = elementOffset;
        localMesh.elementCount      = static_cast<GLsizei> (elements.size());
        
        // Obtain the required vertex information.
        auto vertices = util::assembleVertices (sceneMesh);

        // Fill the vertex buffer objects with data.
        glBufferSubData (GL_ARRAY_BUFFER,           vertexIndex * sizeof (Vertex),  vertices.size() * sizeof (Vertex),          vertices.data());
        glBufferSubData (GL_ELEMENT_ARRAY_BUFFER,   elementOffset,                  elements.size() * sizeof (unsigned int),    elements.data());

        // The vertexIndex needs an actual index value whereas elementOffset needs to be in bytes.
        vertexIndex += vertices.size();
        elementOffset += elements.size() * sizeof (unsigned int);
    }
}


void Geometry::buildLighting (Internals& internals, Mesh& quad, Mesh& sphere, Mesh& cone) const noexcept
{

}


void Geometry::fillStaticBuffers (Internals& internals, Buffer& drawCommands,
            const std::map<scene::MeshId, std::vector<scene::Instance>>& staticInstances) const noexcept
{
    
}