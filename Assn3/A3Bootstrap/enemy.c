#include "enemy.h"

char* enemyHeadGraphic[ENEMY_ANIM_TILES][ENEMY_HEIGHT] =
{
  {"B",
   "-"},
  {"3",
   ","},
};

char* enemyBodyGraphic[ENEMY_ANIM_TILES][ENEMY_HEIGHT] =
{
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

  if((e->row)%4 == 2) { // moving left
    // Maybe add a var to enemy keeping track how much body is left to wrap
    // if e->col < 0, then we start wrapping
    //  e->row++ to move down one
    //  e->wrapping = true;
    //  e->numToBeWrapped = ENEMY_WIDTH-1

    // draw the head
  	consoleDrawImage(e->row, e->col, enemyHeadGraphic[e->animTile], ENEMY_HEIGHT);
    // draw the body
    for( int i = 0; i < ENEMY_WIDTH; i++){
      consoleDrawImage(e->row, (e->col)+(1+i), enemyBodyGraphic[(i/3)%ENEMY_ANIM_TILES], ENEMY_HEIGHT);
    }
  } else { // moving right
    // draw the head
    consoleDrawImage(e->row, (e->col)+ENEMY_WIDTH+1, enemyHeadGraphic[e->animTile], ENEMY_HEIGHT);
    // draw the body
    for( int i = 0; i < ENEMY_WIDTH; i++){
      consoleDrawImage(e->row, (e->col)+(1+i), enemyBodyGraphic[(i/3)%ENEMY_ANIM_TILES], ENEMY_HEIGHT);
    }
  }

	wrappedMutexUnlock(&(e->mutex));
	//TODO: unlock screen
}

void enemyRedraw(enemy *e, bool lock)
{
	_enemyRedrawMoved(e, e->row, e->col, lock);
}

void enemyMove(enemy *e)
{
  wrappedMutexLock(&(e->mutex));
  if((e->row)%4 == 2) { // Move right every forth row (because the enemy has 2 height)
    consoleClearImage(e->row, e->col, ENEMY_HEIGHT, ENEMY_WIDTH+1);
  	e->col += -1;
  } else {
    consoleClearImage(e->row, e->col, ENEMY_HEIGHT, ENEMY_WIDTH+1);
  	e->col += 1;
  }
	enemyRedraw(e, false);
  wrappedMutexUnlock(&(e->mutex));
}

/**THREAD FUNCTIONS**/

enemy* spawnEnemy(int startRow, int startCol)
{
	enemy* e = (enemy*)(malloc(sizeof(enemy)));
	e->startCol = startCol;
	e->startRow = startRow;
	e->running = true;
  e->numToBeWrapped = 0;
  e->wrapping = false;

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
    //enemyMove(e, 0, -1);
    enemyMove(e);
    enemyRedraw(e, false);
		sleepTicks(ENEMY_ANIM_TICKS);
	}
	return NULL;
}
