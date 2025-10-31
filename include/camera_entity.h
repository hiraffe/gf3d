#ifndef __CAMERA_ENTITY_H__
#define __CAMERA_ENTITY_H__

#include "entity.h"

typedef struct
{
	Entity* target;
	float followHeight;
	float followDistance;
	float angle;
}CameraEntityData;

Entity* camera_entity_spawn(GFC_Vector3D position, Entity* target);

#endif