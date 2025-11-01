#include "simple_logger.h"

#include "crop.h"

static SJson* cropDefs = NULL;

void crop_free(Entity* crop)
{
	//
}

void crop_think(Entity* self)
{
	//
}

SJson* crop_load(const char* filename)
{
	SJson* json;

	if (!filename)
	{
		slog("no filename provided for crop initialization");
		return;
	}
	json = sj_load(filename);
	if (!json)
	{
		slog("failed to load the json for the crop definition");
		return;
	}
	cropDefs = sj_object_get_value(json, "crops");
	if (!cropDefs)
	{
		slog("crop definition file %s does not contain crops list", filename);
		sj_free(json);
		json = NULL;
		return;
	}
	//atexit(crop_close);
}

SJson* get_crop_by_name(const char *name)
{
	int i, c;
	SJson* crop;
	const char* cropName = NULL;
	if (!name) return NULL;
	if (!cropDefs)
	{
		slog("no crop definitions loaded");
		return NULL;
	}
	c = sj_array_get_count(cropDefs);
	for (int i = 0; i < c; i++)
	{
		crop = sj_array_get_nth(cropDefs, i);
		if (!crop) continue;
		cropName = sj_object_get_value_as_string(crop, "name");
		if (!cropName) continue;
		if (gfc_strlcmp(name, cropName) == 0) {
			//found it
			return crop;
		}
	}
	slog("no crop found by name", name);
	return NULL;
}

Entity* crop_spawn(GFC_Vector3D position, const char* name)
{
	SJson* def;
	Entity* self;
	CropEntityData* data;
	self = entity_new();
	if (!self) return NULL;
	data = gfc_allocate_array(sizeof(CropEntityData), 1);
	if (!data)
	{
		free(self);
		return NULL;
	}
	self->data = data;

	//populate data
	def = get_crop_by_name(name);

	gfc_line_cpy(self->name, sj_object_get_value_as_string(def, "name"));
	self->mesh = gf3d_mesh_load(sj_object_get_value_as_string(def, "cropMesh"));
	
	self->position = position;
	self->color = GFC_COLOR_WHITE;
	self->rotation = gfc_vector3d(0, 0, 135);

	data->unripeTexture = gf3d_texture_load(sj_object_get_value_as_string(def, "unripeTexture"));
	sj_object_get_value_as_int(def, "y", &data->ripenTime);

	//void			(*draw)(struct Entity_S* self);
	self->think = crop_think;
	//void			(*update)(struct Entity_S* self);
	self->free = crop_free;

	slog("Monster spawned: %s", self->name);
	return self;
}





