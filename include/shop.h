#ifndef __SHOP_H__
#define __SHOP_H__

#include "entity.h"
#include "inventory.h"
#include "ui.h"

typedef struct
{
	Inventory*		sell_list;
	Uint8			isOpen;
	UI*				menu;
}ShopEntityData;

/**
 * @brief spawn in a new shop
 * @return NULL on error, or a new shop
 */
Entity* shop_spawn(GFC_Vector3D position, const char* name);

/**
 * @brief free a previously loaded crop
 * @param world the world to free
 */
void shop_free(Entity* crop);

float shop_sell_item(Inventory* inv, Inventory* otherInv, int itemIndex);

void shop_sell_all(Entity* self, Entity* monster);

#endif