#include "Loot.h"


Loot::Loot(olc::vf2d vPos)
    : Entity(vPos)
{
}


Loot::~Loot() {}


void Loot::OnCreate()
{
    m_pLootSprite = std::make_unique<olc::Renderable>();
    m_pLootSprite->Load("./res/sprites/tilemap.png");
}


void Loot::DrawSelf(olc::TileTransformedView* tv)
{
    tv->DrawPartialDecal(m_vPos, m_pLootSprite->Decal(), { 112.0f, 16.0f }, { 16.0f, 16.0f }, { 2.0f, 2.0f });
}
