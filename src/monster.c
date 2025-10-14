#include "simple_logger.h"

#include "monster.h"

void monster_think(Entity* self)
{
	if (!self)return;
	if (fabs(self->position.z) > 10); self->velocity.z *= -1;
	
}

Entity *monster_spawn(GFC_Vector3D position, GFC_Color color)
{
	Entity* self;
	self = entity_new();
	if (!self)return;

	//populate monster data
	gfc_line_cpy(self->name, "notAgumon");
	self->mesh = gf3d_mesh_load("models/dino/dino.obj");
	self->texture = gf3d_texture_load("models/dino/dino.png");
	self->color = color;
	self->position = position;
	//self->velocity.z = gfc_crandom();

	self->rotation.z = 180;
	self->velocity.x = 0;

	//void			(*draw)(struct Entity_S* self);
	self->think = monster_think;
	//void			(*update)(struct Entity_S* self);
	slog("End of monster spawn");
	return self;
}