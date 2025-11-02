#include "simple_logger.h"
#include "gfc_vector.h"
#include "gfc_input.h"

#include "gf3d_camera.h"

#include "camera_entity.h"

void camera_entity_free(Entity* self)
{
	CameraEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;
	free(self->data);
	//free(self);
}

void camera_entity_think(Entity* self)
{
	const Uint8* keystate = SDL_GetKeyboardState(NULL);
	GFC_Vector3D d, offset;
	CameraEntityData* data;
	float turnSpeed;
	if ((!self) || (!self->data)) return;
	data = self->data;
	if (!data->target)return;

	turnSpeed = 0.05;
	if (keystate[SDL_SCANCODE_LEFT])
	{
		data->angle += turnSpeed;
	}
	//if (gfc_input_command_down("walkright"))
	if (keystate[SDL_SCANCODE_RIGHT])
	{
		data->angle -= turnSpeed;
	}

	offset = gfc_vector3d(0, 1, 0);
	gfc_vector3d_rotate_about_z(&offset, data->angle);
	gfc_vector3d_copy(d, data->target->position);
	gfc_vector3d_scale(offset, offset, data->followDistance);
	//offset.z = data->followHeight;

	self->position.x = data->target->position.x - offset.x; //
	self->position.y = data->target->position.y - offset.y; //
	self->position.z = data->target->position.z + data->followHeight;
	gf3d_camera_look_at(data->target->position, &self->position);
}

Entity* camera_entity_spawn(GFC_Vector3D position, Entity* target)
{
	GFC_Vector3D dir = { 0 };
	Entity* self;
	CameraEntityData* data;
	self = entity_new();
	if (!self) return;
	data = gfc_allocate_array(sizeof(CameraEntityData), 1);
	if (!data)
	{
		free(self);
		return NULL;
	}
	self->data = data;

	self->position = position;
	self->think = camera_entity_think;
	self->free = camera_entity_free;
	data->target = target;
	data->followHeight = 5;
	data->followDistance = 50;
	data->angle = 0;

	return self;
}