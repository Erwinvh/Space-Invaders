#ifndef Invaders_H
#define Invaders_H

//Controls
#define Up 0
#define Down 1
#define Shoot 2
#define Play 3
#define Stop 4
#define Empty 9

//game states
#define MenuState 0
#define InGameState 1
#define WinLossState 2

//Counter/timer limits
#define TurnTime 1000000
#define WinLossTime 15

//List limits
#define ShotsLimit 50
#define EnemiesLimit 50

//Score costs and rewards
#define EnemyKilledReward 100
#define ShotCost 10

//The struct for a position on the LCD screen
//      xpos is the X position on the LCD
//      ypos is the y position on the LCD
//      (X is the long side and y the short side)
typedef struct
{
    int yPos;
    int xPos;
} LCDPOS_T;

//The struct for the enemy/invader object
//      Entry turn defines when the invader comes onto the screen and from there gets ingremented and moved
//      position is the position on the LCD
//      isAlive is to check whether the invader is still alive. if all are dead you win
typedef struct
{
    int entryTurn;
    LCDPOS_T position;
    int isAlive;
} INVADER_T;

//The struct for the shot the player fires at the enemy
//      isValid checks whether the shot is still useful to the game
//      position contains the position of the shot on the LCD
typedef struct
{
    int isValid;
    LCDPOS_T position;
} SHOT_T;

//The struct for the game object
//      enemies is a list of all enemies
//      player is the current position of the player on the screen.
//      timecounter is the counter that keeps track of which turn it is in the game
//      shots is a list of all shots
typedef struct
{
    INVADER_T enemies[EnemiesLimit];
    LCDPOS_T Player;
    int timeCounter;
    SHOT_T shots[ShotsLimit];
} GAME_T;

#endif