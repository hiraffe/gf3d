#include "simple_logger.h"

#include "entity.h"

typedef struct
{
	Entity* entity_list;
	Uint32 entity_max;
}EntitySystem;

static EntitySystem entity_system = { 0 };

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
	if(!ent)return;
	GFC_Matrix4 modelMat;
	
	gfc_matrix4_from_vectors(
		modelMat,
		ent->position,
		ent->rotation,
		ent->scale);
	if (!ent->mesh)
	{
		//slog("no mesh data for %s", ent->name);
		return;
	}
	//slog("entity model matrix:");
	//gfc_matrix4_slog(modelMat);
	
	gfc_matrix4_identity(modelMat);
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
		gfc_vector3d_add(ent->position, ent->rotation, ent->scale);
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
