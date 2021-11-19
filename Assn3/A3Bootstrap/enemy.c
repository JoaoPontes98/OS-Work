#include "enemy.h"
#define GAME_ROWS 24
#define GAME_COLS 80
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

void enemyRedraw(enemy *e, bool lock)
{
  //TODO
	//Dear students, this function is NOT THREAD SAFE and will require fixing
	if(lock) return;

	//TODO: lock screen (critical shared resource)
	wrappedMutexLock(&(e->mutex));

  e_section* e_head = e->spriteLL;
  e_section* e_curr = e_head;
  int i = 0;

  while(e_curr){
    if(i == 0){ // draw the head
      consoleDrawImage(e_curr->row, e_curr->col, enemyHeadGraphic[e->animTile], ENEMY_HEIGHT);
    }else{ // draw the body
      consoleDrawImage(e_curr->row, e_curr->col, enemyBodyGraphic[(i/3)%ENEMY_ANIM_TILES], ENEMY_HEIGHT);
    }
    e_curr = e_curr->next;
    i++;
  }
	wrappedMutexUnlock(&(e->mutex));
	//TODO: unlock screen
}

void enemyMove(enemy *e)
{
  //wrappedMutexLock(&(e->mutex));
  e_section* e_head = e->spriteLL;
  e_section* e_curr = e_head;
  e_section* e_tail = e_head->tail;

  //only clear the last section
  consoleClearImage(e_tail->row, e_tail->col, ENEMY_HEIGHT, 2);
  while(e_curr && e_curr->col != 0)
  {
      if(((e_curr->col) - 1) < 1 || ((e_curr->col) + 1) > GAME_COLS) // wrap sprite
      {
        e_curr->row += 2;
      }

      if((e_curr->row)%4 == 2) { // move LEFT
        e_curr->col += -1;
      }else{ //move RIGHT
        e_curr->col += 1;
      }

      e_curr=e_curr->next;
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
  for( i = 1; i < length; i++){
    e_curr->next = mallocSpriteNode();
    e_curr->row = head_row;
    e_curr->col = head_col + (i + 1);
    e_head->tail = e_curr;
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
		sleepTicks(ENEMY_ANIM_TICKS);
	}
	return NULL;
}
