#ifndef __ANIMAL_H__
#define __ANIMAL_H__

#include "simple_json.h"

#include "entity.h"

typedef struct
{
	int				species;			/**< which animal it is */
	GFC_Vector3D	home;				/**< central starting position */
	GFC_Vector3D	target;				/**< where animal is moving towards */
	float			roamingRadius;		/**< how far the animal can go */
	float			speed;				/**< movement speed */
	float			idleTime;			/**< time to wait before next move */
}AnimalEntityData;

/**
 * @brief spawn in a new crop
 * @return NULL on error, or a new crop
 */
Entity* animal_spawn(GFC_Vector3D position, const char* name);

/**
 * @brief initialize and load crop definitions from a json file
 * @param filename json file containing crop data
 */
void animals_init(const char* filename);

/**
 * @brief crop definition by its name
 * @param name the search criteria
 * @return NULL if not found, otherwise the definition information of the crop
 */
SJson* animal_get_def_by_name(const char* name);

/**
 * @brief free a previously loaded crop
 * @param world the world to free
 */
void animal_free(Entity* crop);

#endif