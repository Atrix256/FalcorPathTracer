#include "ComputeShader.h"

void ComputeShader::onGuiRender(SampleCallbacks* pSample, Gui* pGui)
{
    pGui->addCheckBox("Pixelate", mbPixelate);
}

Texture::SharedPtr createTmpTex(uint32_t width, uint32_t height)
{
    return Texture::create2D(width, height, ResourceFormat::RGBA8Unorm, 1, 1, nullptr, Resource::BindFlags::ShaderResource | Resource::BindFlags::UnorderedAccess);
}

void ComputeShader::onLoad(SampleCallbacks* pSample, RenderContext::SharedPtr pContext)
{
    mpProg = ComputeProgram::createFromFile("compute.hlsl", "main");
    mpState = ComputeState::create();
    mpState->setProgram(mpProg);
    mpProgVars = ComputeVars::create(mpProg->getReflector());

    Fbo::SharedPtr pFbo = pSample->getCurrentFbo();
    mpTmpTexture = createTmpTex(pFbo->getWidth(), pFbo->getHeight());

    loadImageFromFile(pSample, "Data/BlueNoise.bmp");
}

void ComputeShader::loadImageFromFile(SampleCallbacks* pSample, std::string filename)
{
    mpImage = createTextureFromFile(filename, false, true);
    mpProgVars->setTexture("gInput", mpImage);
    mpTmpTexture = createTmpTex(mpImage->getWidth(), mpImage->getHeight());

    pSample->resizeSwapChain(mpImage->getWidth(), mpImage->getHeight());
}

void ComputeShader::onFrameRender(SampleCallbacks* pSample, RenderContext::SharedPtr pContext, Fbo::SharedPtr pTargetFbo)
{
	const glm::vec4 clearColor(0.38f, 0.52f, 0.10f, 1);

    if(mpImage)
    {
        pContext->clearUAV(mpTmpTexture->getUAV().get(), clearColor);

        if (mbPixelate)
        {
            mpProg->addDefine("_PIXELATE");
        }
        else
        {
            mpProg->removeDefine("_PIXELATE");
        }
        mpProgVars->setTexture("gOutput", mpTmpTexture);

        pContext->setComputeState(mpState);
        pContext->setComputeVars(mpProgVars);

        uint32_t w = (mpImage->getWidth() / 16) + 1;
        uint32_t h = (mpImage->getHeight() / 16) + 1;
        pContext->dispatch(w, h, 1);
        pContext->copyResource(pTargetFbo->getColorTexture(0).get(), mpTmpTexture.get());
    }
    else
    {
        pContext->clearRtv(pTargetFbo->getRenderTargetView(0).get(), clearColor);
    }
}

void ComputeShader::onResizeSwapChain(SampleCallbacks* pSample, uint32_t width, uint32_t height)
{
    if (mpTmpTexture)
    {
        mpTmpTexture = createTmpTex(width, height);
    }
}

#ifdef _WIN32
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#else
int main(int argc, char** argv)
#endif
{
    ComputeShader::UniquePtr pRenderer = std::make_unique<ComputeShader>();
    SampleConfig config;
    config.windowDesc.title = "Path Tracer";
    config.windowDesc.resizableWindow = true;
    config.deviceDesc.depthFormat = ResourceFormat::Unknown;
#ifdef _WIN32
    Sample::run(config, pRenderer);
#else
    config.argc = (uint32_t)argc;
    config.argv = argv;
    Sample::run(config, pRenderer);
#endif
    return 0;
}
