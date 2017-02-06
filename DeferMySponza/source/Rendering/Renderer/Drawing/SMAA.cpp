#include "SMAA.hpp"


// Engine headers.
#include <smaa/Textures/AreaTex.h>
#include <smaa/Textures/SearchTex.h>


// Personal headers.
#include <Rendering/Binders/FramebufferBinder.hpp>
#include <Rendering/Binders/ProgramBinder.hpp>
#include <Rendering/Binders/TextureBinder.hpp>
#include <Rendering/Binders/VertexArrayBinder.hpp>
#include <Rendering/Renderer/Programs/HardCodedShaders.hpp>


bool SMAA::isInitialised() const noexcept
{
    return m_edgeDetectionPass.isInitialised() && m_edgeDetectionFBO.fbo.isInitialised() && 
        m_edgeDetectionFBO.output.isInitialised() && m_weightingPass.isInitialised() && 
        m_weightingFBO.fbo.isInitialised() && m_weightingFBO.output.isInitialised() &&
        m_blendingPass.isInitialised() && m_areaTexture.isInitialised() && m_searchTexture.isInitialised() &&
        m_stencil.isInitialised();
}


bool SMAA::initialise (Quality quality, GLsizei width, GLsizei height, GLuint startingTextureUnit,
    bool usePredication) noexcept
{
    // Clean the object if no antialiasing is to be performed.
    if (quality == Quality::None)
    {
        clean();   
        return false;
    }

    // Ensure the width and height are valid.
    if (width < 1 || height < 1)
    {
        return false;
    }

    // Create temporary objects.
    decltype (m_edgeDetectionPass)  edgeProg, weightProg, blendProg;
    decltype (m_edgeDetectionFBO)   edgeFBO, weightFBO;
    decltype (m_areaTexture)        areaTex, searchTex, stencil;

    // Initialise objects.
    if (!(edgeProg.initialise() && weightProg.initialise() && blendProg.initialise() &&
        edgeFBO.fbo.initialise() && edgeFBO.output.initialise (startingTextureUnit) &&
        weightFBO.fbo.initialise() && weightFBO.output.initialise (startingTextureUnit) &&
        areaTex.initialise (startingTextureUnit + 1), searchTex.initialise (startingTextureUnit + 2) &&
        stencil.initialise (startingTextureUnit)))
    {
        return false;
    }

    // Load the precalculated textures.
    loadTextures (areaTex, searchTex);

    // Compile each program.
    if (!compilePrograms (edgeProg, weightProg, blendProg, areaTex, searchTex, 
        quality, width, height, startingTextureUnit, usePredication))
    {
        return false;
    }

    // Prepare the render targets.
    if (!configureRenderTargets (edgeFBO, weightFBO, stencil, width, height))
    {
        return false;
    }

    // Finally make the temporary objects permanent.
    m_edgeDetectionPass = std::move (edgeProg);
    m_edgeDetectionFBO  = std::move (edgeFBO);
    m_weightingPass     = std::move (weightProg);
    m_weightingFBO      = std::move (weightFBO);
    m_blendingPass      = std::move (blendProg);
    m_areaTexture       = std::move (areaTex);
    m_searchTexture     = std::move (searchTex);
    m_stencil           = std::move (stencil);
    return true;
}

        
void SMAA::clean() noexcept
{
    if (isInitialised())
    {
        m_edgeDetectionPass.clean();
        m_edgeDetectionFBO.fbo.clean();
        m_edgeDetectionFBO.output.clean();
        m_weightingPass.clean();
        m_weightingFBO.fbo.clean();
        m_weightingFBO.output.clean();
        m_blendingPass.clean();
        m_areaTexture.clean();
        m_searchTexture.clean();
        m_stencil.clean();
    }
}


