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
    uint geoID;
};

struct Quad
{
    float3 a, b, c, d;
    float3 normal;
    float3 albedo;
    float3 emissive;
    uint geoID;
};

struct PLight
{
    float3 position;
    float3 color;
};

struct PTScene
{
    glm::vec3 cameraPos;
    float3 skyColor;

    std::vector<Sphere> spheres;
    std::vector<Sphere> lightSpheres;
    std::vector<Quad>   quads;
    std::vector<PLight> pLights;
};

PTScene Scene_Box =
{
    // camera position
    { 2.780f, 2.730f, -8.0f },

    // sky color
    { 0.01f, 0.01f, 0.01f },

    // spheres
    {
        {{ 1.5f, 1.5f, 2.5f }, 0.8f, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }},
        {{ 4.5f, 1.5f, 2.5f }, 0.8f, { 0.1f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }},
        {{ 2.0f, 3.5f, 3.5f }, 0.8f, { 1.0f, 0.1f, 1.0f }, { 0.0f, 0.0f, 0.0f }},
        {{ 3.0f, 1.5f, 4.5f }, 0.8f, { 1.0f, 1.0f, 0.1f }, { 0.0f, 0.0f, 0.0f }},
    },

    // light spheres
    {
        {{ 5.0f, 0.5f, 1.0f }, 0.2f, { 1.0f, 1.0f, 0.0f }, {  1.0f, 25.0f, 25.0f }},
        {{ 0.4f, 1.5f, 1.0f }, 0.2f, { 1.0f, 1.0f, 0.0f }, { 25.0f,  1.0f, 25.0f }},
        {{ 4.0f, 3.5f, 4.0f }, 0.2f, { 1.0f, 1.0f, 0.0f }, { 25.0f, 25.0f,  1.0f }},
    },

    // quads
    {
        // floor
        {{ 5.528f, 0.0f, 0.0f }, { 0.0f, 0.0f,   0.0f }, {   0.0f, 0.0f, 5.592f },{ 5.496f, 0.0f, 5.592f }, {0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }},

        // Light
        //{{ 3.430f, 5.486f, 2.270f },{ 3.43f, 5.486f, 3.32f },{ 2.13f, 5.486f, 3.32f },{ 2.13f, 5.486f, 2.27f },{ 0.0f, 0.0f, 0.0f },{ 0.78f, 0.78f, 0.78f },{25.0f, 25.0f, 25.0f}},

        // Cieling
        {{ 5.560f, 5.488f,   0.0f },{ 5.56f, 5.488f, 5.592f },{ 0.0f, 5.488f, 5.592f },{ 0.0f, 5.488f,   0.0f },{ 0.0f, 0.0f, 0.0f },{ 1.0f, 1.0f, 1.0f },{0.0f, 0.0f, 0.0f}},

        // back wall
        {{5.496f,   0.0f, 5.592f},{  0.0f,   0.0f, 5.592f},{  0.0f, 5.488f, 5.592f},{5.56f, 5.488f, 5.592f},{ 0.0f, 0.0f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f, 0.0f }},

        // left wall
        {{0.0f,   0.0f, 5.592f},{0.0f,   0.0f,   0.0f},{0.0f, 5.488f,   0.0f},{0.0f, 5.488f, 5.592f},{ 0.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 0.0f }},

        // right wall
        {{5.528f,   0.0f,   0.0f},{5.496f,   0.0f, 5.592f},{5.56f, 5.488f, 5.592f},{5.56f, 5.488f,   0.0f},{ 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 0.0f }},
    },

    // point lights
    {
        {{ 2.0f, 1.5f, 0.5f },{ 0.0f, 0.0f, 1.0f }}
    },
};

