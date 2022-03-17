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

            cWorld.PrintWorld();

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
            cPlayer.DrawDebug(this);
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
//            if (GetKey(olc::Key::W).bHeld)
//            {
//                cPlayer.SetState(PLAYER_WALK_UP_DOWN);
//                cPlayer.AddMoveVel({  0.0f, -1.0f });
//            }
//            if (GetKey(olc::Key::S).bHeld)
//            {
//                cPlayer.SetState(PLAYER_WALK_UP_DOWN);
//                cPlayer.AddMoveVel({  0.0f,  1.0f });
//            }
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
                cPlayer.m_bSprint = true;
            }
            else
                cPlayer.m_bSprint = false;
            // Normalize velocity vector
//            if (cPlayer.GetMoveVel().mag2() > 0)
//            {
//                const int iVelMul = (cPlayer.m_bSprint ? 7.0f : 4.0f);
//                cPlayer.SetMoveVel(cPlayer.GetMoveVel().norm() * iVelMul);
//
//                // TODO - Player stamina drain
//            }

            const int iVelMul = (cPlayer.m_bSprint ? 16.0f : 8.0f);
            cPlayer.SetMoveVel(cPlayer.GetMoveVel() * iVelMul);

            if (GetKey(olc::Key::SPACE).bPressed)
            {
                cPlayer.Jump();
            }

            cPlayer.AddVel(vGravityVec);
            olc::vf2d newPos = CheckForMapCollisions(&cPlayer, fElapsedTime);
            cPlayer.SetPos(newPos);
        }


        olc::vf2d CheckForMapCollisions(Entity* entity, float fElapsedTime)
        {
            olc::vf2d vPotentialPosition = entity->GetPos() + (entity->GetVel() * fElapsedTime);

            // Extract potential collision region
            olc::vi2d vCurrentCell = entity->GetPos().floor();
            olc::vi2d vTargetCell = vPotentialPosition;
            olc::vi2d vAreaTL = (vCurrentCell.min(vTargetCell) - olc::vi2d(1, 1)).max({ 0, 0});
            olc::vi2d vAreaBR = (vCurrentCell.max(vTargetCell) + olc::vi2d(1, 1)).min(cWorld.GetSize());

            // Iterate through each cell in potential collision region
            olc::vi2d vCell;
            for (vCell.y = vAreaTL.y; vCell.y <= vAreaBR.y; vCell.y++)
            {
                for (vCell.x = vAreaTL.x; vCell.x <= vAreaBR.x; vCell.x++)
                {
                    int index = vCell.y * cWorld.GetSize().x + vCell.x;

                    // Account for edge case where index evaluates to out side of map
                    if (index >= cWorld.sMap.length())
                        index -= cWorld.GetSize().x;

                    if (cWorld.sMap[index] == '#')
                    {
                        if (CheckBoxCollision(vPotentialPosition, entity->GetBoxCollider(), vCell, { 1.0f, 1.0f }))
                        {
                            // TODO - Fix collisions in x-axis
                            vPotentialPosition = { vPotentialPosition.x, entity->GetPos().y };
                            entity->SetVel({ 0.0f, 0.0f });
                        }
                    }
                }
            }
            return vPotentialPosition;
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
