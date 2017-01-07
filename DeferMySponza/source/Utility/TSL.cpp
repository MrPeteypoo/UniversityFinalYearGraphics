#include "TSL.hpp"


// STL headers.
#include <algorithm>


namespace util
{
    void addTSLMeshData (std::vector<glm::vec3>& vertices, std::vector<GLuint>& elements, 
        const tsl::IndexedMeshPtr& meshPointer) noexcept
    {
        // Retrieve the data for the mesh.
        const auto mesh             = meshPointer.get();
        const auto shapeVertices    = mesh->positionArray();
        const auto vertexCount      = mesh->vertexCount();
        const auto shapeElements    = mesh->indexArray();
        const auto elementCount     = mesh->indexCount();

        // Add each position.
        std::for_each (shapeVertices, shapeVertices + vertexCount, 
            [&] (const auto& position) { vertices.push_back (toGLM (position)); });

        // Add each element.
        std::for_each (shapeElements, shapeElements + elementCount,
            [&] (const auto element) { elements.push_back (static_cast<GLuint> (element)); });
    }
}
