#include "Materials.hpp"


// STL headers.
#include <utility>


// Engine headers.
#include <tygra/FileHelper.hpp>


// Personal headers.
#include <Rendering/Renderer/Materials/Internals/Internals.hpp>
#include <Utility/OpenGL/Textures.hpp>
#include <Utility/Algorithm.hpp>
#include <Utility/Scene.hpp>


// Namespaces.
using namespace std::string_literals;


Materials::Materials() noexcept
{
    m_internals = std::make_unique<Internals>();
}


Materials::Materials (Materials&& move) noexcept
{
    *this = std::move (move);
}


Materials::~Materials()
{
}


MaterialID Materials::operator[] (const scene::MaterialId sceneID) const noexcept
{
    const auto result = m_materialIDs.find (sceneID);

    return result != std::end (m_materialIDs) ? result->second : std::numeric_limits<MaterialID>::max();
}


GLint Materials::getMaterialTextureUnit() const noexcept
{
    return m_internals->materials.texture.getDesiredTextureUnit();
}


GLint Materials::getTextureArrayStartingUnit() const noexcept
{
    return m_internals->rgb.front().getDesiredTextureUnit();
}


GLint Materials::getTextureArrayCount() const noexcept
{
    return static_cast<GLint> (m_internals->rgb.size() + m_internals->rgba.size());
}


