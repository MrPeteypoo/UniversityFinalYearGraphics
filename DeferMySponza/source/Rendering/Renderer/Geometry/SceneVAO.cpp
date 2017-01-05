#include "SceneVAO.hpp"


void SceneVAO::configureAttributes() noexcept
{
    // Enable each attribute.
    vao.setAttributeStatus (positionAttributeIndex, true);
    vao.setAttributeStatus (normalAttributeIndex, true);
    vao.setAttributeStatus (texturePointAttributeIndex, true);
    vao.setAttributeStatus (materialIDAttributeIndex, true);
    vao.setAttributeStatus (modelTransformAttributeIndex, true);

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
                            3, GL_FLOAT, 12);
    vao.setAttributeFormat (texturePointAttributeIndex, VertexArray::AttributeLayout::Float32,
                            2, GL_FLOAT, 24);

    // The material ID should be stored as an integer.
    vao.setAttributeFormat (materialIDAttributeIndex, VertexArray::AttributeLayout::Integer,
                            1, GL_INT, 0);

    // The model transform must be added as three separate columns.
    vao.setAttributeFormat (modelTransformAttributeIndex, VertexArray::AttributeLayout::Float32,
                            4, GL_FLOAT, 0);
    vao.setAttributeFormat (modelTransformAttributeIndex + 1, VertexArray::AttributeLayout::Float32,
                            4, GL_FLOAT, sizeof (GLfloat) * 4);
    vao.setAttributeFormat (modelTransformAttributeIndex + 2, VertexArray::AttributeLayout::Float32,
                            4, GL_FLOAT, sizeof (GLfloat) * 8);
}