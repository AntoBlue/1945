#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAP_TILE_SIZE 32 //specify tile size 
#define BULLET_LIFETIME 3 //bullet are deleted after this amount of time
#define MAX_PLAYER_BULLETS 10 //max amount of bullets on screen
#define MAX_ENEMIES 10 //max enemies on screen
#define MAX_ENEMY_BULLETS 10 // max enemy bullets on screen

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
        Rectangle pCollider;
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
        Rectangle collider;
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
        Rectangle collider;
        int enemySpeed;
        int framesSpeed; //how many frames per second?
        int enemyCurrentFrame;
        int enemyframesCounter;
        bool shootAvaible;
    } Enemy;

    
//------------------------------------------------------------------------------------
// Define Enemy Bullets
//------------------------------------------------------------------------------------
    typedef struct EnemyBullet 
    {
        Texture2D enemyBulletTex;
        Vector2 enemyBulletPos;
        Rectangle enemyBulletFrameRec;
        Rectangle collider;
        bool isActive;
    } EnemyBullet;

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
    Rectangle pCollider = { planePos.x, planePos.y, (float)planeTex.width/3, (float)planeTex.height };


    //sprite anim frames
    int currentFrame;
    int framesCounter;
    int framesSpeed = 20; //how many frames per second?

    //movement speed
    int playerSpeed = 5;

    ////PLAYER BULLETS////

    //sprite
    Texture2D pBulTex = LoadTexture("resources/player/bullet.png");

    //init
    pBullet p_bullet[MAX_PLAYER_BULLETS];
    
    //attributes
    float pBulletSpeed = 7;
    int shootCoolDownTimer = 60;
    
    Vector2 pBulPos = {-50, -50};
    Rectangle pBulRect = { 0.0f, 0.0f, (float)pBulTex.width, (float)pBulTex.height};
    int pbulcount = 0;
    int bulletSideId = 0;
    

    //bullet array
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
    {
        p_bullet[i].pBulRect = pBulRect;
        p_bullet[i].pBulPos = pBulPos;
        p_bullet[i].collider = pBulRect;
    }

    ////ENEMY PLANES////


    //sprite
    Texture2D enemyTex = LoadTexture("resources/enemy/enemy1_strip3.png");

    //init enemies

    float enemySpeed = 3;
    Rectangle enemyFrameRec = { 0.0f, 0.0f, (float)enemyTex.width/3, (float)enemyTex.height };
    bool shootAvaible = true;
    
    //sprite anim frames
    int enemyCurrentFrame;
    int enemyFramesCounter;
    //int eFramesSpeed = 20; //how many frames per second?

    //enemy array

    Enemy enemy[MAX_ENEMIES];

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemy[i].enemyFrameRec = enemyFrameRec;
        enemy[i].enemyPos.x = (float)(GetRandomValue(0, screenWidth - 30));
        enemy[i].enemyPos.y = (float)(GetRandomValue(-1000, -30));
        enemy[i].collider = enemyFrameRec;
        enemy[i].shootAvaible = true;
    }

    ////ENEMY BULLETS////

    EnemyBullet e_bullet[MAX_ENEMY_BULLETS];
    
    Vector2 enemyBulletPos = {screenHeight + 50, 0};

    //sprite
    Texture2D enemyBulletTex = LoadTexture("resources/enemy/enemybullet1.png");
    Rectangle enemyBulRec = { 0.0f, 0.0f, (float)enemyBulletTex.width, (float)enemyBulletTex.height };
    enemyBulletPos.y = screenHeight + 30;

    //enemy bullet array
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        e_bullet[i].enemyBulletFrameRec = enemyBulRec;
        e_bullet[i].enemyBulletPos = enemyBulletPos;
        e_bullet[i].collider = enemyBulRec;
        //e_bullet[i].isActive = false;
    }

    //attributes
    float enemyBulletSpeed = 5;
    int enemyShootTimer = 0;



    ////HUD
    //hud bar
    Texture2D hudBar = LoadTexture("resources/ui/bottom.png");

    //hud info
    int score;
    float life = 100;
    Rectangle lifeBar = {0, 0, life, 50};

    ////LEVEL
    //map
    Map map = {0};
    map.tilesX = 32;
    map.tilesY = 32;

    //sea tiles
    Texture seaTiles = LoadTexture("resources/map/water.png");

    //scroll
    float scrollingSpeed = 10;

    ////debug hitbox
    bool draw = false;

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
        enemyShootTimer ++;

        if (framesCounter >= (60/framesSpeed))
        {
            framesCounter = 0;
            currentFrame++;
            enemyCurrentFrame++;

            if (currentFrame > 5) currentFrame = 0;
            if (enemyCurrentFrame > 5) enemyCurrentFrame = 0;


            planeFrameRec.x = (float)currentFrame*(float)planeTex.width/3;
            enemyFrameRec.x = (float)enemyCurrentFrame*(float)enemyTex.width/3;
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
        pCollider.x = planePos.x; pCollider.y = planePos.y;
        ////init player bullets
        
        

        //shoot bullet
        
        for(int i = 0; i < MAX_PLAYER_BULLETS; i++)
        {
            if(IsKeyDown(KEY_SPACE) && shootCoolDownTimer > 5 && p_bullet[i].isActive == false)
            {
                        
                p_bullet[i].pBulPos.y = planePos.y;
                p_bullet[i].pBulPos.x = planePos.x + (float)planeFrameRec.width/3.5f;
                p_bullet[i].isActive = true;
                p_bullet[i].collider.x = p_bullet[i].pBulPos.x;
                shootCoolDownTimer = 0;                   
            }
            
        }

        //update bullet
        
        for(int i = 0; i < MAX_PLAYER_BULLETS;  i++)
        {
            if (p_bullet[i].isActive)
            {
                p_bullet[i].pBulPos.y -= pBulletSpeed;
                p_bullet[i].collider.y = p_bullet[i].pBulPos.y;
            }
        }

        for(int i = 0; i < MAX_PLAYER_BULLETS;  i++)
        {
            if(p_bullet[i].pBulPos.y < -30) 
            {
                p_bullet[i].isActive = false;
                p_bullet[i].collider.y = screenHeight + 900;
            }
        }

        //update enemies

        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            enemy[i].enemyPos.y += enemySpeed;
            enemy[i].collider.y = enemy[i].enemyPos.y;
            enemy[i].collider.x = enemy[i].enemyPos.x;
        }

        //(re)spawn enemies
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if(enemy[i].enemyPos.y > screenHeight + 30)
            {
                enemy[i].enemyPos.x = (float)(GetRandomValue(0, screenWidth - 30)); 
                enemy[i].enemyPos.y = (float)(GetRandomValue(-1000, -30));
                //enemy[i].collider.x = enemy[i].enemyPos.x;
                //enemy[i].collider.y = enemy[i].enemyPos.y;
            }
        }

        ////enemy bullets

        //update enemy bullet
        for(int i = 0; i < MAX_ENEMY_BULLETS;  i++)
        {
            if (e_bullet[i].isActive)
            {
                e_bullet[i].enemyBulletPos.y += enemyBulletSpeed;
                e_bullet[i].collider.y = e_bullet[i].enemyBulletPos.y;
                e_bullet[i].collider.x = e_bullet[i].enemyBulletPos.x;
            }
        }

        for(int i = 0; i < MAX_ENEMY_BULLETS;  i++)
        {
            if(e_bullet[i].enemyBulletPos.y < 0 || e_bullet[i].enemyBulletPos.y > screenHeight) 
            {
                e_bullet[i].isActive = false;
                //e_bullet[i].collider.y = screenHeight + 900;
            }
        }

        //enemy shoots bullets
        
        for(int i = 0; i < MAX_ENEMIES; i++)
        {
            for(int j = 0; j < MAX_ENEMY_BULLETS; j++)
            {
                if(e_bullet[j].isActive == false && enemyShootTimer > 15 && enemy[i].shootAvaible && enemy[i].enemyPos.y > 0 && enemy[i].enemyPos.y < screenHeight)
                {
                    e_bullet[j].isActive = true;
                    e_bullet[j].enemyBulletPos.x = enemy[i].enemyPos.x + (float)enemy[i].enemyFrameRec.width/3;
                    e_bullet[j].enemyBulletPos.y = enemy[i].enemyPos.y;

                    enemy[i].shootAvaible = false;
                    enemyShootTimer = 0;
                        //needed to make bullets come out of ; //different planes (otherwise only one plane can fire)
                    //sleep_ms(90);
                    //enemy[i].shootAvaible = true;
                }

                //enemyShootTimer = GetRandomValue(3, 30);
                
                //WaitTime(5);

            }
        }
        
        //needed to make bullets come out of different planes (otherwise only one plane can fire)
        for(int i = 0; i < MAX_ENEMIES; i++)
        {
            if(enemy[i].enemyPos.y > 0 && enemy[i].enemyPos.y < 5) enemy[i].shootAvaible = true;
        }

        ////collision checks
        
        //bool playerBulletCollision = CheckCollisionRecs(enemyFrameRec, pBulRect);

        for(int i = 0; i < MAX_PLAYER_BULLETS; i++)
        {
            for(int j = 0; j < MAX_ENEMIES; j ++)
            {
                //if(p_bullet[i].pBulRect.x < (enemy[j].enemyFrameRec.x + enemy[j].enemyFrameRec.width) && enemy[j].enemyFrameRec.x < (p_bullet[i].pBulRect.x + p_bullet[i].pBulRect.width) &&
                //p_bullet[i].pBulRect.y < (enemy[j].enemyFrameRec.y + enemy[j].enemyFrameRec.height) && enemy[j].enemyFrameRec.y < (p_bullet[i].pBulRect.y + p_bullet[i].pBulRect.height) && p_bullet[i].isActive)
                if(CheckCollisionRecs(enemy[j].collider, p_bullet[i].collider) && p_bullet[i].isActive)
                {
                    enemy[j].enemyPos.x = (float)(GetRandomValue(0, screenWidth - 30)); 
                    enemy[j].enemyPos.y = (float)(GetRandomValue(-1000, -30));
                    p_bullet[i].isActive = false;
                    score += 100;
                    //Rectangle GetCollisionRec(Rectangle pBulRect, Rectangle enemyFrameRec);
                }
            }
        }

        ////enable debug hitbox
        if(IsKeyPressed(KEY_Z))
        {
            if(draw == false) draw = true;
            else draw = false;
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

            ////draw player bullets
            
            for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
            {
                if(p_bullet[i].isActive)
                {
                    DrawTextureRec(pBulTex, p_bullet[i].pBulRect, p_bullet[i].pBulPos, WHITE );
                    if(draw == true) DrawRectangleLinesEx(p_bullet[i].collider, 3, GREEN);
                }
            }

            //draw enemy bullets

            for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
            {
                if(e_bullet[i].isActive)
                {
                    DrawTextureRec(enemyBulletTex, e_bullet[i].enemyBulletFrameRec, e_bullet[i].enemyBulletPos, WHITE );
                    if(draw == true) DrawRectangleLinesEx(e_bullet[i].collider, 3, RED);
                }
            }
            
            ////draw enemies
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                DrawTextureRec(enemyTex, enemy[i].enemyFrameRec, enemy[i].enemyPos, WHITE);
                if(draw == true) DrawRectangleLinesEx(enemy[i].collider, 3, RED);
            }

            ////draw player
            
            DrawTextureRec(planeTex, planeFrameRec, planePos, WHITE);
            if(draw == true) DrawRectangleLinesEx(pCollider, 3, GREEN);


                          
            //draw hud
            DrawTexture(hudBar, 0, screenHeight - hudBar.height, WHITE);
            DrawText(TextFormat("%03i", score), 230, screenHeight - 67, 20, YELLOW);
            DrawRectangle( 12, screenHeight - 31, (float)life * 1.26f, (float)10.5f, GREEN);

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
//player shoot /done
//animated player sprite /done
//player hurt
//collision
//hud bar /done
//hud life count 
//hud energy
//hud score /done
//enemy spawn /done
//enemy movement /done
//enemy shoot
//enemy hurt
//sea background /done
//scrolling sea background
//islands

//////////////////////
//by Antonio Ranieri//
//////////////////////
