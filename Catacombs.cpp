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

        sWorldObject player;

        // Reachgoal - Procedural World Generation
        std::string sWorldMap =
            "################################"
            "#......#########################"
            "######.#########################"
            "######.#########################"
            "######..................########"
            "######.################.########"
            "######.################.########"
            "######.#######........#.########"
            "######.#######.######.#.########"
            "######.#######.#......#.########"
            "######.#######.#......#.########"
            "######.#######.########.########"
            "######.#######..........########"
            "######.#########################"
            "######.#########################"
            "######.#########################"
            "######.#########################"
            "######.........................#"
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
        bool bFollowPlayer = false;

    public:
        bool OnUserCreate() override
        {
            tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 32, 32});
            player.vPos = { 1.0f, 1.0f };

            return true;
        }

        bool OnUserUpdate(float fElapsedTime) override
        {
            if (GetKey(olc::Key::SPACE).bReleased) bFollowPlayer = !bFollowPlayer;

            // Player Control
            player.vVel = { 0.0f, 0.0f };
            if (GetKey(olc::Key::W).bHeld) player.vVel += {  0.0f, -1.0f };
            if (GetKey(olc::Key::S).bHeld) player.vVel += {  0.0f,  1.0f };
            if (GetKey(olc::Key::A).bHeld) player.vVel += { -1.0f,  0.0f };
            if (GetKey(olc::Key::D).bHeld) player.vVel += {  1.0f,  0.0f };

            // Normalize velocity vector
            if (player.vVel.mag2() > 0)
            {
                player.vVel = player.vVel.norm() * (GetKey(olc::Key::SHIFT).bHeld ? 7.0f : 4.0f);

                // Todo - Player stamina drain
            }


            /***************************/
            /*        RENDERING        */
            /***************************/

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
            tv.DrawCircle(player.vPos, player.fRadius, olc::WHITE);

            return true;
        }
};



int main()
{
    Catacombs game;
    if (game.Construct(640, 480, 2, 2, false, true))
        game.Start();

    return 0;
}
