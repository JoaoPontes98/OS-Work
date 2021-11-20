#include "bullet.h"
#define GAME_ROWS 24
#define GAME_COLS 80
//
char* eBulletGraphic[1] =
{
  "@"
};

char* pBulletGraphic[1] =
{
  "A"
};

bullet* mallocBullet()
{
  bullet* b = (bullet*)malloc(sizeof(bullet));
  return b;
}

void firePBullet(player *p)
{
  bullet* b = mallocBullet();
  b->row = p -> row;
  b->col = (p -> col) + 1;
  b->isPBullet = true;
  b->running = true;

  wrappedPthreadCreate(&(b->thread), NULL, runBulletT, (void*)b);
}

void fireEBullet(enemy *e)
{
  bullet* b = mallocBullet();
  e_section* e_head = e->spriteLL;

  b->row = e_head -> row + 2;
  b->col = e_head -> col;
  b->isPBullet = false;
  b->running = true;

  wrappedPthreadCreate(&(b->thread), NULL, runBulletT, (void*)b);
}

void bulletRedraw(bullet *b, bool lock)
{
  if(lock) return;

  if(b->isPBullet){
    consoleDrawImage(b->row, b->col, pBulletGraphic, BULLET_HEIGHT);
  }else{
    consoleDrawImage(b->row, b->col, eBulletGraphic, BULLET_HEIGHT);
  }
}

void bulletMove(bullet *b)
{
  consoleClearImage(b->row, b->col, BULLET_HEIGHT, BULLET_WIDTH);
  if(b->isPBullet)
  {
    b->row += -1;
  }else{
    b->row += 1;
  }

  bulletRedraw(b, false);
}

void killBullet(bullet *b)
{
  b->running = false;
  consoleClearImage(b->row, b->col, BULLET_HEIGHT, BULLET_WIDTH);
}

void *runBulletT(void *data)
{
	bullet* b = (bullet*)data;

	while (b->running)
	{
    bulletMove(b);
    if(b->isPBullet && b->row == 2){ killBullet(b); }
    if(!(b->isPBullet) && b->row == GAME_ROWS - 3){ killBullet(b); }
    sleepTicks(BULLET_ANIM_TICKS);
	}
	return NULL;
}
