#include "simple_logger.h"

#include "plot.h"


void plot_draw(Entity* self, GFC_Vector3D lightPos, GFC_Color lightColor)
{
	GFC_Matrix4 modelMat;
	GFC_Vector3D rotation = { 0 };
	PlotEntityData* data;
	if ((!self) || (!self->data)) return;
	data = self->data;

	gfc_matrix4_from_vectors(modelMat, data->frontR_pos, self->rotation, self->scale);
	gf3d_mesh_draw(data->short_fence, modelMat, self->color, self->texture, lightPos, lightColor);

	gfc_matrix4_from_vectors(modelMat, data->frontL_pos, self->rotation, self->scale);
	gf3d_mesh_draw(data->short_fence, modelMat, self->color, self->texture, lightPos, lightColor);

	gfc_matrix4_from_vectors(modelMat, data->back_pos, self->rotation, self->scale);
	gf3d_mesh_draw(data->long_fence, modelMat, self->color, self->texture, lightPos, lightColor);

	gfc_matrix4_from_vectors(modelMat, data->sideR_pos, rotation, self->scale);
	gf3d_mesh_draw(data->sideways_fence, modelMat, self->color, self->texture, lightPos, lightColor);

	gfc_matrix4_from_vectors(modelMat, data->sideL_pos, rotation, self->scale);
	gf3d_mesh_draw(data->sideways_fence, modelMat, self->color, self->texture, lightPos, lightColor);
}

Entity* plot_spawn(GFC_Vector3D position, GFC_Color color)
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
	self->texture = gf3d_texture_load("models/dino/dino.png");
	self->position = position;
	self->color = color;
	self->rotation = gfc_vector3d(0, 0, 0);

	data->long_fence = gf3d_mesh_load("models/fence/long-fence.obj");
	data->short_fence = gf3d_mesh_load("models/fence/short-fence.obj");
	data->sideways_fence = gf3d_mesh_load("models/fence/long-fence-sideways.obj");
	data->door = door_spawn(position, color);

	gfc_vector3d_add(data->frontR_pos, position, gfc_vector3d(10, 0, 0));
	gfc_vector3d_add(data->frontL_pos, position, gfc_vector3d(-20, 0, 0));
	gfc_vector3d_add(data->sideR_pos, position, gfc_vector3d(30, 0, 0));
	gfc_vector3d_add(data->sideL_pos, position, gfc_vector3d(-20, 0, 0));
	gfc_vector3d_add(data->back_pos, position, gfc_vector3d(-20, 50, 0));


	self->draw = plot_draw;

	slog("Plot spawned: %s", self->name);

	return self;
}