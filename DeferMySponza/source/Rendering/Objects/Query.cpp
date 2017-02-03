#include "Query.hpp"


// STL headers.
#include <utility>


Query::Query (Query&& move) noexcept
{
    *this = std::move (move);
}


Query& Query::operator= (Query&& move) noexcept
{
    if (this != &move)
    {
        clean();

        m_query         = move.m_query;
        m_target        = move.m_target;
        move.m_query    = 0U;
        move.m_target   = 0U;
    }

    return *this;
}


bool Query::initialise (const GLenum target) noexcept
{
    auto query = GLuint { };
    glCreateQueries (target, 1, &query);

    if (query == 0U)
    {
        return false;
    }

    clean();
    m_query     = query;
    m_target    = target;
    return true;
}


void Query::clean() noexcept
{
    if (isInitialised())
    {
        glDeleteQueries (1, &m_query);
        m_target = 0U;
    }
}


void Query::begin() const noexcept
{
    glBeginQuery (m_target, m_query);
}


void Query::end() const noexcept
{
    glEndQuery (m_target);
}


GLuint Query::resultAsUInt (const bool flushGPU) const noexcept
{
    const auto param    = flushGPU ? GL_QUERY_RESULT : GL_QUERY_RESULT_NO_WAIT;
    auto result         = GLuint { 0 };

    glGetQueryObjectuiv (m_query, param, &result);
    return result;
}