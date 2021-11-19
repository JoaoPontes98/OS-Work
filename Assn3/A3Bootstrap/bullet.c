#include "bullet.h"

char* eBulletGraphic[BULLET_ANIM_TILES][BULLET_HEIGHT] =
{
  {"@"}
};

char* pBulletGraphic[BULLET_ANIM_TILES][BULLET_HEIGHT] =
{
  {"A"}
};

bullet* mallocBullet(){
  bullet* b = (bullet*)malloc(sizeof(bullet));
  return b;
}
