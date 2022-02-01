#include "Player.h"


Player::Player(olc::vf2d vPos)
    : Entity(vPos)
{
}

Player::~Player() {}

void Player::Create(olc::vf2d vSpriteSize)
{
    m_vSpriteSize = vSpriteSize;
}

bool Player::Update(int iGameTick)
{
    if (iGameTick >= 5)
    {
        if (++m_iSpriteCurrentCol > m_iSpriteEndFrameWalking)
            m_iSpriteCurrentCol = 0;

        m_vSpriteCurrentPos = { m_vSpriteStartPos[1].x + m_iSpriteCurrentCol * m_vSpriteSize.x,
                                    m_vSpriteStartPos[1].y };
        return true;
    }
    return false;
}

void Player::DrawSelf(olc::TileTransformedView* tv)
{
    tv->DrawPartialDecal({ m_vPos.x  - 0.5f, m_vPos.y - 0.8f }, m_pPlayerDecal,
            m_vSpriteCurrentPos, m_vSpriteSize, { 2.0f, 2.0f });
}
