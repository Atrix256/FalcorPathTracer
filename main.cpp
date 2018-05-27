#include "Falcor.h"
#include "SampleTest.h"

using namespace Falcor;

class Application : public Renderer
{
private:

    ComputeProgram::SharedPtr m_computeProgram;
    ComputeState::SharedPtr m_computeState;
    ComputeVars::SharedPtr m_computeVars;

    Texture::SharedPtr m_blueNoiseTexture;
    Texture::SharedPtr m_output;

    glm::mat4x4 m_projMtx;
    glm::mat4x4 m_invViewProjMtx;

    bool m_pixelate = false;
    float m_fov = 45.0f;

private:

    void UpdateProjectionMatrix(SampleCallbacks* pSample)
    {
        uint32_t width = pSample->getWindow()->getClientAreaWidth();
        uint32_t height = pSample->getWindow()->getClientAreaHeight();

        m_projMtx = glm::perspective(glm::radians(m_fov), float(width) / float(height), 0.1f, 100.0f);

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
        pGui->addCheckBox("Pixelate", m_pixelate);

        if (pGui->addFloatVar("FOV", m_fov, 1.0f, 180.0f, 1.0f))
            UpdateProjectionMatrix(pSample);
    }

    void onLoad(SampleCallbacks* pSample, RenderContext::SharedPtr pContext)
    {
        m_computeProgram = ComputeProgram::createFromFile("compute.hlsl", "main");
        m_computeState = ComputeState::create();
        m_computeState->setProgram(m_computeProgram);
        m_computeVars = ComputeVars::create(m_computeProgram->getReflector());

        m_blueNoiseTexture = createTextureFromFile("Data/BlueNoise.bmp", false, false);
        m_computeVars->setTexture("gBlueNoiseTexture", m_blueNoiseTexture);
    }

    void onFrameRender(SampleCallbacks* pSample, RenderContext::SharedPtr pContext, Fbo::SharedPtr pTargetFbo)
    {
        const glm::vec4 clearColor(0.38f, 0.52f, 0.10f, 1);

        pContext->clearUAV(m_output->getUAV().get(), clearColor);

        if (m_pixelate)
        {
            m_computeProgram->addDefine("_PIXELATE");
        }
        else
        {
            m_computeProgram->removeDefine("_PIXELATE");
        }

        ConstantBuffer::SharedPtr pShaderConstants = m_computeVars["ShaderConstants"];
        pShaderConstants["fillColor"] = glm::vec3(0.0f, 0.0f, 1.0f);
        pShaderConstants["invViewProjMtx"] = m_invViewProjMtx;\
        pShaderConstants["sphere1"] = glm::vec4(0.0f, 0.0f, 10.0f, 1.0f);
        pShaderConstants["sphere2"] = glm::vec4(2.0f, 0.0f, 10.0f, 1.0f);

        m_computeVars->setTexture("gOutput", m_output);

        pContext->setComputeState(m_computeState);
        pContext->setComputeVars(m_computeVars);

        uint32_t w = pSample->getWindow()->getClientAreaWidth();
        uint32_t h = pSample->getWindow()->getClientAreaHeight();

        pContext->dispatch(w, h, 1);
        pContext->copyResource(pTargetFbo->getColorTexture(0).get(), m_output.get());
    }

    void onResizeSwapChain(SampleCallbacks* pSample, uint32_t width, uint32_t height)
    {
        m_output = Texture::create2D(width, height, ResourceFormat::RGBA8Unorm, 1, 1, nullptr, Resource::BindFlags::ShaderResource | Resource::BindFlags::UnorderedAccess);

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
