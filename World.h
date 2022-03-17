#include <iostream>
#include <memory>
#include <fstream>
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "ProcGen.h"

class World
{
    public:
        World();
        ~World();

    public:
        std::unique_ptr<olc::Renderable> m_pMapSprite;
        std::string sGreeting;
        std::string sMap;
        olc::vi2d vSize;
        void GenerateWorld();
        void StaticGenMap();
        void ProcGenMap();
        void LoadMapFromFile(std::string filename);
        void PrintWorld();
        olc::vf2d FindSpawnableCell();
        olc::vi2d GetSize();
        void DrawMap(olc::TileTransformedView* tv);
        olc::vf2d FindRandomOpenSpot();
};
