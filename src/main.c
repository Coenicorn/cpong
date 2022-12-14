#include <raylib.h>
#include <math.h>
#include <stdio.h>

#define W_WIDTH 600
#define W_HEIGHT 600

#define TARGETFPS 60

#define P_WIDTH 15
#define P_HEIGHT 80

#define B_WIDTH 10
#define B_HEIGHT 10

#define DISTANCE_SIDE 50

#define BALLSPEEDX 3
#define BALLSPEEDY 3
#define PADDLESPEED 4

#define WIN_POINTS 11

// change to 1 for only right ai, 2 for left and right ai
#define DO_AI 2

// change to 1 for easier AI, won't look ahead multiple bounces
#define AI_LOOKAHEAD 5

int points1, points2, won = 0;

typedef struct Ball
{
    Rectangle aabb;
    float velX, velY;
} Ball;

typedef Rectangle Paddle;

Paddle paddleLeft, paddleRight;
Ball ball;

// Try and move paddle
void movePaddle(Paddle *p, int dirY, float dt)
{
    p->y += dirY * PADDLESPEED * dt;

    // restrict to screen bounds
    if (p->y < 0)
        p->y = 0;
    if (p->y + p->height > W_WIDTH)
        p->y = W_WIDTH - p->height;
}

// ai is always paddleRight
void moveAI(Paddle *p, float dt)
{
    // if the ball isn't coming towards us, do nothing
    if ((p->x > W_WIDTH/2 && ball.velX < 0 ) || (p->x < W_WIDTH/2 && ball.velX > 0) || ball.velX == 0)
        return;

    // calculate where the ball will be when it's x is the same as that
    // of the paddle

    // ball: posB + velB * i = next pos

    // check at what timestep (i) the x of the ball is the same as that
    // of the paddle

    // xB + vxB * i = xP
    // vxB * i = xP - xB
    // i = (xP - xB) / vxB


    float vxB, xP, xB, vyB, yB, i, bYi;

    vyB = ball.velY;
    yB = ball.aabb.y;
    vxB = ball.velX;

    if (vxB > 0)
    {
        xB = ball.aabb.x+ball.aabb.width;
        xP = p->x;
    }
    else
    {
        xB = ball.aabb.x;
        xP = p->x + p->width;
    }

    for (int iter = 0; iter < AI_LOOKAHEAD; iter++)
    {
        // do da calculation
        i = (xP - xB) / vxB;

        // now check the y position of the ball at that point
        bYi = yB + vyB * i;

        // if the y position is not on screen, calculate the x and y of the
        // closest position in bounds and repeat
        if (bYi > W_HEIGHT-ball.aabb.height)
        {
            float ti = ((W_HEIGHT-ball.aabb.height) - yB) / vyB;

            xB = xB + vxB * ti;
            yB = W_HEIGHT-ball.aabb.height;
            vyB *= -1;
        }
        else if (bYi < 0)
        {
            float ti = (0 - yB) / vyB;

            xB = xB + vxB * ti;
            yB = 0;
            vyB *= -1;
        }
        else
            break;
    }

    // check if we need to move down or up
    float d = (p->y + p->height/2) - bYi;

    if (fabs(d) < PADDLESPEED)
        return;

    if (d < 0)
        movePaddle(p, 1, dt);
    else
        movePaddle(p, -1, dt);
}

void resolveCollision(Ball *r1, Paddle *r2)
{
    // calculate the collision surface on the bottom/top and on the left/right
    // make it work with differences in width and height between entities
    // begin on the left and right (so with differences in y)
    // I wrote this function some time ago and have since
    // completely forgotten how it works, but it seems to do fine lol

    // If the two entities aren't touching eachother, do nothing
    if (
        r1->aabb.x + r1->aabb.width <= r2->x ||
        r1->aabb.x >= r2->x + r2->width ||
        r1->aabb.y + r1->aabb.height <= r2->y ||
        r1->aabb.y >= r2->y + r2->height
    )
        return;

    float dT, dB, sY;

    dT = r2->y - r1->aabb.y;
    dB = (r1->aabb.y + r1->aabb.height) - (r2->y + r2->height);

    sY = (r1->aabb.height + r2->height - fabs(dT) - fabs(dB)) / 2;

    float dL, dR, sX;

    dL = r2->x - r1->aabb.x;
    dR = (r1->aabb.x + r1->aabb.width) - (r2->x + r2->width);

    sX = (r1->aabb.width + r2->width - fabs(dL) - fabs(dR)) / 2;

    // change sign based on direction; sX & sY are always positive

    // collision surface on the left or right is greater than that on top or bottom
    if (sY < sX)
    {
        // e moving down
        if (dT > 0)
            sY *= -1;

        r1->aabb.y += sY;

        // change velY based on where the ball hit the paddle
        r1->velY *= -1;
    }
    else
    {
        // e moving to the right
        if (dL > 0)
            sX *= -1;

        r1->aabb.x += sX;
        r1->velX *= -1;

        // uncomment to make y velocity change based on where the
        // ball hits the paddle
        // float cY;

        // if (dT < dB)
        // {
        //     cY = -(r2->height/2 + dT)/(P_HEIGHT/2);
        // }
        // else
        // {
        //     cY = (r2->height/2 + dB)/(P_HEIGHT/2);
        // }

        // r1->velY += cY;
    }
}

