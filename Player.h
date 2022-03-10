#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "Entity.h"

#define PLAYER_REST         0
#define PLAYER_WALK_LEFT    1
#define PLAYER_WALK_RIGHT   2
#define PLAYER_RUN_LEFT     3
#define PLAYER_RUN_RIGHT    4


class Player : public Entity
{
    private:
        std::vector<std::unique_ptr<olc::Renderable>> m_pPlayerSprite;

        olc::vf2d m_vSpriteSize = { 16.0f, 22.0f };

        std::vector<olc::vf2d> m_vSpriteStartPos;

        olc::vf2d m_vSpriteCurrentPos;
        olc::vf2d m_vSpriteTransform;
        int m_iSpriteStartFrame = 0;
        int m_iSpriteEndFrame = 5;
        int m_iSpriteCurrentCol;
        int m_iSpriteCurrentRow;
        int m_iSpriteDir;

        int m_iPlayerState;
        int m_iAnimInterval;

    public:
        bool m_bSprint = false;

    public:
        Player(olc::vf2d vPos);
        ~Player();

        void OnCreate();

        bool Update(int iGameTick);
        void SetState(int iState);
        void DrawSelf(olc::TileTransformedView* tv);

        int State();
};