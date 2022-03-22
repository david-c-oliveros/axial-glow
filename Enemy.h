#include "Entity.h"
#include "MathUtils.h"


#define REST 0
#define ATTACKING 1


class Enemy : public Entity
{
    private:
        olc::vf2d m_vOrigin;
        int m_iHealthPoints;
        int m_iAnimFrame;
        int m_iAnimLen;
        float m_fRadius;
        bool m_bIncrease;
        int m_iState;
        olc::Pixel m_pColor;
        int m_iDir;
        float m_fIdleMoveRange;
        float debug_offsetFromOrigin;
        olc::vf2d debug_vecToPlayer;

    public:
        Enemy(olc::vf2d vPos, int bStartDir);
        ~Enemy();

        void OnCreate();
        void Update(olc::vf2d vPlayerPos) override;
        void Move(olc::vf2d vTarget);
        olc::vf2d GetVecToObj(olc::vf2d vObjPos);
        void DrawSelf(olc::TileTransformedView* tv);
        void DrawDebug(olc::PixelGameEngine* pge, olc::TileTransformedView* tv, int iIndex);
};
