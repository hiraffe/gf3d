#include "simple_json.h"
#include "simple_logger.h"

#include "gfc_config.h"
#include "gf3d_obj_load.h"

#include "animal.h"
#include "shop.h"
#include "shop_menu.h"
#include "ui.h"
#include "npc.h"
#include "door.h"

#include "world.h"

//static World* theWorld; //do that thing yeah

void spawn_animals()
{
	GFC_Vector3D pos = gfc_vector3d(0,-100,0);
	Entity* a1 = animal_spawn(pos, "deer");
	Entity* a2 = animal_spawn(pos, "cow");
	Entity* a3 = animal_spawn(pos, "chicken");
	Entity* a4 = animal_spawn(pos, "lamb");
	Entity* a5 = animal_spawn(pos, "cat");
}

void spawn_shops()
{
	Entity* shop = shop_spawn(gfc_vector3d(-50, -50, 6), "Scary Seeds");
	Entity* shop2 = shop_spawn(gfc_vector3d(-70, -50, 6), "Terrifying Tools");
}

void spawn_npcs()
{
	Entity* npc = npc_spawn("npc1");
	Entity* npc2 = npc_spawn("npc2");
	Entity* npc3 = npc_spawn("npc3");
	Entity* npc4 = npc_spawn("npc4");
	Entity* npc5 = npc_spawn("npc5");
}

World* world_new()
{
	World* world;
	world = gfc_allocate_array(sizeof(World), 1);
	if (!world) return NULL;

	// load all the crop defs

	//spawn_animals();
	//spawn_shops(); //shop menus dont work for some reason (issue is in ui_new ?
	spawn_npcs();
	Entity* plot = plot_spawn(gfc_vector3d(-100, 100, 0), GFC_COLOR_WHITE);

	return world;
}

World* world_load(const char* filename)
{
	World* world;
	const char* str;
	SJson* json, * config;
	json = sj_load(filename);
	if (!json)
	{
		slog("failed to load world file %s", filename);
		return NULL;
	}
	world = world_new();
	if (!world)
	{
		slog("failed ot allocate a world for %s", filename);
		sj_free(json);
		return NULL;
	}
	config = sj_object_get_value(json, "world");
	if (!config)
	{
		slog("failed to parse a world for %s", filename);
		sj_free(json);
		sj_free(world);
		return NULL;
	}
	str = sj_object_get_string(config, "terrainMesh");
	world->terrain = gf3d_mesh_load(str);
	world->texture = gf3d_texture_load(sj_object_get_string(config, "terrainTexture"));
	world->lightColor = GFC_COLOR_WHITE;
	sj_object_get_color_value(config, "lightColor", &world->lightColor);
	sj_object_get_vector3d(config, "lightPos", &world->lightPos);
	sj_free(json);

	return world;
}

World* world_free(World* world)
{
	if (!world) return;
	gf3d_mesh_free(world->terrain);
	gf3d_texture_free(world->texture);
	world_free(world);
}

World* world_draw(World* world)
{
	GFC_Matrix4 modelMat;
	if (!world)return;
	gfc_matrix4_identity(modelMat);
	gf3d_mesh_draw(world->terrain, modelMat, GFC_COLOR_WHITE, world->texture, world->lightPos, world->lightColor);

	//entity system draw all entites in the world entity list
}

Uint8 world_edge_test(World* world, GFC_Vector3D start, GFC_Vector3D end)
{
	int i, j, c, d;
	GFC_Edge3D e;
	GFC_Triangle3D t;
	MeshPrimitive* primitive;
	if (!world) return 0;
	e = gfc_edge3d_from_vectors(start, end);
	c = gfc_list_count(world->terrain->primitives);
	for (i = 0; i < c; i++)
	{
		primitive = gfc_list_nth(world->terrain->primitives, 1);
		if ((!primitive) || (!primitive->objData)) continue;
		//set t
		d = primitive->objData->face_count;
		for (j = 0; j < d; i++)
		{
			t.a = primitive->objData->faceVertices[primitive->objData->outFace[j].verts[0]].vertex;
			t.b = primitive->objData->faceVertices[primitive->objData->outFace[j].verts[1]].vertex;
			t.c = primitive->objData->faceVertices[primitive->objData->outFace[j].verts[2]].vertex;
			//if (gfc_triangle_edge_test(e, t, contact))
			//{
			//	return 1;
			//}
		}
	}
	return 0;
}

