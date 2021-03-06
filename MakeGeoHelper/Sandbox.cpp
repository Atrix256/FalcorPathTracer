#include <random>
#include <stdio.h>

int main()
{
    static std::random_device rd;
    static std::mt19937 mt(rd());
    

    // generate some spheres
    if(false)
    {
        static std::uniform_real_distribution<float> distRadius(0.2f, 3.0f);
        static std::uniform_real_distribution<float> distPosXZ(-20.0f, 20.0f);
        static std::uniform_real_distribution<float> distPosY(1.0f, 3.0f);

        static std::uniform_real_distribution<float> distColor(0.1f, 1.0f);

        for (int i = 0; i < 20; ++i)
        {
            float radius = distRadius(mt);
            float posX = distPosXZ(mt);
            float posY = distPosY(mt);
            float posZ = distPosXZ(mt);

            float R = distColor(mt);
            float G = distColor(mt);
            float B = distColor(mt);

            float rgbLen = sqrtf(R*R + G * G + B * B);
            R /= rgbLen;
            G /= rgbLen;
            B /= rgbLen;

            printf("        {{%0.2ff,%0.2ff,%0.2ff},%0.2ff,{%0.2ff,%0.2ff,%0.2ff},{0.0f,0.0f,0.0f}},\n",
                posX, posY, posZ,
                radius,
                R, G, B
            );
        }
        printf("\n\n");
    }

    // generate some light spheres
    if(true)
    {
        static std::uniform_real_distribution<float> distRadius(0.1f, 0.3f);
        static std::uniform_real_distribution<float> distPosX(-30.0f, 30.0f);
        static std::uniform_real_distribution<float> distPosY(3.0f, 20.0f);
        static std::uniform_real_distribution<float> distPosZ(30.0f, 60.0f);

        static std::uniform_real_distribution<float> distColor(0.1f, 1.0f);

        static std::uniform_real_distribution<float> distIntensity(10.0f, 100.0f);

        for (int i = 0; i < 20; ++i)
        {
            float radius = distRadius(mt);
            float posX = distPosX(mt);
            float posY = distPosY(mt);
            float posZ = distPosZ(mt);

            float R = distColor(mt);
            float G = distColor(mt);
            float B = distColor(mt);

            float rgbLen = sqrtf(R*R + G * G + B * B);
            R /= rgbLen;
            G /= rgbLen;
            B /= rgbLen;

            float intensity = distIntensity(mt);
            R *= intensity;
            G *= intensity;
            B *= intensity;

            printf("        {{%0.2ff,%0.2ff,%0.2ff},%0.2ff,{0.0f,0.0f,0.0f},{%0.2ff,%0.2ff,%0.2ff}},\n",
                posX, posY, posZ,
                radius,
                R, G, B
            );
        }
        printf("\n\n");
    }

    system("pause");
    return 0;
}

