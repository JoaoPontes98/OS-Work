/**********************************************************************
  Module: player.c
  Author: Daniel Rea

  Purpose: code to run the player in its own thread.
	For distribution to students. Not all functions implemented.
	This is just from my solution with parts removed.
    Treat it as a guide. Feel free to implement,
	change, remove, etc, in your own solution.

**********************************************************************/

#include "player.h"
#define GAME_ROWS 24
#define GAME_COLS 80
#define PLAYER_AREA_ROWS
#define PLAYER_AREA_COLS 75

//sample player graphic, 3 tile animation.
//feel free to use this or make your own
char* playerGraphic[PLAYER_ANIM_TILES][PLAYER_HEIGHT] =
{
    {"/o\\",
     "|||",
     "/^\\"},
    {"/|\\",
     "-o-",
     "/^\\"},
    {"/|\\",
     "|||",
     "/o\\"}
};

/********************support functions***************/
/* reset the player state to start */
void newPlayer(player *p)
{
	p->row = p->startRow;
	p->col = p->startCol;
	p->animTile = 0;
	p->state = GAME;
}

void _playerRedrawMoved(player *p, int prevRow, int prevCol, bool lock)
{
	//TODO
	//Dear students, this function is NOT THREAD SAFE and will require fixing
	if(lock) return;

	//TODO: lock screen (critical shared resource)
	wrappedMutexLock(&(p->mutex));
	consoleDrawImage(p->row, p->col, playerGraphic[p->animTile], PLAYER_HEIGHT);
	wrappedMutexUnlock(&(p->mutex));
	//TODO: unlock screen
}

void playerRedraw(player *p, bool lock)
{
	_playerRedrawMoved(p, p->row, p->col, lock);
}

/********************THREAD functions***************/

player* spawnPlayer(int startRow, int startCol, int lives)
{
	player* p = (player*)(malloc(sizeof(player)));
	p->lives = lives;
	p->startCol = startCol;
	p->startRow = startRow;
	p->running = true;

	wrappedPthreadCreate(&(p->thread), NULL, runPlayerT, (void*)p);

	wrappedMutexAttrInit(&(p->attr));
	pthread_mutexattr_settype(&(p->attr), PTHREAD_MUTEX_RECURSIVE); //might need to change this attribute
	wrappedMutexInit(&(p->mutex), &(p->attr));

	return p;
}

void *runPlayerT(void *data)
{
	player* p = (player*)data;
	newPlayer(p);

	while (p->running && p->lives >= 0)
	{
		//TODO: not threadsafe!!!!
		switch(p->state)
		{
			case DEAD:
				p->lives--;
				//...other code here...
			default:
				;
		}

		p->animTile++;
		p->animTile %= PLAYER_ANIM_TILES;
		playerRedraw(p, false);
		sleepTicks(PLAYER_ANIM_TICKS);
	}
	return NULL;
}

void playerMove(player *f, int dRow, int dCol)
{
	consoleClearImage(f->row, f->col, PLAYER_HEIGHT, PLAYER_WIDTH);
	if((f->row + dRow) >= 17 && (f->row + dRow)-PLAYER_HEIGHT < 18) { f->row += dRow; }
	if((f->col + dCol) >= 0 && (f->col + dCol)-PLAYER_WIDTH  < PLAYER_AREA_COLS) { f->col += dCol; }
	playerRedraw(f, false);
}
