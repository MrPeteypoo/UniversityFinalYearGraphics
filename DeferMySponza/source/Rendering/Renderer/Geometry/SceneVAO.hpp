#pragma once

#if !defined    _RENDERING_RENDERER_GEOMETRY_SCENE_VAO_
#define         _RENDERING_RENDERER_GEOMETRY_SCENE_VAO_

// Personal headers.
#include <Rendering/Objects/VertexArray.hpp>


/// <summary> 
/// A VAO used for storing scene geometry with compile-time constants for the buffer and attribute indices.
/// </summary>
struct SceneVAO final
{
    VertexArray vao { }; //!< A VAO containing all renderable meshes in the scene.

    constexpr static auto meshesBufferIndex             = GLuint { 0 }; //!< The binding index where the mesh buffer for all objects will be bound.
    constexpr static auto staticTransformsBufferIndex   = GLuint { 1 }; //!< The binding index where the transform buffer for static objects will be bound.
    constexpr static auto staticMaterialIDsBufferIndex  = GLuint { 2 }; //!< The binding index where the material IDs buffer for dynamic objects will be bound.
    constexpr static auto dynamicMaterialIDsBufferIndex = GLuint { 3 }; //!< The base binding index where the material IDs for dynamic objects will be bound.
    constexpr static auto dynamicTransformsBufferIndex  = GLuint { 4 }; //!< The base binding index, which will be adjusted by the amount of buffering of the material IDs buffer, where the transform buffer for dynamic objects will start being bound.
    
    constexpr static auto positionAttributeIndex        = GLuint { 0 }; //!< The attribute index for vertex position.
    constexpr static auto normalAttributeIndex          = GLuint { 1 }; //!< The attribute index for vertex normal.
    constexpr static auto texturePointAttributeIndex    = GLuint { 2 }; //!< The attribute index for vertex texture co-ordinate.
    constexpr static auto materialIDAttributeIndex      = GLuint { 3 }; //!< The attribute index for instanced material IDs.
    constexpr static auto modelTransformAttributeIndex  = GLuint { 4 }; //!< The attribute index for instanced model transforms.

    constexpr static auto modelTransformAttributeCount  = GLuint { 3 }; //!< The model transform requires three attributes.
    

    SceneVAO() noexcept                             = default;
    SceneVAO (SceneVAO&&) noexcept                  = default;
    SceneVAO (const SceneVAO&) noexcept             = default;
    SceneVAO& operator= (const SceneVAO&) noexcept  = default;
    SceneVAO& operator= (SceneVAO&&) noexcept       = default;
    ~SceneVAO()                                     = default;


    /// <summary> Attachs the given buffers to the VAO based on the compile-time indices in the class. </summary>
    template <size_t MaterialIDPartitions, size_t TransformPartitions>
    void attachVertexBuffers (const Buffer& meshes, const Buffer& elements, 
        const Buffer& staticTransforms, const Buffer& staticMaterialIDs,
        const PMB<MaterialIDPartitions>& dynamicMaterialIDs, const PMB<TransformPartitions>& dynamicTransforms) noexcept;

    /// <summary> Sets the binding points and formatting of attributes in the VAO. </summary>
    void configureAttributes() noexcept;

    /// <summary> Configures the instanced attributes to retrieve data from the static buffers. </summary>
    void useStaticBuffers() noexcept;

    /// <summary> Configures the instanced attributes to retreieve data from the dynamic buffers. </summary>
    /// <param name="materialIDIndex"> The partition index of the material ID buffer to use. </param>
    /// <param name="materialIDIndex"> The partition index of the transform buffer to use. </param>
    template <size_t MaterialIDPartitions>
    void useDynamicBuffers (const size_t materialIDIndex, const size_t transformIndex) noexcept;
};


// Engine headers.
#include <glm/mat4x3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


template <size_t MaterialIDPartitions, size_t TransformPartitions>
void SceneVAO::attachVertexBuffers (const Buffer& meshes, const Buffer& elements, 
    const Buffer& staticTransforms, const Buffer& staticMaterialIDs,
    const PMB<MaterialIDPartitions>& dynamicMaterialIDs, const PMB<TransformPartitions>& dynamicTransforms) noexcept
{
    // Position, normal and texture co-ordinates are passed for every vertex.
    constexpr auto meshesStride = GLuint { sizeof (glm::vec3) * 2 + sizeof (glm::vec2) };

    // Material ID is an integer.
    constexpr auto materialIDStride = GLuint { sizeof (GLuint) };

    // Model transforms are a 4x3 matrix.
    constexpr auto modelStride = GLuint { sizeof (glm::mat4x3) };

    // Instancing data contains one item per instance.
    constexpr auto divisor = GLuint { 1 };

    // Attach static buffers.
    vao.attachVertexBuffer (meshes, meshesBufferIndex, 0, meshesStride);
    vao.attachVertexBuffer (staticMaterialIDs, staticMaterialIDsBufferIndex, 0, materialIDStride, divisor);
    vao.attachVertexBuffer (staticTransforms, staticTransformsBufferIndex, 0, modelStride, divisor);
    vao.setElementBuffer (elements);

    // Attach dynamic buffers.
    constexpr auto adjustedTransformIndex = dynamicTransformsBufferIndex + MaterialIDPartitions - 1;

    vao.attachPersistentMappedBuffer (dynamicMaterialIDs, dynamicMaterialIDsBufferIndex, materialIDStride, divisor);
    vao.attachPersistentMappedBuffer (dynamicTransforms, adjustedTransformIndex, modelStride, divisor);
}


template <size_t MaterialIDPartitions>
void SceneVAO::useDynamicBuffers (const size_t materialIDIndex, const size_t transformIndex) noexcept
{
    // We need to adjust the starting index for the transform buffer because the material buffer may be multi-buffered.
    constexpr auto adjustedTransformIndex = dynamicTransformsBufferIndex + MaterialIDPartitions - 1;

    // Now we can calculate the correct buffer index.
    const auto materialIDPartitionIndex = dynamicMaterialIDsBufferIndex + materialIDIndex;
    const auto transformPartitionIndex  = adjustedTransformIndex + transformIndex;

    // Material IDs require a single attribute.
    vao.setAttributeBufferBinding (materialIDAttributeIndex, materialIDPartitionIndex);

    // Model transforms are a 4x3 matrix.
    vao.setAttributeBufferBinding (modelTransformAttributeIndex, modelTransformAttributeCount,
        transformPartitionIndex);
}

#endif // _RENDERING_RENDERER_GEOMETRY_SCENE_VAO_