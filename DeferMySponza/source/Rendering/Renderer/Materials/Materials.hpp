#pragma once

#if !defined    _RENDERING_RENDERER_MATERIALS_
#define         _RENDERING_RENDERER_MATERIALS_

// STL headers.
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>


// Engine headers.
#include <tgl/tgl.h>
#include <scene/scene_fwd.hpp>
#include <tygra/Image.hpp>


// Personal headers.
#include <Rendering/Renderer/Materials/Internals/Material.hpp>
#include <Rendering/Objects/Texture.hpp>


// Forward declarations.
struct PBSMaterial;


// Aliases.
using MaterialID = GLuint;


/// <summary>
/// Contains every piece of geometry in the scene. Static batching is supported with static instances having their
/// transforms permanently stored in the transforms buffer.
/// </summary>
class Materials final
{
    public:

        Materials() noexcept;
        Materials (Materials&&) noexcept;
        Materials& operator= (Materials&&) noexcept = default;
        ~Materials();

        Materials (const Materials&)                = delete;
        Materials& operator= (const Materials&)     = delete;


        /// <summary> Gets the material ID associated with the scene ID given. </summary>
        /// <param name="sceneID"> The scene::MaterialId to retrieve the MaterialID for. </param>
        /// <returns> The material ID if successful, the maximum value if not found. </returns> 
        MaterialID operator[] (const scene::MaterialId sceneID) const noexcept;

        /// <summary> Retrieves the texture unit of the first texture array. </summary>
        GLuint getFirstTextureUnit() const noexcept;

        /// <summary> Retrieves the texture unit of the last texture array. </summary>
        GLuint getLastTextureUnit() const noexcept;


        /// <summary> 
        /// Constructs every material in the scene, including loading every texture and mapping scene::MaterialId 
        /// values to built-in values. Successive calls will not change the object unless initialisation is successful.
        /// </summary>
        /// <param name="scene"> Contains every material in the scene. </param>
        /// <param name="startingTextureUnit"> The initial index to apply to stored textures. </param>
        /// <returns> Whether initialisation was successful or not. </returns>
        bool initialise (const scene::Context& scene, const GLuint startingTextureUnit) noexcept;

        /// <summary> Destroys every stored object and returns to a clean state. </summary>
        void clean() noexcept;


        /// <summary> Binds every texture unit with its associated texture. </summary>
        void bindTextures() const noexcept;

        /// <summary> Unbind every texture, leaving their associated texture units in a clean state. </summary>
        void unbindTextures() const noexcept;

    private:

        class Internals;

        using MaterialIDs   = std::unordered_map<scene::MaterialId, MaterialID>;
        using Pimpl         = std::unique_ptr<Internals>;

        MaterialIDs     m_materialIDs   { };    //!< Maps scene material IDs to stored GPU material IDs.
        Pimpl           m_internals     { };    //!< A pointer to internal managed data.

    private:

        /// <summary> This can't be an alias because Visual Studio truncates long symbol names. </summary>
        struct Images final
        {
            using ImageWithID = std::pair<std::string, tygra::Image>;
            std::vector<ImageWithID> vector { };
        };
        
        using FileLocations     = std::unordered_set<std::string>;
        using Dimensions        = size_t;
        using Components        = size_t;
        using TexturesToBuffer  = std::unordered_map<Dimensions, std::unordered_map<Components, Images>>;

        /// <summary> Generates the material data in the scene. </summary>
        bool generateMaterials (MaterialIDs& materialIDs, Internals& internals, 
            const scene::Context& scene) const noexcept;

        /// <summary> 
        /// Performs a pass through the given materials list, loading all textures in the scene, allocating enough
        /// memory in the texture arrays and finally loading the images into their respective arrays.
        /// </summary>
        bool loadTextures (Internals& internals, const std::vector<PBSMaterial>& materials) const noexcept;

        /// <summary> Iterates through the list of materials, collecting every texture map file location. </summary>
        FileLocations collectFileLocations (const std::vector<PBSMaterial>& materials) const noexcept;

        /// <summary> 
        /// Goes through the given set of file locations, loading each into a tygra::Image and mapping it based on its
        /// components and dimensions. 
        /// </summary>
        std::pair<bool, TexturesToBuffer> openTextures (FileLocations& files) const noexcept;

        /// <summary> Loads the given textures into texture arrays stored on the GPU. </summary>
        bool bufferTextures (Internals& internals, TexturesToBuffer& textures) const noexcept;

        /// <summary> Allocates memory for 2048 textures in the 1x1 array and adds default data. </summary>
        void prepare1x1TextureArrays (Internals& internals) const noexcept;

        /// <summary> Adds all of the given images to the given texture array, also updates the texture IDs. </summary>
        void addTexturesToArray (Internals& internals, Texture2DArray& array, const GLuint arrayIndex, 
            const size_t dimensions, const size_t components, const Images& images) const noexcept;

        /// <summary> Constructs a new material from the given scene material. </summary>
        std::pair<bool, Material> generateMaterial (Internals& internals, const PBSMaterial& sceneMaterial) const noexcept;
};

#endif // _RENDERING_RENDERER_MATERIALS_