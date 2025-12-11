#include "simple_logger.h"

#include "monster.h"

#include "door.h"

void door_free(Entity* self)
{
	DoorEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;
}

void door_open(Entity* self)
{
	DoorEntityData* data;
	if ((!self)||(!self->data)) return NULL;
	data = self->data;

	self->mesh = data->openMesh;
	data->isOpen = 1;
	self->collisionRadius = 0;
}

void door_close(Entity* self)
{
	DoorEntityData* data;
	if ((!self) || (!self->data)) return NULL;
	data = self->data;

	self->mesh = data->closedMesh;
	data->isOpen = 0;
	self->collisionRadius = 2;
}

void door_think(Entity* self)
{
	DoorEntityData* data;
	Entity* monster = monster_get_the();
	if ((!self) || (!self->data) || (!monster)) return;
	data = self->data;

	if (gfc_vector3d_magnitude_between(monster->position, self->position) < 15)
	{
		if (gfc_input_command_released("select"))
		{
			if (data->isOpen)
			{
				door_close(self);
			}
			else
			{
				door_open(self);
			}
		}
	}
}

Entity* door_spawn(GFC_Vector3D position, GFC_Color color)
{
	Entity* self;
	DoorEntityData* data;
	self = entity_new();
	if (!self) return NULL;
	data = gfc_allocate_array(sizeof(DoorEntityData), 1);
	if (!data)
	{
		free(self);
		return NULL;
	}
	self->data = data;

	gfc_line_cpy(self->name, "door");
	self->entityType = "door";
	self->mesh = gf3d_mesh_load("models/fence/closed-door.obj");
	self->texture = gf3d_texture_load("models/fence/wood-texture.png");
	self->position = position;
	self->color = color;
	self->rotation = gfc_vector3d(0, 0, 0);
	self->collisionRadius = 2;

	data->openMesh = gf3d_mesh_load("models/fence/open-door.obj");
	data->closedMesh = gf3d_mesh_load("models/fence/closed-door.obj");
	data->isOpen = 0;

	self->think = door_think;

	slog("Door spawned: %s", self->name);

	return self;
}