#pragma once

#if !defined    _RENDERING_COMPOSITES_DRAW_COMMANDS_
#define         _RENDERING_COMPOSITES_DRAW_COMMANDS_

// Engine headers.
#include <tgl/tgl.h>


// Personal headers.
#include <Rendering/Binders/BufferBinder.hpp>


// Enforce 4-byte alignment so draw commands can be made with a stride of 0 bytes.
#pragma pack (push, 4)

/// <summary> A command which enables the drawing of multiple instances of an object. </summary>
struct MultiDrawElementsIndirectCommand final
{
    GLuint elementCount     { 0 };  //!< How many elements make up the object.
    GLuint instanceCount    { 0 };  //!< How many instances should be drawn.
    GLuint firstElement     { 0 };  //!< The index of the first element.
    GLuint baseVertex       { 0 };  //!< The index of the first vertex.
    GLuint baseInstance     { 0 };  //!< Which instance to start drawing from.

    MultiDrawElementsIndirectCommand (const GLuint elementCount, const GLuint instanceCount, const GLuint firstElement,
        const GLuint baseVertex, const GLuint baseInstance) noexcept
        :   elementCount (elementCount), instanceCount (instanceCount), firstElement (firstElement), 
            baseVertex (baseVertex), baseInstance (baseInstance)
    {
    }
    
    MultiDrawElementsIndirectCommand() noexcept                                                     = default;
    MultiDrawElementsIndirectCommand (MultiDrawElementsIndirectCommand&&) noexcept                  = default;
    MultiDrawElementsIndirectCommand (const MultiDrawElementsIndirectCommand&) noexcept             = default;
    MultiDrawElementsIndirectCommand& operator= (const MultiDrawElementsIndirectCommand&) noexcept  = default;
    MultiDrawElementsIndirectCommand& operator= (MultiDrawElementsIndirectCommand&&) noexcept       = default;
    ~MultiDrawElementsIndirectCommand()                                                             = default;
};

#pragma pack (pop)


/// <summary> 
/// Contains the parameters required to perform a glMultiDrawElementsIndirect call. 
/// </summary>
template <typename BufferType>
struct MultiDrawCommands final
{
    BufferType  buffer      { };                    //!< A buffer containing draw commands.
    GLenum      mode        { GL_TRIANGLES };       //!< The primitive type to use for drawing.
    GLenum      type        { GL_UNSIGNED_INT };    //!< The underlying type of the element array buffer.
    size_t      start       { 0 };                  //!< The byte offset to the first draw command in the buffer.
    GLsizei     count       { 0 };                  //!< How many draw commands from the starting offset to call.
    GLsizei     capacity    { 0 };                  //!< The maximum capacity of the draw command buffer, useful for batching.


    MultiDrawCommands (const GLenum mode, const GLenum type, const size_t start, const GLsizei count, 
        const GLsizei capacity) noexcept
        : mode (mode), type (type), start (start), count (count), capacity (capacity)
    {
    }
    
    MultiDrawCommands()                                                 = default;
    MultiDrawCommands (MultiDrawCommands&&)                             = default;
    MultiDrawCommands (const MultiDrawCommands&) noexcept               = default;
    MultiDrawCommands& operator= (const MultiDrawCommands&) noexcept    = default;
    MultiDrawCommands& operator= (MultiDrawCommands&&) noexcept         = default;
    ~MultiDrawCommands()                                                = default;

    /// <summary>
    /// Binds the stored buffer to GL_DRAW_INDIRECT_BUFFER and performs a draw of the buffer with the stored offset and
    /// count values. The buffer will be unbound after drawing.
    /// </summary>
    void draw() const noexcept
    {
        const auto binder = BufferBinder<GL_DRAW_INDIRECT_BUFFER> (buffer.getID());
        drawWithoutBinding();
    }

    /// <summary> 
    /// Performs a draw command with the stored offset and count values. Assumes the buffer has already been bound to
    /// GL_DRAW_INDIRECT_BUFFER.
    /// </summary>
    void drawWithoutBinding() const noexcept
    {
        glMultiDrawElementsIndirect (mode, type, (void*) start, count, 0);
    }
};

#endif // _RENDERING_COMPOSITES_DRAW_COMMANDS_