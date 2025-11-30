#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAP_TILE_SIZE 32 //specify tile size 
#define BULLET_LIFETIME 3 //bullet are deleted after this amount of time
#define PLAYER_BULLET_MAX_AMOUNT 30 //max amount of bullets on screen
#define MAX_ENEMIES 5 //max enemies on screen

//------------------------------------------------------------------------------------
// Define the game map
//------------------------------------------------------------------------------------
    typedef struct Map 
    {
        unsigned int tilesX;             
        unsigned int tilesY;            
        unsigned char *tileIds;         // Tile ids (tilesX*tilesY), defines type of tile to draw
    } Map;

//------------------------------------------------------------------------------------
// Define Player
//------------------------------------------------------------------------------------
    typedef struct Player 
    {
        Texture2D planeTex;
        Vector2 planePos;
        Rectangle planeFrameRec;
        int playerSpeed;
        int framesSpeed; //how many frames per second?
        int currentFrame;
        int framesCounter;
    } Player;

//------------------------------------------------------------------------------------
// Define Player Bullets
//------------------------------------------------------------------------------------
    typedef struct pBullet 
    {
        Rectangle pBulRect;
        int pBulletSpeed;
        Texture2D pBulTex;
        Vector2 pBulPos;
        bool active;
    } pBullet;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    const int screenWidth = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "1945");
    SetTargetFPS(60);

    const char* app_dir = GetApplicationDirectory();
    TraceLog(LOG_INFO, "Bin directory: %s", app_dir);
    ChangeDirectory(app_dir);

    ////PLAYER PLANE

    //Player player (Texture2D planeTex, Vector2 planePos, Rectangle planeFrameRec, int playerSpeed,
    //        int frameSpeed, int currentFrame, int framesCounter);
    Player player;

    //sprite
    Texture2D planeTex = LoadTexture("resources/player/myplane_strip3.png");
    Rectangle planeFrameRec = { 0.0f, 0.0f, (float)planeTex.width/3, (float)planeTex.height };
    Vector2 planePos = {screenWidth/2 - planeFrameRec.width/2, screenHeight/2 - planeFrameRec.height/2};

    //sprite anim frames
    int currentFrame;
    int framesCounter;
    int framesSpeed = 20; //how many frames per second?

    //movement speed
    int playerSpeed = 5;

    ////PLAYER BULLETS
    Texture2D pBullet = LoadTexture("resources/player/bullet.png");
    float pBulletSpeed = 15;
    int pBulletCount;
    float pBulletLife;
    bool active = false;
    int enemySpeed = 2;

    //sprite
    Texture2D enemyTex1 = LoadTexture("resources/enemy/enemy1_strip3.png");
    Texture2D enemyTex2 = LoadTexture("resources/enemy/enemy2_strip3.png");
    Texture2D enemyTex3 = LoadTexture("resources/enemy/enemy3_strip3.png");
    Texture2D enemyTex = LoadTexture("resources/enemy/enemy1_strip3.png");

    //sprite anim frames
    int eCurrentFrame;
    int eFramesCounter;
    int eFramesSpeed = 20; //how many frames per second?



    ////HUD
    //hud bar
    Texture2D hudBar = LoadTexture("resources/ui/bottom.png");

    //hud info
    int score;

    ////LEVEL
    //map
    Map map = {0};
    map.tilesX = 32;
    map.tilesY = 32;

    //sea tiles
    Texture seaTiles = LoadTexture("resources/map/water.png");

    //scroll
    float scrollingSpeed = 10;

    //Font my_font = LoadFont("....ttf");

    InitAudioDevice();

    Sound sound = LoadSound("path to sound");
    
    Music music = LoadMusicStream("path to music");
    music.looping = true;
    PlayMusicStream(music);
    
    char title[100];
    while (!WindowShouldClose()) 
    {

        framesCounter++;

        if (framesCounter >= (60/framesSpeed))
        {
            framesCounter = 0;
            currentFrame++;

            if (currentFrame > 5) currentFrame = 0;

            planeFrameRec.x = (float)currentFrame*(float)planeTex.width/3;
            //enemyFrameRec.x = (float)currentFrame*(float)enemyTex.width/3;
        }

        float delta_time = GetFrameTime();
        int fps = GetFPS();
        sprintf(title, "Delta: %.4f - FPS: %d", delta_time, fps);
        SetWindowTitle(title);

        static float time = 0; // GetTime()
        time += delta_time;
        unsigned char r = (unsigned char) ((sin(time) * 0.5f + 0.5f) * 255);

        Color clear = (Color){r, 0, 0, 255};

        Vector2 mouse_pos = GetMousePosition();

        Color color = MAGENTA;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            PlaySound(sound);
            color = GREEN;
        }

        if (IsMusicStreamPlaying(music)) {
            UpdateMusicStream(music);
        }

        // PauseMusicStream(music);
        // ResumeMusicStream(music);

        //scrollingSpeed = 20.0f;

        ////init player
        // Player player (Texture2D planeTex, Vector2 planePos, Rectangle planeFrameRec, int playerSpeed,
        //     int frameSpeed, int currentFrame, int framesCounter);
        Player player;

        ////move player
        if (IsKeyDown(KEY_RIGHT) && planePos.x < (screenWidth - planeTex.width/3) ) planePos.x += playerSpeed;
        if (IsKeyDown(KEY_LEFT) && planePos.x > 0) planePos.x -= playerSpeed;
        if (IsKeyDown(KEY_UP) && planePos.y > 0) planePos.y -= playerSpeed;
        if (IsKeyDown(KEY_DOWN) && planePos.y < (screenHeight - hudBar.height) - planeTex.height) planePos.y += playerSpeed;

        //move player bullets

        ////DRAW
        BeginDrawing();

            //ClearBackground(clear);
            ClearBackground(BLACK);

            //tile map
            for (unsigned int i = 0; i < MAP_TILE_SIZE; i++)
            {
                for (unsigned int j = 0; j < MAP_TILE_SIZE; j++)
                {
                    DrawTexture(seaTiles, map.tilesX * i, map.tilesY * j, WHITE);
                    //DrawTextureEx(seaTiles, (Vector2){ scrollingSpeed, 20 }, 0.0f, 2.0f, WHITE);
                }
            }

            ////draw player
            //DrawTexture(planeTex, screenWidth/2 - planeTex.width/2, screenHeight/2 - planeTex.height/2, WHITE);
            DrawTextureRec(planeTex, planeFrameRec, planePos, WHITE);
                          
            //draw hud
            DrawTexture(hudBar, 0, screenHeight - hudBar.height, WHITE);
            DrawText(TextFormat("%03i", score), 230, screenHeight - 67, 20, YELLOW);

            //DrawTexture(pBullet, planePos.y, planePos.x, WHITE);
            


        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    UnloadTexture(planeTex);
    //UnloadFont(my_font);

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


/////CHECKLIST
//player sprite /done
//player movement with input /done
//player movement bounds /done
//player shoot
//animated player sprite /done
//player hurt
//hud bar /done
//hud life count 
//hud energy
//hud score /done
//enemy spawn
//enemy movement
//enemy shoot
//sea background /done
//scrolling sea background
//islands
