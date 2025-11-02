#include "simple_logger.h"

#include "gfc_input.h"

#include "crop.h"
#include "camera_entity.h"
#include "monster.h"

void monster_free(Entity* self)
{
	MonsterEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;
	inventory_cleanup(&data->inventory);
	free(data);
	//entity_free(self);
}

static char *crop_held = "Pumpkin";

void get_crop_held()
{
	const Uint8* keystate = SDL_GetKeyboardState(NULL);

	if (keystate[SDL_SCANCODE_1])
		crop_held = "Pumpkin";
	if (keystate[SDL_SCANCODE_2])
		crop_held = "Brain Berries";
	if (keystate[SDL_SCANCODE_3])
		crop_held = "Ghost Pepper";
	if (keystate[SDL_SCANCODE_4])
		crop_held = "Candy Corn";
	if (keystate[SDL_SCANCODE_5])
		crop_held = "Chocolate";
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

	// get current item
	get_crop_held();

	// plant crops
	if (keystate[SDL_SCANCODE_Q])
	{
		GFC_Vector3D cropLocation = self->position;
		gfc_vector3d_add(cropLocation, cropLocation, gfc_vector3d(0,10,2));
		crop_spawn(cropLocation, crop_held);
		// if theres another crop in the area, dont plant it
	}

	//add function to harvest crops as well
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
	self->data = data;

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

	
	slog("Monster spawned: %s", self->name);
	return self;
}