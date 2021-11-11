#include "enemy.h"

char* enemyHeadGraphic[ENEMY_ANIM_TILES][ENEMY_HEIGHT] =
{
  {"B3",
   "-."},
  {"3B",
   "--"},
};

char* enemyBodyGraphic[ENEMY_ANIM_TILES][ENEMY_HEIGHT] =
{
  // {"[[[}[[[}[[[}[[[}[[[}",
  //  ";:;:;:;:;:;:;:;:;:;:"}
  {"[",
   ";"},
  {"{",
   ","}
};

/**SUPPORT FUNCTIONS**/
/* reset the enemy state to start */
void newEnemy(enemy *e)
{
	e->row = e->startRow;
	e->col = e->startCol;
	e->animTile = 0;
}

void _enemyRedrawMoved(enemy *e, int prevRow, int prevCol, bool lock)
{
	//TODO
	//Dear students, this function is NOT THREAD SAFE and will require fixing
	if(lock) return;

	//TODO: lock screen (critical shared resource)
	wrappedMutexLock(&(e->mutex));

  // draw the head
	consoleDrawImage(e->row, e->col, enemyHeadGraphic[e->animTile], ENEMY_HEIGHT);
  //draw the body
  for( int i = 0; i < ENEMY_WIDTH; i++){
    consoleDrawImage(e->row, (e->col)+(2+i), enemyBodyGraphic[(i/3)%ENEMY_ANIM_TILES], ENEMY_HEIGHT);
  }

	wrappedMutexUnlock(&(e->mutex));
	//TODO: unlock screen
}

void enemyRedraw(enemy *e, bool lock)
{
	_enemyRedrawMoved(e, e->row, e->col, lock);
}

void enemyMove(enemy *e, int dRow, int dCol)
{
	consoleClearImage(e->row, e->col, ENEMY_HEIGHT, ENEMY_WIDTH+5);
	e->row += dRow;
	e->col += dCol;
	enemyRedraw(e, false);
}

/**THREAD FUNCTIONS**/

enemy* spawnEnemy(int startRow, int startCol)
{
	enemy* e = (enemy*)(malloc(sizeof(enemy)));
	e->startCol = startCol;
	e->startRow = startRow;
	e->running = true;

	wrappedPthreadCreate(&(e->thread), NULL, runEnemyT, (void*)e);

	wrappedMutexAttrInit(&(e->attr));
	pthread_mutexattr_settype(&(e->attr), PTHREAD_MUTEX_RECURSIVE); //might need to change this attribute
	wrappedMutexInit(&(e->mutex), &(e->attr));

	return e;
}

void *runEnemyT(void *data)
{
	enemy* e = (enemy*)data;
	newEnemy(e);

	while (e->running)
	{
		//TODO: not threadsafe!!!!

		e->animTile++;
		e->animTile %= ENEMY_ANIM_TILES;
    enemyMove(e, 0, -1);
    enemyRedraw(e, false);
		sleepTicks(ENEMY_ANIM_TICKS);
	}
	return NULL;
}
