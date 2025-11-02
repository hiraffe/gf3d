#ifndef __CROP_H__
#define __CROP_H__

#include "simple_json.h"

#include "entity.h"

typedef enum
{
	C_SEED,
	C_UNRIPE,
	C_RIPE
}CropGrowth;

typedef struct
{
	Mesh*			cropMesh;		
	Texture*		ripeTexture;
	Texture*		unripeTexture;
	Uint32			spawnTime;
	float			ripenTime;
	CropGrowth		growth;
}CropEntityData;

/**
 * @brief spawn in a new crop
 * @return NULL on error, or a new crop
 */
Entity* crop_spawn(GFC_Vector3D position, const char *name);

/**
 * @brief initialize and load crop definitions from a json file
 * @param filename json file containing crop data
 */
void crops_init(const char* filename);

/**
 * @brief crop definition by its name
 * @param name the search criteria
 * @return NULL if not found, otherwise the definition information of the crop
 */
SJson* crop_get_def_by_name(const char* name);

/**
 * @brief free a previously loaded crop
 * @param world the world to free
 */
void crop_free(Entity* crop);

#endif