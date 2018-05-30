#include "Falcor.h"
#include "SampleTest.h"
#include <random>

using namespace Falcor;

struct Sphere
{
    float3 position;
    float radius;
    float3 albedo;
    float3 emissive;
};

Sphere g_spheres[] =
{
    {{ 0.0f, 0.0f, 10.0f }, 1.1f, { 1.0f, 0.1f, 0.1f }, {0.0f, 0.0f, 0.0f}},
    {{ 3.0f, 0.0f, 10.0f }, 1.2f, { 0.1f, 1.0f, 0.1f }, {0.0f, 0.0f, 0.0f}},
    {{ 1.5f, 0.0f, 13.0f }, 1.3f, { 0.1f, 0.1f, 1.0f }, {0.0f, 0.0f, 0.0f}},

    {{ 1.5f, 2.2f, 11.0f }, 1.4f, { 1.0f, 1.0f, 0.1f }, {0.0f, 0.0f, 0.0f}},

    // bright light
    {{ 1.5f, -5.0f, 10.0f }, 1.4f, { 0.0f, 0.0f, 0.0f }, {50.0f, 50.0f, 50.0f}},

    // grey background
    {{ 0.0f, 0.0f, 0.0f }, 100.0f, { 0.0f, 0.0f, 0.0f }, {0.1f, 0.1f, 0.1f}},
};

static float RandomFloat()
{
    // from 0 to 1
    static std::random_device rd;
    static std::mt19937 mt(rd());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(mt);
}

static uint32 RandomUint32()
{
    // from 0 to 1
    static std::random_device rd;
    static std::mt19937 mt(rd());
    static std::uniform_int_distribution<uint32> dist(0,(uint32)-1);
    return dist(mt);
}

class Application : public Renderer
{
private:

    ComputeProgram::SharedPtr m_computeProgram;
    ComputeState::SharedPtr m_computeState;
    ComputeVars::SharedPtr m_computeVars;

    Texture::SharedPtr m_blueNoiseTexture;
    Texture::SharedPtr m_outputF32;
    Texture::SharedPtr m_outputU8;

    glm::mat4x4 m_projMtx;
    glm::mat4x4 m_viewMtx;
    glm::mat4x4 m_invViewProjMtx;

    bool m_keyState[256];

    glm::vec3 m_cameraPos = { 0.0f, 0.0f, 0.0f };

    glm::vec2 m_mouseDragPos;
    bool m_mouseDown = false;
    float m_yaw = 90.0f;
    float m_pitch = 0.0f;
    size_t m_frameCount = 0;
    size_t m_sampleCount = 0;
    float m_startTime = 0.0f;
    float m_stopTime = 0.0f;

    // values controled by the UI
    float m_fov = 45.0f;
    bool m_jitter = false;
    bool m_integrate = true;
    int m_samplesPerFrame = 1;
    int m_maxRayBounces = 4;
    int m_StopAtSampleCount = 0;

    bool m_useBlueNoiseRNG = false;

    // options to speed up rendering
    bool m_cosineWeightedhemisphereSampling = true;

private:

    void ResetIntegration(SampleCallbacks* pSample)
    {
        m_frameCount = 0;
        m_sampleCount = 0;
        m_startTime = pSample->getCurrentTime();
    }

    void UpdateProjectionMatrix(SampleCallbacks* pSample)
    {
        uint32_t width = pSample->getWindow()->getClientAreaWidth();
        uint32_t height = pSample->getWindow()->getClientAreaHeight();

        m_projMtx = glm::perspective(glm::radians(m_fov), float(width) / float(height), 0.1f, 100.0f);

        UpdateViewMatrix(pSample);
    }

    void UpdateViewMatrix(SampleCallbacks* pSample)
    {
        glm::vec3 forward;
        forward.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
        forward.y = sin(glm::radians(m_pitch));
        forward.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));

        m_viewMtx = glm::lookAt(m_cameraPos, m_cameraPos + forward, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4x4 viewProjMtx = m_projMtx * m_viewMtx;
        m_invViewProjMtx = glm::inverse(viewProjMtx);

        ResetIntegration(pSample);
    }

