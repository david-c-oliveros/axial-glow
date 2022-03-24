#include "Player.h"
#include "DebugUtils.h"


Player::Player(olc::vf2d vPos)
    : Entity(vPos), m_cAnimCounter(Counter(6)), m_cJumpCounter(6)
{
}


Player::~Player() {}


void Player::OnCreate()
{
    for (int i = 0; i <= 4; i++)
    {
        m_pPlayerSprite.push_back(std::make_unique<olc::Renderable>());
    }

    m_pPlayerSprite[0]->Load("./res/sprites/char/cyberpunk-characters/3\ Cyborg/Cyborg_idle.png");
    m_pPlayerSprite[1]->Load("./res/sprites/char/cyberpunk-characters/3\ Cyborg/Cyborg_run_right.png");
    m_pPlayerSprite[2]->Load("./res/sprites/char/cyberpunk-characters/3\ Cyborg/Cyborg_jump_right.png");

    m_iSpriteCurFrame = m_iSpriteStartFrame;

    m_iMoveDir = 1;
    m_iState = IDLE;
    m_cAnimCounter.Start();

    m_vMoveVel = { 0.0f, 0.0f };
    m_iCoin = 0;
}


void Player::Update()
{
    if (m_iState != JUMP_RIGHT && m_iState != JUMP_LEFT)
    {
        if (bSprint)
            m_cAnimCounter.ChangeInterval(4);
        else
            m_cAnimCounter.ChangeInterval(6);
    }

    m_cAnimCounter.Update();

    if ((m_iState == JUMP_LEFT || m_iState == JUMP_RIGHT) && m_iSpriteCurFrame == m_iSpriteEndFrame)
        m_iState = IDLE;

    if (m_cAnimCounter.Check())
    {
        m_cAnimCounter.Reset();
        m_cAnimCounter.Start();

        /******************************************************/
        /*       Ensure the animation runs currect way        */
        /******************************************************/
        if (++m_iSpriteCurFrame > m_iSpriteEndFrame)
            m_iSpriteCurFrame = m_iSpriteStartFrame;

        switch(m_iState)
        {
            case(IDLE):
                m_iSpriteCurPix = m_iSpriteCurFrame * m_vSpriteSize.x;
                break;

            case(RUN_LEFT):
                m_iSpriteStartFrame = 0;
                m_iSpriteEndFrame = 3;
                m_iMoveDir = -1;
                m_iSpriteCurPix = m_iSpriteCurFrame * m_vSpriteSize.x;
                break;

            case(RUN_RIGHT):

                m_iSpriteEndFrame = 3;
                m_iMoveDir = 1;
                m_iSpriteCurPix = m_iSpriteCurFrame * m_vSpriteSize.x;
                break;

            case(JUMP_LEFT):
                m_iSpriteStartFrame = 0;
                m_iSpriteEndFrame = 3;
                m_iMoveDir = -1;
                m_iSpriteCurPix = m_iSpriteCurFrame * m_vSpriteSize.x;
                break;

            case(JUMP_RIGHT):
                m_iSpriteStartFrame = 0;
                m_iSpriteEndFrame = 3;
                m_iMoveDir = 1;
                m_iSpriteCurPix = m_iSpriteCurFrame * m_vSpriteSize.x;
                break;
        }
    }
}


State Player::GetState()
{
    return m_iState;
}


void Player::SetState(State iState)
{
    m_iState = iState;
}