PTScene Scene_PlaneSpheres =
{
    // camera position
    { 0.0f, 2.0f, -10.0f },

    // sky color
    { 0.01f, 0.01f, 0.01f },

    // spheres
    {
        { { -16.30f,1.35f,-2.51f },1.99f,{ 0.46f,0.66f,0.59f },{ 0.0f,0.0f,0.0f } },
        { { 9.84f,2.24f,11.83f },0.76f,{ 0.64f,0.14f,0.75f },{ 0.0f,0.0f,0.0f } },
        { { 7.17f,1.11f,-17.87f },1.92f,{ 0.13f,0.78f,0.61f },{ 0.0f,0.0f,0.0f } },
        { { 19.09f,1.74f,-19.47f },2.31f,{ 0.50f,0.67f,0.55f },{ 0.0f,0.0f,0.0f } },
        { { -8.08f,2.05f,-11.77f },2.35f,{ 0.91f,0.23f,0.35f },{ 0.0f,0.0f,0.0f } },
        { { -16.68f,1.27f,-12.19f },1.91f,{ 0.30f,0.67f,0.68f },{ 0.0f,0.0f,0.0f } },
        { { -4.29f,1.13f,-11.04f },2.02f,{ 0.57f,0.34f,0.75f },{ 0.0f,0.0f,0.0f } },
        { { -0.61f,1.39f,9.48f },2.28f,{ 0.82f,0.40f,0.42f },{ 0.0f,0.0f,0.0f } },
        { { -11.62f,1.83f,6.84f },2.83f,{ 0.73f,0.53f,0.43f },{ 0.0f,0.0f,0.0f } },
        { { -14.08f,2.99f,-7.77f },1.96f,{ 0.62f,0.66f,0.43f },{ 0.0f,0.0f,0.0f } },
        { { 7.82f,1.65f,-12.32f },2.38f,{ 0.53f,0.83f,0.15f },{ 0.0f,0.0f,0.0f } },
        { { 9.10f,1.91f,5.50f },1.65f,{ 0.59f,0.61f,0.53f },{ 0.0f,0.0f,0.0f } },
        { { 17.67f,2.99f,-13.78f },2.29f,{ 0.34f,0.39f,0.85f },{ 0.0f,0.0f,0.0f } },
        { { -14.27f,1.21f,15.30f },0.97f,{ 0.53f,0.65f,0.54f },{ 0.0f,0.0f,0.0f } },
        { { -19.16f,1.57f,14.65f },2.43f,{ 0.59f,0.34f,0.73f },{ 0.0f,0.0f,0.0f } },
        { { -6.66f,2.00f,-15.64f },0.85f,{ 0.70f,0.61f,0.36f },{ 0.0f,0.0f,0.0f } },
        { { -6.85f,1.27f,16.96f },2.21f,{ 0.61f,0.72f,0.32f },{ 0.0f,0.0f,0.0f } },
        { { 1.58f,1.25f,14.96f },2.89f,{ 0.60f,0.79f,0.15f },{ 0.0f,0.0f,0.0f } },
        { { 0.28f,1.50f,6.29f },1.38f,{ 0.22f,0.32f,0.92f },{ 0.0f,0.0f,0.0f } },
        { { 6.79f,2.81f,-8.67f },2.19f,{ 0.35f,0.82f,0.46f },{ 0.0f,0.0f,0.0f } },
    },

    // light spheres
    {
        { { -14.93f,4.33f,-12.66f },0.11f,{ 0.0f,0.0f,0.0f },{ 693.56f,665.82f,763.67f } },
        { { -13.21f,9.37f,-13.13f },0.17f,{ 0.0f,0.0f,0.0f },{ 16.60f,944.96f,593.69f } },
        { { -19.88f,6.18f,-15.01f },0.12f,{ 0.0f,0.0f,0.0f },{ 780.40f,789.29f,851.35f } },
        { { 4.61f,3.97f,12.76f },0.15f,{ 0.0f,0.0f,0.0f },{ 512.67f,848.88f,183.62f } },
        { { 3.41f,7.22f,-18.33f },0.19f,{ 0.0f,0.0f,0.0f },{ 391.45f,415.40f,695.93f } },
        { { -12.21f,3.72f,-2.17f },0.16f,{ 0.0f,0.0f,0.0f },{ 182.93f,947.90f,938.42f } },
        { { -19.93f,7.82f,15.20f },0.17f,{ 0.0f,0.0f,0.0f },{ 723.61f,469.79f,26.05f } },
        { { 10.17f,8.21f,-10.32f },0.19f,{ 0.0f,0.0f,0.0f },{ 475.16f,506.65f,871.29f } },
        { { 19.24f,9.82f,9.33f },0.14f,{ 0.0f,0.0f,0.0f },{ 979.16f,61.38f,145.07f } },
        { { 7.86f,6.59f,14.52f },0.14f,{ 0.0f,0.0f,0.0f },{ 721.67f,484.52f,433.09f } },
        { { -7.12f,6.17f,-15.74f },0.15f,{ 0.0f,0.0f,0.0f },{ 24.08f,564.86f,41.92f } },
        { { -14.23f,3.99f,16.02f },0.16f,{ 0.0f,0.0f,0.0f },{ 64.38f,591.51f,402.69f } },
        { { -6.92f,8.05f,-15.41f },0.16f,{ 0.0f,0.0f,0.0f },{ 84.55f,735.65f,113.17f } },
        { { -5.25f,7.08f,4.04f },0.13f,{ 0.0f,0.0f,0.0f },{ 568.06f,954.55f,603.87f } },
        { { -16.08f,9.15f,15.00f },0.20f,{ 0.0f,0.0f,0.0f },{ 44.06f,249.89f,761.35f } },
        { { 10.67f,8.56f,-6.21f },0.11f,{ 0.0f,0.0f,0.0f },{ 273.80f,883.60f,27.95f } },
        { { -12.12f,4.01f,-1.04f },0.11f,{ 0.0f,0.0f,0.0f },{ 21.78f,680.70f,667.71f } },
        { { 1.99f,8.16f,14.74f },0.15f,{ 0.0f,0.0f,0.0f },{ 871.56f,765.77f,897.82f } },
        { { 6.20f,3.16f,-15.25f },0.14f,{ 0.0f,0.0f,0.0f },{ 577.82f,798.67f,350.24f } },
        { { -11.46f,4.74f,-12.92f },0.14f,{ 0.0f,0.0f,0.0f },{ 867.04f,270.83f,923.71f } },
    },

    // quads
    {
        // floor
        {{ 100.0f, 0.0f, -100.0f }, { -100.0f, 0.0f, -100.0f }, { -100.0f, 0.0f, 100.0f },{ 100.0f, 0.0f, 100.0f }, {0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }},
    },

    // point lights
    {
    },
};

