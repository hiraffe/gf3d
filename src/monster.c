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
	int max;
	Item* item;
	MonsterEntityData* data;
	if ((!self) || !(self->data)) return;
	data = self->data;

	if ((!data->inventory) || (!data->inventory->itemslist) || (data->inventory->itemslist->count <= 0))
	{
		data->item_held = NULL;
		data->item_index = 0;
		return;
	}

	max = data->inventory->itemslist->count;

	if (gfc_input_command_pressed("itemright"))
	{
		data->item_index = (data->item_index + 1) % max;
	}
	if (gfc_input_command_pressed("itemleft"))
	{
		data->item_index = (data->item_index - 1 + max) % max;  // prevents negatives
	}

	item = gfc_list_get_nth(data->inventory->itemslist, data->item_index);
	if (!item) return;

	data->item_held = item;
}

void select_nearest_crop(Entity* self)
{
	int i;
	Entity* nearest = NULL;
	float nearestDist = 10;
	EntitySystem entity_system = entity_get_system();
	MonsterEntityData* data = self->data;
	CropEntityData* cropData = NULL;

	for (i = 0; i < entity_system.entity_max; i++)
	{
		Entity* ent = &entity_system.entity_list[i];
		if (!ent->_inuse) continue;
		if (!ent->data) continue;
		if (ent->entityType != "crop") continue;

		// find the nearest crop
		if (gfc_vector3d_distance_between_less_than(self->position, ent->position, nearestDist))
		{
			cropData = ent->data;
			nearest = ent;
			nearestDist = gfc_vector3d_magnitude_between(self->position, ent->position);
		}
	}

	if ((!nearest)) return; // no crops nearby

	// harvest if ripe
	if (cropData->growth == C_RIPE)
	{
		inventory_add_item(data->inventory, nearest->name);
		entity_free(nearest);
		//data->item_held = gfc_list_get_nth(data->inventory->itemslist, data->item_index);
		Mix_PlayChannel(0, data->harvest_sound, 0);
	}

	// fertilize seeds
	if (strcmp(data->item_held->type, "fertilizer") == 0)
	{
		cropData->ripenTime *= data->item_held->speedMod;
		data->item_held->count--;
	}
}

void monster_think(Entity* self)
{
	const Uint8* keystate = SDL_GetKeyboardState(NULL);
	GFC_Vector3D dir, cameraDir;
	float move = 0, moveSide = 0;
	float moveStep = 0.5;
	GFC_Vector3D stepPosition;
	MonsterEntityData* data;
	if ((!self) || !(self->data)) return;
	data = self->data;
	if (!data->cam)return;
	CameraEntityData* camData;
	camData = data->cam->data;
	if (!camData) return;

	// --- movement function ---
	if (gfc_input_command_down("walkforward"))
		move += moveStep;
	if (gfc_input_command_down("walkback"))
		move -= moveStep;
	if (gfc_input_command_down("walkleft"))
		moveSide -= moveStep;
	if (gfc_input_command_down("walkright"))
		moveSide += moveStep;

	GFC_Vector3D forward = { 0, 1, 0 };
	GFC_Vector3D right = { 1, 0, 0 };
	gfc_vector3d_rotate_about_z(&forward, camData->angle);
	gfc_vector3d_rotate_about_z(&right, camData->angle);

	GFC_Vector3D movement = { 0 };
	gfc_vector3d_scale(forward, forward, move);
	gfc_vector3d_scale(right, right, moveSide);
	gfc_vector3d_add(movement, forward, right);

	stepPosition = self->position;
	gfc_vector3d_add(stepPosition, stepPosition, movement);

	if (!entity_check_collision(self, stepPosition, self->collisionRadius))
	{
		gfc_vector3d_add(self->position, self->position, movement);
	}

	// get current item held
	set_item_held(self);

	// --- plant crops ---
	if (gfc_input_command_pressed("use"))
	{
		if (data->item_held && data->item_held->type && strcmp(data->item_held->type, "seed") == 0)
		{
			slog("item: %s, type: %s", data->item_held->name, data->item_held->type);
			if (data->item_held->count > 0)
			{
				GFC_Vector3D cropLocation = self->position;
				gfc_vector3d_add(cropLocation, cropLocation, gfc_vector3d(0, 10, 0));
				gfc_vector3d_sub(cropLocation, cropLocation, gfc_vector3d(0, 0, 6));
				crop_spawn(cropLocation, data->item_held->crop);
				Mix_PlayChannel(0, data->plant_sound, 0);
				data->item_held->count--;
			}
		}
	}

	// --- harvest crops ---
	if (gfc_input_command_pressed("select"))
	{
		select_nearest_crop(self);
	}

	// --- print inventory ---
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
	//self->mesh = gf3d_mesh_load("models/dino/dino.obj");
	self->mesh = gf3d_mesh_load("models/character/body.obj");
	self->texture = gf3d_texture_load("models/dino/dino.png");
	self->position = position;
	self->color = color;
	self->rotation = gfc_vector3d(0, 0, 135);
	self->velocity = gfc_vector3d(0, 0, 0);
	self->collisionRadius = 4;

	data->gold = 100;
	inventory = inventory_new();
	inventory_add_item(inventory, "hoe");
	data->inventory = inventory;
	data->item_held = inventory_get_item_by_name(inventory, "hoe");
	data->item_index = 0;

	data->harvest_sound = Mix_LoadWAV("audio/effects/harvest.wav");
	data->plant_sound = Mix_LoadWAV("audio/effects/plant.wav");

	self->think = monster_think;
	//self->update = monster_update;
	self->free = monster_free;

	slog("Monster spawned: %s", self->name);
	theMonster = self;
	return self;
}