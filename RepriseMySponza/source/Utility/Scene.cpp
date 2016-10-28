#include "Scene.hpp"


// STL headers.
#include <algorithm>


// Engine headers.
#include <scene/Material.hpp>
#include <scene/Mesh.hpp>
#include <tygra/FileHelper.hpp>


// Personal headers.
#include <Misc/Vertex.hpp>


// Namespaces.
using namespace std::string_literals;


namespace util
{
    void calculateVBOSize (const std::vector<scene::Mesh>& meshes, size_t& vertexSize, size_t& elementSize)
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
        vertexSize  = vertices * sizeof (Vertex);
        elementSize = elements * sizeof (unsigned int);
    }


    std::vector<Vertex> assembleVertices (const scene::Mesh& mesh)
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


    void loadImagesFromScene (std::vector<std::pair<std::string, tygra::Image>>& images, const std::vector<scene::Material>& materials)
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