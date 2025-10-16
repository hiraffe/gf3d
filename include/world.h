#ifndef __WORLD_H__
#define __WORLD_H__

#include "gf3d_mesh.h"

typedef struct
{
	Mesh* terrain;
	Texture* texture;
	GFC_Color lightColor;
	GFC_Vector3D lightPos;
	GFC_List* entities; //just the ones in the world	
}World;

World* world_new();

World* world_load(const char* filename);

World* world_free(World* world);

World* world_draw(World* world);

Uint8 world_edge_test(World* world, GFC_Vector3D start, GFC_Vector3D end);

#endif