#include <raylib.h>
#include <math.h>
#include <stdio.h>

#define W_WIDTH 600
#define W_HEIGHT 600

#define P_WIDTH 15
#define P_HEIGHT 80

#define B_WIDTH 10
#define B_HEIGHT 10

#define DISTANCE_SIDE 50

#define BALLSPEEDX 5
#define BALLSPEEDY 8
#define PADDLESPEED 5

#define DO_AI 1
#define AI_LOOKAHEAD 5

int points1 = 0, points2 = 0;

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

    // check at what i the x of the ball is the same as that
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
    ball.aabb = (Rectangle){W_WIDTH/2-B_WIDTH/2, W_HEIGHT/2-B_HEIGHT/2, B_WIDTH, B_HEIGHT};
    ball.velX = ball.velY = 0;
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
}

int main(void)
{
    reset();

    InitWindow(W_WIDTH, W_HEIGHT, "Who called it table tennis?");
    SetTargetFPS(60);

    int slowmo = 1;

    // desired frame time in milliseconds
    float desiredFrameTime = 1000 / 60;

    while (!WindowShouldClose())
    {
        float deltatime = GetFrameTime() * 1000 / desiredFrameTime / slowmo;

        // get user input

        if (IsKeyDown(KEY_S))
            slowmo = 10;
        else
            slowmo = 1;

        // move paddle right
        if (DO_AI)
        {
            moveAI(&paddleRight, deltatime);

            // to let ai play against itself
            // moveAI(&paddleLeft, deltatime);
        }
        else
        {
            if (IsKeyDown(KEY_UP))
                movePaddle(&paddleRight, -1, deltatime);
            if (IsKeyDown(KEY_DOWN))
                movePaddle(&paddleRight, 1, deltatime);
        }

        // move paddle left
        if (IsKeyDown(KEY_W))
            movePaddle(&paddleLeft, -1, deltatime);
        if (IsKeyDown(KEY_S))
            movePaddle(&paddleLeft, 1, deltatime);

        if (IsKeyPressed(KEY_SPACE) && ball.velX == 0 && ball.velY == 0)
        {
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
        DrawText(TextFormat("%d", points1), W_WIDTH/2+W_WIDTH/2/2, 100, 60, WHITE);
        DrawText(TextFormat("%d", points2), W_WIDTH/2-W_WIDTH/2/2, 100, 60, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}