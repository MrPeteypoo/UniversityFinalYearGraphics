#include "LightingVAO.hpp"


// Namespaces.
using namespace types;


void LightingVAO::configureAttributes() noexcept
{
    // Enable each attribute.
    vao.setAttributeStatus (positionAttributeIndex, true);
    vao.setAttributeStatus (modelTransformAttributeIndex, modelTransformAttributeCount, true);

    // Vertex position is the only attribute for meshes.
    vao.setAttributeBufferBinding (positionAttributeIndex, meshesBufferIndex);

    // We require multiple attributes for the model transforms.
    vao.setAttributeBufferBinding (modelTransformAttributeIndex, modelTransformAttributeCount, modelTransformsBufferIndex);

    // We must interleave the vertex information.
    vao.setAttributeFormat (positionAttributeIndex, VertexArray::AttributeLayout::Float32,
                            3, GL_FLOAT, 0);

    // The model transform must be added as multiple separate columns.
    constexpr auto componentCount   = GLint { sizeof (glm::vec3) / sizeof (GLfloat) };
    constexpr auto attributeStride  = GLuint { componentCount * sizeof (GLfloat) };
    vao.setAttributeFormat (modelTransformAttributeIndex, modelTransformAttributeCount, attributeStride,
                            VertexArray::AttributeLayout::Float32, componentCount, GL_FLOAT, 0);
}


void LightingVAO::useTransformPartition (const size_t partition) noexcept
{
    // Now we can calculate the correct buffer index.
    const auto transformPartitionIndex  = modelTransformsBufferIndex + partition;

    // Model transforms are a 4x3 matrix.
    vao.setAttributeBufferBinding (modelTransformAttributeIndex, modelTransformAttributeCount,
        static_cast<GLuint> (transformPartitionIndex));
}