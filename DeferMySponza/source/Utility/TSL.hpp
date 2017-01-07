#pragma once

#if !defined    _UTIL_TSL_
#define         _UTIL_TSL_

// STL headers.
#include <vector>


// Engine headers.
#include <glm/vec3.hpp>
#include <tgl/tgl.h>
#include <tsl/geometry.hpp>


namespace util
{
    /// <summary> Adds TSL mesh to the given vertices and elements vector. </summary>
    /// <param name="vertices"> The vertices vector to add vertices to. </param>
    /// <param name="elements"> The elements vector to add elements to. </param>
    void addTSLMeshData (std::vector<glm::vec3>& vertices, std::vector<GLuint>& elements, 
        const tsl::IndexedMeshPtr& meshPointer) noexcept;

    /// <summary> Constructs a glm::vec3 from a tsl::Vector3. </summary>
    inline glm::vec3 toGLM (const tsl::Vector3& vector) noexcept
    {
        return { vector.x, vector.y, vector.z };
    }
}

#endif // _UTIL_TSL_