#include <iostream>
#include <vector>
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
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


        Player sHero;
        int iGameTick;

        // Reachgoal - Procedural World Generation
        std::string sWorldMap =
            "################################"
            "#......#########################"
            "#......#########################"
            "#####..#########################"
            "#####...................########"
            "#####....................#######"
            "#####..################..#######"
            "#####..######.........#..#######"
            "#####..######..######.#..#######"
            "#####..######..#......#..#######"
            "#####..######..#......#..#######"
            "#####..######..########..#######"
            "#####..######............#######"
            "#####..######............#######"
            "#####..#########################"
            "#####..#########################"
            "#####..........................#"
            "#####..........................#"
            "################################"
            "################################"
            "################################"
            "################################"
            "################################"
            "################################"
            "################################"
            "################################"
            "################################"
            "################################"
            "################################"
            "################################"
            "################################"
            "################################"
            "################################";

        olc::vi2d vWorldSize = { 32, 32 };
        bool bDebug = false;

    public:
        bool OnUserCreate() override
        {
            iGameTick = 0;
            tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 32, 32});
            sHero.vSpriteStartPos.push_back({ 11.0f, 10.0f });
            sHero.vSpriteStartPos.push_back({ 11.0f, 41.0f });
            sHero.vSpriteCurrentPos = sHero.vSpriteStartPos[1];
            sHero.SetPos({ 1.5f, 1.5f });
            sHero.pPlayerSprite = new olc::Sprite("./res/sprites/ForestBoy_alpha.png");
            sHero.pPlayerDecal = new olc::Decal(sHero.pPlayerSprite);

            return true;
        }

        bool OnUserUpdate(float fElapsedTime) override
        {
            if (GetKey(olc::Key::SPACE).bReleased) bDebug = !bDebug;

            MovePlayer(fElapsedTime);
            HandlePanAndZoom();
            UpdateEntities();
            Render();

            iGameTick++;

            return true;
        }


        /*************************************************/
        /*                Update Entities                */
        /*************************************************/
        void UpdateEntities()
        {
            if (sHero.Update(iGameTick)) iGameTick = 0;
        }


        /****************************************/
        /*                Render                */
        /****************************************/
        void Render()
        {

            // Clear World
            Clear(olc::BLACK);

            // Draw World
            olc::vi2d vTopLeft = tv.GetTopLeftTile().max({ 0, 0, });
            olc::vi2d vBottomRight = tv.GetBottomRightTile().min(vWorldSize);
            olc::vi2d vTile;

            for (vTile.y = vTopLeft.y; vTile.y < vBottomRight.y; vTile.y++)
            {
                for (vTile.x = vTopLeft.x; vTile.x < vBottomRight.x; vTile.x++)
                {
                    if (sWorldMap[vTile.y * vWorldSize.x + vTile.x] == '#')
                    {
                        // Todo - Replace rectangles with sprites (decals)
                        tv.DrawRect(vTile, { 1.0f, 1.0f }, olc::WHITE);
                    }
                }
            }

            // Draw Player
            sHero.DrawSelf(&tv);
        }


        void MovePlayer(float fElapsedTime)
        {

            // Player Control
            sHero.SetVel({ 0.0f, 0.0f });
            if (GetKey(olc::Key::W).bHeld) sHero.SetVel(sHero.GetVel() + {  0.0f, -1.0f });
            if (GetKey(olc::Key::S).bHeld) sHero.SetVel(sHero.GetVel() + {  0.0f,  1.0f });
            if (GetKey(olc::Key::A).bHeld) sHero.SetVel(sHero.GetVel() + { -1.0f,  0.0f });
            if (GetKey(olc::Key::D).bHeld) sHero.SetVel(sHero.GetVel() + {  1.0f,  0.0f });

            // Normalize velocity vector
            if (sHero.GetVel().mag2() > 0)
            {
                sHero.SetVel(sHero.GetVel().norm() * (GetKey(olc::Key::SHIFT).bHeld ? 7.0f : 4.0f));

                // Todo - Player stamina drain
            }
            olc::vf2d vPotentialPosition = sHero.GetPos() + (sHero.GetVel() * fElapsedTime);

            // Extract potential collision region
            olc::vi2d vCurrentCell = sHero.GetPos().floor();
            olc::vi2d vTargetCell = vPotentialPosition;
            olc::vi2d vAreaTL = (vCurrentCell.min(vTargetCell) - olc::vi2d(1, 1)).max({ 0, 0});
            olc::vi2d vAreaBR = (vCurrentCell.max(vTargetCell) + olc::vi2d(1, 1)).min(vWorldSize);

            // Iterate through each cell in potential collision region
            olc::vi2d vCell;
            for (vCell.y = vAreaTL.y; vCell.y <= vAreaBR.y; vCell.y++)
            {
                for (vCell.x = vAreaTL.x; vCell.x <= vAreaBR.x; vCell.x++)
                {
                    if (sWorldMap[vCell.y * vWorldSize.x + vCell.x] == '#')
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
                        float fOverlap = sHero.fColliderRadius - vRayToNearest.mag();

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

            sHero.SetPos(vPotentialPosition);
        }

        void HandlePanAndZoom()
        {
            if (!bDebug)
            {
                tv.SetWorldOffset(sHero.GetPos() - tv.ScaleToWorld(olc::vf2d(ScreenWidth() / 2.0f,
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
