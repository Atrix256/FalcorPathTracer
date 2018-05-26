#include "Falcor.h"
#include "SampleTest.h"

using namespace Falcor;

class Application : public Renderer
{
private:

    ComputeProgram::SharedPtr mpProg;
    ComputeState::SharedPtr mpState;
    ComputeVars::SharedPtr mpProgVars;

    bool mbPixelate = false;
    bool m_orthographicProjection = false;

    Texture::SharedPtr gBlueNoiseTexture;

    Texture::SharedPtr gOutput;

    glm::mat4x4 m_projMtx;
    glm::mat4x4 m_invViewProjMtx;

private:

    void UpdateProjectionMatrix(SampleCallbacks* pSample)
    {
        uint32_t width = pSample->getWindow()->getClientAreaWidth();
        uint32_t height = pSample->getWindow()->getClientAreaHeight();

        if (m_orthographicProjection)
            m_projMtx = glm::orthoLH(-float(width / 2), float(width / 2), -float(height / 2), float(height / 2), 0.1f, 100.0f);
        else
            m_projMtx = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);

        UpdateViewMatrix();
    }

    void UpdateViewMatrix()
    {
        glm::vec3 pos, at, up;
        pos = glm::vec3(0.0f, 0.0f, 0.0f);
        at = glm::vec3(0.0f, 0.0f, 1.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4x4 viewMtx = glm::lookAt(pos, at, up);

        glm::mat4x4 viewProjMtx = m_projMtx * viewMtx;
        m_invViewProjMtx = glm::inverse(viewProjMtx);

        // TODO: remove when things are working well. debug code
        /*
        glm::vec4 A(-1.0f, 0.0f, 0.0f, 1.0f);
        glm::vec4 B(-1.0f, 0.0f, 1.0f, 1.0f);

        glm::vec4 APrime = m_invViewProjMtx * A;
        glm::vec4 BPrime = m_invViewProjMtx * B;

        glm::vec4 AFinal = APrime / APrime.w;
        glm::vec4 BFinal = BPrime / BPrime.w;

        int ijkl = 0;
        */
    }

public:

    void onGuiRender(SampleCallbacks* pSample, Gui* pGui)
    {
        pGui->addCheckBox("Pixelate", mbPixelate);

        if (pGui->addCheckBox("Orthographic Projection", m_orthographicProjection))
        {
            UpdateProjectionMatrix(pSample);
        }
    }

    void onLoad(SampleCallbacks* pSample, RenderContext::SharedPtr pContext)
    {
        mpProg = ComputeProgram::createFromFile("compute.hlsl", "main");
        mpState = ComputeState::create();
        mpState->setProgram(mpProg);
        mpProgVars = ComputeVars::create(mpProg->getReflector());

        gBlueNoiseTexture = createTextureFromFile("Data/BlueNoise.bmp", false, false);
        mpProgVars->setTexture("gBlueNoiseTexture", gBlueNoiseTexture);
    }

    void onFrameRender(SampleCallbacks* pSample, RenderContext::SharedPtr pContext, Fbo::SharedPtr pTargetFbo)
    {
        const glm::vec4 clearColor(0.38f, 0.52f, 0.10f, 1);

        pContext->clearUAV(gOutput->getUAV().get(), clearColor);

        if (mbPixelate)
        {
            mpProg->addDefine("_PIXELATE");
        }
        else
        {
            mpProg->removeDefine("_PIXELATE");
        }

        ConstantBuffer::SharedPtr pShaderConstants = mpProgVars["ShaderConstants"];
        pShaderConstants["fillColor"] = glm::vec3(0.0f, 0.0f, 1.0f);
        pShaderConstants["invViewProjMtx"] = m_invViewProjMtx;
        pShaderConstants["sphere1"] = glm::vec4(0.0f, 0.0f, 90.0f, 1.0f);
        pShaderConstants["sphere2"] = glm::vec4(2.0f, 0.0f, 90.0f, 1.0f);

        mpProgVars->setTexture("gOutput", gOutput);

        pContext->setComputeState(mpState);
        pContext->setComputeVars(mpProgVars);

        uint32_t w = pSample->getWindow()->getClientAreaWidth();
        uint32_t h = pSample->getWindow()->getClientAreaHeight();

        pContext->dispatch(w, h, 1);
        pContext->copyResource(pTargetFbo->getColorTexture(0).get(), gOutput.get());
    }

    void onResizeSwapChain(SampleCallbacks* pSample, uint32_t width, uint32_t height)
    {
        gOutput = Texture::create2D(width, height, ResourceFormat::RGBA8Unorm, 1, 1, nullptr, Resource::BindFlags::ShaderResource | Resource::BindFlags::UnorderedAccess);

        UpdateProjectionMatrix(pSample);
    }
};

#ifdef _WIN32
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#else
int main(int argc, char** argv)
#endif
{
    Application::UniquePtr pRenderer = std::make_unique<Application>();
    SampleConfig config;
    config.windowDesc.title = "Path Tracer";
    config.windowDesc.resizableWindow = true;
    config.windowDesc.width = 800;
    config.windowDesc.height = 600;

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
