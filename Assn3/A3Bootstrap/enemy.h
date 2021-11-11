#ifndef ENEMY_H
#define ENEMY_H

#define ENEMY_ANIM_TILES 2
#define ENEMY_ANIM_TICKS 50
#define ENEMY_HEIGHT 2
#define ENEMY_WIDTH 22

#include "console.h"
#include "threadwrappers.h"
#include "centipede.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct enemy_struct
{
	/* remember the start parameters internally */
	int startCol;
	int startRow;

	bool running;
	int row;
	int col;
	int animTile;
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_mutexattr_t attr;
} enemy;

void *runEnemyT(void *data);

enemy* spawnEnemy(int startRow, int startCol);

/* thread safe for enemy & ALSO LOCKS SCREEN */
//yours may or may not lock the screen on call.
void enemyMove(enemy *f, int dRow, int dCol);

/* NOT thread safe */
/* removes a life, ends the game if all lives gone */
//students: I actually left this as not thread safe in my final solution
//and I made sure it was used in a thread safe way
void killEnemy(enemy* p);


#endif
