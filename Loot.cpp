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
    m_iAnimFrame = 1;
    m_iAnimLen = 3;
    m_vSpriteSize = { 1.0f, 1.0f };
    m_bIncrease = true;
    m_iValue = 5;
}


void Loot::Update()
{
    m_vSpriteSize = { 1.0f, 1.0f };
    if (m_bIncrease)
    {
        if (++m_iAnimFrame > m_iAnimLen)
            m_bIncrease = false;
    }
    else
    {
        if (--m_iAnimFrame < 1)
            m_bIncrease = true;
    }

    m_vSpriteSize = { MathUtils::ReMap(m_iAnimFrame, 1.0f, m_iAnimLen, 0.7f, 1.0f) * m_vSpriteSize.x,
                      MathUtils::ReMap(m_iAnimFrame, 1.0f, m_iAnimLen, 0.7f, 1.0f) * m_vSpriteSize.y };
}


void Loot::DrawSelf(olc::TileTransformedView* tv)
{
    tv->DrawPartialRotatedDecal(m_vPos, m_pLootSprite->Decal(), 0.0f, { 8.0f, 8.0f },
                                { 112.0f, 16.0f }, { 16.0f, 16.0f }, m_vSpriteSize);
}


int Loot::GetValue()
{
    return m_iValue;
}