void reset(void)
{
    ball.velX = ball.velY = 0;
    ball.aabb = (Rectangle){W_WIDTH/2-B_WIDTH/2, W_HEIGHT/2-B_HEIGHT/2, B_WIDTH, B_HEIGHT};
    // left of center
    paddleLeft = (Rectangle){DISTANCE_SIDE, W_HEIGHT/2-P_HEIGHT/2, P_WIDTH, P_HEIGHT};
    // right of center
    paddleRight = (Rectangle){W_WIDTH-DISTANCE_SIDE, W_HEIGHT/2-P_HEIGHT/2, P_WIDTH, P_HEIGHT};
}

void moveBall(float dt)
{
    ball.aabb.x += ball.velX * dt;
    ball.aabb.y += ball.velY * dt;

    // ball is outside the screen on the x axis, increment relevant points
    if (ball.aabb.x < 0)
    {
        points1++;
        reset();
    }
    if (ball.aabb.x + ball.aabb.width > W_WIDTH)
    {
        points2++;
        reset();
    }
    if (ball.aabb.y < 0)
    {
        ball.aabb.y = 0;
        ball.velY *= -1;
    }
    if (ball.aabb.y + ball.aabb.height > W_HEIGHT)
    {
        ball.aabb.y = W_HEIGHT - ball.aabb.height;
        ball.velY *= -1;
    }

    if (points1 >= WIN_POINTS || points2 >= WIN_POINTS)
        won = 1;
}

void newGame(void)
{
    reset();

    points1 = points2 = 0;
    won = 0;
}

int main(void)
{
    newGame();

    InitWindow(W_WIDTH, W_HEIGHT, "Who called it table tennis?");
    SetTargetFPS(TARGETFPS);

    float targetFrameTime = 1000 /TARGETFPS;

    while (!WindowShouldClose())
    {
        // map from 0 to 1
        float deltatime = GetFrameTime() * 1000 / targetFrameTime;

        // get user input

        if (DO_AI >= 1)
            moveAI(&paddleRight, deltatime);
        else
        {
            if (IsKeyDown(KEY_UP))
                movePaddle(&paddleRight, -1, deltatime);
            if (IsKeyDown(KEY_DOWN))
                movePaddle(&paddleRight, 1, deltatime);
        }

        if (DO_AI >= 2)
            moveAI(&paddleLeft, deltatime);
        else
        {
            if (IsKeyDown(KEY_W))
                movePaddle(&paddleLeft, -1, deltatime);
            if (IsKeyDown(KEY_S))
                movePaddle(&paddleLeft, 1, deltatime);
        }

        if ((IsKeyPressed(KEY_SPACE) || DO_AI >= 2) && ball.velX == 0 && ball.velY == 0)
        {
            if (won)
                newGame();
            ball.velX = GetRandomValue(0, 1) > .5 ? BALLSPEEDX : -BALLSPEEDX;
            ball.velY = GetRandomValue(0, 1) > .5 ? BALLSPEEDY : -BALLSPEEDY;
        }

        // update positions
        moveBall(deltatime);

        if (ball.aabb.x < W_WIDTH/2)
            resolveCollision(&ball, &paddleLeft);
        else
            resolveCollision(&ball, &paddleRight);
        
        BeginDrawing();
        ClearBackground(BLACK);

        // draw paddles
        // DrawRectangleRec(paddleLeft, WHITE);
        DrawRectangle(paddleLeft.x, paddleLeft.y, paddleLeft.width, paddleLeft.height, WHITE);
        DrawRectangleRec(paddleRight, WHITE);

        // draw ball
        DrawRectangleRec(ball.aabb, WHITE);

        // draw line through middle
        DrawLine(W_WIDTH/2, 0, W_WIDTH/2, W_HEIGHT, WHITE);

        // draw score in the middle of each side of the screen
        if (!won)
        {
            DrawText(TextFormat("%d", points1), W_WIDTH/2+W_WIDTH/2/2, 100, 60, WHITE);
            DrawText(TextFormat("%d", points2), W_WIDTH/2-W_WIDTH/2/2, 100, 60, WHITE);
        }
        else
        {
            if (points1 > points2)
                DrawText("Won", W_WIDTH/2+W_WIDTH/2/2, 100, 60, WHITE);
            else if (points1 < points2)
                DrawText("Won", W_WIDTH/2-W_WIDTH/2/2, 100, 60, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}