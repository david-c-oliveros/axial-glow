#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "Entity.h"


class Player : public Entity
{
    private:
        olc::Sprite* m_pPlayerSprite = nullptr;
        olc::Decal* m_pPlayerDecal = nullptr;

        olc::vf2d m_vSpriteSize;

        olc::vf2d m_vSpriteStartPos = [{ 11.0f, 10.0f },
                                       { 11.0f, 10.0f }];
        olc::vf2d m_vSpriteCurrentPos;
        const int m_iSpriteStartFrameWalking = 0;
        const int m_iSpriteEndFrameWalking = 5;
        int m_iSpriteCurrentCol;
        int m_iSpriteCurrentRow;

    public:
        Player(olc::vf2d vPos);
        ~Player();

        void SetSpriteSize(olc::vf2d vSpriteSize);

        bool Update(int iGameTick);
        void DrawSelf(olc::TileTransformedView* tv);
};
