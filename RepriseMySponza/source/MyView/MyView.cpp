#include "MyView.hpp"


// STL headers.
#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>


// Engine headers.
#include <glm/gtc/matrix_transform.hpp>
#include <scene/scene.hpp>
#include <tgl/tgl.h>
#include <tygra/FileHelper.hpp>


// Personal headers.
#include <Misc/Vertex.hpp>
#include <MyView/Internals/Material.hpp>
#include <MyView/Internals/Mesh.hpp>
#include <Rendering/Uniforms/UniformData.hpp>
#include <Utility/Algorithm.hpp>
#include <Utility/OpenGL.hpp>
#include <Utility/Scene.hpp>


// Literals.
using namespace std::string_literals;


MyView::~MyView()
{
    // Never leave a byte of data behind!
    windowViewDidStop ({ nullptr });
}


void MyView::rebuildShaders()
{
    // We should be able to simply delete our current program, rebuild it and reset the VAO.
    glDeleteProgram (m_program);
    // TODO: Update MyView::rebuildShaders().
    buildProgram();
    bindUniformBufferObject();
    constructVAO();
}


void MyView::windowViewWillStart (tygra::Window*)
{
    assert (m_scene != nullptr);
    // TODO: Move OpenGL preparation into the main rendering loop.
    // Set up OpenGL as required by the application!
    //glEnable (GL_DEPTH_TEST);
    //glEnable (GL_CULL_FACE);
    //glClearColor (0.f, 0.1f, 0.f, 0.f);
    // TODO: Create an object to store and manage all required programs.
    // Attempt to build the program, if it fails the user can reload after correcting any syntax errors.
    //buildProgram();
    m_configurator.initialise();
    m_program = m_configurator.get();
    // TODO: Some form of renderer component which stores and constructs buffers.
    // Generate the buffers.
    generateOpenGLObjects();

    // Retrieve the Sponza data ready for rendering.
    buildMeshData();
    
    // Allocate the required run-time memory for instancing.
    allocateExtraBuffers();

    // Ensure we have the required materials.
    buildMaterialData();

    // Prepare the UBO for usage.
    bindUniformBufferObject();

    // Now we can construct the VAO so we're reading for rendering.
    constructVAO();
}


bool MyView::buildProgram()
{ // TODO: Move into a program generation class/struct.
    // Create the program to attach shaders to.
    m_program = glCreateProgram();

    // Attempt to compile the shaders.
    const auto vertexShaderLocation     = "content:///sponza_vs.glsl"s;
    const auto fragmentShaderLocation   = "content:///sponza_fs.glsl"s;
    
    const auto vertexShader     = util::compileShaderFromFile (vertexShaderLocation,    GL_VERTEX_SHADER);
    const auto fragmentShader   = util::compileShaderFromFile (fragmentShaderLocation,  GL_FRAGMENT_SHADER);

    // Attach the shaders to the program we created.
    const std::vector<GLchar*> vertexAttributes     = { "position", "normal", "textureCoord", "model", "pvm" };
    const std::vector<GLchar*> fragmentAttributes   = {  };

    util::attachShader (m_program,  vertexShader,   vertexAttributes);
    util::attachShader (m_program,  fragmentShader, fragmentAttributes);

    // Link the program.
    if (util::linkProgram (m_program))
    {
        std::cout << "OpenGL application built successfully." << std::endl;
        return true;
    }

    return false;
}


void MyView::generateOpenGLObjects()
{ // TODO: Move into a buffer management class/struct.
    glGenVertexArrays (1, &m_sceneVAO);

    glGenBuffers (1, &m_vertexVBO);
    glGenBuffers (1, &m_elementVBO);
    glGenBuffers (1, &m_uniformUBO);
    glGenBuffers (1, &m_materials.vbo);
    glGenBuffers (1, &m_poolTransforms);
    glGenBuffers (1, &m_poolMaterialIDs.vbo);
    
    glGenTextures (1, &m_textureArray);
    glGenTextures (1, &m_materials.tbo);
    glGenTextures (1, &m_poolMaterialIDs.tbo);
}


