#include "Sync.hpp"


// STL headers.
#include <cassert>
#include <utility>


Sync::Sync() noexcept
{
}


Sync::Sync (Sync&& move) noexcept
{
    *this = std::move (move);
}


Sync& Sync::operator= (Sync&& move) noexcept
{
    if (this != &move)
    {
        clean();

        m_sync      = move.m_sync;
        move.m_sync = 0;
    }
    
    return *this;
}


bool Sync::initialise() noexcept
{
    // Don't overwrite the sync unless we succeed. Only the fence sync exists and it requires the given parameters.
    auto sync = glFenceSync (GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    if (sync == 0)
    {
        return false;
    }

    // Ensure we don't leak.
    clean();
    m_sync = sync;

    return true;
}


void Sync::clean() noexcept
{
    if (isInitialised())
    {
        glDeleteSync (m_sync);
        m_sync = 0;
    }
}


bool Sync::checkIfSignalled() const noexcept
{
    // Ensure we don't cause a GPU flush and instantly timeout.
    return waitOnCPUThread (0, 0);
}


bool Sync::waitForSignal (const bool flushGPU, const GLuint64 timeout) const noexcept
{
    return flushGPU ? waitOnCPUThread (GL_SYNC_FLUSH_COMMANDS_BIT, timeout) : waitOnCPUThread (0, timeout);
}


bool Sync::waitOnCPUThread (const GLbitfield flags, const GLuint64 timeout) const noexcept
{
    // We need to verify the result of the wait.
    const auto result = glClientWaitSync (m_sync, flags, timeout);

    switch (result)
    {
        // The sync object has been signalled.
        case GL_ALREADY_SIGNALED:
        case GL_CONDITION_SATISFIED:
            return true;

        // We don't handle the case of the wait failing.
        case GL_WAIT_FAILED:
            assert (false);
    }
    
    return false;
}