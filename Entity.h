#pragma once
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"

class Entity
{
    protected:
        olc::vf2d m_vPos;
        olc::vf2d m_vVel;

    public:

        // TODO - Convert to box collider
        float fColliderRadius = 0.5f;

    public:
        Entity(olc::vf2d vPos);
        ~Entity();

        olc::vf2d GetPos();
        virtual olc::vf2d GetVel();

        void SetPos(olc::vf2d vPos);
        void SetVel(olc::vf2d vVel);
        void AddVel(olc::vf2d vVel);

        virtual void Update() {};
        virtual void DrawSelf(olc::TileTransformedView* tv) = 0;
        virtual int GetValue() {};
};