public:

    void onGuiRender(SampleCallbacks* pSample, Gui* pGui)
    {
        if (pGui->addCheckBox("Use Cosine Weighted Hemisphere Samples", m_cosineWeightedhemisphereSampling))
            ResetIntegration(pSample);

        if (pGui->addFloatVar("FOV", m_fov, 1.0f, 180.0f, 1.0f))
            UpdateProjectionMatrix(pSample);

        if (pGui->addButton("Restart Integration"))
            ResetIntegration(pSample);

        if (pGui->addCheckBox("Jitter Camera", m_jitter))
            ResetIntegration(pSample);

        if (pGui->addCheckBox("Use Blue Noise RNG", m_useBlueNoiseRNG))
            ResetIntegration(pSample);

        if (pGui->addCheckBox("Integrate", m_integrate))
            ResetIntegration(pSample);

        pGui->addIntVar("Stop At Sample Count", m_StopAtSampleCount, 0);

        pGui->addIntVar("Samples Per Frame", m_samplesPerFrame, 1, 10);

        if (pGui->addIntVar("Max Ray Bounces", m_maxRayBounces, 1, 10))
            ResetIntegration(pSample);

        uint32_t width = pSample->getWindow()->getClientAreaWidth();
        uint32_t height = pSample->getWindow()->getClientAreaHeight();

        char buffer[256];
        sprintf(buffer, "%zu samples", m_sampleCount);
        pGui->addText(buffer);
        size_t rayCount = m_sampleCount * size_t(width) * size_t(height);
        sprintf(buffer, "%f M primary rays", double(rayCount) / 1000000.0);
        pGui->addText(buffer);

        float duration = m_stopTime - m_startTime;
        if (duration == 0.0f)
            duration = pSample->getLastFrameTime();

        sprintf(buffer, "%f seconds", duration);
        pGui->addText(buffer);

        double sps = double(m_sampleCount) / double(duration);
        sprintf(buffer, "%f samples per second", sps);
        pGui->addText(buffer);

        double rps = double(rayCount) / double(duration);
        rps /= 1000000.0;
        sprintf(buffer, "%f M primary rays per second", rps);
        pGui->addText(buffer);
    }

    void onLoad(SampleCallbacks* pSample, RenderContext::SharedPtr pContext)
    {
        m_computeProgram = ComputeProgram::createFromFile("compute.hlsl", "main");
        m_computeState = ComputeState::create();
        m_computeState->setProgram(m_computeProgram);
        m_computeVars = ComputeVars::create(m_computeProgram->getReflector());

        m_blueNoiseTexture = createTextureFromFile("Data/BlueNoise.bmp", false, false);
        m_computeVars->setTexture("gBlueNoiseTexture", m_blueNoiseTexture);

        m_computeVars->setStructuredBuffer("gSpheres", StructuredBuffer::create(m_computeProgram, "gSpheres", countof(g_spheres)));

        std::fill(&m_keyState[0], &m_keyState[255], false);
    }

    void UpdateCamera(SampleCallbacks* pSample)
    {
        static const float c_moveSpeed = 10.0f;

        glm::vec3 offset(0.0f, 0.0f, 0.0f);

        glm::vec4 forward = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f) * m_viewMtx;
        glm::vec4 left = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) * m_viewMtx;

        if (m_keyState['W'])
            offset += glm::vec3(forward.x, forward.y, forward.z);
        
        if (m_keyState['S'])
            offset -= glm::vec3(forward.x, forward.y, forward.z);

        if (m_keyState['A'])
            offset -= glm::vec3(left.x, left.y, left.z);

        if (m_keyState['D'])
            offset += glm::vec3(left.x, left.y, left.z);

        if (offset.x != 0 || offset.y != 0)
        {
            offset *= pSample->getLastFrameTime() * c_moveSpeed;
            m_cameraPos += offset;
            UpdateViewMatrix(pSample);
        }
    }

    void onFrameRender(SampleCallbacks* pSample, RenderContext::SharedPtr pContext, Fbo::SharedPtr pTargetFbo)
    {
        if (m_StopAtSampleCount > 0 && m_sampleCount >= m_StopAtSampleCount)
        {
            pContext->copyResource(pTargetFbo->getColorTexture(0).get(), m_outputU8.get());
            return;
        }

        if (!m_integrate)
            ResetIntegration(pSample);

        UpdateCamera(pSample);

        uint32_t width = pSample->getWindow()->getClientAreaWidth();
        uint32_t height = pSample->getWindow()->getClientAreaHeight();

        char buffer[256];
        sprintf(buffer, "%i", m_samplesPerFrame);
        m_computeProgram->addDefine("SAMPLES_PER_FRAME", buffer);

        sprintf(buffer, "%i", m_maxRayBounces);
        m_computeProgram->addDefine("MAX_RAY_BOUNCES", buffer);

        if (m_cosineWeightedhemisphereSampling)
            m_computeProgram->addDefine("COSINE_WEIGHTED_HEMISPHERE_SAMPLING");
        else
            m_computeProgram->removeDefine("COSINE_WEIGHTED_HEMISPHERE_SAMPLING");

        if (m_useBlueNoiseRNG)
            m_computeProgram->addDefine("USE_BLUENOISE_RNG");
        else
            m_computeProgram->removeDefine("USE_BLUENOISE_RNG");

        // jitter the camera if we should
        glm::mat4x4 invViewProjMtx = m_invViewProjMtx;
        if (m_jitter)
        {
            float jitterX = (RandomFloat() - 0.5f) / float(width);
            float jitterY = (RandomFloat() - 0.5f) / float(height);

            glm::mat4x4 viewProjMtx = m_projMtx * m_viewMtx;

            glm::mat4x4 jitterMtx = glm::mat4();
            jitterMtx[3] = glm::vec4(jitterX, jitterY, 0.0f, 1.0f);

            glm::mat4x4 tempMtx = jitterMtx * viewProjMtx;
            invViewProjMtx = glm::inverse(tempMtx);
        }

        ConstantBuffer::SharedPtr pShaderConstants = m_computeVars["ShaderConstants"];
        pShaderConstants["invViewProjMtx"] = invViewProjMtx;
        pShaderConstants["lerpAmount"] = 1.0f / float(m_frameCount + 1);
        pShaderConstants["frameRand"] = (uint)RandomUint32();
        pShaderConstants["frameNumber"] = (uint)m_frameCount;


        for (uint i = 0; i < countof(g_spheres); ++i)
        {
            m_computeVars->getStructuredBuffer("gSpheres")[i]["position"] = g_spheres[i].position;
            m_computeVars->getStructuredBuffer("gSpheres")[i]["radius"]   = g_spheres[i].radius;
            m_computeVars->getStructuredBuffer("gSpheres")[i]["albedo"]   = g_spheres[i].albedo;
            m_computeVars->getStructuredBuffer("gSpheres")[i]["emissive"] = g_spheres[i].emissive;
        }

        m_computeVars->setTexture("gOutputF32", m_outputF32);
        m_computeVars->setTexture("gOutputU8", m_outputU8);

        pContext->setComputeState(m_computeState);
        pContext->setComputeVars(m_computeVars);

        pContext->dispatch(width, height, 1);
        pContext->copyResource(pTargetFbo->getColorTexture(0).get(), m_outputU8.get());

        m_frameCount++;
        m_sampleCount += m_samplesPerFrame;

        m_stopTime = pSample->getCurrentTime();
    }

    void onResizeSwapChain(SampleCallbacks* pSample, uint32_t width, uint32_t height)
    {
        m_outputU8 = Texture::create2D(width, height, ResourceFormat::RGBA8Unorm, 1, 1, nullptr, Resource::BindFlags::ShaderResource | Resource::BindFlags::UnorderedAccess);
        m_outputF32 = Texture::create2D(width, height, ResourceFormat::RGBA32Float, 1, 1, nullptr, Resource::BindFlags::ShaderResource | Resource::BindFlags::UnorderedAccess);

        UpdateProjectionMatrix(pSample);
    }

    bool onMouseEvent(SampleCallbacks* pSample, const MouseEvent& mouseEvent)
    {
        if (mouseEvent.type == MouseEvent::Type::LeftButtonDown)
        {
            m_mouseDragPos = mouseEvent.pos;
            m_mouseDown = true;
            return true;
        }
        else if (mouseEvent.type == MouseEvent::Type::LeftButtonUp)
        {
            m_mouseDown = false;
            return true;
        }
        else if (mouseEvent.type == MouseEvent::Type::Move)
        {
            if (m_mouseDown)
            {
                glm::vec2 mouseDelta = mouseEvent.pos - m_mouseDragPos;

                if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
                {
                    m_mouseDragPos = mouseEvent.pos;

                    mouseDelta *= pSample->getLastFrameTime() * 100000.0f;
                    m_yaw += mouseDelta.x;
                    m_pitch += mouseDelta.y;

                    if (m_pitch > 89.0f)
                        m_pitch = 89.0f;
                    else if (m_pitch < -89.0f)
                        m_pitch = -89.0f;

                    UpdateViewMatrix(pSample);

                    return true;
                }
            }
        }

        return false;
    }

    bool onKeyEvent(SampleCallbacks* pSample, const KeyboardEvent& keyEvent)
    {
        if ((uint32_t)keyEvent.key >= 256)
            return false;

        if (keyEvent.type != KeyboardEvent::Type::KeyPressed && keyEvent.type != KeyboardEvent::Type::KeyReleased)
            return false;

        m_keyState[(uint32_t)keyEvent.key] = (keyEvent.type == KeyboardEvent::Type::KeyPressed);
        return true;
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