bool Materials::initialise (const scene::Context& scene, const GLuint startingTextureUnit) noexcept
{
    // Create new objects.
    auto ids        = MaterialIDs { };
    auto internals  = std::make_unique<Internals>();

    // Ensure initialisation works.
    if (!internals->initialise (startingTextureUnit))
    {
        return false;
    }

    // We need to determine how much memory to allocate for the texture arrays.
    if (!generateMaterials (ids, *internals, scene))
    {
        return false;
    }

    // Finally make use of the new data.
    m_materialIDs   = std::move (ids);
    m_internals     = std::move (internals);

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


bool Materials::generateMaterials (MaterialIDs& materialIDs, Internals& internals, 
            const scene::Context& scene) const noexcept
{
    // First we must obtain every material available in the scene.
    const auto sceneMaterials = util::getAllMaterials (scene);

    // Ensure textures load successfully before constructing the materials.
    if (!loadTextures (internals, sceneMaterials))
    {
        return false;
    }

    // We need to collect every generated material to load it into the GPU.
    auto materials = std::vector<Material> { };

    // Now we must parse each scene material and actually construct the GPU materials.
    for (const auto& sceneMaterial : sceneMaterials)
    {
        // Generate the new material.
        auto successAndMaterial = generateMaterial (internals, sceneMaterial);

        // Ensure no error occurred.
        if (!successAndMaterial.first)
        {
            return false;
        }

        materialIDs[sceneMaterial.id] = static_cast<GLuint> (materials.size());
        materials.emplace_back (std::move (successAndMaterial.second));
    }

    internals.materials.buffer.immutablyFillWith (materials);
    return true;
}


bool Materials::loadTextures (Internals& internals, const std::vector<PBSMaterial>& materials) const noexcept
{
    // Firstly we need to parse the materials to retrieve unique file locations.
    auto files = collectFileLocations (materials);

    // Using these files we can attempt to open them and sort them by format.
    auto textureResult = openTextures (files);

    // Ensure all textures loaded successfully.
    if (textureResult.first)
    {
        // Now we can load the textures into the GPU.
        return bufferTextures (internals, textureResult.second);
    }

    return false;
}


Materials::FileLocations Materials::collectFileLocations (const std::vector<PBSMaterial>& materials) const noexcept
{
    // We'll need a set to load strings into.
    auto files = FileLocations { };

    // Avoid duplication of code.
    const auto addIfNotEmpty = [&] (const auto& file) { if (!file.empty()) files.emplace (file); };

    // Simply interate through each material retrieving texture maps.
    for (const auto& material : materials)
    {
        addIfNotEmpty (material.physicsMap);
        addIfNotEmpty (material.albedoMap);
        addIfNotEmpty (material.normalMap);
    }

    return files;
}


std::pair<bool, Materials::TexturesToBuffer> Materials::openTextures (FileLocations& files) const noexcept
{
    // We'll need to store and modify the result.
    auto result = std::pair<bool, TexturesToBuffer> { };

    // Default the result to represent failure.
    result.first = false;

    // We need to interate through each file.
    for (auto& file : files)
    {
        // Attempt to load it.
        auto image = tygra::createImageFromPngFile (file);

        // Cache the format of the image.
        const auto width        = image.width();
        const auto height       = image.height();
        const auto components   = image.componentsPerPixel();

        // Ensure the image loaded properly and is a valid resolution.
        if (!(image.doesContainData() && Internals::areDimensionsSupported (width, height)))
        {
            return result;
        }

        // Map it based on it's dimensions and then component count.
        auto pair = std::make_pair (file, std::move (image));
        result.second[width][components].vector.emplace_back (std::move (pair));
    }

    // We've succeeded.
    result.first = true;
    return result;
}


bool Materials::bufferTextures (Internals& internals, TexturesToBuffer& textures) const noexcept
{
    // Start by ensuring we allocate the maximum amount 1x1 textures that we can.
    prepare1x1TextureArrays (internals);

    // We only support 3 and 4 channels right now so other images have to converted.
    auto extra = Images { };

    for (auto& dimensionMap : textures)
    {
        const auto dimensions = dimensionMap.first;

        for (size_t components { 1 }; components <= 4; ++components)
        {
            // Cache the image array.
            auto& images            = dimensionMap.second[components];
            const auto imageCount   = extra.vector.size() + images.vector.size();

            if (imageCount == 0)
            {
                continue;
            }

            // Check whether the current configuration has a dedicated texture unit.
            const auto indexAndArray    = internals.get (components, dimensions);
            const auto index            = indexAndArray.first;
            const auto textureArray     = indexAndArray.second;
            const auto format           = util::internalFormat (components);
            const auto levels           = static_cast<GLsizei> (5);

            if (textureArray)
            {
                // Allocate enough data if the textures aren't 1x1.
                if (dimensions != 1)
                {
                    const auto dim      = static_cast<GLsizei> (dimensions);
                    const auto count    = static_cast<GLsizei> (imageCount); 
                    textureArray->allocateImmutableStorage (format, dim, dim, count, levels);
                    textureArray->setParameter (GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    textureArray->setParameter (GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    textureArray->setParameter (GL_TEXTURE_WRAP_S, GL_REPEAT);
                    textureArray->setParameter (GL_TEXTURE_WRAP_T, GL_REPEAT);
                }
                    
                addTexturesToArray (internals, *textureArray, index, dimensions, components, images);
                addTexturesToArray (internals, *textureArray, index, dimensions, components, extra);
                extra.vector.clear();

                // Now generate mipmaps.
                textureArray->generateMipmap();
            }

            // If we don't have a texture array for this configuration then use the next.
            else
            {
                extra.vector.reserve (imageCount);
                std::for_each (images.vector, [&] (auto& a) { extra.vector.emplace_back (std::move (a)); });
                images.vector.clear();
            }
        }

        // Some unsupported images slipped through the cracks, this should never happen.
        if (extra.vector.size() > 0)
        {
            return false;
        }
    }

    return true;
}


void Materials::prepare1x1TextureArrays (Internals& internals) const noexcept
{
    // Firstly, we need to find out how many items we can store.
    const auto depth = Internals::getMaxArrayDepth();

    // Now allocate memory.
    internals.rgb.front().allocateImmutableStorage (util::internalFormat (3), 1, 1, depth, 1);
    internals.rgba.front().allocateImmutableStorage (util::internalFormat (4), 1, 1, depth, 1);
}


void Materials::addTexturesToArray (Internals& internals, Texture2DArray& array, const GLuint arrayIndex, 
            const size_t dimensions, const size_t components, const Images& images) const noexcept
{
    // We need to increment the count once we've uploaded the images.
    auto& count = internals.counts[dimensions][components];

    for (const auto& loadedImage : images.vector)
    {
        // Cache useful values.
        const auto& fileLocation    = loadedImage.first;
        const auto& image           = loadedImage.second;

        // Maps components to pixel formats.
        constexpr GLenum pixelFormats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };

        // Each image should take up an entire layer.
        const auto x        = GLint { 0 };
        const auto y        = GLint { 0 };
        const auto z        = static_cast<GLint> (count);
        const auto size     = static_cast<GLsizei> (dimensions);
        const auto depth    = GLsizei { 1 };
        const auto format   = pixelFormats[image.componentsPerPixel()];
        const auto type     = image.bytesPerComponent() == 1 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT;

        // Upload the image data.
        array.placeAt (x, y, z, size, size, depth, format, type, image.pixelData());

        // Finally set the index of the image.
        internals.ids[fileLocation] = { arrayIndex, static_cast<GLuint> (count++) };
    }
}


std::pair<bool, Material> Materials::generateMaterial (Internals& internals, const PBSMaterial& sceneMaterial) const noexcept
{
    // We need a material object to modify.
    auto material = Material { };

    // This is how properties will be set.
    const auto setProperty = [&] (auto& set, const auto& map, const auto& uniform)
    {
        // Use the map if one has been provided. If the map doesn't exist we wouldn't have made it to this point.
        if (!map.empty())
        {
            set = internals.ids[map];
        }

        // We'll need to construct an ID for the uniform and potentially buffer the uniform.
        else
        {
            // The ID should be each component with a space between.
            auto id = ""s;
            for (const auto component : uniform)
            {
                id += std::to_string (component) + " ";
            }

            // It's already been generated so use that index.
            if (internals.contains (id))
            {
                set = internals.ids[id];
            }

            // Add the uniform value manually.
            else
            {
                // Maps components to pixel formats.
                constexpr GLenum pixelFormats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };

                // Retrieve the array.
                const auto indexAndArray = internals.get (uniform.size(), 1);

                // Add the uniform to the end of the 1x1 array.
                auto& array = *indexAndArray.second;
                auto& count = internals.counts[1][uniform.size()];

                // Ensure we aren't going over the maximum depth.
                if (count == Internals::getMaxArrayDepth())
                {
                    return false;
                }

                // Place the uniform values at the end.
                array.placeAt (0, 0, static_cast<GLint> (count), 1, 1, 1, 
                    pixelFormats[uniform.size()], GL_UNSIGNED_BYTE, uniform.data());

                // Finally add the ID.
                internals.ids[id] = { indexAndArray.first, static_cast<GLuint> (count++) };
            }
        }

        return true;
    };

    // Set each property.
    if (setProperty (material.properties, sceneMaterial.physicsMap, sceneMaterial.physics) &&
        setProperty (material.albedo, sceneMaterial.albedoMap, sceneMaterial.albedo) &&
        setProperty (material.normal, sceneMaterial.normalMap, sceneMaterial.normal))
    {
        return { true, material };
    }

    return { false, Material { } };
}