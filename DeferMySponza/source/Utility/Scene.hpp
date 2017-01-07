#pragma once

#if !defined    _UTIL_SCENE_MODEL_
#define         _UTIL_SCENE_MODEL_

// STL headers.
#include <vector>


// Engine headers.
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x3.hpp>
#include <glm/mat4x4.hpp>
#include <scene/scene_fwd.hpp>
#include <scene/types.hpp>


// Forward declarations.
namespace tygra { class Image; }
struct Vertex;


namespace util
{
    /// <summary> Fills a given vector with vertex information which is obtained from the given mesh. </summary>
    /// <param name="mesh"> The mesh to retrieve Vertex data from. </param>
    /// <returns> An array to be filled with Vertex information. </returns>
    std::vector<Vertex> assembleVertices (const scene::Mesh& mesh) noexcept;
    

    /// <summary> Iterates through each given scene::Mesh and calculates the total number of vertices and elements. </summary>
    /// <param name="meshes"> A container of all meshes in the scene. </param>
    /// <param name="vertexCount"> Where to output the calculated number of required vertices. </param>
    /// <param name="elementCount"> Where to output the calculated number of required elements. </param>
    void calculateSceneSize (const std::vector<scene::Mesh>& meshes, 
        size_t& vertexCount, size_t& elementCount) noexcept;


    /// <summary> Iterates through every material in a scene and fills the given vector with image data. </summary>
    /// <param name="images"> The vector to fill with data. This will generate filename-image pairs. </param>
    /// <param name="materials"> A container of materials to iterate through. </param>
    void loadImagesFromScene (std::vector<std::pair<std::string, tygra::Image>>& images, 
        const std::vector<scene::Material>& materials) noexcept;


    inline glm::vec2 toGLM (const scene::Vector2& vector);
    inline glm::vec3 toGLM (const scene::Vector3& vector);
    inline glm::vec4 toGLM (const scene::Vector4& vector);
    inline glm::mat4x3 toGLM (const scene::Matrix4x3& matrix);
    inline glm::mat4 toGLM (const scene::Matrix4x4& matrix);
}


namespace util
{
    inline glm::vec2 toGLM (const scene::Vector2& vector)
    {
        return { vector.x, vector.y };
    }


    inline glm::vec3 toGLM (const scene::Vector3& vector)
    {
        return { vector.x, vector.y, vector.z };
    }


    inline glm::vec4 toGLM (const scene::Vector4& vector)
    {
        return { vector.x, vector.y, vector.z, vector.w };
    }


    inline glm::mat4x3 toGLM (const scene::Matrix4x3& matrix)
    {
        return 
        { 
            matrix.m00, matrix.m01, matrix.m02,
            matrix.m10, matrix.m11, matrix.m12,
            matrix.m20, matrix.m21, matrix.m22,
            matrix.m30, matrix.m31, matrix.m32
        };
    }


    inline glm::mat4 toGLM (const scene::Matrix4x4& matrix)
    {
        return 
        { 
            matrix.m00, matrix.m01, matrix.m02, matrix.m03,
            matrix.m10, matrix.m11, matrix.m12, matrix.m13,
            matrix.m20, matrix.m21, matrix.m22, matrix.m23,
            matrix.m30, matrix.m31, matrix.m32, matrix.m33
        };
    }
}

#endif // _UTIL_SCENE_MODEL_