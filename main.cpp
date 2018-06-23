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

struct PTScene
{
    glm::vec3 cameraPos;
    float yaw;
    float pitch;

    BokehShape DOFBokehShape;
    float DOFFocalLength;
    float DOFApertureRadius;

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

    // yaw and pitch
    90.0f,
    0.0f,

    // DOF bokeh shape, Focal length and aperture radius
    BokehShape::Circle,
    8.0f,
    0.1f,

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
    { 0.0f, 5.0f, -16.0f },

    // yaw and pitch
    -270.0f,
    -1.5f,

    // DOF bokeh shape, Focal length and aperture radius
    BokehShape::SOD,
    16.0f,
    1.0f,

    // sky color
    { 0.01f, 0.01f, 0.01f },

    // spheres
    {
        { { -1.0f, 2.0f, 2.0f },3.0f,{ 1.0f, 0.25f, 0.25f },{ 0.0f,0.0f,0.0f } },
        { {  1.0f, 1.5f, -1.0f },1.5f,{ 0.25f, 1.0f, 0.25f },{ 0.0f,0.0f,0.0f } },

        { {  4.0f, 2.0f, 0.0f },1.0f,{ 0.25f, 0.25f, 1.0f },{ 0.0f,0.0f,0.0f } },

        { {  -0.8f, 3.5f, -0.8f },1.0f,{ 1.0f, 1.0f, 1.0f },{ 0.0f,0.0f,0.0f } },
        { {   1.6f, 3.5f,  0.8f },1.0f,{ 1.0f, 1.0f, 1.0f },{ 0.0f,0.0f,0.0f } },

        { {  -0.6f, 3.5f, -1.0f },0.75f,{ 0.1f, 0.1f, 0.1f },{ 0.0f,0.0f,0.0f } },
        { {   1.8f, 3.5f,  0.6f },0.75f,{ 0.1f, 0.1f, 0.1f },{ 0.0f,0.0f,0.0f } },
    },

    // light spheres
    {
        { { -4.0f,  8.0f, -1.0f},0.5f,{ 0.0f,0.0f,0.0f },{ 200.0f / 3.0f, 100.0f / 3.0f, 100.0f / 3.0f } },
        { {  7.0f,  9.0f, -2.0f},0.5f,{ 0.0f,0.0f,0.0f },{ 100.0f / 3.0f, 200.0f / 3.0f, 100.0f / 3.0f } },
        { {  2.0f, 12.0f,  6.0f},0.5f,{ 0.0f,0.0f,0.0f },{ 100.0f / 3.0f, 100.0f / 3.0f, 200.0f / 3.0f } },


        {{22.49f,15.34f,48.69f},0.22f,{0.0f,0.0f,0.0f},{56.18f,60.93f,36.63f}},
        {{-5.84f,15.38f,32.09f},0.27f,{0.0f,0.0f,0.0f},{20.18f,4.83f,29.09f}},
        {{-4.21f,4.72f,58.04f},0.27f,{0.0f,0.0f,0.0f},{42.33f,42.99f,48.20f}},
        {{-19.26f,12.98f,49.72f},0.19f,{0.0f,0.0f,0.0f},{47.91f,59.19f,54.09f}},
        {{17.93f,12.47f,47.98f},0.15f,{0.0f,0.0f,0.0f},{9.02f,30.82f,32.28f}},
        {{-24.54f,19.56f,40.60f},0.30f,{0.0f,0.0f,0.0f},{10.86f,32.18f,22.76f}},
        {{-6.36f,13.52f,44.59f},0.29f,{0.0f,0.0f,0.0f},{44.18f,45.12f,65.58f}},
        {{17.06f,5.17f,47.90f},0.18f,{0.0f,0.0f,0.0f},{11.27f,16.52f,16.08f}},
        {{9.59f,16.46f,52.93f},0.26f,{0.0f,0.0f,0.0f},{23.62f,5.81f,39.09f}},
        {{23.42f,15.64f,36.89f},0.14f,{0.0f,0.0f,0.0f},{12.54f,5.65f,10.91f}},
        {{-24.46f,12.72f,55.70f},0.21f,{0.0f,0.0f,0.0f},{20.20f,16.17f,5.96f}},
        {{16.94f,4.11f,57.63f},0.26f,{0.0f,0.0f,0.0f},{3.57f,10.80f,24.26f}},
        {{26.09f,4.35f,30.40f},0.18f,{0.0f,0.0f,0.0f},{76.78f,10.68f,49.14f}},
        {{29.38f,12.78f,41.91f},0.18f,{0.0f,0.0f,0.0f},{45.91f,48.66f,38.07f}},
        {{17.15f,19.63f,56.44f},0.24f,{0.0f,0.0f,0.0f},{40.25f,39.04f,35.76f}},
        {{-27.11f,11.20f,57.16f},0.23f,{0.0f,0.0f,0.0f},{48.02f,37.35f,48.05f}},
        {{14.84f,7.79f,58.12f},0.23f,{0.0f,0.0f,0.0f},{20.68f,24.47f,11.51f}},
        {{15.32f,4.82f,44.63f},0.20f,{0.0f,0.0f,0.0f},{26.22f,36.43f,39.77f}},
        {{9.42f,11.09f,44.35f},0.23f,{0.0f,0.0f,0.0f},{4.11f,3.96f,23.77f}},
        {{15.66f,5.08f,45.98f},0.21f,{0.0f,0.0f,0.0f},{16.72f,15.99f,18.70f}},
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

        if (pGui->addCheckBox("Use Blue Noise RNG", m_useBlueNoiseRNG))
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

        pGui->addSeparator();
        sprintf(buffer, "Camera Pos: %f, %f, %f", m_cameraPos.x, m_cameraPos.y, m_cameraPos.z);
        pGui->addText(buffer);
        sprintf(buffer, "Yaw, pitch = %f, %f", m_yaw, m_pitch);
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
        m_yaw = scene.yaw;
        m_pitch = scene.pitch;

        m_DOFBokehShape = scene.DOFBokehShape;
        m_DOFFocalLength = scene.DOFFocalLength;
        m_DOFApertureRadius = scene.DOFApertureRadius;

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

        ConstantBuffer::SharedPtr pShaderConstants = m_computeVars["ShaderConstants"];
        pShaderConstants["invViewProjMtx"] = m_invViewProjMtx;
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