void MyView::buildMeshData()
{ // TODO: Move into buffer management class/struct.
    // Begin to construct sponza.
    const auto& builder = scene::GeometryBuilder();
    const auto& meshes  = builder.getAllMeshes();

    // Resize our vector to speed up the loading process.
    m_meshes.resize (meshes.size());

    // Start by allocating enough memory in the VBOs to contain the scene.
    size_t vertexSize { 0 }, elementSize { 0 };
    util::calculateVBOSize (meshes, vertexSize, elementSize);

    util::allocateBuffer (m_vertexVBO,  vertexSize,     GL_ARRAY_BUFFER,            GL_STATIC_DRAW);
    util::allocateBuffer (m_elementVBO, elementSize,    GL_ELEMENT_ARRAY_BUFFER,    GL_STATIC_DRAW);
    
    // Bind our VBOs.
    glBindBuffer (GL_ARRAY_BUFFER, m_vertexVBO);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_elementVBO);

    // Iterate through each mesh adding them to the mesh container.
    auto vertexIndex    = GLintptr { 0 };
    auto elementOffset  = GLintptr { 0 };
    
	for (size_t i { 0 }; i < meshes.size(); ++i)
    {
        // Cache the required mesh data.
        const auto& sceneMesh   = meshes[i];
        const auto& elements    = sceneMesh.getElementArray();
        auto&       localMesh   = m_meshes[i].second;
        
        // Assign the local mesh an ID.
        m_meshes[i].first = sceneMesh.getId();

        localMesh.verticesIndex     = vertexIndex;
        localMesh.elementsOffset    = elementOffset;
        localMesh.elementCount      = elements.size();
        
        // Obtain the required vertex information.
        auto vertices = util::assembleVertices (sceneMesh);

        // Fill the vertex buffer objects with data.
        glBufferSubData (GL_ARRAY_BUFFER,           vertexIndex * sizeof (Vertex),  vertices.size() * sizeof (Vertex),          vertices.data());
        glBufferSubData (GL_ELEMENT_ARRAY_BUFFER,   elementOffset,                  elements.size() * sizeof (unsigned int),    elements.data());

        // The vertexIndex needs an actual index value whereas elementOffset needs to be in bytes.
        vertexIndex += vertices.size();
        elementOffset += elements.size() * sizeof (unsigned int);
    }

    // Unbind the buffers.
    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
}


void MyView::allocateExtraBuffers()
{ // TODO: Move to buffer management class/struct.
    // We'll need to keep track of the highest number of instances in the scene.
    m_instancePoolSize = highestInstanceCount();
    // TODO: Perhaps MyView::allocateExtraBuffers() this can be made less brittle?
    // We need to store two matrices per instance and we need to ensure the materialID pool aligns to a glm::vec4, otherwise we end up with missing data.
    const auto transformSize    = m_instancePoolSize * sizeof (glm::mat4) * 2;
    const auto materialIDSize   = (m_instancePoolSize + m_instancePoolSize % 4) * sizeof (MaterialID);

    // The UBO will contain every uniform variable apart from textures. 
    util::allocateBuffer (m_uniformUBO, sizeof (UniformData), GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW);

    // The matrices pool stores the model and PVM transformation matrices of each instance, therefore we need two.
    util::allocateBuffer (m_poolTransforms, transformSize, GL_ARRAY_BUFFER, GL_STREAM_DRAW);

    // The material ID pool contains the instance-specific material ID required for correct shading.
    util::allocateBuffer (m_poolMaterialIDs.vbo, materialIDSize, GL_TEXTURE_BUFFER, GL_STREAM_DRAW);
}


void MyView::bindUniformBufferObject()
{// TODO: Move into buffer management class/struct?
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformUBO);

    // Determine the UBO indices.
    const auto scene    = glGetUniformBlockIndex (m_program, "scene");
    const auto lighting = glGetUniformBlockIndex (m_program, "lighting");

    // Bind each part of the UBO to the correct block.
    glUniformBlockBinding (m_program, scene,    UniformData::sceneBlock());
    glUniformBlockBinding (m_program, lighting, UniformData::lightingBlock());

    // Use the magic data contained in UniformData to separate the UBO into segments.
    glBindBufferRange (GL_UNIFORM_BUFFER, UniformData::sceneBlock(),    m_uniformUBO, UniformData::sceneOffset(),    UniformData::sceneSize());
    glBindBufferRange (GL_UNIFORM_BUFFER, UniformData::lightingBlock(), m_uniformUBO, UniformData::lightingOffset(), UniformData::lightingSize());

    // Unbind the buffer.
    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}


