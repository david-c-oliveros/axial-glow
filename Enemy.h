#include "Entity.h"
#include "MathUtils.h"


class Enemy : public Entity
{
    private:
        int m_iHealthPoints;
        int m_iAnimFrame;
        int m_iAnimLen;
        float m_fRadius;
        bool m_bIncrease;

    public:
        Enemy(olc::vf2d vPos);
        ~Enemy();

        void OnCreate();
        void Update();
        void DrawSelf(olc::TileTransformedView* tv);
};
