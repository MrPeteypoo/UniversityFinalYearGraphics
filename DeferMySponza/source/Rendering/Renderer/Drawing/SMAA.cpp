#include "SMAA.hpp"


bool SMAA::isInitialised() const noexcept
{
    return false;
}


bool SMAA::initialise (GLsizei width, GLsizei height, Quality quality, const Texture& depthStencilBuffer) noexcept
{
    return false;
}

        
void SMAA::clean() noexcept
{
    if (isInitialised())
    {
        m_edgeDetectionPass.clean();
        m_edgeDetectionFBO.fbo.clean();
        m_edgeDetectionFBO.output.clean();
        m_blendWeightPass.clean();
        m_blendWeightFBO.fbo.clean();
        m_blendWeightFBO.output.clean();
        m_blendingPass.clean();
        m_areaTexture.clean();
        m_searchTexture.clean();
    }
}

 
void SMAA::run (const FullScreenTriangleVAO& vao, const Texture2D& aliasedTexture, const Framebuffer* output) noexcept
{

}