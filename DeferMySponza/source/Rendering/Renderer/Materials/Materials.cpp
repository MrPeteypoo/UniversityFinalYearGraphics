#include "Materials.hpp"


// STL headers.
#include <utility>


// Personal headers.
#include <Rendering/Renderer/Materials/Internals/Internals.hpp>


Materials::Materials() noexcept
{
    m_internals = std::make_unique<Internals>();
}


Materials::Materials (Materials&& move) noexcept
{
    *this = std::move (move);
}


MaterialID Materials::operator[] (const scene::MaterialId sceneID) const noexcept
{
    const auto result = m_materialIDs.find (sceneID);

    return result != std::end (m_materialIDs) ? result->second : std::numeric_limits<MaterialID>::max();
}


bool Materials::initialise (const scene::Context* const scene, const GLuint startingTextureUnit) noexcept
{
    return true;
}


void Materials::clean() noexcept
{
    m_materialIDs.clear();
    m_internals->clean();
}


void Materials::bindTextures() const noexcept
{
    m_internals->bind();
}


void Materials::unbindTextures() const noexcept
{
    m_internals->unbind();
}