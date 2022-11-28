# Pong in C

This is my implementation of pong in C.

I used the Raylib library in this project, go check it out at https://github.com/raysan5/raylib, it's awesome!

W/S & UP/DOWN to move the two paddles

There's also an AI system, set the variable DO_AI on line 20 to 1, like so:

```
#define DO_AI 1
```

to enable it FOR THE RIGHT PADDLE. If you want to enable it for both paddles (thus making the game play itself),
uncomment line 262, like so:

```
moveAI(&paddleLeft, deltatime);
```

Hope you enjoy!