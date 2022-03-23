#include "Enemy.h"


Enemy::Enemy(olc::vf2d vPos, int bStartDir)
    : Entity(vPos, olc::vf2d(0.1 * bStartDir, 0.0f)), m_vOrigin(vPos),
      m_iState(0), m_iDir(bStartDir)
{
}


Enemy::~Enemy() {}


void Enemy::OnCreate()
{
    m_iAnimFrame = 1;
    m_iAnimLen = 5;
    m_fRadius = 0.5;
    m_bIncrease = true;
    m_pColor = olc::WHITE;
}


void Enemy::Update(olc::vf2d vPlayerPos)
{
    /************************************************/
    /*        Advance Sprite Animation Frame        */
    /************************************************/
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

    /**********************************/
    /*        Control Movement        */
    /**********************************/
    debug_offsetFromOrigin = abs(MathUtils::DistanceBetweenPoints(m_vPos, m_vOrigin));
    if (abs(MathUtils::DistanceBetweenPoints(m_vPos, m_vOrigin)) > 1.0)
        m_iDir = -m_iDir;
    if (MathUtils::DistanceBetweenPoints(m_vPos, vPlayerPos) < 5.0f)
        m_iState = ATTACKING;
    else
        m_iState = REST;

    Move(vPlayerPos);
}


void Enemy::Move(olc::vf2d vTarget)
{
    switch(m_iState)
    {
        case(REST):
            m_pColor = olc::WHITE;
            m_vPos += { 0.1f * m_iDir, 0.0f };
            break;

        case(ATTACKING):
            m_pColor = olc::RED;
            m_vPos += 0.03f * GetVecToObj(vTarget);
            break;
    }
}


olc::vf2d Enemy::GetVecToObj(olc::vf2d vObjPos)
{
    debug_vecToPlayer = (vObjPos - m_vPos).norm();
    return (vObjPos - m_vPos).norm();
}


void Enemy::DrawSelf(olc::TileTransformedView* tv)
{
    //tv->FillCircle(m_vPos, m_fRadius, m_pColor);
    tv->FillRectDecal(m_vPos, olc::vf2d(1.0f, 1.0f));
}


void Enemy::DrawDebug(olc::PixelGameEngine* pge, olc::TileTransformedView* tv, int iIndex)
{
    if (m_iState == ATTACKING)
        tv->DrawLine(m_vPos, debug_vecToPlayer + m_vPos);

    std::string sState = "Enemy State: " + std::to_string(m_iState);
    std::string sOffset = "Enemy Offset: " + std::to_string(debug_offsetFromOrigin);
    olc::vf2d str_pos0 = { 5.0f, 55.0f + (iIndex * 10) };
    olc::vf2d str_pos1 = { 5.0f, 55.0f + (iIndex * 10) };
    //pge->DrawStringDecal(str_pos0, sState);
    //pge->DrawStringDecal(str_pos1, sOffset);
}
