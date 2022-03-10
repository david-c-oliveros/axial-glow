#include <iostream>
#include <vector>
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "World.h"
#include "Entity.h"
#include "Player.h"


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

        olc::vi2d vWorldSize = { 32, 32 };
        bool bDebug = false;

    public:
        bool OnUserCreate() override
        {
            std::cout << "Map size: " << cWorld.sMap.length() << std::endl;
            std::cout << cWorld.sMap << std::endl;
            iGameTick = 0;
            tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 32, 32 });

            cWorld.GenerateWorld();
            cPlayer.OnCreate();
            olc::vf2d spawn = cWorld.FindSpawnableCell();
            std::cout << "Player spawn: " << spawn << std::endl;
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
            if (cPlayer.Update(iGameTick)) iGameTick = 0;
        }


        /****************************************/
        /*                Render                */
        /****************************************/
        void RenderDebug()
        {
            std::string sPlayerPos = "Position: " + std::to_string(cPlayer.GetPos().x) + ", " + std::to_string(cPlayer.GetPos().y);
            std::string sPlayerVel = "Velocity: " + std::to_string(cPlayer.GetVel().x) + ", " + std::to_string(cPlayer.GetVel().y);
            olc::vf2d str_vPos = { 5.0f, 5.0f };
            olc::vf2d str_pPos = { 5.0f, 15.0f };
            DrawStringDecal(str_pPos, sPlayerPos);
            DrawStringDecal(str_vPos, sPlayerVel);
        }


        void Render()
        {

            // Clear World
            Clear(olc::VERY_DARK_BLUE);

            // Draw World
            olc::vi2d vTopLeft = tv.GetTopLeftTile().max({ 0, 0 });
            olc::vi2d vBottomRight = tv.GetBottomRightTile().min(vWorldSize);
            olc::vi2d vTile;

            int count = 0;
            for (vTile.y = vTopLeft.y; vTile.y < vBottomRight.y; vTile.y++)
            {
                for (vTile.x = vTopLeft.x; vTile.x < vBottomRight.x; vTile.x++)
                {
                    int index = vTile.y * vWorldSize.x + vTile.x;
                    if (cWorld.sMap[index] == '#')
                    {
                        // TODO - Replace rectangles with sprites (decals)
                        tv.DrawRect(vTile, { 1.0f, 1.0f }, olc::WHITE);
                    }
                }
            }

            // Draw Player
            cPlayer.DrawSelf(&tv);
        }


        void MovePlayer(float fElapsedTime)
        {
            cPlayer.SetState(PLAYER_REST);

            // Player Control
            cPlayer.SetVel({ 0.0f, 0.0f });
            if (GetKey(olc::Key::W).bHeld)
            {
                cPlayer.AddVel({  0.0f, -1.0f });
            }
            if (GetKey(olc::Key::S).bHeld)
            {
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
            olc::vi2d vAreaBR = (vCurrentCell.max(vTargetCell) + olc::vi2d(1, 1)).min(vWorldSize);

            // Iterate through each cell in potential collision region
            olc::vi2d vCell;
            for (vCell.y = vAreaTL.y; vCell.y <= vAreaBR.y; vCell.y++)
            {
                for (vCell.x = vAreaTL.x; vCell.x <= vAreaBR.x; vCell.x++)
                {
                    if (cWorld.sMap[vCell.y * vWorldSize.x + vCell.x] == '#')
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
};



int main()
{
    Catacombs game;
    if (game.Construct(640, 480, 2, 2, false, true))
        game.Start();

    return 0;
}
