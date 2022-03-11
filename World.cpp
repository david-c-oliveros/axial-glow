#include "World.h"

World::World()
{
    sGreeting = "Welcome to the world";
}

World::~World() {}

void World::GenerateWorld()
{
    sMap =
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################"
        "################################################################";

    vSize = { 64, 64 };

    olc::vf2d center = { 32.0f, 32.0f };
    int radius = 31.0f;
    int total = 1024;
    for (int i = 0; i < total; i++)
    {
        olc::vf2d cartCoords = procgen::GetRandomPointInCircle(radius, center);
        int index = (int)cartCoords.y * (int)sqrt(sMap.length()) + (int)cartCoords.x;
        int count = 0;
        if (sMap[index] == '#')
        {
            sMap.replace((int)index, 1, ".");
        }
        else
        {
            while (sMap[index] == '.')
            {
                index += vSize.x;
                if (index > sMap.length() - vSize.x)
                {
                    index = sMap.length() - vSize.x - 1;
                    break;
                }
            }
            sMap.replace((int)index, 1, ".");
        }
    }
    sMap[sMap.length() - vSize.x - 1] = '#';
}

void World::PrintWorld()
{
    for (int i = 0; i < vSize.x; i++)
    {
        for (int j = 0; j < vSize.y; j++)
        {
            std::cout << sMap[i * vSize.x + j];
        }
        std::cout << std::endl;
    }
}


// TODO - Make better (right now just loops through string and finds the first emtpy cell)
olc::vf2d World::FindSpawnableCell()
{
    for (int i = 0; i < vSize.x; i++)
    {
        for (int j = 0; j < vSize.y; j++)
        {
            if (sMap[i * 64 + j] == '.')
            {
                return { j + 0.5f, i + 0.5f };
            }
        }
    }
}


olc::vi2d World::GetSize()
{
    return vSize;
}
