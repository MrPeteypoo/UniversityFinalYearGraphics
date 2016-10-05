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
    /// <param name="vertices"> An array to be filled with Vertex information. </param>
    /// <param name="mesh"> The mesh to retrieve Vertex data from. </param>
    void assembleVertices (std::vector<Vertex>& vertices, const scene::Mesh& mesh);
    

    /// <summary> Iterates through each SceneMode::Mesh in meshes calculating the total buffer size required for a vertex VBO and element VBO. </summary>
    /// <param name="meshes"> A container of all meshes which will exist in a VBO. </param>
    /// <param name="vertexSize"> The calculated size that a vertex array buffer needs to be. </param>
    /// <param name="elementSize"> The calculated size that an element array buffer needs to be. </param>
    void calculateVBOSize (const std::vector<scene::Mesh>& meshes, size_t& vertexSize, size_t& elementSize);


    /// <summary> Iterates through every material in a scene and fills the given vector with image data. </summary>
    /// <param name="images"> The vector to fill with data. This will generate filename-image pairs. </param>
    /// <param name="materials"> A container of materials to iterate through. </param>
    void loadImagesFromScene (std::vector<std::pair<std::string, tygra::Image>>& images, const std::vector<scene::Material>& materials);


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