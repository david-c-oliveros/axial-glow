#include <iostream>
#include <vector>
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "World.h"
#include "Entity.h"
#include "Player.h"
#include "Counter.h"
#include "MathUtils.h"
#include "Loot.h"


struct posvel_t {
    olc::vf2d pos;
    olc::vf2d vel;
};


class Catacombs : public olc::PixelGameEngine
{
    public:
        Catacombs()
        {
            sAppName = "Catacombs";
        }

    private:
        olc::TileTransformedView tv;

        Player cPlayer = Player({ 1.5f, 1.5f });
        int iGameTick;
        bool bDebug = false;

        World cWorld = World();
        std::vector<std::unique_ptr<Entity>> vEntities;
        Counter cTickCounterEntity = Counter(5);

        const olc::vf2d vGravityVec = { 0.0f, 1.0f };


    public:
        bool OnUserCreate() override
        {
            iGameTick = 0;
            tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 32, 32 });

            cWorld.GenerateWorld();
            GenerateEntities();

            cPlayer.OnCreate();
            cTickCounterEntity.Start();

            olc::vf2d spawn = cWorld.FindSpawnableCell();
            cPlayer.SetPos(spawn);

            return true;
        }

        bool OnUserUpdate(float fElapsedTime) override
        {
            if (GetKey(olc::Key::M).bReleased) bDebug = !bDebug;

            MovePlayer(fElapsedTime);
            HandlePanAndZoom();
            UpdateEntities();
            CheckForEntityCollisions();
            Render();
            RenderDebug();

            iGameTick++;

            return true;
        }


        /*************************************************/
        /*                Update Entities                */
        /*************************************************/
        void UpdateEntities()
        {
            cPlayer.Update(iGameTick);
            cTickCounterEntity.Update();
            if (cTickCounterEntity.Check())
            {
                cTickCounterEntity.Reset();
                cTickCounterEntity.Start();
                for (int i = 0; i < vEntities.size(); i++)
                {
                    vEntities[i]->Update();
                }
            }
        }


        /****************************************/
        /*                Render                */
        /****************************************/
        void RenderDebug()
        {
            cPlayer.DrawDebug(this, &tv);
        }


        void Render()
        {
            Clear(olc::VERY_DARK_BLUE);
            cWorld.DrawMap(&tv);
            cPlayer.DrawSelf(&tv);
            cPlayer.DrawStats(this);
            for (int i = 0; i < vEntities.size(); i++)
            {
                vEntities[i]->DrawSelf(&tv);
            }
        }


        void MovePlayer(float fElapsedTime)
        {
            cPlayer.SetState(PLAYER_REST);

            // Player Control
            cPlayer.SetMoveVel({ 0.0f, 0.0f });
            if (GetKey(olc::Key::W).bHeld)
            {
                cPlayer.SetState(PLAYER_WALK_UP_DOWN);
                cPlayer.AddMoveVel({  0.0f, -1.0f });
            }
            if (GetKey(olc::Key::S).bHeld)
            {
                cPlayer.SetState(PLAYER_WALK_UP_DOWN);
                cPlayer.AddMoveVel({  0.0f,  1.0f });
            }
            if (GetKey(olc::Key::A).bHeld)
            {
                cPlayer.SetState(PLAYER_WALK_LEFT);
                cPlayer.AddMoveVel({ -1.0f,  0.0f });
            }
            if (GetKey(olc::Key::D).bHeld)
            {
                cPlayer.SetState(PLAYER_WALK_RIGHT);
                cPlayer.AddMoveVel({  1.0f,  0.0f });
            }
            if (GetKey(olc::Key::SHIFT).bHeld && cPlayer.GetState() != PLAYER_REST)
            {
                cPlayer.bSprint = true;
            }
            else
                cPlayer.bSprint = false;
            // Normalize velocity vector
            if (cPlayer.GetMoveVel().mag2() > 0)
            {
                const int iVelMul = (cPlayer.bSprint ? 7.0f : 4.0f);
                cPlayer.SetMoveVel(cPlayer.GetMoveVel().norm() * iVelMul);

                // TODO - Player stamina drain
            }

            if (GetKey(olc::Key::SPACE).bPressed)
            {
                cPlayer.Jump();
            }

            cPlayer.AddVel(vGravityVec);
            posvel_t pvNewPosVel = ResolveMapCollisions(cPlayer.GetPos(), cPlayer.GetVel(), fElapsedTime);
            cPlayer.SetPos(pvNewPosVel.pos);
            cPlayer.SetVel(pvNewPosVel.vel);
            std::cout << "Pos: " << cPlayer.GetPos() << std::endl;
            std::cout << "Vel: " << cPlayer.GetVel() << std::endl;
        }


        posvel_t ResolveMapCollisions(olc::vf2d vPos, olc::vf2d vVel, float fElapsedTime)
        {
            olc::vf2d vPotentialPos = vPos + (vVel * fElapsedTime);
            olc::vf2d vNewVel = vVel;

            if (vVel.x <= 0)
            {
                if (cWorld.GetTile({ vPotentialPos.x + 0.0f, vPos.y + 0.0f }) != '.' ||
                    cWorld.GetTile({ vPotentialPos.x + 0.0f, vPos.y + 0.9f }) != '.')
                {
                    vPotentialPos.x = (int)vPotentialPos.x + 1;
                    vNewVel.x = 0.0f;
                }
            }
            else
            {
                if (cWorld.GetTile({ vPotentialPos.x + 1.0f, vPos.y + 0.0f }) != '.' ||
                    cWorld.GetTile({ vPotentialPos.x + 1.0f, vPos.y + 0.9f }) != '.')
                {
                    vPotentialPos.x = (int)vPotentialPos.x;
                    vNewVel.x = 0.0f;
                }
            }

            if (vVel.y <= 0)
            {
                if (cWorld.GetTile({ vPotentialPos.x + 0.0f, vPotentialPos.y }) != '.' ||
                    cWorld.GetTile({ vPotentialPos.x + 0.9f, vPotentialPos.y }) != '.')
                {
                    vPotentialPos.y = (int)vPotentialPos.y + 1;
                    vNewVel.y = 0.0f;
                }
            }
            else
            {
                if (cWorld.GetTile({ vPotentialPos.x + 0.0f, vPotentialPos.y + 1.0f }) != '.' ||
                    cWorld.GetTile({ vPotentialPos.x + 0.9f, vPotentialPos.y + 1.0f }) != '.')
                {
                    vPotentialPos.y = (int)vPotentialPos.y;
                    vNewVel.y = 0.0f;
                }
            }

            posvel_t pvResolvedCollision;
            pvResolvedCollision.pos = vPotentialPos;
            pvResolvedCollision.vel = vNewVel;
            return pvResolvedCollision;
        }


        bool CheckBoxCollision(olc::vf2d vPosA, olc::vf2d vDimA, olc::vf2d vPosB, olc::vf2d vDimB)
        {
            return (vPosA.x < vPosB.x + vDimB.x &&
                    vPosA.x + vDimA.x > vPosB.x &&
                    vPosA.y < vPosB.y + vDimB.y &&
                    vDimA.y + vPosA.y > vPosB.y);
        }


        void CheckForEntityCollisions()
        {
            for (int i = vEntities.size() - 1; i >= 0; i--)
            {
                if (MathUtils::DistanceBetweenPoints(cPlayer.GetPos(), vEntities[i]->GetPos()) < 0.7f)
                {
                    cPlayer.AddCoin(vEntities[i]->GetValue());
                    vEntities.erase(vEntities.begin() + i);
                }
            }
        }


        void HandlePanAndZoom()
        {
            if (!bDebug)
            {
                tv.SetWorldOffset(cPlayer.GetPos() - tv.ScaleToWorld(olc::vf2d(ScreenWidth() / 2.0f,
                                ScreenHeight() / 2.0f)));
                return;
            }

            if (GetMouse(2).bPressed) tv.StartPan(GetMousePos());
            if (GetMouse(2).bHeld) tv.UpdatePan(GetMousePos());
            if (GetMouse(2).bReleased) tv.EndPan(GetMousePos());

            if (GetMouseWheel() > 0) tv.ZoomAtScreenPos(2.0f, GetMousePos());
            if (GetMouseWheel() < 0) tv.ZoomAtScreenPos(0.5f, GetMousePos());
        }


        void GenerateEntities()
        {
            for(int i = 0; i < 20; i++)
            {
                olc::vf2d vec = cWorld.FindRandomOpenSpot();
                vec = { vec.x + 0.5f, vec.y + 0.5f };
                std::unique_ptr<Loot> loot = std::make_unique<Loot>(vec);
                loot->OnCreate();
                vEntities.push_back(std::move(loot));
            }
        }
};



int main()
{
    Catacombs game;
    if (game.Construct(640, 480, 2, 2, false, true))
        game.Start();

    return 0;
}