void SMAA::run (const FullScreenTriangleVAO& triangle, const Texture2D& aliasedTexture, 
    const Texture2D* predication, const Framebuffer* output) noexcept
{   
    // Perform the edge detection pass.
    const auto vaoBinder    = VertexArrayBinder { triangle.vao };
    const auto inputBinder  = TextureBinder { aliasedTexture };
    const auto progBinder   = ProgramBinder { m_edgeDetectionPass };
    const auto fboBinder    = FramebufferBinder<GL_FRAMEBUFFER> { m_edgeDetectionFBO.fbo };

    // Start by setting the program uniforms for the input. The input should always be bound to zero.
    glProgramUniform1i (m_edgeDetectionPass.getID(), 0, inputBinder.getTextureUnit());
    glProgramUniform1i (m_blendingPass.getID(), 0, inputBinder.getTextureUnit());
    
    // Antialiasing only needs access to the stencil buffer.
    glDisable (GL_DEPTH_TEST);
    glDisable (GL_BLEND);
    glCullFace (GL_BACK);

    glEnable (GL_STENCIL_TEST);
    glStencilFunc (GL_ALWAYS, 1, ~0);
    glStencilOp (GL_ZERO, GL_ZERO, GL_REPLACE);

    glClearColor (0.f, 0.f, 0.f, 0.f);
    glClearStencil (0);
    glClear (GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Enable predication thresholding if necessary.
    if (predication)
    {
        // The predication buffer should always be in location 1.
        const auto binder = TextureBinder { *predication };
        glProgramUniform1i (m_edgeDetectionPass.getID(), 1, binder.getTextureUnit());
        glDrawArrays (GL_TRIANGLES, 0, triangle.vertexCount);
    }

    else
    {
        glDrawArrays (GL_TRIANGLES, 0, triangle.vertexCount);
    }

    // Perform the weight calculation pass.
    const auto resultBinder = TextureBinder { m_edgeDetectionFBO.output };
    const auto areaBinder   = TextureBinder { m_areaTexture };
    const auto searchBinder = TextureBinder { m_searchTexture };
    progBinder.bind (m_weightingPass);
    fboBinder.bind (m_weightingFBO.fbo);

    // Here we only execute when the pixel is an edge.
    glStencilFunc (GL_NOTEQUAL, 0, ~0);
    glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);

    glClear (GL_COLOR_BUFFER_BIT);
    glDrawArrays (GL_TRIANGLES, 0, triangle.vertexCount);

    // Finally perform the blending pass.
    resultBinder.bind (m_weightingFBO.output);
    progBinder.bind (m_blendingPass);
    
    if (output)
    {
        fboBinder.bind (*output);
    }

    else
    {
        fboBinder.unbind();
    }

    glDisable (GL_STENCIL_TEST);
    glDrawArrays (GL_TRIANGLES, 0, triangle.vertexCount);
}


