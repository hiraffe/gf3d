#ifndef __WORLD_H__
#define __WORLD_H__

#include "gf3d_mesh.h"

typedef struct
{
	Mesh*			terrain;
	Texture*		texture;
	Mesh*			town;
	Texture*		townTexture;
	Mesh*			sign;
	Texture*		signTexture;
	GFC_Color		lightColor;
	GFC_Vector3D	lightPos;
	GFC_List*		entities; //just the ones in the world	
}World;

/**
 * @brief get a new empty world
 * @return NULL on error, or a new empty world
 */
World* world_new();

/**
 * @brief load world data from a json file
 * @param filename name of the file to load
 * @return NULL on error or the world
 */
World* world_load(const char* filename);

/**
 * @brief free a previously loaded world
 * @param world the world to free
 */
World* world_free(World* world);


/**
 * @brief draw the world and all entities that belong in it
 * @param world the world to draw
 */
World* world_draw(World* world);

Uint8 world_edge_test(World* world, GFC_Vector3D start, GFC_Vector3D end);

#endif