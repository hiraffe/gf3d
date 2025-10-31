#include "body.h"

Body* body_new()
{
	Body* b;
	b = gfc_allocate_array(sizeof(Body), 1);
	if (!b) return NULL;

	b->volumes = gfc_list_new();

	return b;
}

Body* body_free(Body* b)
{
	if (!b) return NULL;
	if (b->volumes)
	{
		gfc_list_foreach(b->volumes, free);
		gfc_list_delete(b->volumes);
	}
	free(b);
}

void body_add_volume(Body *b, GFC_Primitive v)
{
	GFC_Primitive* p;
	if (!b) return;

	p = gfc_allocate_array(sizeof(GFC_Primitive), 1);
	memcpy(p, &v, sizeof(GFC_Primitive));
	gfc_list_append(b->volumes, p);
}

void body_set_colliison(Body* b, body_collide_func* collide, void *data)
{
	if ((!b) || (!collide)) return;
	b->onCollide = collide;
	b->data = data;
}

void body_reset_for_updates(Body* b, float factor)
{
	//sanity check
	b->stepPosition = b->position;
	gfc_vector3d_scale(b->stepVelocity, b->velocity, factor);
	b->stopped = 0;
}

int body_test_body(Body* a, Body* b)
{
	int i, c, j, d;
	GFC_Primitive *ap, *bp;
	GFC_Primitive apTest, bpTest;
	if ((!a) || (!b)) return 0;
	//for each primitive in a, test each primitive in b
	c = gfc_list_count(a->volumes);
	for (i = 0; i < c; i++)
	{
		ap = gfc_list_nth(a->volumes, i);
		apTest = gfc_primitive_offset(*ap, a->stepPosition);
		d = gfc_list_count(b->volumes);
		for (j = 0; j < d; j++)
		{
			bp = gfc_list_nth(b->volumes, j);
			bpTest = gfc_primitive_offset(*bp, b->stepPosition);
			//TODO: test if the two primitives, now in world space collide
			// if so, call callbacks on a AND b
			// if so, return 1
		}
	}
	return 0;
}