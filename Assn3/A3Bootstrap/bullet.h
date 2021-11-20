#ifndef BULLET_H
#define BULLET_H

#define BULLET_ANIM_TILES 1
#define BULLET_ANIM_TICKS 5
#define BULLET_HEIGHT 1
#define BULLET_WIDTH 1

#include "console.h"
#include "threadwrappers.h"
#include "centipede.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct bullet
{
	int row;
	int col;
  bool running;
  bool isPBullet;

	struct bullet *next;

  pthread_t thread;
  pthread_mutex_t mutex;
  pthread_mutexattr_t attr;
} bullet;

void* runBulletT(void *data);

bullet* mallocBullet();

void firePBullet(player *p);

void fireEBullet(enemy *e);

void killBullet(bullet *b);

void bulletMove(bullet *b);

void bulletRedraw(bullet *b, bool lock);

#endif
