#include "simple_logger.h"

#include "animal.h"

static SJson* animalDefs = NULL;

void animal_free(Entity* animal)
{
	animalDefs = NULL;
}

void animals_close()
{
	animalDefs = NULL;
}

void animal_think(Entity* self)
{
	AnimalEntityData* data;
	if (!self || !self->data) return;
	data = self->data;

	// Countdown idle timer
	data->idleTime -= 0.1f;
	if (data->idleTime > 0)
	{
		return; // still resting
	}

	// Compute direction to current target
	GFC_Vector3D dir;
	gfc_vector3d_sub(dir, data->target, self->position);
	float dist = gfc_vector3d_magnitude(dir);

	if (dist < 2.0f)
	{
		// reached target — choose a new one
		data->idleTime = (float)(rand() % 50) / 10.0f; // random idle 0–5s
		float angle = ((float)rand() / RAND_MAX) * GFC_PI * 2;
		float radius = ((float)rand() / RAND_MAX) * data->roamingRadius;

		data->target.x = data->home.x + cosf(angle) * radius;
		data->target.y = data->home.y + sinf(angle) * radius;
		data->target.z = self->position.z;

		return;
	}

	// Move toward target
	gfc_vector3d_normalize(&dir);
	GFC_Vector3D step;
	gfc_vector3d_scale(step, dir, data->speed);
	gfc_vector3d_add(self->position, self->position, step);

	// Optional: rotate to face movement direction
	self->rotation.z = atan2f(dir.y, dir.x) * (180.0f / GFC_PI);
}

void animals_init(const char* filename)
{
	SJson* json;
	json = sj_load(filename);
	animalDefs = sj_object_get_value(json, "animals");
	if (!animalDefs)
	{
		slog("animal definition file %s does not contain animals list", filename);
		sj_free(json);
		json = NULL;
		return;
	}
	atexit(animals_close);
}

SJson* animal_get_def_by_name(const char* name)
{
	int i, c;
	SJson* animal;
	const char* animalName = NULL;
	if (!name) return NULL;
	if (!animalDefs)
	{
		slog("no animal definitions loaded");
		return NULL;
	}
	c = sj_array_get_count(animalDefs);
	for (int i = 0; i < c; i++)
	{
		animal = sj_array_get_nth(animalDefs, i);
		if (!animal) continue;
		animalName = sj_object_get_value_as_string(animal, "name");
		if (!animalName) continue;
		if (gfc_strlcmp(name, animalName) == 0) {
			//found it
			return animal;
		}
	}
	slog("no animal found by name", name);
	return NULL;
}

Entity* animal_spawn(GFC_Vector3D position, const char* name)
{
	SJson* def;
	AnimalEntityData* data;
	Entity* self;
	self = entity_new();
	if (!self) return NULL;
	data = gfc_allocate_array(sizeof(AnimalEntityData), 1);
	if (!data)
	{
		free(self);
		return NULL;
	}
	self->data = data;

	def = animal_get_def_by_name(name);

	gfc_line_cpy(self->name, name);
	gfc_line_cpy(self->displayName, sj_object_get_value_as_string(def, "displayName"));
	self->entityType = "animal";
	self->mesh = gf3d_mesh_load(sj_object_get_string(def, "mesh"));
	self->texture = gf3d_texture_load(sj_object_get_value_as_string(def, "texture"));
	self->position = position;
	self->color = GFC_COLOR_WHITE;
	self->rotation = gfc_vector3d(0, 0, 0);

	data->home = position;
	data->target = position;
	data->roamingRadius = 300;
	data->speed = 0.2f;
	data->idleTime = 0;

	self->think = animal_think;
	self->free = animal_free;

	slog("Animal spawned: %s", name);
	return self;
}