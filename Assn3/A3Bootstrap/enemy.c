#include "enemy.h"

char* enemyHeadGraphic[ENEMY_HEAD_ANIM_TILES][ENEMY_HEIGHT] =
{
  {"B3"},
  {"3B"},
};

char* enemyBodyGraphic[ENEMY_BODY_ANIM_TILES][ENEMY_HEIGHT] =
{
  {"["},
  {"}"}
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
	consoleDrawImage(e->row, e->col, enemyHeadGraphic[e->animTile], ENEMY_HEIGHT);
	wrappedMutexUnlock(&(e->mutex));
	//TODO: unlock screen
}

void enemyRedraw(enemy *e, bool lock)
{
	_enemyRedrawMoved(e, e->row, e->col, lock);
}

/**THREAD FUNCTIONS**/
void *runEnemyT(void *data)
{
	enemy* e = (enemy*)data;
	newEnemy(e);

	while (e->running)
	{
		//TODO: not threadsafe!!!!

		e->animTile++;
		e->animTile %= ENEMY_HEAD_ANIM_TILES;
		enemyRedraw(e, false);
		sleepTicks(ENEMY_ANIM_TICKS);
	}
	return NULL;
}
