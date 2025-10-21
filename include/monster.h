#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "gfc_vector.h"
#include "entity.h"

Entity *monster_spawn(GFC_Vector3D position, GFC_Color color, Entity* cam);


#endif