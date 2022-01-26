#include <iostream>
#include <vector>
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"


class Catacombs : public olc::PixelGameEngine
{
    public:
        Catacombs()
        {
            sAppName = "Catacombs";
        }

    private:
        olc::TileTransformedView tv;

        struct sWorldObject
        {
            olc::vf2d vPos;
            olc::vf2d vVel;
            float fRadius = 0.5f;
        };

        struct sPlayer : public sWorldObject
        {
            olc::Sprite* pPlayerSprite = nullptr;
            olc::Decal* pPlayerDecal = nullptr;

            olc::vf2d vSpriteSize = { 16.0f, 22.0f };

            std::vector<olc::vf2d> vSpriteStartPos;

            olc::vf2d vSpriteCurrentPos;
            const int iSpriteStartFrameWalking = 0;
            const int iSpriteEndFrameWalking = 5;
            int iSpriteCurrentCol;
            int iSpriteCurrentRow;
        };



        sPlayer sHero;
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
            sHero.iSpriteCurrentCol = 0;
            sHero.iSpriteCurrentRow = 1;
            sHero.vSpriteCurrentPos = sHero.vSpriteStartPos[1];
            sHero.vPos = { 1.5f, 1.5f };
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
            if (iGameTick >= 5)
            {
                if (++sHero.iSpriteCurrentCol > sHero.iSpriteEndFrameWalking) sHero.iSpriteCurrentCol = 0;
                iGameTick = 0;
            }

            sHero.vSpriteCurrentPos = { sHero.vSpriteStartPos[1].x + sHero.iSpriteCurrentCol * sHero.vSpriteSize.x,
                                        sHero.vSpriteStartPos[1].y };
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
//            tv.DrawCircle(sHero.vPos, 0.5f, olc::WHITE);
            tv.DrawPartialDecal({ sHero.vPos.x  - 0.5f, sHero.vPos.y - 0.8f }, sHero.pPlayerDecal,
                    sHero.vSpriteCurrentPos, sHero.vSpriteSize, { 2.0f, 2.0f });
        }


        void MovePlayer(float fElapsedTime)
        {

            // Player Control
            sHero.vVel = { 0.0f, 0.0f };
            if (GetKey(olc::Key::W).bHeld) sHero.vVel += {  0.0f, -1.0f };
            if (GetKey(olc::Key::S).bHeld) sHero.vVel += {  0.0f,  1.0f };
            if (GetKey(olc::Key::A).bHeld) sHero.vVel += { -1.0f,  0.0f };
            if (GetKey(olc::Key::D).bHeld) sHero.vVel += {  1.0f,  0.0f };

            // Normalize velocity vector
            if (sHero.vVel.mag2() > 0)
            {
                sHero.vVel = sHero.vVel.norm() * (GetKey(olc::Key::SHIFT).bHeld ? 7.0f : 4.0f);

                // Todo - Player stamina drain
            }
            olc::vf2d vPotentialPosition = sHero.vPos + (sHero.vVel * fElapsedTime);

            // Extract potential collision region
            olc::vi2d vCurrentCell = sHero.vPos.floor();
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
                        float fOverlap = sHero.fRadius - vRayToNearest.mag();

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

            sHero.vPos = vPotentialPosition;
        }

        void HandlePanAndZoom()
        {
            if (!bDebug)
            {
                tv.SetWorldOffset(sHero.vPos - tv.ScaleToWorld(olc::vf2d(ScreenWidth() / 2.0f,
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
