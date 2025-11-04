#include "simple_logger.h"

#include "crop.h"

static SJson* cropDefs = NULL;

void crop_free(Entity* crop)
{
	cropDefs = NULL;
}

void crop_think(Entity* self)
{
	Uint32 currentTime;
	float delay;
	CropEntityData* data;
	if ((!self) || !(self->data)) return;
	data = self->data;

	currentTime = SDL_GetTicks();
	delay = data->ripenTime * 1000;

	if (data->growth == C_SEED)
	{
		if (currentTime - data->spawnTime >= delay / 2)
		{
			data->growth = C_UNRIPE;
			self->mesh = data->cropMesh;
			self->texture = data->unripeTexture;
		}
	}
	else if (data->growth == C_UNRIPE)
	{
		if (currentTime - data->spawnTime >= delay)
		{
			data->growth = C_RIPE;
			self->texture = data->ripeTexture;
		}
	}
}

void crops_init(const char* filename)
{
	SJson* json;
	json = sj_load(filename);
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

SJson* crop_get_def_by_name(const char *name)
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
	const char* cmesh, *rtexture, *utexture;
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
	def = crop_get_def_by_name(name);

	gfc_line_cpy(self->name, name);
	gfc_line_cpy(self->displayName, sj_object_get_value_as_string(def, "displayName"));
	self->entityType = "crop";
	self->mesh = gf3d_mesh_load("models/crops/test-seed.obj");
	self->texture = gf3d_texture_load("models/crops/green.png");
	self->position = position;
	self->color = GFC_COLOR_WHITE;

	gfc_line_cpy(data->seed, sj_object_get_value_as_string(def, "seed"));
	data->cropMesh = gf3d_mesh_load(sj_object_get_string(def, "cropMesh"));
	data->ripeTexture = gf3d_texture_load(sj_object_get_value_as_string(def, "ripeTexture"));
	data->unripeTexture = gf3d_texture_load(sj_object_get_value_as_string(def, "unripeTexture"));
	sj_object_get_value_as_float(def, "ripenTime", &data->ripenTime);
	data->spawnTime = SDL_GetTicks();

	self->think = crop_think;
	self->free = crop_free;

	slog("Crop spawned: %s", name);
	return self;
}