/**********************************************************************
  Module: centipede.c
  Author: Daniel Rea

  Purpose: the core source file for the game engine. This manages the
  thread initialization of all parts, provides functions for global lock
  coordination for the screen, and helps synchronize threads when necessary.

  Use this to get an idea of how to add things as part of the game.
  Feel free to use any of this freely in your solution, or change or modify
  this code.

  Some functions unimplemented, just to show how I roughly went about things.

**********************************************************************/
#include "centipede.h"
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>


//all these constants and gameboard should probably go in a constants file...hint hint
#define GAME_ROWS 24
#define GAME_COLS 80

/**** DIMENSIONS MUST MATCH the ROWS/COLS */
char *GAME_BOARD[] = {
"                   Score:          Lives:",
"=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-centipiede!=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"",
"",
"",
"",
"",
"",
"",
"" };


// #define ENEMY_HEIGHT 2
// #define ENEMY_WIDTH 1
// #define ENEMY_BODY_ANIM_TILES 4
// char* ENEMY_BODY[ENEMY_BODY_ANIM_TILES][ENEMY_HEIGHT] =
// {
//   {"1",
//    "1"},
//   {"2",
//    "2"},
//   {"3",
//    "3"},
//   {"4",
//    "4"}
// };

bool end_game = false;

typedef struct screen_struct
{
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_mutexattr_t attr;
} screen;

void* screenRoutine(void *args){
	player *p = (player *)args;
	char scoreboard[GAME_COLS];
	while(!end_game){
		snprintf(scoreboard, sizeof(scoreboard), "                   Score:          Lives:%d",p->lives);
		putString(scoreboard, 0, 0, 50);
		consoleRefresh();
		sleepTicks(1);
	}
	return 0;
}

int kbhit()
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, NULL) > 0;
}


void* keyboardRoutine(void *args){
	player *p = (player *)args;
	char input;

	// Middleware to allow keyboard detection without clicking the return key.
	static struct termios oldt, newt;
	tcgetattr( STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON);

	while(p->running){
		//This needs to wrap the kb input
		tcsetattr( STDIN_FILENO, TCSANOW, &newt);
		if(kbhit()){
			input = getchar();
			// Mapping controls
			if(input == 'w'){
				playerMove(p, -1, 0);
			}else if(input == 'a'){
				playerMove(p, 0, -1);
			}else if(input == 's'){
				playerMove(p, 1, 0);
			}else if(input == 'd'){
				playerMove(p, 0, 1);
			}else if(input == 'q'){
				end_game = true;
			}else if(input == 'f'){
				p->lives = p->lives;
			}
		}
		tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
	}
	return 0;
}

//the rest will hold the main game engine
//it's likely you'll add quite a bit to it (input, cleanup, synchronization, etc)

//you'll probably want a sort of globally accessible function to the player, enemy, etc
//to control screen locking here.

void centipedeRun()
{
  if (consoleInit(GAME_ROWS, GAME_COLS, GAME_BOARD))
  {
    //create player
    player *p = spawnPlayer(20, 5, 3);

    //create screen
    screen *Screen = (screen*)malloc(sizeof(screen));
    wrappedPthreadCreate(&(Screen->thread), NULL, &screenRoutine, p);

    //create keyboard
    pthread_t keyboardT;
    wrappedPthreadCreate(&keyboardT, NULL, &keyboardRoutine, p);

    //create Enenmy *This is temp*
    enemy *e = spawnEnemy(4, 70);

    //above, initialize all the threads you need
    //below, you should make a "gameplay loop" that manages screen drawing
    //that  waits on a condition variable until the game is over
    //and coordinates all threads to end

    //GAMEPLAY loop
    while(!end_game)
    {

    }

    //note after this the player thread keeps running and isn't cleaned
    //up properly. Why don't we see it update on screen?
    putBanner("Hope you had fun!");
    finalKeypress(); /* wait for final key before killing curses and game */
    p->running = false;
    wrappedPthreadJoin(p->thread, NULL);
    wrappedPthreadJoin(Screen->thread, NULL);
    wrappedPthreadJoin(keyboardT, NULL);

  }
  consoleFinish();
}
