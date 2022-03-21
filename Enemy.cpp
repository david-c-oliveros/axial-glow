#include "Enemy.h"


Enemy::Enemy(olc::vf2d vPos)
    : Entity(vPos)
{
}


Enemy::~Enemy() {}


void Enemy::OnCreate()
{
    m_iAnimFrame = 1;
    m_iAnimLen = 5;
    m_fRadius = 0.5;
    m_bIncrease = true;
}


void Enemy::Update()
{
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

    m_fRadius = MathUtils::ReMap(m_iAnimFrame, 0.5f, m_iAnimLen, 0.4, 0.5f);
}


void Enemy::DrawSelf(olc::TileTransformedView* tv)
{
    tv->FillCircle(m_vPos, m_fRadius);
}
