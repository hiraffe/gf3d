#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_text.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_primitives.h"

#include "gf3d_mesh.h"

typedef struct Entity_S
{
	Uint8			_inuse;
	GFC_TextLine	name;
	GFC_TextLine	displayName;
	const char*		entityType;
	Mesh*			mesh;
	Texture*		texture;
	GFC_Color		color;
	GFC_Matrix4		matrix;
	GFC_Vector3D	position;
	GFC_Vector3D	rotation;
	GFC_Vector3D	scale;
	GFC_Vector3D	velocity;

	GFC_Box			bounds;
	float			collisionRadius;
	void			(*draw)(struct Entity_S *self, GFC_Vector3D lightPos, GFC_Color lightColor);
	void			(*think)(struct Entity_S *self);
	void			(*update)(struct Entity_S *self);
	void			(*free)(struct Entity_S *self);
	Uint8			doGenericUpdate;
	void*			data;
}Entity;

typedef struct
{
	Entity* entity_list;
	Uint32 entity_max;
}EntitySystem;

/**
 * @brief get the entity system
 * @return the entity system
 */
EntitySystem entity_get_system();

/**
 * @brief get a pointer to a new blank entity
 * @return NULL on out of memory or other error, pointer to blank entity otherwise
 */
Entity* entity_new();

/**
 * @brief free a previouslt new'd entity
 * @param ent the entity to be freed
 */
void entity_free(Entity *ent);

/**
 * @brief initialozes the entity subsystem
 * @param max_ents how many to support concurrently
 */
void entity_system_init(Uint32 max_ents);

/**
 * @bried draw all entities with the provided light
 */
void entity_system_draw_all(GFC_Vector3D lightPos, GFC_Color lightColor);
void entity_system_think_all();
void entity_system_update_all();

int entity_check_collision(Entity* self, GFC_Vector3D newPos, float radius);

#endif