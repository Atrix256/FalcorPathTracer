#pragma once
#include "Falcor.h"
#include "SampleTest.h"

using namespace Falcor;

class ComputeShader : public Renderer
{
public:
    void onLoad(SampleCallbacks* pSample, RenderContext::SharedPtr pContext) override;
    void onFrameRender(SampleCallbacks* pSample, RenderContext::SharedPtr pContext, Fbo::SharedPtr pTargetFbo) override;
    void onGuiRender(SampleCallbacks* pSample, Gui* pGui) override;
    void onResizeSwapChain(SampleCallbacks* pSample, uint32_t width, uint32_t height);

private:
    ComputeProgram::SharedPtr mpProg;
    ComputeState::SharedPtr mpState;
    bool mbPixelate = false;
    ComputeVars::SharedPtr mpProgVars;
    Texture::SharedPtr mpImage;

    Texture::SharedPtr mpTmpTexture;
};
