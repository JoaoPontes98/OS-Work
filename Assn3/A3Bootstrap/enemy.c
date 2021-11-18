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

  e_section* e_head = e->spriteLL;
  e_section* e_body = e_head->next;
  int i = 0;

  if((e->row)%4 == 2) { // moving left
    // draw the head
  	consoleDrawImage(e_head->row, e_head->col, enemyHeadGraphic[e->animTile], ENEMY_HEIGHT);
    // draw the body
    while(e_body){
      consoleDrawImage(e_body->row, e_body->col, enemyBodyGraphic[(i/3)%ENEMY_ANIM_TILES], ENEMY_HEIGHT);
      e_body = e_body->next;
      i++;
    }
  } else { // moving right
    // draw the head
    consoleDrawImage(e_head->row, e_head->col, enemyHeadGraphic[e->animTile], ENEMY_HEIGHT);
    // draw the body
    while(e_body){
      consoleDrawImage(e_body->row, e_body->col, enemyBodyGraphic[(i/3)%ENEMY_ANIM_TILES], ENEMY_HEIGHT);
      e_body = e_body->next;
      i++;
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
  //wrappedMutexLock(&(e->mutex));
  e_section* e_head = e->spriteLL;
  e_section* e_body = e_head->next;

  if((e_head->row)%4 == 2) { // Move right every forth row (because the enemy has 2 height)
    consoleClearImage(e_head->row, e_head->col, ENEMY_HEIGHT, ENEMY_WIDTH+1);
  	e_head->col += -1;
    while(e_body){
      e_body -> col += -1;
      e_body = e_body->next;
    }
  } else {
    consoleClearImage(e_head->row, e_head->col, ENEMY_HEIGHT, ENEMY_WIDTH+1);
    e_head->col += 1;
    while(e_body){
      e_body -> col += 1;
      e_body = e_body->next;
    }
  }
	enemyRedraw(e, false);
  //wrappedMutexUnlock(&(e->mutex));
}

e_section* mallocSpriteNode()
{
  e_section* s_node = (e_section*)malloc(sizeof(e_section));
  s_node->next = NULL;
  return s_node;
}

e_section* genEnemySprite(int head_col, int head_row, int length)
{
  e_section* e_head = mallocSpriteNode();
  e_section* e_curr = e_head;

  // Make the head
  e_head->col = head_col;
  e_head->row = head_row;

  int i;
  for( i = 0; i < length; i++){
    e_curr->next = mallocSpriteNode();
    e_curr->row = head_row;
    e_curr->col = head_col + (i + 1);
    e_curr = e_curr->next;
  }

  return e_head;
}


/**THREAD FUNCTIONS**/

enemy* spawnEnemy(int startRow, int startCol)
{
	enemy* e = (enemy*)(malloc(sizeof(enemy)));
	e->startCol = startCol;
	e->startRow = startRow;
	e->running = true;
  e->spriteLL = genEnemySprite(startCol, startRow, ENEMY_WIDTH);

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
    enemyMove(e);
    enemyRedraw(e, false);
		sleepTicks(ENEMY_ANIM_TICKS);
	}
	return NULL;
}
