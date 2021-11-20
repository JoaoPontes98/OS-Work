#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "bullet.h"
#include "enemy.h"

typedef struct enemyNode
{
   enemy* data;
   enemy* next;
} enemyNode;

typedef struct bulletNode
{
   bullet* data;
   bullet* next;
} bulletNode;

#endif