void MyView::buildMaterialData()
{// TODO: Consider large refactoring of the entire MyView::buildMaterialData() function.
    // Obtain every material in the scene.
    const auto& materials = m_scene->getAllMaterials();

    // Load all of the images in the scen
    auto images = std::vector<std::pair<std::string, tygra::Image>> { };
    util::loadImagesFromScene (images, materials);

    // Iterate through them creating a buffer-ready material for each ID.
    auto bufferMaterials = std::vector<Material> (materials.size());

    for (size_t id = 0; id < materials.size(); ++id)
    {
        // Cache the material.
        const auto& material = materials[id];

        // Check which texture ID to use. If it can't be determined then -1 indicates none.
        const auto& texture   = "resource:///hex.png"s;
        auto        textureID = -1.f;

        if (!texture.empty())
        {
            // Determine the textureID.
            for (size_t i = 0; i < images.size(); ++i)
            {
                // 0 means equality.
                if (texture == images[i].first)
                {
                    // Cast back to an int otherwise we may have errors.
                    textureID = (float) i;
                    break;
                }
            }
        }
        // TODO: Material struct will need to be rewritten to feature physics-based rendering techniques.
        // Create a buffer-ready material and fill it with correct data.
        auto bufferMaterial = Material { };
        bufferMaterial.diffuseColour    = util::toGLM (material.getDiffuseColour());
        bufferMaterial.textureID        = textureID;
        bufferMaterial.specularColour   = util::toGLM (material.getSpecularColour());
        bufferMaterial.shininess        = material.getShininess();

        // Prepare to add it to the GPU and add the ID to the map. We need to remember that a material takes up two columns so the ID must be multiplied by two.
        bufferMaterials[id] = std::move (bufferMaterial);
        m_materialIDs.emplace (material.getId(), MaterialID (id * 2));
    }

    // Load the materials into the GPU and link the buffers together.
    util::fillBuffer (m_materials.vbo, bufferMaterials, GL_TEXTURE_BUFFER, GL_STATIC_DRAW);

    if (!images.empty())
    {
        prepareTextureData (images[0].second.width(), images[0].second.height(), images.size());
    }

    // Just prepare the materials.
    else
    {
        prepareTextureData (1, 1, 1);
    }

    // Finally load the images onto the GPU.
    loadTexturesIntoArray (images);
}


void MyView::constructVAO()
{ // TODO: Remove justifications at some point. 
    // TODO: Move into buffer management class.
    /// Here we combine all vertex attributes into a 32-byte aligned interleaved VBO. The reason for this is that being a power of two maps
    /// perfectly with the memory bus width of many GPUs, in this particular case a 256-bit wide memory bus could load a vertex in a single segment.
    /// It also allows us to consolidate vertex-specific information into a unique buffer so we know where everything is.
    ///
    /// We also use a single VBO to store the entire scene, since the vertex attributes of every mesh in the scene is static and only the transformation
    /// matrices change, we don't need to flexibility that having multiple VBOs provides. The VBO also contains meshes which are perfectly aligned to
    /// the order of which we draw meshes, e.g. the third mesh in the draw loop is the third mesh stored in the GPU, potentially avoiding cache misses.
    ///
    /// Finally we use a single VAO for the scene because it allows us to avoid the cost required to bind a different VAO each mesh. Perhaps if we
    /// separated the scene into static and dynamic objects with a different VAO for each there would be some benefit but in this case we don't.

    // Obtain the attribute pointer locations we'll be using to construct the VAO.
    const auto position         = GLint { 0 };
    const auto normal           = GLint { 1 };
    const auto textureCoord     = GLint { 2 };
    const auto modelTransform   = GLint { 3 };
    const auto pvmTransform     = GLint { 7 };

    // Initialise the VAO.
    glBindVertexArray (m_sceneVAO);

    // Bind the element buffer to the VAO.
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_elementVBO);

    // Enable each attribute pointer.
    glEnableVertexAttribArray (position);
    glEnableVertexAttribArray (normal);
    glEnableVertexAttribArray (textureCoord);

    // Begin creating the vertex attribute pointer from the interleaved buffer.
    glBindBuffer (GL_ARRAY_BUFFER, m_vertexVBO);

    // Set the properties of each attribute pointer.
    glVertexAttribPointer (position,        3, GL_FLOAT, GL_FALSE, sizeof (Vertex), TGL_BUFFER_OFFSET (0));
    glVertexAttribPointer (normal,          3, GL_FLOAT, GL_FALSE, sizeof (Vertex), TGL_BUFFER_OFFSET (12));
    glVertexAttribPointer (textureCoord,    2, GL_FLOAT, GL_FALSE, sizeof (Vertex), TGL_BUFFER_OFFSET (24));

    // Now we need to create the instanced matrices attribute pointers.
    glBindBuffer (GL_ARRAY_BUFFER, m_poolTransforms);
    // TODO: Make significantly less brittle.
    // We'll combine our matrices into a single VBO so we need the stride to be double.
    util::createMatrix4Attribute (modelTransform, sizeof (glm::mat4) * 2);
    util::createMatrix4Attribute (pvmTransform,   sizeof (glm::mat4) * 2, sizeof (glm::mat4));

    // Unbind all buffers.
    glBindVertexArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
}


