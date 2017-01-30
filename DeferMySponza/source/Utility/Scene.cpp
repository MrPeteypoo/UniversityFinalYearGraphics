#include "Scene.hpp"


// STL headers.
#include <algorithm>


// Engine headers.
#include <scene/scene.hpp>
#include <tygra/FileHelper.hpp>


// Personal headers.
#include <Rendering/Renderer/Geometry/Internals/Vertex.hpp>
#include <Utility/Maths.hpp>


// Namespaces.
using namespace std::string_literals;


namespace util
{
    void calculateSceneSize (const std::vector<scene::Mesh>& meshes, 
        size_t& vertexCount, size_t& elementCount) noexcept
    {
        // Create temporary accumlators.
        size_t vertices { 0 }, elements { 0 };  

        // We need to loop through each mesh adding up as we go along.
        for (const auto& mesh : meshes)
        {
            vertices += mesh.getPositionArray().size();
            elements += mesh.getElementArray().size();
        }

        // Calculate the final values.
        vertexCount     = vertices;
        elementCount    = elements;
    }


    std::vector<Vertex> assembleVertices (const scene::Mesh& mesh) noexcept
    {
        // Obtain each attribute.
        const auto& positions       = mesh.getPositionArray();
        const auto& normals         = mesh.getNormalArray();
        const auto& texturePoints   = mesh.getTextureCoordinateArray();

        // Check how much data we need to allocate.
        const auto posSize  = positions.size(),
                   normSize = normals.size(),
                   texSize  = texturePoints.size();

        auto vertices = std::vector<Vertex> { posSize };

        // Fill the actual data.
        for (size_t i { 0 }; i < posSize; ++i)
        {
            auto& vertex    = vertices[i];
            vertex.position = toGLM (positions[i]);

            if (i < normSize)
            {
                vertex.normal = toGLM (normals[i]);
            }

            if (i < texSize)
            {
                vertex.texturePoint = toGLM (texturePoints[i]);
            }
        }

        return vertices;
    }


    std::vector<PBSMaterial> getAllMaterials (const scene::Context& scene) noexcept
    {
        // The materials we've been provided aren't suitable for physically-based shading techniques. Therefore a hacky
        // interpretation is required.
        const auto& sceneMaterials = scene.getAllMaterials();

        // The following code won't be pretty, without modifying the framework a function like this needs to exist.
        auto materials = std::vector<PBSMaterial> { };
        materials.reserve (sceneMaterials.size());

        // Might as well make SOME effort to keep it clean.
        constexpr auto smoothness = 0, reflectance = 1, conductivity = 2;

        for (const auto& sceneMaterial : sceneMaterials)
        {
            // The material ID should be a straight copy.
            auto material = PBSMaterial { };
            material.id = sceneMaterial.getId();

            // Cache the material properties.
            const auto& diffuse     = sceneMaterial.getDiffuseColour();
            const auto& specular    = sceneMaterial.getSpecularColour();

            // Infer smoothness from the specular luminance of the material.
            material.physics[smoothness] = static_cast<GLubyte> (255 * (specular.x * 0.2126f + specular.y * 0.7151f + specular.z * 0.0722f));

            // Infer reflectance from the diffuse luminance of the material.
            material.physics[reflectance] = static_cast<GLubyte> (255 * (diffuse.x * 0.2126f + diffuse.y * 0.7151f + diffuse.z * 0.0722f));

            // We will treat "shiny" materials as if they're conductive.
            if (sceneMaterial.isShiny())
            {
                const auto shininess = util::max (sceneMaterial.getShininess() / 128.f, 1.f);
                material.physics[conductivity] = static_cast<GLubyte> (255 * shininess);
            }

            // The albedo should just be the diffuse colour.
            material.albedo[0] = static_cast<GLubyte> (255 * diffuse.x);
            material.albedo[1] = static_cast<GLubyte> (255 * diffuse.y);
            material.albedo[2] = static_cast<GLubyte> (255 * diffuse.z);

            // Now do any material specific overloading.
            switch (sceneMaterial.getId())
            {
                // Rock walls.
                case 200:
                    material.physics[smoothness]    = 111;
                    material.physics[reflectance]   = 127;
                    material.physics[conductivity]  = 0;

                    material.albedo = { 156, 146, 131 };
                    break;

                // Satin drapes and roof.
                case 201:
                    material.physics[smoothness]    = 114;
                    material.physics[reflectance]   = 99;
                    material.physics[conductivity]  = 0;

                    material.albedo = { 175, 68, 51 };
                    break;

                // Green leaves.
                case 202:
                    material.physics[smoothness]    = 145;
                    material.physics[reflectance]   = 151;
                    material.physics[conductivity]  = 0;

                    material.albedo = { 85, 89, 45 };
                    break;

                // Brass metallic poles.
                case 203:
                    material.physics[smoothness]    = 140;
                    material.physics[conductivity]  = 255;

                    material.albedo = { 250, 230, 150 };
                    break;

                // Copper buddah.
                case 204:
                    material.physics[smoothness]    = 170;
                    material.physics[conductivity]  = 255;

                    material.albedo = { 250, 190, 160 };
                    break;

                // Rubber rabbit.
                case 205:
                    material.physics[smoothness]    = 116;
                    material.physics[reflectance]   = 105;
                    material.physics[conductivity]  = 0;

                    material.albedo = { 66, 65, 61 };
                    break;

                // Silver dragon.
                case 206:
                    material.physics[smoothness]    = 150;
                    material.physics[conductivity]  = 255;

                    material.albedo = { 252, 250, 245 };
                    break;

                // Do nothing my lord!
                default:
                    break;
            }

            materials.push_back (std::move (material));
        }

        return materials;
    }


    void loadImagesFromScene (std::vector<std::pair<std::string, tygra::Image>>& images, 
        const std::vector<scene::Material>& materials) noexcept
    {
        // Ensure the vector is empty.
        images.clear();

        for (const auto& material : materials)
        {
            // Attempt to load each image.
			auto filename = "resource:///kappa.png"s; // material.getAmbientMap();
            auto image    = tygra::createImageFromPngFile (filename);

            if (image.doesContainData())
            {
                images.push_back ({ std::move (filename), std::move (image) });
            }
        }
    }
}