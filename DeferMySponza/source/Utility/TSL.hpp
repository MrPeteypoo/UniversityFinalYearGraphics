#pragma once

#if !defined    _UTIL_TSL_
#define         _UTIL_TSL_

// STL headers.
#include <vector>


// Engine headers.
#include <glm/vec3.hpp>
#include <tgl/tgl.h>
#include <tsl/geometry.hpp>


// Personal headers.
#include <Rendering/Renderer/Geometry/Mesh.hpp>
#include <Rendering/Renderer/Types.hpp>


namespace util
{
    /// <summary> Adds TSL mesh to the given vertices and elements vector. </summary>
    /// <param name="meshBeingAdded"> The mesh to set the data for. </param>
    /// <param name="vertices"> The vertices vector to add vertices to. </param>
    /// <param name="elements"> The elements vector to add elements to. </param>
    /// <param name="meshPointer"> A mesh pointer containing triangle data. </param>
    /// <param name="offset"> Any applicable offset to be applied to each vertex position. </param>
    void addTSLMeshData (Mesh& meshBeingAdded, std::vector<types::VertexPosition>& vertices, 
        std::vector<types::Element>& elements, const tsl::IndexedMeshPtr& meshPointer, 
        const types::VertexPosition& offset = types::VertexPosition { 0.f }) noexcept;

    /// <summary> Constructs a glm::vec3 from a tsl::Vector3. </summary>
    inline glm::vec3 toGLM (const tsl::Vector3& vector) noexcept
    {
        return { vector.x, vector.y, vector.z };
    }
}

#endif // _UTIL_TSL_