void MyView::prepareTextureData (const GLsizei textureWidth, const GLsizei textureHeight, const GLsizei textureCount)
{ // TODO: Perhaps this should move into a texture management class?
    // Activate the material TBO by pointing it to the material VBO.
    glBindTexture (GL_TEXTURE_BUFFER, m_materials.tbo);
    glTexBuffer (GL_TEXTURE_BUFFER, GL_RGBA32F, m_materials.vbo);

    // Do the same for the material ID instance pool.
    glBindTexture (GL_TEXTURE_BUFFER, m_poolMaterialIDs.tbo);
    glTexBuffer (GL_TEXTURE_BUFFER, GL_RGBA32I, m_poolMaterialIDs.vbo);

    // Enable the 2D texture array and prepare its storage. Use 4 mipmap levels.
    glBindTexture (GL_TEXTURE_2D_ARRAY, m_textureArray);
    glTexStorage3D (GL_TEXTURE_2D_ARRAY, 4, GL_RGBA32F, textureWidth, textureHeight, textureCount);

    // Enable standard filters.
    glTexParameteri (GL_TEXTURE_2D_ARRAY,   GL_TEXTURE_MAG_FILTER,  GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D_ARRAY,   GL_TEXTURE_MIN_FILTER,  GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri (GL_TEXTURE_2D_ARRAY,   GL_TEXTURE_WRAP_S,      GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D_ARRAY,   GL_TEXTURE_WRAP_T,      GL_REPEAT);

    // Unbind the textures.
    glBindTexture (GL_TEXTURE_BUFFER, 0);
    glBindTexture (GL_TEXTURE_2D_ARRAY, 0);
}


void MyView::loadTexturesIntoArray (const std::vector<std::pair<std::string, tygra::Image>>& images)
{ // TODO: Move into texture management class?
    /// Here we load a container of images into the GPU using a 2D texture array. The reason I've chosen this route is that it means that I can avoid binding
    /// a different texture every time the material changes. Instead of binding the correct texture we just provide an ID in each material which links to the
    /// texture in the array. Therefore we avoid binding calls, we store the materials in the GPU so the information is easily accessible and if a shader
    /// decided it wanted to combine textures it can.

    glBindTexture (GL_TEXTURE_2D_ARRAY, m_textureArray); 

    for (size_t i { 0 }; i < images.size(); ++i)
    {
        // Cache the image.
        const auto& image = images[i].second;

        // Only load the image if it contains data.
        if (image.doesContainData()) 
        {
            // Enable each different pixel format.
            GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };

            glTexSubImage3D (   GL_TEXTURE_2D_ARRAY, 0, 
                
                                // Offsets.
                                0, 0, i,
                            
                                // Dimensions and border.
                                image.width(), image.height(), 1,   
                      
                                // Format and type.
                                pixel_formats[image.componentsPerPixel()], image.bytesPerComponent() == 1 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT,
                      
                                // Data.
                                image.pixelData());
        }
    }
    
    // Generate the mipmaps from the loaded texture and finish.
    glGenerateMipmap (GL_TEXTURE_2D_ARRAY);
    glBindTexture (GL_TEXTURE_2D_ARRAY, 0);
}


size_t MyView::highestInstanceCount() const
{ // TODO: Perhaps a utility function?
	// We'll need a temporary variable to keep track.
    size_t highest  { 0 };
   
    // Iterate through each mesh ID.
    for (const auto& pair : m_meshes)
    {
        const auto current = m_scene->getInstancesByMeshId (pair.first).size();

        if (current > highest)
        {
            highest = current;
        }
    }

    // Return the calculated figure.
    return highest;
}


