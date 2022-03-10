#include <iostream>
#include <memory>
#include "olcPixelGameEngine.h"
#include "ProcGen.h"

class World
{
    public:
        World();
        ~World();

    public:
        std::string sGreeting;
        std::string sMap;
        void GenerateWorld();
        void PrintWorld();
        olc::vf2d FindSpawnableCell();
};
