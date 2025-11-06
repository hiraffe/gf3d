#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_vector.h"
#include "gf2d_font.h"

#include "crop.h"
#include "camera_entity.h"
#include "monster.h"

static Entity* theMonster = NULL;

Entity* monster_get_the()
{
	return theMonster;
}

void monster_free(Entity* self)
{
	MonsterEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;
	inventory_cleanup(&data->inventory);
	item_free(data->item_held);
	free(data);
}

void set_item_held(Entity* self)
{
	const Uint8* keystate = SDL_GetKeyboardState(NULL);
	MonsterEntityData* data;
	if ((!self) || !(self->data)) return;
	data = self->data;

	if (keystate[SDL_SCANCODE_1])
		data->item_held = inventory_get_item_by_name(data->inventory, "pumpkin_seeds");
	if (keystate[SDL_SCANCODE_2])
		data->item_held = inventory_get_item_by_name(data->inventory, "brain_seeds");
	if (keystate[SDL_SCANCODE_3])
		data->item_held = inventory_get_item_by_name(data->inventory, "pepper_seeds");
	if (keystate[SDL_SCANCODE_4])
		data->item_held = inventory_get_item_by_name(data->inventory, "corn_seeds");
	if (keystate[SDL_SCANCODE_5])
		data->item_held = inventory_get_item_by_name(data->inventory, "cocoa_seeds");
}

void harvest_nearest_crop(Entity* self)
{
	int i;
	Entity* nearest = NULL;
	float nearestDist = 10;
	EntitySystem entity_system = entity_get_system();
	MonsterEntityData* data = self->data;
	CropEntityData* cropData;

	for (i = 0; i < entity_system.entity_max; i++)
	{
		Entity* ent = &entity_system.entity_list[i];
		if (!ent->_inuse) continue;
		if (!ent->data) continue;
		if (ent->entityType != "crop") continue;

		cropData = ent->data;
		if (cropData->growth != C_RIPE) continue; // only harvest ripe crops
		if(gfc_vector3d_distance_between_less_than(self->position, ent->position, nearestDist))
		{
			nearest = ent;
			nearestDist = gfc_vector3d_magnitude_between(self->position, ent->position);
		}
	}

	if (!nearest) return; //no crops nearby

	inventory_add_item(data->inventory, nearest->name);
	nearest->_inuse = 0;
	entity_free(nearest);
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
	if (gfc_input_command_down("walkforward"))
		move += moveStep;
	if (gfc_input_command_down("walkback"))
		move -= moveStep;
	if (gfc_input_command_down("walkleft"))
		moveSide -= moveStep;
	if (gfc_input_command_down("walkright"))
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

	// get current item held
	set_item_held(self);

	// plant crops
	if (gfc_input_command_pressed("plant"))
	{
		if (strcmp(data->item_held->type, "seed") == 0)
		{
			if (data->item_held->count > 0)
			{
				GFC_Vector3D cropLocation = self->position;
				gfc_vector3d_add(cropLocation, cropLocation, gfc_vector3d(0, 10, 2));
				crop_spawn(cropLocation, data->item_held->crop);
				data->item_held->count--;
			}
			else
			{
				//slog("No more %s left!", data->item_held->displayName);
			}
			// if theres another crop in the area, dont plant it
		}
	}

	//add function to harvest crops as well
	if (gfc_input_command_pressed("select"))
	{
		harvest_nearest_crop(self);
	}

	// get inventory
	inventory_update(data->inventory);
	if (gfc_input_command_pressed("inventory"))
	{
		inventory_print(data->inventory);
	}

	// get position
	/*
	if (gfc_input_command_pressed("select"))
	{
		slog("%i, %i, %i", self->position.x, self->position.y, self->position.z);
	}
	*/
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
	Inventory* inventory;
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
	self->entityType = "monster";
	self->mesh = gf3d_mesh_load("models/dino/dino.obj");
	self->texture = gf3d_texture_load("models/dino/dino.png");
	self->position = position;
	self->color = color;
	self->rotation = gfc_vector3d(0, 0, 135);
	self->velocity = gfc_vector3d(0, 0, 0);

	data->gold = 200;
	inventory = inventory_new();
	inventory_add_item(inventory, "hoe");
	data->inventory = inventory;
	data->item_held = inventory_get_item_by_name(inventory, "hoe");

	self->think = monster_think;
	self->free = monster_free;

	slog("Monster spawned: %s", self->name);
	theMonster = self;
	return self;
}