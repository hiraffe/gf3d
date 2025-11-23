#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "gfc_vector.h"
#include "inventory.h"
#include "item.h"
#include "gfc_audio.h"

#include "entity.h"

typedef struct
{
	Entity*		cam;
	float		gold;
	Inventory*	inventory;
	Item*		item_held;
	Uint32		item_index;
	Mix_Chunk*  plant_sound;
	Mix_Chunk*  harvest_sound;
}MonsterEntityData;

/**
 * @brief get the monster (player)
 * @return the monster
 */
Entity* monster_get_the();

/**
 * @brief spawn in a new monster
 * @return NULL on error, or a new monster
 */
Entity* monster_spawn(GFC_Vector3D position, GFC_Color color);

/**
 * @brief set the camera entity that will follow the player around
 */
void monster_set_camera_ent(Entity* self, Entity* camera);


#endif