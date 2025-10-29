#include "simple_logger.h"

#include "gfc_input.h"

#include "monster.h"

typedef struct
{
	Entity* cam;
}MonsterEntityData;

void monster_free(Entity* self)
{
	//MonsterEntityData* data;
	//if ((!self) || (!self->data)) return;
	//data = self->data;
	//free(self->data);
	//free(self);
}

void monster_think(Entity* self)
{
	GFC_Vector3D cameraDir;
	float move = 0;
	float moveStep = 0.5;
	MonsterEntityData* data;
	if ((!self) || !(self->data)) return;
	data = self->data;
	if (!data->cam)return;

	gfc_vector3d_sub(cameraDir, self->position, data->cam->position);
	gfc_vector3d_normalize(&cameraDir);
	if (gfc_input_command_down("walkforward"))
	{
		move += moveStep;
	}
	if (gfc_input_command_down("walkback"))
	{
		move -= moveStep;
	}
	if (move)
	{
		gfc_vector2d_scale(cameraDir, cameraDir, move);
		gfc_vector2d_add(self->position, self->position, cameraDir);
	}

	move = 0;
	gfc_vector3d_sub(cameraDir, self->position, data->cam->position);
	gfc_vector3d_normalize(&cameraDir);
	gfc_vector3d_rotate_about_z(&cameraDir, GFC_HALF_PI);
	if (gfc_input_command_down("walkleft"))
	{
		move += moveStep;
	}
	if (gfc_input_command_down("walkright"))
	{
		move -= moveStep;
	}
	if (move)
	{
		gfc_vector2d_scale(cameraDir, cameraDir, move);
		gfc_vector2d_add(self->position, self->position, cameraDir);
	}
}

void monster_set_camera_ent(Entity* self, Entity* camera)
{
	if ((!self) || !camera) return;
	MonsterEntityData* data;
	if ((!self) || !(self->data)) return;
	data = self->data;

	data->cam = camera;
}

Entity *monster_spawn(GFC_Vector3D position, GFC_Color color)
{
	Entity* self;
	MonsterEntityData* data;
	self = entity_new();
	if (!self) return NULL;
	data = gfc_allocate_array(sizeof(MonsterEntityData), 1);
	if (!data)
	{
		free(self);
		return NULL;
	}
	//populate monster data
	gfc_line_cpy(self->name, "notAgumon");
	self->mesh = gf3d_mesh_load("models/dino/dino.obj");
	self->texture = gf3d_texture_load("models/dino/dino.png");
	self->position = position;
	self->color = color;

	self->rotation = gfc_vector3d(0, 0, 0);
	self->rotation.z = 180;
	self->velocity = gfc_vector3d(0, 0, 0);
	self->velocity.x = 0;

	//void			(*draw)(struct Entity_S* self);
	self->think = monster_think;
	//void			(*update)(struct Entity_S* self);

	self->data = data;
	slog("Monster spawned: %s", self->name);
	return self;
}