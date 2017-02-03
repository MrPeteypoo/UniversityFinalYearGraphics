#pragma once

#if !defined    _RENDERING_OBJECTS_SYNC_
#define         _RENDERING_OBJECTS_SYNC_

// Engine headers.
#include <tgl/tgl.h>


/// <summary>
/// An RAII encapsulation of a sync object. Sync objects can be used to create fences which allow for the application
/// to synchronise with the GPU.
/// </summary>
class Sync final
{
    public:

        Sync() noexcept;
        Sync (Sync&& move) noexcept;
        Sync& operator= (Sync&& move) noexcept;

        Sync (const Sync&)              = delete;
        Sync& operator= (const Sync&)   = delete;

        ~Sync() { clean(); }


        /// <summary> Check if the sync is valid.. </summary>
        inline bool isInitialised() const noexcept  { return m_sync != 0U; }
        
        /// <summary> Gets the OpenGL ID of sync object. </summary>
        inline GLsync getID() const noexcept        { return m_sync; }


        /// <summary> 
        /// Attempt to create the sync object as a fence. Successive calls will only modify the object if successful.
        /// </summary>
        /// <returns> Whether the sync was successfully created or not. </returns>
        bool initialise() noexcept;

        /// <summary> 
        /// The object will be flagged for deletion by OpenGL and may not be deleted until all blocking commands
        /// have completed.
        /// </summary> 
        void clean() noexcept;


        /// <summary> Checks if the sync has been signalled by the GPU. This will not cause waiting. </summary>
        /// <returns> Whether the sync has been signalled by the GPU or not. </returns>
        bool checkIfSignalled() const noexcept;

        /// <summary> 
        /// This will trigger a wait on the CPU thread until the sync is signalled or the timeout expires. 
        /// </summary>
        /// <param name="flushGPU"> Whether the commands on the GPU should be flushed to force the signalling. </param>
        /// <param name="timeout"> Any timeout to apply to the waiting. This is in nanoseconds. </param>
        /// <returns> Whether the object was successfully signalled. </returns>
        bool waitForSignal (const bool flushGPU, const GLuint64 timeout) const noexcept;

    private:

        GLsync m_sync { 0 }; //!< An OpenGL sync object.

    private:

        /// <summary> Potentially waits on the CPU thread for the sync to be signalled. </summary>
        /// <param name="flags"> The flags to apply to the wait command. </param>
        /// <param name="timeout"> The maximum amount of time to wait. 0 causes no waiting. </param>
        /// <returns> Whether the sync object was signalled. </returns>
        bool waitOnCPUThread (const GLbitfield flags, const GLuint64 timeout) const noexcept;
};

#endif // _RENDERING_OBJECTS_SYNC_