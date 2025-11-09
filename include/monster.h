#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "gfc_vector.h"
#include "inventory.h"
#include "item.h"

#include "entity.h"

typedef struct
{
	Entity*		cam;
	float		gold;
	Inventory*	inventory;
	Item*		item_held;
	Uint32		item_index;
}MonsterEntityData;

Entity* monster_get_the();

Entity* monster_spawn(GFC_Vector3D position, GFC_Color color);

void monster_set_camera_ent(Entity* self, Entity* camera);


#endif