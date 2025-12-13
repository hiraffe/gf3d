#include "simple_logger.h"

#include "plot.h"
#include "door.h"

#include "entity.h"

static EntitySystem entity_system = { 0 };

EntitySystem entity_get_system()
{
	return entity_system;
}

Entity* entity_new()
{
	int i;
	if (entity_system.entity_list)
	{
		for (i = 0; i < entity_system.entity_max; i++)
		{
			if (!entity_system.entity_list[i]._inuse)
			{
				//set all defaults here
				entity_system.entity_list[i]._inuse = 1;
				entity_system.entity_list[i].color = GFC_COLOR_WHITE;
				entity_system.entity_list[i].scale = gfc_vector3d(1, 1, 1);
				entity_system.entity_list[i].doGenericUpdate = 1;

				return &entity_system.entity_list[i];
			}
		}
	}
	return NULL;
}

void entity_free(Entity* ent)
{
	if (!ent)return;
	if (ent->free)ent->free(ent);
	gf3d_mesh_free(ent->mesh);
	gf3d_texture_free(ent->texture);
	memset(ent, 0, sizeof(Entity));
	ent->_inuse = 0;
}

void entity_system_close()
{
	int i;
	if (entity_system.entity_list)
	{
		for (i = 0; i < entity_system.entity_max; i++)
		{
			if (entity_system.entity_list[i]._inuse)
			{
				entity_free(&entity_system.entity_list[i]);
			}
		}
		free(entity_system.entity_list);
	}
	memset(&entity_system,0,sizeof(EntitySystem));
	slog("entity system closed");
}

void entity_system_init(Uint32 max_ents)
{
	if (!max_ents)
	{
		slog("cannot init entity system with zero ents");
		return;
	}
	entity_system.entity_list = gfc_allocate_array(sizeof(Entity), max_ents);
	if (!entity_system.entity_list)
	{
		slog("failed to allocate %i entities for the system", max_ents);
		return;
	}
	entity_system.entity_max = max_ents;
	atexit(entity_system_close);

	slog("entity system initialized");
}

void entity_draw(Entity* ent, GFC_Vector3D lightPos, GFC_Color lightColor)
{
	GFC_Matrix4 modelMat;
	if(!ent)return;

	if (ent->draw)
	{
		ent->draw(ent, lightPos, lightColor);
		return;
	}

	if (!ent->mesh)
	{
		//slog("no mesh data for %s", ent->name);
		return;
	}
	
	gfc_matrix4_from_vectors(
		modelMat,
		ent->position,
		ent->rotation,
		ent->scale);
	
	gf3d_mesh_draw(
		ent->mesh,
		modelMat,
		ent->color,
		ent->texture,
		lightPos,
		lightColor);
}

void entity_system_draw_all(GFC_Vector3D lightPos, GFC_Color lightColor)
{
	int i;
	for (i = 0; i < entity_system.entity_max; i++)
	{
		if (entity_system.entity_list[i]._inuse)
		{
			entity_draw(&entity_system.entity_list[i], lightPos, lightColor);
			//slog("entity drawn");
		}
	}
}

void entity_think(Entity *ent)
{
	if (!ent) return;
	if (ent->think)ent->think(ent);
}

void entity_system_think_all()
{
	int i;
	for (i = 0; i < entity_system.entity_max; i++)
	{
		if (entity_system.entity_list[i]._inuse)
		{
			entity_think(&entity_system.entity_list[i]);
		}
	}
}

void entity_update(Entity* ent)
{
	if (!ent)return;
	// all the generic updates
	if (ent->doGenericUpdate)
	{
		gfc_vector3d_add(ent->position, ent->position, ent->velocity);
	}
}

void entity_system_update_all()
{
	int i;
	for (i = 0; i < entity_system.entity_max; i++)
	{
		if (entity_system.entity_list[i]._inuse)
		{
			entity_update(&entity_system.entity_list[i]);
		}
	}
}

int entity_check_collision(Entity* self, GFC_Vector3D newPos, float radius)
{
	for (int i = 0; i < entity_system.entity_max; i++)
	{
		Entity* other = &entity_system.entity_list[i];
		if (!other->_inuse) continue;
		if (other == self) continue;

		// plots collision
		if (strcmp(other->entityType, "plot") == 0)
		{
			if ((!other) || (!other->data)) return 0;
			PlotEntityData* data = other->data;

			Uint8 wasInside = plot_inside(other, self->position);
			Uint8 willBeInside = plot_inside(other, newPos);

			if (wasInside && !willBeInside)
			{
				if (door_is_open(data->door) && plot_inside_doorway(other, newPos))
				{
					return 0;
				}
				return 1;
			}
			if (!wasInside && willBeInside)
			{
				if (door_is_open(data->door) && plot_inside_doorway(other, newPos))
				{
					return 0;
				}
				return 1;
			}
			if (wasInside && willBeInside)
			{
				self->inPlot = 1;
			}
			else
			{
				self->inPlot = 0;
			}
		}


		if (other->collisionRadius <= 0) continue; // skip non-collidable

		float combinedRadius = radius + other->collisionRadius;
		if (gfc_vector3d_distance_between_less_than(newPos, other->position, combinedRadius))
		{
			return 1; // collision detected
		}
	}
	return 0; // no collision
}
