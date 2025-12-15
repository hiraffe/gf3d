#include "simple_logger.h"

#include "monster.h"

#include "animal.h"

static SJson* animalDefs = NULL;

void animal_free(Entity* self)
{
	AnimalEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;
	free(data);
}

void animals_close()
{
	animalDefs = NULL;
}

void animal_think(Entity* self)
{
	GFC_Vector3D dir;
	AnimalEntityData* data;
	if (!self || !self->data) return;
	data = self->data;

	if (data->state == AES_Grazing)
	{
		// reached target - start grazing
		data->idleTime -= 0.1f;
		if (data->idleTime > 0)
		{
			data->state == AES_Grazing;
			return; // still grazing
		}

		// choose a new target
		data->idleTime = (float)(rand() % 250) / 10.0f; // random idle 0–5s
		float angle = ((float)rand() / RAND_MAX) * GFC_PI * 2;
		float radius = ((float)rand() / RAND_MAX) * data->roamingRadius;

		if (self->inPlot)
		{
			// captive
			data->target.x = self->position.x + cosf(angle) * 50;
			data->target.y = self->position.y + sinf(angle) * 50;
			data->target.z = self->position.z;

			data->state = AES_Roaming;
			return; 
		}
		data->target.x = data->home.x + cosf(angle) * radius;
		data->target.y = data->home.y + sinf(angle) * radius;
		data->target.z = self->position.z;

		data->state = AES_Roaming;
	}

	if (data->state == AES_Roaming)
	{
		// compute direction to current target
		gfc_vector3d_sub(dir, data->target, self->position);
		float dist = gfc_vector3d_magnitude(dir);

		if (dist < 2.0f)
		{
			data->state = AES_Grazing;
			return;
		}
		
		// move toward target
		gfc_vector3d_normalize(&dir);
		GFC_Vector3D step;
		gfc_vector3d_scale(step, dir, data->speed);

		if (!entity_check_collision(self, step, self->collisionRadius))
		{
			//roation
			if (dir.x != 0 || dir.y != 0)
			{
				float angle = atan2(dir.y, dir.x);
				angle -= 30;
				self->rotation.z = angle;
			}
			
			gfc_vector3d_add(self->position, self->position, step);
			return;
		}

		/*
		if (self->inPlot)
		{
			gfc_vector3d_add(self->position, self->position, step);
			return;
		}
		*/

		data->state = AES_Grazing;
	}

	if (data->state == AES_Following)
	{
		//slog("%s following human", self->name);
		Entity* monster = monster_get_the();
		data->target = monster->position;
		self->collisionRadius = 0;
		gfc_vector3d_sub(dir, data->target, self->position);
		float dist = gfc_vector3d_magnitude(dir);

		if (dist < 9.0f)
		{
			return;
		}

		// move toward target
		gfc_vector3d_normalize(&dir);
		GFC_Vector3D step;
		gfc_vector3d_scale(step, dir, data->speed);

		if (dir.x != 0 || dir.y != 0)
		{
			float angle = atan2(dir.y, dir.x);
			angle -= 30;
			self->rotation.z = angle;
		}
			gfc_vector3d_add(self->position, self->position, step);
		return;
	}
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
	self->collisionRadius = 2;

	data->home = position;
	data->target = position;
	data->roamingRadius = 50;
	data->speed = 0.2f;
	data->idleTime = 0;

	self->think = animal_think;
	self->free = animal_free;

	slog("Animal spawned: %s", name);
	return self;
}