void MyView::windowViewDidStop (tygra::Window*)
{ // TODO: Correct MyView::windowViewDidStop() once refactoring has completed.
    // Clean up after ourselves by getting rid of the stored meshes/materials.
    cleanMeshMaterials();

    // Clean up the OpenGL side of things.
    deleteOpenGLObjects();
}


void MyView::cleanMeshMaterials()
{ // TODO: Move into other class.
    m_meshes.clear();
    m_materialIDs.clear();
}


void MyView::deleteOpenGLObjects()
{ // TODO: Call delete on each refactored child object.
    // Delete the program.
    //glDeleteProgram (m_program);
    
    // Delete the VAO.
    glDeleteVertexArrays (1, &m_sceneVAO);
    
    // Delete all VBOs.
    glDeleteBuffers (1, &m_vertexVBO);
    glDeleteBuffers (1, &m_elementVBO);
    glDeleteBuffers (1, &m_uniformUBO);
    glDeleteBuffers (1, &m_materials.vbo);
    glDeleteBuffers (1, &m_poolMaterialIDs.vbo);
    glDeleteBuffers (1, &m_poolTransforms);

    // Delete all textures.
    glDeleteTextures (1, &m_textureArray);
    glDeleteTextures (1, &m_materials.tbo);
    glDeleteTextures (1, &m_poolMaterialIDs.tbo);
}


void MyView::windowViewDidReset (tygra::Window*, int width, int height)
{
    // Reset the viewport and recalculate the aspect ratio.
    glViewport (0, 0, width, height);
    m_aspectRatio = width / static_cast<float> (height);
}


void MyView::windowViewRender (tygra::Window*)
{ // TODO: Split into a Renderer class of some kind.
    /// For the rendering of the scene I have chosen to implement instancing. A traditional approach of rendering would be looping through each instance,
    /// assigning the correct model and PVM transforms, then drawing that one mesh before repeating the process. I don't use that method here, instead
    /// I loop through mesh, obtain the number of instances, load in the data specific to those instances and draw them all at once, letting the shaders
    /// obtain the correct information. I choose this method because although it doesn't help in a simple scene like sponza; scenes with particle systems,
    /// large-scale mesh duplication and such would really benefit from reducing the overhead that bindings, uniform specification and draw calls cost.
    assert (m_scene != nullptr);

    // Prepare the draw.
    m_configurator.prepareDraw();
    m_configurator.switchToSceneConstructionMode();

    // Define matrices.
    const auto& camera      = m_scene->getCamera();
    const auto  projection	= glm::perspective (glm::radians (camera.getVerticalFieldOfViewInDegrees()), m_aspectRatio, camera.getNearPlaneDistance(), camera.getFarPlaneDistance()),
                view        = glm::lookAt (util::toGLM (camera.getPosition()), util::toGLM (camera.getPosition()) + util::toGLM (camera.getDirection()), util::toGLM (m_scene->getUpDirection()));
    
    // Set the uniforms.
    setUniforms (projection, view);
    
    // Specify the VAO to use.
    glBindVertexArray (m_sceneVAO);

    // Specify the buffers to use.
    glBindBuffer (GL_ARRAY_BUFFER, m_poolTransforms);
    glBindBuffer (GL_TEXTURE_BUFFER, m_poolMaterialIDs.vbo);

    // Specify the textures to use.
    glActiveTexture (GL_TEXTURE0 + m_textureArray);
    glBindTexture (GL_TEXTURE_2D_ARRAY, m_textureArray);

    glActiveTexture (GL_TEXTURE0 + m_materials.tbo);
    glBindTexture (GL_TEXTURE_BUFFER, m_materials.tbo);

    glActiveTexture (GL_TEXTURE0 + m_poolMaterialIDs.tbo);
    glBindTexture (GL_TEXTURE_BUFFER, m_poolMaterialIDs.tbo);

    // Use vectors for storing instancing data> This requires a material ID, a model transform and a PVM transform.
	static auto materialIDs = std::vector<MaterialID> (m_instancePoolSize);
    static auto matrices    = std::vector<glm::mat4> (m_instancePoolSize * 2); // TODO: Make this less brittle.

    // Iterate through each mesh using instancing to reduce GL calls.
    for (const auto& pair : m_meshes)
    {
        // Obtain the instances to draw for the current mesh.
        const auto& instances	= m_scene->getInstancesByMeshId (pair.first);
        const auto size         = instances.size();

        // Check if we need to do any rendering at all.
        if (size != 0U)
        {
            // Update the instance-specific information.
            for (size_t i { 0 }; i < size; ++i)
            {
                // Cache the current instance.
                const auto& instance = m_scene->getInstanceById (instances[i]);

                // Obtain the current instances model transformation.
                const auto model = glm::mat4 (util::toGLM (instance.getTransformationMatrix()));

                // We have both the model and pvm matrices in the buffer so we need an offset.
                const auto offset    = i * 2;
                matrices[offset]     = model;
                matrices[offset + 1] = projection * view * model;

                // Now deal with the materials.
                materialIDs[i] = m_materialIDs.at (instance.getMaterialId());
            }
			// TODO: Make this less brittle.
            // Only overwrite the required data to speed up the buffering process. Avoid glMapBuffer because it's ridiculously slow in this case.
            glBufferSubData (GL_ARRAY_BUFFER,   0,  sizeof (glm::mat4) * 2 * size,  matrices.data());
            glBufferSubData (GL_TEXTURE_BUFFER, 0,  sizeof (MaterialID) * size,     materialIDs.data());
            
            // Cache access to the current mesh.
            const auto& mesh = pair.second;

            // Finally draw all instances at the same time.
            glDrawElementsInstancedBaseVertex (GL_TRIANGLES, mesh.elementCount, GL_UNSIGNED_INT, (void*) mesh.elementsOffset, size, mesh.verticesIndex);
        }
    }

    // UNBIND IT ALL CAPTAIN!
    glBindVertexArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindBuffer (GL_TEXTURE_BUFFER, 0);

    glActiveTexture (GL_TEXTURE1);
    glBindTexture (GL_TEXTURE_BUFFER, 0);

    glActiveTexture (GL_TEXTURE0);
    //glBindTexture (GL_TEXTURE_2D_ARRAY, 0);
}


