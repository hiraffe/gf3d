#include "simple_logger.h"

#include "monster.h"
#include "door.h"

#include "plot.h"

Uint8 plot_inside(Entity* plot, GFC_Vector3D pos)
{
	PlotEntityData* data;
	if ((!plot)||(!plot->data)) return 0;
	data = plot->data;

	if (pos.x >= data->aabb_min.x && pos.x <= data->aabb_max.x && pos.y >= data->aabb_min.y && pos.y <= data->aabb_max.y)
	{
		return 1;
	}
	return 0;
}

Uint8 plot_inside_doorway(Entity* plot, GFC_Vector3D pos)
{
	PlotEntityData* data;
	if ((!plot) || (!plot->data)) return 0;
	data = plot->data;

	if (pos.x >= data->door_min.x && pos.x <= data->door_max.x && pos.y >= data->door_min.y && pos.y <= data->door_max.y)	
	{
		return 1;
	}
	return 0;
}

void plot_think(Entity* self)
{
	PlotEntityData* data;
	if ((!self) || (!self ->data)) return 0;
	data = self->data;

	Entity* monster = monster_get_the();
	if (plot_inside(self, monster->position))
	{
		slog("monster inside");
	}
}

void plot_draw(Entity* self, GFC_Vector3D lightPos, GFC_Color lightColor)
{
	GFC_Matrix4 modelMat;
	GFC_Vector3D rotation = { 0 };
	PlotEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;

	if (strcmp(data->size, "small") == 0) 
	{
		gfc_matrix4_from_vectors(modelMat, data->frontR_pos, self->rotation, self->scale);
		gf3d_mesh_draw(data->short_fence, modelMat, self->color, self->texture, lightPos, lightColor);

		gfc_matrix4_from_vectors(modelMat, data->frontL_pos, self->rotation, self->scale);
		gf3d_mesh_draw(data->short_fence, modelMat, self->color, self->texture, lightPos, lightColor);
	}
	else
	{
		gfc_matrix4_from_vectors(modelMat, data->frontR_pos, self->rotation, self->scale);
		gf3d_mesh_draw(data->long_fence, modelMat, self->color, self->texture, lightPos, lightColor);

		gfc_matrix4_from_vectors(modelMat, data->frontL_pos, self->rotation, self->scale);
		gf3d_mesh_draw(data->long_fence, modelMat, self->color, self->texture, lightPos, lightColor);

		GFC_Vector3D back;
		gfc_vector3d_add(back, gfc_vector3d(50, 0, 0), data->back_pos);
		gfc_matrix4_from_vectors(modelMat, back, self->rotation, self->scale);
		gf3d_mesh_draw(data->long_fence, modelMat, self->color, self->texture, lightPos, lightColor);
	}

	gfc_matrix4_from_vectors(modelMat, data->back_pos, self->rotation, self->scale);
	gf3d_mesh_draw(data->long_fence, modelMat, self->color, self->texture, lightPos, lightColor);

	gfc_matrix4_from_vectors(modelMat, data->sideR_pos, rotation, self->scale);
	gf3d_mesh_draw(data->sideways_fence, modelMat, self->color, self->texture, lightPos, lightColor);

	gfc_matrix4_from_vectors(modelMat, data->sideL_pos, rotation, self->scale);
	gf3d_mesh_draw(data->sideways_fence, modelMat, self->color, self->texture, lightPos, lightColor);
}

void plot_set_size(Entity* self)
{
	PlotEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;
	
	if (strcmp(data->size, "small") == 0)
	{
		gfc_vector3d_add(data->frontR_pos, self->position, gfc_vector3d(5, 0, 0));
		gfc_vector3d_add(data->frontL_pos, self->position, gfc_vector3d(-25, 0, 0));
		gfc_vector3d_add(data->sideR_pos, self->position, gfc_vector3d(25, 0, 0));
		gfc_vector3d_add(data->sideL_pos, self->position, gfc_vector3d(-25, 0, 0));
		gfc_vector3d_add(data->back_pos, self->position, gfc_vector3d(-25, 50, 0));
	}
	else
	{
		gfc_vector3d_add(data->frontR_pos, self->position, gfc_vector3d(5, 0, 0));
		gfc_vector3d_add(data->frontL_pos, self->position, gfc_vector3d(-55, 0, 0));
		gfc_vector3d_add(data->sideR_pos, self->position, gfc_vector3d(55, 0, 0));
		gfc_vector3d_add(data->sideL_pos, self->position, gfc_vector3d(-55, 0, 0));
		gfc_vector3d_add(data->back_pos, self->position, gfc_vector3d(-55, 50, 0));
	}

	// AABB calculation
	float halfW = (strcmp(data->size, "small") == 0) ? 25 : 55;
	float frontOffset = 5;        // your frontR offset
	float backOffset = 50;       // your back_pos offset

	// Convert to world coordinates
	data->aabb_min = gfc_vector3d(
		self->position.x - halfW,
		self->position.y,
		-9999  // Z not relevant for top-down
	);

	data->aabb_max = gfc_vector3d(
		self->position.x + halfW,
		self->position.y + backOffset,
		9999
	);

	// Door opening width
	float doorWidth = 10;
	float halfDoor = doorWidth * 0.5f;

	// Y position of the front fence
	float frontY = self->position.y;

	// Doorway AABB
	data->door_min = gfc_vector3d(
		self->position.x - halfDoor,
		frontY - 5,   // let them enter slightly before touching fence
		-9999
	);

	data->door_max = gfc_vector3d(
		self->position.x + halfDoor,
		frontY + 5,
		9999
	);
}

Entity* plot_spawn(GFC_Vector3D position, const char* size)
{
	Entity* self;
	PlotEntityData* data;
	self = entity_new();
	if (!self) return NULL;
	data = gfc_allocate_array(sizeof(PlotEntityData), 1);
	if (!data)
	{
		free(self);
		return NULL;
	}
	self->data = data;

	gfc_line_cpy(self->name, "plot");
	self->entityType = "plot";
	self->mesh = gf3d_mesh_load("models/fence/closed-door.obj");
	self->texture = gf3d_texture_load("models/fence/wood-texture.png");
	self->position = position;
	self->color = GFC_COLOR_WHITE;
	self->rotation = gfc_vector3d(0, 0, 0);
	self->collisionRadius = 0;

	data->size = size;
	data->long_fence = gf3d_mesh_load("models/fence/long-fence.obj");
	data->short_fence = gf3d_mesh_load("models/fence/short-fence.obj");
	data->sideways_fence = gf3d_mesh_load("models/fence/long-fence-sideways.obj");
	data->door = door_spawn(position, self->color);

	plot_set_size(self);

	self->think = plot_think;
	self->draw = plot_draw;

	slog("Plot spawned: %s", self->name);

	return self;
}