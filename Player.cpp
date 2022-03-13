#include "Player.h"
#include "DebugUtils.h"


Player::Player(olc::vf2d vPos)
    : Entity(vPos)
{
}


Player::~Player() {}


void Player::OnCreate()
{
    m_vSpriteStartPos.push_back({ 11.0f, 10.0f });
    m_vSpriteStartPos.push_back({ 11.0f, 41.0f });

    m_pPlayerSprite.push_back(std::make_unique<olc::Renderable>());
    m_pPlayerSprite.push_back(std::make_unique<olc::Renderable>());
    m_pPlayerSprite[0]->Load("./res/sprites/ForestBoy_alpha_Left.png");
    m_pPlayerSprite[1]->Load("./res/sprites/ForestBoy_alpha_Right.png");

    m_vSpriteCurrentPos = m_vSpriteStartPos[1];

    m_iSpriteDir = 1;
    m_iSpriteCurrentCol = 0;
    m_iPlayerState = PLAYER_REST;
}


bool Player::Update(int iGameTick)
{
    m_iAnimInterval = m_bSprint ? 3 : 5;

    if (iGameTick % m_iAnimInterval == 0)
    {
        /******************************************************/
        /*       Ensure the animation runs currect way        */
        /******************************************************/
        if (m_iSpriteDir == 1)
        {
            if (++m_iSpriteCurrentCol > m_iSpriteEndFrame)
                m_iSpriteCurrentCol = m_iSpriteStartFrame;
        } else if(m_iSpriteDir == 0)
        {
            if (--m_iSpriteCurrentCol < m_iSpriteStartFrame)
                m_iSpriteCurrentCol = m_iSpriteEndFrame;
        }

        switch(m_iPlayerState)
        {
            case(PLAYER_REST):
                if (m_iSpriteDir == 0)
                {
                    m_iSpriteStartFrame = 3;
                    m_iSpriteEndFrame   = 5;
                }
                else if (m_iSpriteDir == 1)
                {
                    m_iSpriteStartFrame = 0;
                    m_iSpriteEndFrame   = 2;
                }

                m_iSpriteCurrentCol = std::clamp(m_iSpriteCurrentCol, m_iSpriteStartFrame, m_iSpriteEndFrame);
                m_vSpriteCurrentPos = { m_vSpriteStartPos[0].x + m_iSpriteCurrentCol * m_vSpriteSize.x,
                                        m_vSpriteStartPos[0].y };
                break;

            case(PLAYER_WALK_LEFT):
                m_iSpriteStartFrame = 0;
                m_iSpriteEndFrame = 5;
                m_iSpriteDir = 0;
                m_vSpriteCurrentPos = { m_vSpriteStartPos[1].x + m_iSpriteCurrentCol * m_vSpriteSize.x,
                                        m_vSpriteStartPos[1].y };
                break;

            case(PLAYER_WALK_RIGHT):
                m_iSpriteStartFrame = 0;
                m_iSpriteEndFrame = 5;
                m_iSpriteDir = 1;
                m_vSpriteCurrentPos = { m_vSpriteStartPos[1].x + m_iSpriteCurrentCol * m_vSpriteSize.x,
                                        m_vSpriteStartPos[1].y };
                break;

            case(PLAYER_WALK_UP_DOWN):
                m_iSpriteStartFrame = 0;
                m_iSpriteEndFrame = 5;
                m_vSpriteCurrentPos = { m_vSpriteStartPos[1].x + m_iSpriteCurrentCol * m_vSpriteSize.x,
                                        m_vSpriteStartPos[1].y };
                break;

            default:
                if (m_iSpriteDir == 0)
                {
                    m_iSpriteStartFrame = 0;
                    m_iSpriteEndFrame   = 2;
                }
                else if (m_iSpriteDir == 1)
                {
                m_iSpriteStartFrame = 0;
                m_iSpriteEndFrame   = 2;
            }

            m_vSpriteCurrentPos = { m_vSpriteStartPos[0].x + m_iSpriteCurrentCol * m_vSpriteSize.x,
                                    m_vSpriteStartPos[0].y };
            break;
        }

        return true;
    }
    return false;
}


void Player::SetState(int iState)
{
    m_iPlayerState = iState;
}


void Player::DrawSelf(olc::TileTransformedView* tv)
{
    tv->DrawPartialDecal({ m_vPos.x  - 0.5f, m_vPos.y - 0.8f }, m_pPlayerSprite[m_iSpriteDir]->Decal(),
            m_vSpriteCurrentPos, m_vSpriteSize, { 2.0f, 2.0f });
}


int Player::State()
{
    return m_iPlayerState;
}


void Player::DrawDebug(olc::PixelGameEngine* pge)
{
    std::string sPlayerPos = "Position: " + std::to_string(m_vPos.x) + ", " + std::to_string(m_vPos.y);
    std::string sPlayerVel = "Velocity: " + std::to_string(m_vVel.x) + ", " + std::to_string(m_vVel.y);
    std::string sPlayerState = "State: " + std::to_string(m_iPlayerState);
    std::string sPlayerSprint = "Sprint: " + std::to_string(m_bSprint);
    olc::vf2d str_velocityPos = { 5.0f, 5.0f };
    olc::vf2d str_positionPos = { 5.0f, 15.0f };
    olc::vf2d str_statePos = { 5.0f, 25.0f };
    olc::vf2d str_sprintPos = { 5.0f, 35.0f };
    pge->DrawStringDecal(str_positionPos, sPlayerPos);
    pge->DrawStringDecal(str_velocityPos, sPlayerVel);
    pge->DrawStringDecal(str_statePos, sPlayerState);
    pge->DrawStringDecal(str_sprintPos, sPlayerSprint);
}
