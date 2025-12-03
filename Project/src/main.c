#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAP_TILE_SIZE 32 //specify tile size 
#define BULLET_LIFETIME 3 //bullet are deleted after this amount of time
#define PLAYER_BULLET_MAX_AMOUNT 60 //max amount of bullets on screen
//#define MAX_ENEMIES 5 //max enemies on screen

//------------------------------------------------------------------------------------
// Define the game map
//------------------------------------------------------------------------------------
    typedef struct Map 
    {
        unsigned int tilesX;             
        unsigned int tilesY;            
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
        //float pBulletSpeed;
        Texture2D pBulTex;
        Vector2 pBulPos;
        bool isActive;
    } pBullet;

//------------------------------------------------------------------------------------
// Define Player
//------------------------------------------------------------------------------------
    typedef struct Enemy 
    {
        Texture2D enemyTex;
        Vector2 enemyPos;
        Rectangle enemyFrameRec;
        int enemySpeed;
        int framesSpeed; //how many frames per second?
        int enemyCurrentFrame;
        int enemyframesCounter;
    } Enemy;


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

    ////PLAYER PLANE////

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

    ////PLAYER BULLETS////
    //pBullet *p_bullets = (pBullet *)RL_CALLOC(PLAYER_BULLET_MAX_AMOUNT, sizeof(pBullet)); //p bullet array
    //pBullet p_bullet[PLAYER_BULLET_MAX_AMOUNT] = {0};

    //sprite
    Texture2D pBulTex = LoadTexture("resources/player/bullet.png");

    //init
    pBullet p_bullet[PLAYER_BULLET_MAX_AMOUNT];
    
    //attributes
    float pBulletSpeed = 9;
    int pBulletCount;
    float pBulletLife;
    bool isActive = false;
    int pBulletUnactiveCount;
    int shootCoolDownTimer = 60;
    int shootId;
    
    Vector2 pBulPos = {-50, -50};
    Rectangle pBulRect = { 0.0f, 0.0f, pBulTex.width * 2, pBulTex.height};
    int pbulcount = 0;
    int bulletSideId = 0;

    //bullet array
    for (int i = 0; i < 5; i++)
    {
        p_bullet[i].pBulRect = pBulRect;
        p_bullet[i].pBulPos = pBulPos;
    }

    ////ENEMY PLANES////


    //sprite
    Texture2D enemyTex1 = LoadTexture("resources/enemy/enemy1_strip3.png");
    Texture2D enemyTex2 = LoadTexture("resources/enemy/enemy3_strip3.png");

    //init enemies

    Enemy enemy[6];

    float enemySpeed = 3;
    Rectangle enemyFrameRec = { 0.0f, 0.0f, (float)enemyTex1.width/3, (float)enemyTex1.height };


    //sprite anim frames
    int enemyCurrentFrame;
    int enemyFramesCounter;
    //int eFramesSpeed = 20; //how many frames per second?

    //enemy array

    enemy[0].enemyFrameRec = enemyFrameRec;
    enemy[0].enemyTex = enemyTex1;
    enemy[1].enemyFrameRec = enemyFrameRec;
    enemy[1].enemyTex = enemyTex2;
    enemy[2].enemyFrameRec = enemyFrameRec;
    enemy[2].enemyTex = enemyTex1;
    enemy[3].enemyFrameRec = enemyFrameRec;
    enemy[3].enemyTex = enemyTex2;
    enemy[4].enemyFrameRec = enemyFrameRec;
    enemy[4].enemyTex = enemyTex1;
    enemy[5].enemyFrameRec = enemyFrameRec;
    enemy[5].enemyTex = enemyTex2;


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
        shootCoolDownTimer ++;

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
        Player player;

        ////move player
        if (IsKeyDown(KEY_RIGHT) && planePos.x < (screenWidth - planeTex.width/3) ) planePos.x += playerSpeed;
        if (IsKeyDown(KEY_LEFT) && planePos.x > 0) planePos.x -= playerSpeed;
        if (IsKeyDown(KEY_UP) && planePos.y > 0) planePos.y -= playerSpeed;
        if (IsKeyDown(KEY_DOWN) && planePos.y < (screenHeight - hudBar.height) - planeTex.height) planePos.y += playerSpeed;

        ////init player bullets
        
        

        //shoot bullet
        
        for(int i = 0; i < PLAYER_BULLET_MAX_AMOUNT; i++)
        {
            if(IsKeyDown(KEY_SPACE) && shootCoolDownTimer > 10 && p_bullet[i].isActive == false)
            {
                        
                p_bullet[i].pBulPos.y = planePos.y;
                p_bullet[i].pBulPos.x = planePos.x;
                p_bullet[i].isActive = true;
                shootCoolDownTimer = 0;
                printf("bullet");
                   
            }
            
        }

        //update bullet
        
        for(int i = 0; i < PLAYER_BULLET_MAX_AMOUNT;  i++)
        {
            if (p_bullet[i].isActive)
            {
                p_bullet[i].pBulPos.y -= pBulletSpeed;
            }
        }

        for(int i = 0; i < PLAYER_BULLET_MAX_AMOUNT;  i++)
        {
            if(p_bullet[i].pBulPos.y < -30) p_bullet[i].isActive = false;
        }
        
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
            
            

            //draw player bullets
            
            for (int i = 0; i < PLAYER_BULLET_MAX_AMOUNT; i++)
            {
                DrawTextureRec(pBulTex, p_bullet[i].pBulRect, p_bullet[i].pBulPos, WHITE );
            }

            ////draw player
            
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

//////////////////////
//by Antonio Ranieri//
//////////////////////
