#include "TSL.hpp"


// STL headers.
#include <algorithm>
#include <numeric>


// Personal headers.
#include <Rendering/Renderer/Types.hpp>


namespace util
{
    void addTSLMeshData (Mesh& meshBeingAdded, std::vector<glm::vec3>& vertices, std::vector<types::Element>& elements, 
        const tsl::IndexedMeshPtr& meshPointer) noexcept
    {
        // Retrieve the data for the mesh.
        const auto mesh             = tsl::cloneIndexedMeshAsTriangleListPtr (meshPointer.get());
        const auto shapeVertices    = mesh->positionArray();
        const auto vertexCount      = mesh->vertexCount();
        const auto shapeElements    = mesh->indexArray();
        const auto elementCount     = mesh->indexCount();

        // Modify the mesh first.
        meshBeingAdded.verticesIndex    = static_cast<GLuint> (vertices.size());
        meshBeingAdded.elementsIndex    = static_cast<GLuint> (elements.size());
        meshBeingAdded.elementCount     = static_cast<GLuint> (elementCount);

        // Add each position.
        std::for_each (shapeVertices, shapeVertices + vertexCount, 
            [&] (const auto& position) { vertices.push_back (toGLM (position)); });

        // Add each element.
        std::for_each (shapeElements, shapeElements + elementCount,
            [&] (const auto element) { elements.push_back (element); });
    }
}
