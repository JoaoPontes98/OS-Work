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

// A linked list to represent the enemy's body sections
typedef struct e_section
{
	int row;
	int col;
	struct e_section *next;
} e_section;

typedef struct enemy_struct
{
	/* remember the start parameters internally */
	int startCol;
	int startRow;

	// running/animation trackers
	bool running;
	int animTile;

	// cordinates of the enemy
	int row;
	int col;
	e_section* spriteLL;

	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_mutexattr_t attr;
} enemy;

void *runEnemyT(void *data);

enemy* spawnEnemy(int startRow, int startCol);

/* thread safe for enemy & ALSO LOCKS SCREEN */
//yours may or may not lock the screen on call.
void enemyMove(enemy *f);

/* NOT thread safe */
/* removes a life, ends the game if all lives gone */
//students: I actually left this as not thread safe in my final solution
//and I made sure it was used in a thread safe way
void killEnemy(enemy* p);

// Generate the linked list sprite of the enemy.
// Input is the length (including head) of enemy.
// Only generates enemies sprites facing left.
e_section* genEnemySprite(int head_col, int head_row, int length);

#endif
