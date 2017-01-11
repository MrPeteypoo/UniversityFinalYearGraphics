#pragma once

#if !defined    _RENDERING_UNIFORMS_
#define         _RENDERING_UNIFORMS_

// STL headers.
#include <type_traits>
#include <unordered_map>


// Personal headers.
#include <Rendering/Composites/PersistentMappedBuffer.hpp>
#include <Rendering/Renderer/Uniforms/Textures.hpp>


// Forward declarations.
struct DirectionalLight;
class GeometryBuffer; 
class Materials;
class Program;
struct Programs;
struct PointLight;
struct Scene;
struct Spotlight;
template <typename T> struct FullBlock;


/// <summary>
/// Contains and manages the uniform buffer objects used by all the programs used by the renderer.
/// </summary>
class Uniforms final
{
    public:

        /// <summary>
        /// A mapped pointer and byte offset pair, used to write to the UBO.
        /// </summary>
        template <typename T, GLuint Block>
        struct Data final
        {
            T*          data    { nullptr };    //!< A pointer to the start of the uniform data.
            GLintptr    offset  { 0 };          //!< The amount of bytes into the buffer where the data starts.
            
            constexpr static auto blockBinding = Block; //!< The desired block binding of the uniform block.
        };

        // Aliases.
        using GBuffer           = Data<Textures::GBuffer,           0>;
        using TextureArrays     = Data<Textures::Arrays,            1>;
        using Scene             = Data<Scene,                       2>;
        using DirectionalLights = Data<FullBlock<DirectionalLight>, 3>;
        using PointLights       = Data<FullBlock<PointLight>,       4>;
        using Spotlights        = Data<FullBlock<Spotlight>,        5>;

    public:

        Uniforms() noexcept {}
        Uniforms (Uniforms&&) noexcept              = default;
        Uniforms& operator= (Uniforms&&) noexcept   = default;
        ~Uniforms()                                 = default;

        Uniforms (const Uniforms&)                  = delete;
        Uniforms& operator= (const Uniforms&)       = delete;
        

        /// <summary> Gets a copy of the pointer and partition offset to modifiable scene data. </summary>
        Scene getWritableSceneData() const noexcept                         { return m_scene; }

        /// <summary> Gets a copy of the pointer and partition offset to modifiable directional light data. </summary>
        DirectionalLights getWritableDirectionalLightData() const noexcept  { return m_directional; }

        /// <summary> Gets a copy of the pointer and partition offset to modifiable point light data. </summary>
        PointLights getWritablePointLightData() const noexcept              { return m_point; }

        /// <summary> Gets a copy of the pointer and partition offset to modifiable spotlight data. </summary>
        Spotlights getWritableSpotlightData() const noexcept                { return m_spot; }


        /// <summary>
        /// Attempts to initialise the uniform buffer by allocating enough memory for each uniform block. Also fills
        /// the static uniform blocks with data, such as the Gbuffer and texture arrays block. This will reset the
        /// bound partition to zero. Successive calls will not modify the object if initialisation fails.
        /// </summary>
        /// <param name="geometryBuffer"> Used to map the gbuffer textures to the correct sampler. </param>
        /// <param name="materials"> Used to map the texture arrays to the correct samplers. </param>
        /// <returns> Whether initialisation was successful. </returns>
        bool initialise (const GeometryBuffer& geometryBuffer, const Materials& materials) noexcept;

        /// <summary> Cleans every stored object, freeing memory for the GPU. </summary>
        void clean() noexcept;


        /// <summary> Attempts to bind every block to the given program. </summary>
        void bindBlocksToProgram (const Programs& programs) const noexcept;

        /// <summary> 
        /// Resets the bound range of each dynamic uniform block to the given partition. This will invalidate any
        /// previously retrieved pointers.
        /// </summary>
        void bindBlocksToPartition (const size_t partitionIndex) noexcept;

        /// <summary> Informs OpenGL that the given data range has been written to. </summary>
        /// <param name="range"> The range of the modified data. </param>
        void notifyModifiedDataRange (const ModifiedRange& range) noexcept;

    private:

        using BlockNames = std::unordered_map<GLuint, const char*>;
        
        Scene               m_scene;            //!< Contains universal data about the scene.
        DirectionalLights   m_directional;      //!< Contains the parameters of every directional light in the scene.
        PointLights         m_point;            //!< Contains the parameters of many point lights in the scene.
        Spotlights          m_spot;             //!< Contains the parameters of many spotlights in the scene.

        Buffer              m_staticBlocks;     //!< Contains static uniform buffer data such as Gbuffer and array textures.
        SinglePMB           m_dynamicBlocks;    //!< A multi-buffered uniform buffer object containing uniform block data.
        size_t              m_partition;        //!< The currently cound partition.

        /// <summary> Maps block binding indices to block names as found in shaders. </summary>
        const BlockNames m_blockNames
        {
            { GBuffer::blockBinding,            "gbuffer" },
            { TextureArrays::blockBinding,      "textures" },
            { Scene::blockBinding,              "scene" },
            { DirectionalLights::blockBinding,  "directionalLights" },
            { PointLights::blockBinding,        "pointLights" },
            { Spotlights::blockBinding,         "spotlights" },
        };
        
        static GLint alignment; //!< How many bytes the uniform buffer blocks must be aligned to.

    private:

        /// <summary> Calculate the amount of memory to allocate for the dynamic blocks UBO. </summary>
        GLintptr calculateDynamicBlockSize() const noexcept;

        /// <summary> Determines the size of a type, ensuring it's aligned with a block boundary. </summary>
        template <typename T>
        GLintptr calculateAlignedSize() const noexcept;

        /// <summary> Build data for static blocks and bind each block correctly. </summary>
        /// <returns> Whether the static block buffer was built correctly. </returns>
        bool buildStaticBlocks (Buffer& staticBlocks, 
            const GeometryBuffer& gbuffer, const Materials& materials) const noexcept;

        /// <summary> Binds an individual block to an individual program. </summary>
        void bindBlockToProgram (const Program& program, const GLuint blockBinding) const noexcept;

        /// <summary> Resets the pointer and offset of every stored data block. </summary>
        void resetBlockData() noexcept;

        /// <summary> Binds each UBO block containing static data to their associated index. </summary>
        void rebindStaticBlocks() const noexcept;

        /// <summary> Binds each block to ranges in the current partition. </summary>
        void rebindDynamicBlocks() const noexcept;
};


template <typename T>
GLintptr Uniforms::calculateAlignedSize() const noexcept
{
    constexpr auto size = sizeof (std::remove_pointer_t<T>);
    const auto remainder = size % alignment;

    return remainder ? size + alignment - size % alignment : size;
}

#endif // _RENDERING_UNIFORMS_
