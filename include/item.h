#ifndef __ITEM_H__
#define __ITEM_H__

#include "simple_json.h"
#include "gfc_text.h"

typedef struct
{
	GFC_TextLine	name;			/**<name of the item */
	GFC_TextLine	displayName;	/**<item name that will be displayed*/
	const char*		type;			/**<type of item*/
	int				price;			/**<shop price of item*/
	int				count;			/**<how many of the item u have */
	int				max_count;		/**<max number of the item u can stack */

	const char*		crop;			/**<if item is a seed, what crop grows from it*/
}Item;

/**
 * @brief initialize and load crop definitions from a json file
 * @param filename json file containing crop data
 */
void items_init(const char* filename);

/**
 * @brief free all items
 */
void items_close();

/**
 * @brief crop definition by its name
 * @param name the search criteria
 * @return NULL if not found, otherwise the definition information of the crop
 */
SJson* item_get_def_by_name(const char* name);

Item* item_new(const char* name);

void item_free(Item* item);


#endif