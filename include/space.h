#ifndef __SPACE_H__
#define __SPACE_H__

#include "gfc_list.h"

#include "body.h"

typedef struct
{
	GFC_List* staticMeshes; //terrain(s)
	GFC_List* bodies; //for entities and the like
	GFC_List* staticBodies; //for non-moving platforms
	Uint32 iterations; //how many movement subdivisions to use
	float step; //should be 1/iterations

}Space;

Space* space_new();

void space_set_iterations(Uint32 iterations);

void space_run(Space *space);

void space_add_body(Space* space, Body* body);

#endif