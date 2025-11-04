#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_vector.h"
#include "gf2d_font.h"

#include "crop.h"
#include "camera_entity.h"
#include "monster.h"

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

Inventory* populate_inventory(Inventory* inv)
{
	int i;
	for (i = 0; i < 5; i++)
	{
		inventory_add_item(inv, "pumpkin_seeds");
		inventory_add_item(inv, "brain_seeds");
		inventory_add_item(inv, "pepper_seeds");
		inventory_add_item(inv, "corn_seeds");
		inventory_add_item(inv, "cocoa_seeds");
	}

	return inv;
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

	if (!nearest)
	{
		slog("There are no crops nearby for you to harvest");
		return;
	}

	nearest->_inuse = 0;
	entity_free(nearest);
	inventory_add_item(data->inventory, nearest->name);
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

	// get current item held
	set_item_held(self);
	gf2d_font_draw_line_tag("Item held:", FT_H1, GFC_COLOR_WHITE, gfc_vector2d(15, 15));
	//gf2d_font_draw_line_tag(data->item_held->displayname, FT_H1, GFC_COLOR_WHITE, gfc_vector2d(10, 10));

	// plant crops
	if (keystate[SDL_SCANCODE_Q])
	{
		if (data->item_held->count <= 0)
		{
			slog("No more %s left!", data->item_held->displayName);
		}
		else
		{
			GFC_Vector3D cropLocation = self->position;
			gfc_vector3d_add(cropLocation, cropLocation, gfc_vector3d(0, 10, 2));
			crop_spawn(cropLocation, data->item_held->crop);
			data->item_held->count--;
		}
		// if theres another crop in the area, dont plant it
	}

	//add function to harvest crops as well
	if (keystate[SDL_SCANCODE_E])
	{
		harvest_nearest_crop(self);
	}

	if (keystate[SDL_SCANCODE_I])
	{
		inventory_print(data->inventory);
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

void monster_free(Entity* self)
{
	MonsterEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;
	inventory_cleanup(&data->inventory);
	item_free(data->item_held);
	free(data);
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

	data->money = 100;
	inventory = inventory_new();
	inventory = populate_inventory(inventory);
	data->inventory = inventory;
	data->item_held = inventory_get_item_by_name(inventory, "pumpkin_seeds");

	self->think = monster_think;
	self->free = monster_free;

	slog("Monster spawned: %s", self->name);
	return self;
}