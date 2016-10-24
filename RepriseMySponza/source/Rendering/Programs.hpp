#pragma once

#if !defined	_RENDERING_PROGRAMS_
#define			_RENDERING_PROGRAMS_


// Engine headers.
#include <tgl/tgl.h>


/// <summary>
/// Contains the OpenGL programs required to perform a rendering pass.
/// </summary>
class CorePrograms final
{
	public:
		CorePrograms() = default;
		CorePrograms (CorePrograms&&) = default;
		CorePrograms (const CorePrograms&) = delete;
		CorePrograms& operator= (const CorePrograms&) = delete;
	private:
		GLuint m_ambient { 0 };
};

#endif // _RENDERING_PROGRAMS_