#ifndef __BODY_H__
#define __BODY_H__

#include "gfc_text.h"
#include "gfc_list.h"
#include "gfc_primitives.h"

typedef enum
{
	CFM_none = 0,
	CFM_Player = 1,
	CFM_Monster = 2,
	CFM_Item = 4,
	CFM_World = 8,
	CFM_Interactable = 16
}CollisionFilterMask;

typedef struct Body_S
{
	GFC_TextLine		name;
	CollisionFilterMask mask;		//only collide when there is overlap
	Uint8				team;
	GFC_Vector3D		position;	//center of mass
	GFC_Vector3D		velocity;	//how fast we are currently moving
	GFC_List*			volumes;	//some number of bounding shapes for collisions
	void				(*onCollide)(struct Body_S* self, struct Body_S other, void* data); //call this whenever we hit
	void*				data;

	GFC_Vector3D		stepPosition;
	GFC_Vector3D		stepVelocity;

	Uint8				stopped;	//stop iterating on this body, we already hit something
}Body;

typedef void body_collide_func(Body* self, Body* other, void* data);

Body* body_new();

Body* body_free(Body* b);

void body_reset_for_updates(Body* b, float factor);

void body_set_colliison(Body *b, body_collide_func *collide, void *data);

/*
 * return 0 no collision, 1 if collision
 */
int body_test_body(Body* a, Body* b);

#endif