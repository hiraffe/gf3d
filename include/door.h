#ifndef __DOOR_H_
#define __DOOR_H_

#include "entity.h"

typedef struct
{
	Mesh*	openMesh;
	Mesh*	closedMesh;
	Uint8	isOpen;
}DoorEntityData;

Entity* door_spawn(GFC_Vector3D position, GFC_Color color);

Uint8 door_is_open(Entity* self);

#endif