void Player::DrawSelf(olc::TileTransformedView* tv)
{
    olc::vf2d vSprPos = m_vPos;
    olc::vf2d vSprSourcePos = olc::vf2d(m_iSpriteCurPix, 0.0f);
    olc::vf2d vSprScale = olc::vf2d(m_iMoveDir, 1.0f);
    int iSprIndex;
    std::cout << vSprSourcePos / m_vSpriteSize << std::endl;
    if (m_iState == JUMP_LEFT || m_iState == JUMP_RIGHT)
    {
        iSprIndex = 2;
    }
    else if (m_iState == RUN_LEFT || m_iState == RUN_RIGHT)
    {
        iSprIndex = 1;
    }
    else
    {
        iSprIndex = 0;
    }

    if (m_iMoveDir == -1)
        vSprPos = { m_vPos.x + 0.9f, m_vPos.y - 0.5f };
    else
        vSprPos = { m_vPos.x + 0.08f, m_vPos.y - 0.5f };

    tv->DrawPartialDecal(vSprPos, m_pPlayerSprite[iSprIndex]->Decal(),
            vSprSourcePos, m_vSpriteSize, vSprScale);
}


void Player::DrawDebug(olc::PixelGameEngine* pge, olc::TileTransformedView* tv)
{
    tv->FillRectDecal(m_vPos, { 1.0, 1.0, }, olc::Pixel(255, 255, 255, 64));
    std::string sPlayerPos = "Position: " + std::to_string(m_vPos.x) + ", " + std::to_string(m_vPos.y);
    std::string sPlayerVel = "Velocity: " + std::to_string(m_vVel.x) + ", " + std::to_string(m_vVel.y);
    std::string sPlayerMoveVel = "Move Velocity: " + std::to_string(m_vMoveVel.x) +
                                 ", " + std::to_string(m_vMoveVel.y);
    std::string sPlayerState = "State: " + std::to_string(m_iState);
    std::string sPlayerSprint = "Sprint: " + std::to_string(bSprint);
    olc::vf2d str_velocityPos = { 5.0f, 5.0f };
    olc::vf2d str_positionPos = { 5.0f, 15.0f };
    olc::vf2d str_statePos = { 5.0f, 25.0f };
    olc::vf2d str_sprintPos = { 5.0f, 35.0f };
    olc::vf2d str_moveVelocityPos = { 5.0f, 45.0f };
    olc::vf2d str_collide = { 5.0f, 55.0f };
    pge->DrawStringDecal(str_positionPos, sPlayerPos);
    pge->DrawStringDecal(str_velocityPos, sPlayerVel);
    pge->DrawStringDecal(str_statePos, sPlayerState);
    pge->DrawStringDecal(str_sprintPos, sPlayerSprint);
    pge->DrawStringDecal(str_moveVelocityPos, sPlayerMoveVel);
    if (bCollide)
        pge->DrawStringDecal(str_collide, "Collision!");

}


void Player::DrawStats(olc::PixelGameEngine* pge)
{
    std::string sCoin = "Coin: " + std::to_string(m_iCoin);
    olc::vf2d str_pos = { 5.0f, pge->ScreenHeight() - 16.0f };
    pge->DrawStringDecal(str_pos, sCoin);
}


void Player::AddCoin(int iCoin)
{
    m_iCoin += iCoin;
}


olc::vf2d Player::GetVel()
{
    return m_vVel;
}


olc::vf2d Player::GetMoveVel()
{
    return m_vMoveVel;
}


olc::vf2d Player::GetBoxCollider()
{
    return m_vBoxCollider;
}


void Player::SetVelX(float fX)
{
    m_vVel.x = fX;
}


void Player::SetVelY(float fY)
{
    m_vVel.y = fY;
}


void Player::SetColArea(olc::vi2d vAreaTL, olc::vi2d vAreaBR)
{
    m_vColAreaTL = vAreaTL;
    m_vColAreaBR = vAreaBR;
}


void Player::Jump()
{
    m_cAnimCounter.ChangeInterval(10);
    m_cAnimCounter.Reset();
    m_cAnimCounter.Start();
    m_iSpriteCurFrame = 0;
    m_iState = m_iMoveDir == 1 ? JUMP_RIGHT : JUMP_LEFT;
    if (m_vVel.y == 0.0f)
    {
      SetVel({ 0.0f, -24.0f});
    }
}


void Player::TakeDamage(int iHP)
{
    m_iHealthPoints -= iHP;
}


void Player::Heal(int iHP)
{
    m_iHealthPoints += iHP;
}
