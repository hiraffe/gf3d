#include "simple_logger.h"

#include "item.h"

static SJson* itemJson = NULL;
static SJson* itemDefs = NULL;

void items_close()
{
	if (itemJson)
	{
		sj_free(itemJson);
	}
	itemJson = NULL;
	itemDefs = NULL;
}

void items_init(const char* filename)
{
	SJson* json;
	json = sj_load(filename);
	itemDefs = sj_object_get_value(json, "items");
	if (!itemDefs)
	{
		slog("item definition file %s does not contain items list", filename);
		sj_free(json);
		json = NULL;
		return;
	}
	atexit(items_close);
}

SJson* item_get_def_by_name(const char* name)
{
	int i, c;
	SJson* item;
	const char* itemName = NULL;
	if (!name) return NULL;
	if (!itemDefs)
	{
		slog("no item definitions loaded");
		return NULL;
	}
	c = sj_array_get_count(itemDefs);
	for (int i = 0; i < c; i++)
	{
		item = sj_array_get_nth(itemDefs, i);
		if (!item) continue;
		itemName = sj_object_get_value_as_string(item, "name");
		if (!itemName) continue;
		if (gfc_strlcmp(name, itemName) == 0) {
			//found it
			return item;
		}
	}
	slog("no item found by name", name);
	return NULL;
}

Item* item_new(const char* name)
{
	SJson* def;
	Item* item;

	def = item_get_def_by_name(name);
	if (!def)
	{
		slog("no item definition for %s", name);
		return NULL;
	}

	item = gfc_allocate_array(sizeof(Item), 1);
	if (!item) return NULL;

	gfc_line_cpy(item->name, sj_object_get_value_as_string(def, "name"));
	gfc_line_cpy(item->displayName, sj_object_get_value_as_string(def, "displayName"));
	gfc_line_cpy(item->type, sj_object_get_value_as_string(def, "type"));
	sj_object_get_value_as_int(def, "price", &item->price);
	item->count = 1;

	if (gfc_strlcmp(item->type, "seed") == 0)
	{
		item->crop = sj_object_get_value_as_string(def, "crop");
	}

	slog("Item added: %s", item->name);
	return item;
}

void item_free(Item* item)
{
	if (!item) return;
}