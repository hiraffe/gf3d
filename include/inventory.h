#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include "gfc_list.h"

#define MAX_ITEMS 128

typedef enum
{
	II_PumpkinSeeds,
	II_Pumpkin,
	II_BrainSeeds,
	II_BrainBerries,
	II_MAX
}InventoryIndices;

typedef struct
{
	GFC_List *itemslist;
}Inventory;

void inventory_init(Inventory* inventory);

void inventory_cleanup(Inventory* inventory);

void inventory_add_item(Inventory* inventory, const char* name);

#endif