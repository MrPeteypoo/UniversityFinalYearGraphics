#pragma once

#if !defined    _RENDERING_OBJECTS_QUERY_
#define         _RENDERING_OBJECTS_QUERY_

// Engine headers.
#include <tgl/tgl.h>


/// <summary>
/// An RAII encapsulation of a query object. This can be used to query many different attributes such as frame time,
/// elapsed time, primitives generated, etc.
/// </summary>
class Query final
{
    public:

        Query() noexcept                = default;
        Query (Query&& move) noexcept;
        Query& operator= (Query&& move) noexcept;

        Query (const Query&)            = delete;
        Query& operator= (const Query&) = delete;

        ~Query() { clean(); }


        /// <summary> Check if the query is valid. </summary>
        inline bool isInitialised() const noexcept  { return m_query != 0U; }
        
        /// <summary> Gets the OpenGL ID of query object. </summary>
        inline GLuint getID() const noexcept        { return m_query; }


        /// <summary> 
        /// Attempt to create and start the query object. Successive calls will only modify the object if successful.
        /// </summary>
        /// <param name="target"> The type of query to run, e.g. GL_TIME_ELAPSED. </param>
        /// <returns> Whether the query was successfully created or not. </returns>
        bool initialise (const GLenum target) noexcept;

        /// <summary> 
        /// The object will be flagged for deletion by OpenGL.
        /// </summary> 
        void clean() noexcept;


        /// <summary> Flags the query to begin. </summary>
        void begin() const noexcept;

        /// <summary> Flags the query to end. </summary>
        void end() const noexcept;

        /// <summary> Retrieves the result of the query. </summary>
        /// <param name="flushGPU"> Whether the commands on the GPU should be flushed to force the result. </param>
        GLuint resultAsUInt (const bool flushGPU) const noexcept;

    private:

        GLuint m_query  { 0 };  //!< An OpenGL query object.
        GLenum m_target { 0 };  //!< The type of the query.
};

#endif // _RENDERING_OBJECTS_QUERY_