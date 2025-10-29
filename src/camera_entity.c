#include "simple_logger.h"
#include "gfc_vector.h"
#include "gfc_input.h"

#include "gf3d_camera.h"

#include "camera_entity.h"

typedef struct
{
	Entity* target;
	float followHeight;
	float followDistance;
	float angle;
}CameraEntityData;

void camera_entity_free(Entity* self)
{
	CameraEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;
	free(self->data);
	free(self);
}

void camera_entity_think(Entity* self)
{
	/*float move = 5;
	float turn = 0.05;
	float pitch = 0;
	float yaw = 0;
	GFC_Vector3D movement = { 0 };
	GFC_Vector3D dir = { 0 };
	CameraEntityData* data;
	if ((!self) || (!self->data)) return;

	if (gfc_input_command_down("walkforward"))
	{
		movement.y += move;
	}
	if (gfc_input_command_down("walkright"))
	{
		movement.x += move;
	}
	if (gfc_input_command_down("walkback"))
	{
		movement.y -= move;
	}
	if (gfc_input_command_down("walkleft"))
	{
		movement.x -= move;
	}
	gfc_vector3d_scale(dir, dir, movement.y);
	gfc_vector3d_add(self->position, self->position, movement);
	gfc_vector3d_add(data->target, data->target, movement);

	gfc_vector3d_normalize(&dir);
	gfc_vector3d_rotate_about_z(&dir, GFC_PI_HALFPI);

	gfc_vector3d_scale(dir, dir, movement.x);
	gfc_vector3d_add(self->position, self->position, movement);
	gfc_vector3d_add(data->target, data->target, movement);

	if (gfc_input_command_down("panleft"))
	{
		yaw += turn;
	}
	if (gfc_input_command_down("panright"))
	{
		yaw -= turn;
	}
	if (gfc_input_command_down("panup"))
	{
		pitch += turn;
	}
	if (gfc_input_command_down("pandown"))
	{
		pitch -= turn;
	}

	if (pitch)
	{
		data->target.z += pitch;
	}
	if(yaw)
	{
		gfc_vector3d_sub(dir, data->target, self->position);
		gfc_vector3d_normalize(&dir);
		gfc_vector3d_rotate_about_z(&dir, yaw);
		gfc_vector3d_add(data->target, self->position, dir);
	}
	gf3d_camera_look_at(data->target, &self->position);
	*/

	GFC_Vector3D d, offset;
	CameraEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;
	if (!data->target)return;

	offset = gfc_vector3d(0, 1, 0);
	gfc_vector3d_rotate_about_z(&offset, data->angle);
	gfc_vector3d_copy(d, data->target->position);
	gfc_vector3d_scale(offset, offset, data->followDistance);
	offset.z = data->followHeight;

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
	self->position = position;
	self->think = camera_entity_think;
	self->free = camera_entity_free;
	self->data = data;
	data->target = target;

	//gfc_vector3d_sub(dir, target, position);
	//gfc_vector3d_normalize(&dir);
	//gfc_vector3d_add(data->target, position, dir);
	//gf3d_camera_look_at(data->target, &self->position);

	return self;
}