#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "Entity.h"


class Loot : public Entity
{
    private:
        int m_iValue;
        int m_iAnimFrame;
        int m_iAnimLen;
        bool m_bIncrease;
        olc::vf2d m_vSpriteSize;
        olc::vf2d m_vSpritePos;
        std::unique_ptr<olc::Renderable> m_pLootSprite;

    public:
        Loot(olc::vf2d vPos);
        ~Loot();

        void OnCreate();
        void Update();
        void DrawSelf(olc::TileTransformedView* tv);
};
