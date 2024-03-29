#include "World.h"

World::World()
{
    sGreeting = "Welcome to the world";
}

World::~World() {}


void World::GenerateWorld()
{
    for (int i = 0; i < 3; i++)
    {
        m_vMapSprites.push_back(std::make_unique<olc::Renderable>());
    }
    m_vMapSprites[0]->Load("./res/sprites/map/green-zone-tileset/1 Tiles/Tile_01.png");
    m_vMapSprites[1]->Load("./res/sprites/map/industrial-zone-tileset/1 Tiles/IndustrialTile_65.png");
    m_vMapSprites[2]->Load("./res/sprites/map/green-zone-tileset/1 Tiles/Tile_01.png");

    m_pBGSprite  = std::make_unique<olc::Renderable>();
    m_pBGSprite->Load("./res/sprites/map/green-zone-tileset/2 Background/Day/Background.png");
    std::string sFilename = "maps/world_map_36.txt";
    LoadMapFromFile(sFilename);
}


void World::LoadMapFromFile(std::string filename)
{
    vSize = { 24, 24 };
    std::ifstream mapfile;
    mapfile.open(filename);
    std::string str;
    int i = 0;
    while(mapfile >> str)
    {
        if (i == 0)
            vSize.x = str.length();
        sMap += str;
        i++;
    }
    std::cout << i << std::endl;
}


void World::PrintWorld()
{
    for (int i = 0; i < vSize.y; i++)
    {
        for (int j = 0; j < vSize.x; j++)
        {
            std::cout << sMap[i * vSize.x + j];
        }
        std::cout << std::endl;
    }
}


// TODO - Make better (right now just loops through string and finds the first emtpy cell)
olc::vf2d World::FindSpawnableCell()
{
    for (int i = 0; i < vSize.y; i++)
    {
        for (int j = 0; j < vSize.x; j++)
        {
            if (sMap[i * vSize.x + j] == '.')
            {
                return { j, i };
            }
        }
    }
}


olc::vi2d World::GetSize()
{
    return vSize;
}


void World::DrawMap(olc::PixelGameEngine* pge, olc::TileTransformedView* tv)
{
    olc::vf2d vBGScale = olc::vf2d(pge->ScreenWidth() / 576.0f, pge->ScreenHeight() / 324.0f);
    pge->DrawDecal(olc::vf2d(0.0f, 0.0f), m_pBGSprite->Decal(), vBGScale);
    olc::vi2d vTL = tv->GetTopLeftTile().max({ 0, 0 });
    olc::vi2d vBR = tv->GetBottomRightTile().min(vSize);
    olc::vi2d vTile;
    for (vTile.y = vTL.y; vTile.y < vBR.y; vTile.y++)
    {
        for (vTile.x = vTL.x; vTile.x < vBR.x; vTile.x++)
        {
            //tv->DrawRect(vTile, { 1.0f, 1.0f }, olc::WHITE);
            switch(sMap[vTile.y * vSize.x + vTile.x])
            {
                case('#'):
                    tv->DrawPartialDecal(vTile, m_vMapSprites[0]->Decal(), { 0.0f, 0.0f }, { 16.0f, 16.0f }, { 2.1f, 2.1f });
                    break;
                case('='):
                    tv->DrawPartialDecal(vTile, m_vMapSprites[1]->Decal(), { 0.0f, 0.0f }, { 16.0f, 16.0f }, { 2.1f, 2.1f });
                    break;
                case('+'):
                    tv->DrawPartialDecal(vTile, m_vMapSprites[2]->Decal(), { 0.0f, 0.0f }, { 16.0f, 16.0f }, { 2.1f, 2.1f });
                    break;
                default:
                    continue;
            }
        }
    }
}


olc::vf2d World::FindRandomOpenSpot()
{
    auto ti = std::chrono::system_clock::now().time_since_epoch().count();
    srand(ti);
    olc::vf2d randCoord = { (int)rand() % vSize.x, (int)rand() % vSize.y };
    int index = randCoord.y * vSize.x + randCoord.x;
    while (index < sMap.length() && sMap[index] == '#')
    {
        randCoord = { rand() % vSize.x, rand() % vSize.y };
        index = randCoord.y * vSize.x + randCoord.x;
    }

    return randCoord;
}


char World::GetTile(olc::vi2d vCoords)
{
    int index = vCoords.y * vSize.x + vCoords.x;
    if (index >= sMap.length())
        return '#';
    return sMap[index];
}