enum class PTScenes
{
    Box,
    PlaneSpheres,

    Count
};

const char* PTScenesNames[] =
{
    "Box",
    "PlaneSpheres"
};
static_assert(countof(PTScenesNames) == (uint)PTScenes::Count, "Wrong number of entries in PTScenesNames");

PTScene& GetScene(PTScenes scene)
{
    switch (scene)
    {
        case PTScenes::Box: return Scene_Box;
        case PTScenes::PlaneSpheres: return Scene_PlaneSpheres;
    }
    static_assert((uint)PTScenes::Count == 2, "Unhandled enum value");
    return Scene_Box;
}

// keep in sync with defines in compute.hlsl
enum class BokehShape : uint32
{
    Circle,
    Square,
    Ring,
    Triangle,
    SOD,

    Count
};

const char* BokehShapeNames[] =
{
    "Circle",
    "Square",
    "Ring",
    "Triangle",
    "Star of David"
};
static_assert(countof(BokehShapeNames) == (uint)BokehShape::Count, "Wrong number of entries in BokehShapeNames");

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

    glm::vec3 m_cameraPos;

    glm::vec2 m_mouseDragPos;
    bool m_mouseDown = false;
    float m_yaw;
    float m_pitch;
    size_t m_frameCount = 0;
    size_t m_sampleCount = 0;
    float m_startTime = 0.0f;
    float m_stopTime = 0.0f;

    float3 m_skyColor;

    // values controled by the UI
    float m_fov = 45.0f;
    bool m_jitter = false;
    bool m_integrate = true;
    int m_samplesPerFrame = 1;
    int m_maxRayBounces = 4;
    int m_StopAtSampleCount = 0;
    bool m_sampleLights = true;
    int m_workGroupSize = 8;

    bool m_DOFEnable = true;
    float m_DOFFocalLength = 8.0f;
    float m_DOFApertureRadius = 0.1f;
    BokehShape m_DOFBokehShape = BokehShape::SOD;

    PTScenes m_scene = PTScenes::PlaneSpheres;

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

        m_viewMtx = glm::lookAtLH(m_cameraPos, m_cameraPos + forward, glm::vec3(0.0f, 1.0f, 0.0f));

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

        if(pGui->addCheckBox("Enable Depth Of Field", m_DOFEnable))
            ResetIntegration(pSample);

        if(pGui->addFloatVar("DOF Focal Length", m_DOFFocalLength))
            ResetIntegration(pSample);

        if(pGui->addFloatVar("DOF Aperture Size", m_DOFApertureRadius))
            ResetIntegration(pSample);

        {
            Falcor::Gui::DropdownList bokehShapes;
            for (uint i = 0; i < (uint)BokehShape::Count; ++i)
            {
                Falcor::Gui::DropdownValue v;
                v.label = BokehShapeNames[i];
                v.value = i;
                bokehShapes.push_back(v);
            }
            if (pGui->addDropdown("DOF Bokeh Shape", bokehShapes, *(uint32*)&m_DOFBokehShape))
                ResetIntegration(pSample);
        }

        {
            Falcor::Gui::DropdownList scenes;
            for (uint i = 0; i < (uint)PTScenes::Count; ++i)
            {
                Falcor::Gui::DropdownValue v;
                v.label = PTScenesNames[i];
                v.value = i;
                scenes.push_back(v);
            }
            if (pGui->addDropdown("Scene", scenes, *(uint32*)&m_scene))
                OnChangeScene(pSample);
        }

        if (pGui->addCheckBox("Jitter Camera", m_jitter))
            ResetIntegration(pSample);

        if (pGui->addCheckBox("Use Blue Noise RNG", m_useBlueNoiseRNG))
            ResetIntegration(pSample);

        if (pGui->addCheckBox("Integrate", m_integrate))
            ResetIntegration(pSample);

        if (pGui->addCheckBox("Explicit Light Sampling", m_sampleLights))
            ResetIntegration(pSample);

        pGui->addIntVar("Stop At Sample Count", m_StopAtSampleCount, 0);

        pGui->addIntVar("Work Group Size", m_workGroupSize, 1);

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

    void OnChangeScene(SampleCallbacks* pSample)
    {
        PTScene& scene = GetScene(m_scene);

        m_computeVars->setStructuredBuffer("g_spheres", scene.spheres.size() == 0 ? nullptr : StructuredBuffer::create(m_computeProgram, "g_spheres", scene.spheres.size()));
        m_computeVars->setStructuredBuffer("g_lightSpheres", scene.lightSpheres.size() == 0 ? nullptr : StructuredBuffer::create(m_computeProgram, "g_lightSpheres", scene.lightSpheres.size()));
        m_computeVars->setStructuredBuffer("g_quads", scene.quads.size() == 0 ? nullptr : StructuredBuffer::create(m_computeProgram, "g_quads", scene.quads.size()));
        m_computeVars->setStructuredBuffer("g_plights", scene.pLights.size() == 0 ? nullptr : StructuredBuffer::create(m_computeProgram, "g_plights", scene.pLights.size()));

        std::fill(&m_keyState[0], &m_keyState[255], false);

        // calculate normals for quads
        for (uint i = 0; i < scene.quads.size(); ++i)
        {
            float3 ab = scene.quads[i].b - scene.quads[i].a;
            float3 ac = scene.quads[i].c - scene.quads[i].a;
            scene.quads[i].normal = normalize(-cross(ab, ac));
        }

        // give all geo a geo id.
        uint nextID = 0;
        for (Sphere& s : scene.spheres)
            s.geoID = nextID++;
        for (Sphere& s : scene.lightSpheres)
            s.geoID = nextID++;
        for (Quad& q : scene.quads)
            q.geoID = nextID++;

        m_cameraPos = scene.cameraPos;
        m_skyColor = scene.skyColor;
        m_yaw = 90.0f;
        m_pitch = 0.0f;
        UpdateViewMatrix(pSample);
    }

    void onLoad(SampleCallbacks* pSample, RenderContext::SharedPtr pContext)
    {
        m_computeProgram = ComputeProgram::createFromFile("compute.hlsl", "main");
        m_computeState = ComputeState::create();
        m_computeState->setProgram(m_computeProgram);
        m_computeVars = ComputeVars::create(m_computeProgram->getReflector());

        m_blueNoiseTexture = createTextureFromFile("Data/BlueNoise.bmp", false, false);
        m_computeVars->setTexture("gBlueNoiseTexture", m_blueNoiseTexture);

        OnChangeScene(pSample);
    }

    void UpdateCamera(SampleCallbacks* pSample)
    {
        static const float c_moveSpeed = 10.0f;

        glm::vec3 offset(0.0f, 0.0f, 0.0f);

        glm::vec4 forward = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) * m_viewMtx;
        glm::vec4 left = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f) * m_viewMtx;

        if (m_keyState['W'])
            offset += glm::vec3(forward.x, forward.y, forward.z);
        
        if (m_keyState['S'])
            offset -= glm::vec3(forward.x, forward.y, forward.z);

        if (m_keyState['A'])
            offset += glm::vec3(left.x, left.y, left.z);

        if (m_keyState['D'])
            offset -= glm::vec3(left.x, left.y, left.z);

        if (offset.x != 0 || offset.y != 0)
        {
            offset *= pSample->getLastFrameTime() * c_moveSpeed;
            m_cameraPos += offset;
            UpdateViewMatrix(pSample);
        }
    }

    void onFrameRender(SampleCallbacks* pSample, RenderContext::SharedPtr pContext, Fbo::SharedPtr pTargetFbo)
    {
        PTScene& scene = GetScene(m_scene);

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

        sprintf(buffer, "%i", m_sampleLights ? 1 : 0);
        m_computeProgram->addDefine("SAMPLE_LIGHTS", buffer);

        sprintf(buffer, "%i", m_maxRayBounces);
        m_computeProgram->addDefine("MAX_RAY_BOUNCES", buffer);

        sprintf(buffer, "%i", m_workGroupSize);
        m_computeProgram->addDefine("WORK_GROUP_SIZE", buffer);

        sprintf(buffer, "%u", (uint32)m_DOFBokehShape);
        m_computeProgram->addDefine("BOKEH_SHAPE", buffer);

        if (m_cosineWeightedhemisphereSampling)
            m_computeProgram->addDefine("COSINE_WEIGHTED_HEMISPHERE_SAMPLING");
        else
            m_computeProgram->removeDefine("COSINE_WEIGHTED_HEMISPHERE_SAMPLING");

        if (m_useBlueNoiseRNG)
            m_computeProgram->addDefine("USE_BLUENOISE_RNG");
        else
            m_computeProgram->removeDefine("USE_BLUENOISE_RNG");

        if (m_DOFEnable)
            m_computeProgram->addDefine("ENABLE_DOF");
        else
            m_computeProgram->removeDefine("ENABLE_DOF");

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
        pShaderConstants["skyColor"] = m_skyColor;
        pShaderConstants["lerpAmount"] = 1.0f / float(m_frameCount + 1);
        pShaderConstants["frameRand"] = (uint)RandomUint32();
        pShaderConstants["frameNumber"] = (uint)m_frameCount;

        pShaderConstants["DOFFocalLength"] = m_DOFFocalLength;
        pShaderConstants["DOFApertureRadius"] = m_DOFApertureRadius;

        for (uint i = 0; i < scene.spheres.size(); ++i)
        {
            m_computeVars->getStructuredBuffer("g_spheres")[i]["position"] = scene.spheres[i].position;
            m_computeVars->getStructuredBuffer("g_spheres")[i]["radius"]   = scene.spheres[i].radius;
            m_computeVars->getStructuredBuffer("g_spheres")[i]["albedo"]   = scene.spheres[i].albedo;
            m_computeVars->getStructuredBuffer("g_spheres")[i]["emissive"] = scene.spheres[i].emissive;
            m_computeVars->getStructuredBuffer("g_spheres")[i]["geoID"] = scene.spheres[i].geoID;
        }

        for (uint i = 0; i < scene.lightSpheres.size(); ++i)
        {
            m_computeVars->getStructuredBuffer("g_lightSpheres")[i]["position"] = scene.lightSpheres[i].position;
            m_computeVars->getStructuredBuffer("g_lightSpheres")[i]["radius"]   = scene.lightSpheres[i].radius;
            m_computeVars->getStructuredBuffer("g_lightSpheres")[i]["albedo"]   = scene.lightSpheres[i].albedo;
            m_computeVars->getStructuredBuffer("g_lightSpheres")[i]["emissive"] = scene.lightSpheres[i].emissive;
            m_computeVars->getStructuredBuffer("g_lightSpheres")[i]["geoID"] = scene.lightSpheres[i].geoID;
        }

        for (uint i = 0; i < scene.quads.size(); ++i)
        {
            m_computeVars->getStructuredBuffer("g_quads")[i]["a"] = scene.quads[i].a;
            m_computeVars->getStructuredBuffer("g_quads")[i]["b"] = scene.quads[i].b;
            m_computeVars->getStructuredBuffer("g_quads")[i]["c"] = scene.quads[i].c;
            m_computeVars->getStructuredBuffer("g_quads")[i]["d"] = scene.quads[i].d;
            m_computeVars->getStructuredBuffer("g_quads")[i]["normal"] = scene.quads[i].normal;
            m_computeVars->getStructuredBuffer("g_quads")[i]["albedo"] = scene.quads[i].albedo;
            m_computeVars->getStructuredBuffer("g_quads")[i]["emissive"] = scene.quads[i].emissive;
            m_computeVars->getStructuredBuffer("g_quads")[i]["geoID"] = scene.quads[i].geoID;
        }

        for (uint i = 0; i < scene.pLights.size(); ++i)
        {
            m_computeVars->getStructuredBuffer("g_plights")[i]["position"] = scene.pLights[i].position;
            m_computeVars->getStructuredBuffer("g_plights")[i]["color"] = scene.pLights[i].color;
        }

        m_computeVars->setTexture("gOutputF32", m_outputF32);
        m_computeVars->setTexture("gOutputU8", m_outputU8);

        pContext->setComputeState(m_computeState);
        pContext->setComputeVars(m_computeVars);

        pContext->dispatch(width/m_workGroupSize, height/m_workGroupSize, 1);
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
                    m_yaw -= mouseDelta.x;
                    m_pitch -= mouseDelta.y;

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
