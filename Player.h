#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "Entity.h"
#include "Counter.h"

#define PLAYER_REST         0
#define PLAYER_WALK_LEFT    1
#define PLAYER_WALK_RIGHT   2
#define PLAYER_WALK_UP_DOWN 3


class Player : public Entity
{
    private:
        std::vector<std::unique_ptr<olc::Renderable>> m_pPlayerSprite;
        std::vector<olc::vf2d> m_vSpriteStartPos;

        olc::vf2d m_vSpriteSize = { 16.0f, 22.0f };
        olc::vf2d m_vSpriteCurrentPos;
        olc::vf2d m_vSpriteTransform;
        olc::vf2d m_vBoxCollider = { 16.0f, 22.0f };
        olc::vi2d m_vColAreaTL;
        olc::vi2d m_vColAreaBR;
        int m_iSpriteStartFrame = 0;
        int m_iSpriteEndFrame = 5;
        int m_iSpriteCurrentCol;
        int m_iSpriteCurrentRow;
        int m_iSpriteDir;
        int m_iPlayerState;
        int m_iAnimInterval;
        Counter m_cAnimCounter;

        olc::vf2d m_vMoveVel;

        int m_iCoin;
        int m_iHealthPoints;

    public:
        bool bSprint = false;
        bool bCollide = false;
        bool bDebug = false;

    public:
        Player(olc::vf2d vPos);
        ~Player();

        void OnCreate();
        void Update();
        int GetState();
        void SetState(int iState);
        void DrawSelf(olc::TileTransformedView* tv);
        void DrawDebug(olc::PixelGameEngine* pge, olc::TileTransformedView* tv);
        void DrawStats(olc::PixelGameEngine* pge);

        void AddCoin(int iCoin);
        olc::vf2d GetVel();
        olc::vf2d GetMoveVel();
        olc::vf2d GetBoxCollider() override;
        void SetVelX(float fX);
        void SetVelY(float fY);
        void SetColArea(olc::vi2d vAreaTL, olc::vi2d vAreaBR);

        void Jump();
        void TakeDamage(int iHP);
        void Heal(int iHP);
};
