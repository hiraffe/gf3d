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

Entity* shop_get_the();

/**
 * @brief spawn in a new shop
 * @return NULL on error, or a new shop
 */
Entity* shop_spawn(GFC_Vector3D position);

/**
 * @brief free a previously loaded crop
 * @param world the world to free
 */
void shop_free(Entity* crop);

#endif