template <GLenum target>
void SMAA::setTextureParameters (TextureT<target>& texture) const noexcept
{
    // Textures need to use linear filtering and clamping should occur.
    texture.setParameter (GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture.setParameter (GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texture.setParameter (GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texture.setParameter (GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


void SMAA::loadTextures (Texture2D& areaTex, Texture2D& searchTex) const noexcept
{
    // The area texture needs to be stored in RGB8 format and loaded using GL_RG.
    areaTex.allocateImmutableStorage (GL_RG8, AREATEX_WIDTH, AREATEX_HEIGHT);
    flipAndLoadTexture (areaTex, AREATEX_WIDTH, AREATEX_HEIGHT, AREATEX_PITCH, AREATEX_SIZE, 
        GL_RG, areaTexBytes);
    
    // The search texture needs to be stored in R8 format and loaded using GL_RED.
    searchTex.allocateImmutableStorage (GL_R8, SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT);
    flipAndLoadTexture (searchTex, SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT, SEARCHTEX_PITCH, SEARCHTEX_SIZE, 
        GL_RED, searchTexBytes);
}


void SMAA::flipAndLoadTexture (Texture2D& texture, GLsizei width, GLsizei height, GLsizei pitch, GLsizei size, 
    GLenum pixelFormat, const GLubyte* pixels) const noexcept
{
    // Big thanks to https://github.com/turol/smaaDemo/blob/master/ for showing how to flip the textures.
    auto flippedPixels = std::vector<GLubyte> (size);

    for (GLsizei y = 0; y < height; ++y)
    {
        const auto srcY = height - 1 - y;
        std::memcpy (&flippedPixels[y * pitch], pixels + srcY * pitch, pitch);
    }

    texture.placeAt (0, 0, width, height, pixelFormat, GL_UNSIGNED_BYTE, flippedPixels.data());
    setTextureParameters (texture);
}


bool SMAA::compilePrograms (Program& edge, Program& weight, Program& blend, const Texture& areaTex, 
    const Texture& searchTex, Quality quality, GLsizei width, GLsizei height, GLuint outputTextureUnit, 
    bool usePredication) const noexcept
{
    // We need to compile shaders before we can link the programs together.
    const auto shaders = compileShaders (calculateDefines (quality, width, height, usePredication));

    // Attach the shaders.
    edge.attachShader (shaders.find (edgeDetectionVS));
    edge.attachShader (shaders.find (edgeDetectionFS));
    weight.attachShader (shaders.find (blendingWeightVS));
    weight.attachShader (shaders.find (blendingWeightFS));
    blend.attachShader (shaders.find (neighborhoodBlendingVS));
    blend.attachShader (shaders.find (neighborhoodBlendingFS));

    // Ensure they all link together.
    if (!(edge.link() && weight.link() && blend.link()))
    {
        return false;
    }

    // Attempt to find uniform locations for each texture.
    const auto edgeResultLocation       = glGetUniformLocation (weight.getID(), "edgeDetectionResult");
    const auto areaTextureLocation      = glGetUniformLocation (weight.getID(), "areaTexture");
    const auto searchTextureLocation    = glGetUniformLocation (weight.getID(), "searchTexture");
    const auto weightResultLocation     = glGetUniformLocation (blend.getID(), "blendWeightingResult");

    // Use a fallback if necessary.
    const auto edgeResultIndex      = edgeResultLocation != -1 ? edgeResultLocation : 0;
    const auto areaTextureIndex     = areaTextureLocation != -1 ? areaTextureLocation : 1;
    const auto searchTextureIndex   = searchTextureLocation != -1 ? searchTextureLocation : 2;
    const auto weightResultIndex    = weightResultLocation != -1 ? weightResultLocation : 1;
    
    // Finish up and we're done!
    glProgramUniform1i (weight.getID(), edgeResultIndex, outputTextureUnit);
    glProgramUniform1i (weight.getID(), areaTextureIndex, areaTex.getDesiredTextureUnit());
    glProgramUniform1i (weight.getID(), searchTextureIndex, searchTex.getDesiredTextureUnit());
    glProgramUniform1i (blend.getID(), weightResultIndex, outputTextureUnit);
    return true;
}


bool SMAA::configureRenderTargets (RenderTarget& edge, RenderTarget& weight, Texture2D& stencil, 
    GLsizei width, GLsizei height) const noexcept
{
    // We can use GL_RG for the edge texture and GL_RGBA for the blend texture.
    edge.output.allocateImmutableStorage (GL_RG8, width, height);
    weight.output.allocateImmutableStorage (GL_RGBA8, width, height);
    stencil.allocateImmutableStorage (GL_STENCIL_INDEX8, width, height);

    // Ensure texture parameters are configured. We can ignore the stencil buffer.
    setTextureParameters (edge.output);
    setTextureParameters (weight.output);

    // Finally configure the framebuffers.
    edge.fbo.attachTexture (edge.output, GL_COLOR_ATTACHMENT0);
    edge.fbo.attachTexture (stencil, GL_STENCIL_ATTACHMENT, false);
    weight.fbo.attachTexture (weight.output, GL_COLOR_ATTACHMENT0);
    weight.fbo.attachTexture (stencil, GL_STENCIL_ATTACHMENT, false);

    return edge.fbo.complete() && weight.fbo.complete();
}


Shader::RawSource SMAA::calculateDefines (Quality quality, GLsizei width, GLsizei height, 
    bool usePredication) const noexcept
{
    // Start with the metrics string, this defines the "screen" resolution.
    const auto widthString  = std::to_string (width);
    const auto heightString = std::to_string (height);
    const auto metrics      = "#define SMAA_RT_METRICS float4 (1.0 / " + widthString + ", 1.0 / " + heightString +
        ", " + widthString + ", " + heightString + ")\n";

    const auto predication = usePredication ? "#define SMAA_PREDICATION 1\n" : "";

    // Now determine the correct preset definition.
    switch (quality)
    {
        case Quality::Ultra:
            return metrics + predication + "#define SMAA_PRESET_ULTRA\n";
        case Quality::High:
            return metrics + predication +"#define SMAA_PRESET_HIGH\n";
        case Quality::Medium:
            return metrics + predication +"#define SMAA_PRESET_MEDIUM\n";
        default:
            return metrics + predication +"#define SMAA_PRESET_LOW\n";
    }
}


Shaders SMAA::compileShaders (Shader::RawSource extraDefines) const noexcept
{
    // We need to manually compile each shader.
    auto shaders = Shaders { };

    // Start with the vertex shaders. Ensure we add the uber shader after every definition.
    shaders.compile (GL_VERTEX_SHADER, edgeDetectionVS,         SMAAVSDefines, extraDefines, SMAAUberShader);
    shaders.compile (GL_VERTEX_SHADER, blendingWeightVS,        SMAAVSDefines, extraDefines, SMAAUberShader);
    shaders.compile (GL_VERTEX_SHADER, neighborhoodBlendingVS,  SMAAVSDefines, extraDefines, SMAAUberShader);

    // Now we can compile the fragment shaders.
    shaders.compile (GL_FRAGMENT_SHADER, edgeDetectionFS,           SMAAFSDefines, extraDefines, SMAAUberShader);
    shaders.compile (GL_FRAGMENT_SHADER, blendingWeightFS,          SMAAFSDefines, extraDefines, SMAAUberShader);
    shaders.compile (GL_FRAGMENT_SHADER, neighborhoodBlendingFS,    SMAAFSDefines, extraDefines, SMAAUberShader);

    // Finally return the compiled shaders.
    return shaders;
}