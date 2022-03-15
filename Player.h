#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "Entity.h"

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
        int m_iSpriteStartFrame = 0;
        int m_iSpriteEndFrame = 5;
        int m_iSpriteCurrentCol;
        int m_iSpriteCurrentRow;
        int m_iSpriteDir;
        int m_iPlayerState;
        int m_iAnimInterval;

        olc::vf2d m_vMoveVel;

        int m_iCoin;

    public:
        bool m_bSprint = false;

    public:
        Player(olc::vf2d vPos);
        ~Player();

        void OnCreate();
        bool Update(int iGameTick);
        int GetState();
        void SetState(int iState);
        void DrawSelf(olc::TileTransformedView* tv) override;
        void DrawDebug(olc::PixelGameEngine* pge);
        void DrawStats(olc::PixelGameEngine* pge);

        void AddCoin(int iCoin);
        olc::vf2d GetVel();
        olc::vf2d GetMoveVel();
        void SetMoveVel(olc::vf2d vVel);
        void AddMoveVel(olc::vf2d vVel);
        void Jump();
};
