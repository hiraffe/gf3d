#include "simple_logger.h"

#include "gfc_input.h"

#include "camera_entity.h"
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
	//free(data);
	//free(self);
}

float gfc_angle_diff(float a, float b)
{
	float diff = fmodf(b - a + GFC_PI, GFC_PI * 2);
	if (diff < 0) diff += GFC_PI * 2;
	return diff - GFC_PI;
}

void monster_think(Entity* self)
{
	const Uint8* keystate = SDL_GetKeyboardState(NULL);
	GFC_Vector3D dir, cameraDir;
	float move = 0, moveSide = 0;
	float moveStep = 0.5;
	MonsterEntityData* data;
	if ((!self) || !(self->data)) return;
	data = self->data;
	if (!data->cam)return;
	CameraEntityData* camData;
	camData = data->cam->data;
	if (!camData) return;

	// --- input ---
	if (keystate[SDL_SCANCODE_W])
		move += moveStep;
	if (keystate[SDL_SCANCODE_S])
		move -= moveStep;
	if (keystate[SDL_SCANCODE_A])
		moveSide -= moveStep;
	if (keystate[SDL_SCANCODE_D])
		moveSide += moveStep;

	// --- get camera facing direction ---
	GFC_Vector3D forward = { 0, 1, 0 };
	GFC_Vector3D right = { 1, 0, 0 };
	gfc_vector3d_rotate_about_z(&forward, camData->angle);
	gfc_vector3d_rotate_about_z(&right, camData->angle);

	// --- apply movement ---
	GFC_Vector3D movement = { 0 };
	gfc_vector3d_scale(forward, forward, move);
	gfc_vector3d_scale(right, right, moveSide);
	gfc_vector3d_add(movement, forward, right);

	gfc_vector3d_add(self->position, self->position, movement);
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
	self->rotation = gfc_vector3d(0, 0, 135);
	self->velocity = gfc_vector3d(0, 0, 0);

	//void			(*draw)(struct Entity_S* self);
	self->think = monster_think;
	//void			(*update)(struct Entity_S* self);
	self->free = monster_free;

	self->data = data;
	slog("Monster spawned: %s", self->name);
	return self;
}