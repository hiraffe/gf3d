#include "simple_logger.h"

#include "monster.h"

#include "shop.h"

void shop_free(Entity* self)
{
	ShopEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;
	inventory_cleanup(&data->sell_list);
	free(data);
}

float shop_item_get_price(Inventory* inv, const char* itemName)
{
	int i;
	Item* item;
	
	item = inventory_get_item_by_name(inv, itemName);
	if (!item)return;
	const char* name = item->name;
	if (strcmp(name, itemName) == 0)
	{
		return item->price;
	}
	return 0;
}


void shop_sell_all(Entity* self, Entity* monster)
{
	Inventory* inv, * monsterInv;
	Item* item;
	ShopEntityData* data;
	MonsterEntityData* monsterData;
	if ((!self) || (!self->data) || (!monster) || (!monster->data)) return;
	data = self->data;
	inv = data->sell_list;
	monsterData = monster->data;
	monsterInv = monsterData->inventory;
	
	float totalGold = 0;
	for (int i = 0; i < inv->itemslist->count; i++)
	{
		item = gfc_list_get_nth(inv->itemslist, i);
		if (item->count <= 0) continue;

		float price = shop_item_get_price(inv, item->name);
		if (price > 0)
		{
			float gain = price * item->count;
			totalGold += gain;
			slog("Sold %d x %s for %.2f gold", item->count, item->name, gain);
			for (int j = 0; j < item->count; j++) {
				inventory_add_item(monsterInv, item->name);
			}
			item->count = 0; // emptied after selling
		}
	}
	monsterData->gold -= totalGold;
	slog("Total spent: %.2f gold (new balance: %.2f)", totalGold, monsterData->gold);
}


void shop_think(Entity* self)
{
	const Uint8* keystate = SDL_GetKeyboardState(NULL);
	Entity* monster = monster_get_the();
	if (!monster) return;

	if (gfc_vector3d_magnitude_between(monster->position, self->position) < 50)
	{
		if (keystate[SDL_SCANCODE_E])
		{
			slog("Open shop...");
			//market_ui_open();
			shop_sell_all(self, monster);
		}
	}
}

Inventory* shop_populate(Inventory* inv)
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

Entity* shop_spawn(GFC_Vector3D position)
{
	Inventory* inv;
	Entity* self;
	ShopEntityData* data;
	self = entity_new();
	if (!self) return NULL;
	data = gfc_allocate_array(sizeof(ShopEntityData), 1);
	if (!data)
	{
		free(self);
		return NULL;
	}
	self->data = data;

	//populate data
	gfc_line_cpy(self->name, "totallyNotAgumon");
	gfc_line_cpy(self->displayName, "Seed Shop");
	self->entityType = "market";
	self->mesh = gf3d_mesh_load("models/dino/dino.obj");
	self->texture = gf3d_texture_load("models/dino/dino.png");
	self->position = position;
	self->color = GFC_COLOR_WHITE;

	inv = inventory_new();
	inv = shop_populate(inv);
	data->sell_list = inv;

	self->think = shop_think;
	self->free = shop_free;

	slog("Shop spawned: %s", self->name);
	return self;
}