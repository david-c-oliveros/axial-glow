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


float ReMap(float x, float fromMin, float fromMax, float toMin, float toMax);
float DistanceBetweenPoints(olc::vf2d vecA, olc::vf2d vecB);


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

        World cWorld = World();
        std::vector<std::unique_ptr<Entity>> vEntities;
        Counter cTickCounterEntity = Counter(5);

        bool bDebug = false;

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
            if (GetKey(olc::Key::SPACE).bReleased) bDebug = !bDebug;

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
            cPlayer.SetVel({ 0.0f, 0.0f });
            if (GetKey(olc::Key::W).bHeld)
            {
                cPlayer.SetState(PLAYER_WALK_UP_DOWN);
                cPlayer.AddVel({  0.0f, -1.0f });
            }
            if (GetKey(olc::Key::S).bHeld)
            {
                cPlayer.SetState(PLAYER_WALK_UP_DOWN);
                cPlayer.AddVel({  0.0f,  1.0f });
            }
            if (GetKey(olc::Key::A).bHeld)
            {
                cPlayer.SetState(PLAYER_WALK_LEFT);
                cPlayer.AddVel({ -1.0f,  0.0f });
            }
            if (GetKey(olc::Key::D).bHeld)
            {
                cPlayer.SetState(PLAYER_WALK_RIGHT);
                cPlayer.AddVel({  1.0f,  0.0f });
            }
            if (GetKey(olc::Key::SHIFT).bHeld && cPlayer.State() != PLAYER_REST)
            {
                cPlayer.m_bSprint = true;
            }
            else
                cPlayer.m_bSprint = false;

            // Normalize velocity vector
            if (cPlayer.GetVel().mag2() > 0)
            {
                const int iVelMul = (cPlayer.m_bSprint ? 7.0f : 4.0f);
                cPlayer.SetVel(cPlayer.GetVel().norm() * iVelMul);

                // TODO - Player stamina drain
            }
            olc::vf2d vPotentialPosition = cPlayer.GetPos() + (cPlayer.GetVel() * fElapsedTime);

            // Extract potential collision region
            olc::vi2d vCurrentCell = cPlayer.GetPos().floor();
            olc::vi2d vTargetCell = vPotentialPosition;
            olc::vi2d vAreaTL = (vCurrentCell.min(vTargetCell) - olc::vi2d(1, 1)).max({ 0, 0});
            olc::vi2d vAreaBR = (vCurrentCell.max(vTargetCell) + olc::vi2d(1, 1)).min(cWorld.GetSize());

            // Iterate through each cell in potential collision region
            olc::vi2d vCell;
            for (vCell.y = vAreaTL.y; vCell.y <= vAreaBR.y; vCell.y++)
            {
                for (vCell.x = vAreaTL.x; vCell.x <= vAreaBR.x; vCell.x++)
                {
                    if (cWorld.sMap[vCell.y * cWorld.GetSize().x + vCell.x] == '#')
                    {
                        // If the region contains a solid cell,
                        // find the nearest point on that cell to the circle
                        olc::vf2d vNearestPoint;
                        vNearestPoint.x = std::max(float(vCell.x), std::min(vPotentialPosition.x,
                                    float(vCell.x + 1)));
                        vNearestPoint.y = std::max(float(vCell.y), std::min(vPotentialPosition.y,
                                    float(vCell.y + 1)));

                        // Cast ray from center of potential circle position to nearest point on solid cell
                        olc::vf2d vRayToNearest = vNearestPoint - vPotentialPosition;
                        // Calculate the overlap between the circle and the solid cell
 
                        // TODO - FIX BUG HERE - If player is inside a solid square,
                        // then vRayToNearest.mag() returns {nan, nan}
//                        if (vRayToNearest.x != 0.0f && vRayToNearest.y != 0.0f )
                        float fOverlap = cPlayer.fColliderRadius - vRayToNearest.mag();

                        // Handle divide-by-zero edge case
                        if  (std::isnan(fOverlap)) fOverlap = 0;

                        // If an overlap exists, resolve it
                        if (fOverlap > 0)
                        {
                            vPotentialPosition = vPotentialPosition - vRayToNearest.norm() * fOverlap;
                        }
                    }
                }
            }

            cPlayer.SetPos(vPotentialPosition);
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
