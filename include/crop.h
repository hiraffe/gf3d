#ifndef __CROP_H__
#define __CROP_H__

#include "simple_json.h"

#include "entity.h"

typedef struct
{
	Texture*		unripeTexture;
	Uint32			ripenTime;
}CropEntityData;

/**
 * @brief spawn in a new crop
 * @return NULL on error, or a new crop
 */
Entity* crop_spawn(GFC_Vector3D position, const char *name);

/**
 * @brief load crop data from a json file
 * @param filename name of the file to load
 * @return NULL on error or the world
 */
SJson* crop_load(const char* filename);

/**
 * @brief free a previously loaded world
 * @param world the world to free
 */
void crop_free(Entity* crop);

#endif