void MyView::setUniforms (const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{ // TODO: Move into some form of Renderer class, also needs significant refactoring once UniformData has been refactored.
    const auto textures     = glGetUniformLocation (m_program, "textures");
    const auto materials    = glGetUniformLocation (m_program, "materials");
    const auto materialIDs  = glGetUniformLocation (m_program, "materialIDs");

    glUniform1i (textures, m_textureArray);
    glUniform1i (materials, m_materials.tbo);
    glUniform1i (materialIDs, m_poolMaterialIDs.tbo);

    // Create data to fill. Avoid creating it every time by using static.
    static auto data = UniformData { };
    
    data.setProjectionMatrix (projectionMatrix);
    data.setViewMatrix (viewMatrix);
    data.setCameraPosition (util::toGLM (m_scene->getCamera().getPosition()));
    data.setAmbientColour (util::toGLM (m_scene->getAmbientLightIntensity()));

    // Add all lights to the scene.
    auto lightCount = 0;
    auto addLight = [&] (const auto& light) { data.setLight(lightCount++, light); };
    
    util::for_each (m_scene->getAllDirectionalLights(), addLight);
    util::for_each (m_scene->getAllPointLights(), addLight);
    util::for_each (m_scene->getAllSpotLights(), addLight);

    // Enable the wireframe light if necessary.
    if (m_wireframeMode)
    {
        addLight(createWireframeLight());
    }

    data.setLightCount (lightCount);

    // Overwrite the current uniform data.
    glBindBuffer (GL_UNIFORM_BUFFER, m_uniformUBO);
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (UniformData), &data);

    // Unbind it since for safety.
    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}


Light MyView::createWireframeLight() const
{
    // Create the light.
    Light wireframe { };

    // Fill it with the correct information.
    const auto& camera  = m_scene->getCamera();
    wireframe.position  = util::toGLM (camera.getPosition());
    wireframe.direction	= util::toGLM (camera.getDirection());

    // Set suitable attenuation values.
    wireframe.aConstant		= 1.0f;
    wireframe.aLinear       = 0.0f;
    wireframe.aQuadratic    = 0.002f;

    // Enable the wireframe and we're done! We only have three modes so use the currently selected.
    const LightType type = static_cast<LightType> (m_wireframeType);

    wireframe.emitWireframe = 1;
    wireframe.setType (type);

    return wireframe;
}