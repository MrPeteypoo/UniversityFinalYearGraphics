#include "LightingVAO.hpp"


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

    // The model transform must be added as three separate columns.
    vao.setAttributeFormat (modelTransformAttributeIndex, modelTransformAttributeCount, sizeof (glm::vec4),
                            VertexArray::AttributeLayout::Float32, 4, GL_FLOAT, 0);
}