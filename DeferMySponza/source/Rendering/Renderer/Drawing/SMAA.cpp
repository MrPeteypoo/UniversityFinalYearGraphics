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
        m_blendingPass.isInitialised() && m_areaTexture.isInitialised() && m_searchTexture.isInitialised();
}


bool SMAA::initialise (Quality quality, GLsizei width, GLsizei height, GLuint startingTextureUnit) noexcept
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
    decltype (m_areaTexture)        areaTex, searchTex;

    // Initialise objects.
    if (!(edgeProg.initialise() && weightProg.initialise() && blendProg.initialise() &&
        edgeFBO.fbo.initialise() && edgeFBO.output.initialise (startingTextureUnit) &&
        weightFBO.fbo.initialise() && weightFBO.output.initialise (startingTextureUnit) &&
        areaTex.initialise (startingTextureUnit + 1), searchTex.initialise (startingTextureUnit + 2)))
    {
        return false;
    }

    // Load the precalculated textures.
    loadTextures (areaTex, searchTex);

    // Compile each program.
    if (!compilePrograms (edgeProg, weightProg, blendProg, areaTex, searchTex, 
        quality, width, height, startingTextureUnit))
    {
        return false;
    }

    // Prepare the render targets.
    if (!configureRenderTargets (edgeFBO, weightFBO, width, height))
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
    }
}


void SMAA::run (const FullScreenTriangleVAO& triangle, const Texture2D& aliasedTexture, const Framebuffer* output) noexcept
{
    // Start by setting the program uniforms for the input. The input should always be bound to zero.
    glProgramUniform1i (m_edgeDetectionPass.getID(), 0, aliasedTexture.getDesiredTextureUnit());
    glProgramUniform1i (m_blendingPass.getID(), 0, aliasedTexture.getDesiredTextureUnit());

    // Bind globals.
    const auto vaoBinder    = VertexArrayBinder { triangle.vao };
    const auto inputBinder  = TextureBinder { aliasedTexture };

    // Perform the edge detection pass.
    const auto progBinder   = ProgramBinder { m_edgeDetectionPass };
    const auto fboBinder    = FramebufferBinder<GL_DRAW_FRAMEBUFFER> { m_edgeDetectionFBO.fbo };

    glClearColor (0.f, 0.f, 0.f, 0.f);
    glClear (GL_COLOR_BUFFER_BIT);
    glDrawArrays (GL_TRIANGLES, 0, triangle.vertexCount);

    // Perform the weight calculation pass.
    const auto resultBinder = TextureBinder { m_edgeDetectionFBO.output };
    const auto areaBinder   = TextureBinder { m_areaTexture };
    const auto searchBinder = TextureBinder { m_searchTexture };
    progBinder.bind (m_weightingPass);
    fboBinder.bind (m_weightingFBO.fbo);

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
    areaTex.allocateImmutableStorage (GL_RGB8, AREATEX_WIDTH, AREATEX_HEIGHT);
    areaTex.placeAt (0, 0, AREATEX_WIDTH, AREATEX_HEIGHT, GL_RG, GL_UNSIGNED_BYTE, areaTexBytes);
    setTextureParameters (areaTex);

    // The search texture needs to be stored in R8 format and loaded using GL_RED.
    searchTex.allocateImmutableStorage (GL_R8, SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT);
    searchTex.placeAt (0, 0, SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT, GL_RED, GL_UNSIGNED_BYTE, searchTexBytes);
    setTextureParameters (searchTex);
}


bool SMAA::compilePrograms (Program& edge, Program& weight, Program& blend, const Texture& areaTex, 
    const Texture& searchTex, Quality quality, GLsizei width, GLsizei height, GLuint outputTextureUnit) const noexcept
{
    // We need to compile shaders before we can link the programs together.
    const auto shaders = compileShaders (calculateDefines (quality, width, height));

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


bool SMAA::configureRenderTargets (RenderTarget& edge, RenderTarget& weight, 
    GLsizei width, GLsizei height) const noexcept
{
    // We can use GL_RG for the edge texture and GL_RGBA for the blend texture.
    edge.output.allocateImmutableStorage (GL_RG8, width, height);
    weight.output.allocateImmutableStorage (GL_RGBA8, width, height);

    // Ensure texture parameters are configured.
    setTextureParameters (edge.output);
    setTextureParameters (weight.output);

    // Finally configure the framebuffers.
    edge.fbo.attachTexture (edge.output, GL_COLOR_ATTACHMENT0);
    weight.fbo.attachTexture (weight.output, GL_COLOR_ATTACHMENT0);

    return edge.fbo.complete() && weight.fbo.complete();
}


Shader::RawSource SMAA::calculateDefines (Quality quality, GLsizei width, GLsizei height) const noexcept
{
    // Start with the metrics string, this defines the "screen" resolution.
    const auto widthString  = std::to_string (width);
    const auto heightString = std::to_string (height);
    const auto metrics      = "#define SMAA_RT_METRICS float4 (1.0 / " + widthString + ", 1.0 / " + heightString +
        ", " + widthString + ", " + heightString + ")\n";

    // Now determine the correct preset definition.
    switch (quality)
    {
        case Quality::Ultra:
            return metrics + "#define SMAA_PRESET_ULTRA\n";
        case Quality::High:
            return metrics + "#define SMAA_PRESET_HIGH\n";
        case Quality::Medium:
            return metrics + "#define SMAA_PRESET_MEDIUM\n";
        default:
            return metrics + "#define SMAA_PRESET_LOW\n";
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