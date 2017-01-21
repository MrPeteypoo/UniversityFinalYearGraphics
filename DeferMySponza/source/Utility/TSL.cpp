#include "TSL.hpp"


// STL headers.
#include <algorithm>
#include <numeric>


// Namespace declarations.
using namespace types;


namespace util
{
    void addTSLMeshData (Mesh& meshBeingAdded, std::vector<types::VertexPosition>& vertices, 
        std::vector<Element>& elements, const tsl::IndexedMeshPtr& meshPointer, 
        const VertexPosition& offset) noexcept
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
            [&] (const auto& position) { vertices.push_back (toGLM (position) + offset); });

        // Add each element.
        std::for_each (shapeElements, shapeElements + elementCount,
            [&] (const auto element) { elements.push_back (element); });
    }
}
