#include "SceneVAO.hpp"


void SceneVAO::configureAttributes() noexcept
{
    // Enable each attribute.
    vao.setAttributeStatus (positionAttributeIndex, true);
    vao.setAttributeStatus (normalAttributeIndex, true);
    vao.setAttributeStatus (texturePointAttributeIndex, true);
    vao.setAttributeStatus (materialIDAttributeIndex, true);
    vao.setAttributeStatus (modelTransformAttributeIndex, modelTransformAttributeCount, true);

    // Vertex information is interleaved in the same buffer.
    vao.setAttributeBufferBinding (positionAttributeIndex, meshesBufferIndex);
    vao.setAttributeBufferBinding (normalAttributeIndex, meshesBufferIndex);
    vao.setAttributeBufferBinding (texturePointAttributeIndex, meshesBufferIndex);
    
    // Use static buffers by default for instance data.
    useStaticBuffers();

    // We must interleave the vertex information.
    vao.setAttributeFormat (positionAttributeIndex, VertexArray::AttributeLayout::Float32,
                            3, GL_FLOAT, 0);
    vao.setAttributeFormat (normalAttributeIndex, VertexArray::AttributeLayout::Float32,
                            3, GL_FLOAT, sizeof (glm::vec3));
    vao.setAttributeFormat (texturePointAttributeIndex, VertexArray::AttributeLayout::Float32,
                            2, GL_FLOAT, sizeof (glm::vec3) * 2);

    // The material ID should be stored as an integer.
    vao.setAttributeFormat (materialIDAttributeIndex, VertexArray::AttributeLayout::Integer,
                            1, GL_INT, 0);

    // The model transform must be added as three separate columns.
    vao.setAttributeFormat (modelTransformAttributeIndex, modelTransformAttributeCount, sizeof (glm::vec4), 
                            VertexArray::AttributeLayout::Float32, 4, GL_FLOAT, 0);
}


void SceneVAO::useStaticBuffers() noexcept
{
    // Material IDs require a single attribute.
    vao.setAttributeBufferBinding (materialIDAttributeIndex, staticMaterialIDsBufferIndex);

    // Model transforms are a 4x3 matrix.
    vao.setAttributeBufferBinding (modelTransformAttributeIndex, modelTransformAttributeCount, 
        staticTransformsBufferIndex);
}