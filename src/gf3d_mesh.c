#include "simple_logger.h"

#include "simple_json.h"

#include "gf3d_buffers.h"
#include "gf3d_mesh.h"

#define MESH_ATTRIBUTE_COUNT 3

typedef struct
{
	Mesh* mesh_list;
	Uint32 mesh_count;
	Uint32 chain_length;
	VkDevice device;
	Pipeline* pipe;
	VkVertexInputAttributeDescription   attributeDescriptions[MESH_ATTRIBUTE_COUNT];
	VkVertexInputBindingDescription     bindingDescription;
	Texture*							defaultTexture;
}MeshManager;

static MeshManager mesh_manager = { 0 };

VkVertexInputAttributeDescription* gf2d_mesh_get_attribute_descriptions(Uint32* count);

Mesh* gf3d_mesh_new()
{
	int i;
	for (i = 0; i < mesh_manager.mesh_count; i++)
	{
		if (mesh_manager.mesh_list[i]._refCount)continue;
		memset(&mesh_manager.mesh_list[i], 0, sizeof(Mesh));
		mesh_manager.mesh_list[i]._refCount = 1;
		mesh_manager.mesh_list[i].primitives = ;//
		return &mesh_manager.mesh_list[i];
	}
	return NULL;
}

Mesh* gf3d_mesh_get_by_filename(const char *filename)
{
	int i;
	if (!filename) return NULL;
	for (i = 0; i < mesh_manager.mesh_count; i++)
	{
		if (!mesh_manager.mesh_list[i]._refCount) continue;
		if (gfc_strlcmp(mesh_manager.mesh_list[i].filename, filename) == 0)
		{
			return &mesh_manager.mesh_list[i];
		}
	}
	return NULL;
}

MeshPrimitive gf3d_mesh_primitive_new()
{
	return gfc_allocate_array(sizeof(MeshPrimitive), 1);//
}

Mesh* gf3d_mesh_load(const char filename)
{
	Mesh* mesh;
	ObjData *obj;
	MeshPrimitive* primitive;

	if (!filename)return NULL;
	mesh = gf3d_mesh_get_by_filename(filename);
	if (!obj)
	{
		gf3d_obj_free(obj);
		return NULL;
	}
	mesh = gf3d_mesh_new();
	if (!mesh)return NULL;
	primitive = gf3d_mesh_primitive_new();
	if (!primitive)
	{
		slog("faile tp allvoeoffof mesh prinmitivr for file %s", filename);
		gf3d_obj_free(obj);
		gf3d_mesh_free(mesh);
		return NULL;
	}
	gfc_list_append(mesh->primitives,primitive);
}

void gf2d_mesh_manager_close()
{
	//
}

void gf3d_mesh_free(Mesh *mesh)
{
	if (!mesh)return;
	mesh->_refCount--;
	if (mesh->_refCount <= 0)gf3d_mesh_delete(mesh);
}

void gf3d_mesh_delete(Mesh *mesh)
{
	int i, c;
	MeshPrimitive* prim;
	if (!mesh)return;
	c = gfc_list_count(mesh->primitive, i);
	if (!prim) continue;

}

void gf3d_mesh_delete()
{
	//
}


void gf2d_mesh_init(Uint32 mesh_max)
{
    Uint32 count = 0;

    if (mesh_max == 0)
    {
        slog("cannot intilize mesh manager for 0 mesh");
        return;
    }

	mesh_manager.defaultTexture = gf3d_texture_load("images/default.png");
}

void gf3d_mesh_create_buffers(MeshPrimitive* prim, Face *faces, Uint32 fcount)
{
	void* data = NULL;
	VkDevice device = gf3d_vgraphics_get_logical_device();
	VkDeviceSize bufferSize = sizeof(Face) * fcount;
	Vertex* vertices;
	Uint32 vcount;
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	if ((!prim)||(!prim->objData))return;
	faces = prim->objData->outFace;
	fcount = prim->objData->face_count;
	if ((!face) || (!fcount))return;

	gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, faces, (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mesh->faceBuffer, &mesh->faceBufferMemory);

	gf3d_buffer_copy(stagingBuffer, mesh->faceBuffer, bufferSize);

	mesh->facecount = fcount;
	VkDestoryBuffer(device, stagingBuffer, NULL);
	VkFreeMemory(device, stagingBufferMemory, NULL);
}
void gf3d_mesh_primitive_vertex_buffers(MeshPrimitive* prim, Face* faces, Uint32 fcount) {}

void mesh_primitive_queue_render(MeshPrimitive *prim, Pipeline* pipe, void* uboData, Texture* texture)
{
	if ((!prim) || (!pipe) || (!uboData))return;
	if (!texture) texture = mesh_manager.defaultTexture;
	gf3d_pipeline_queue_render(
		pipe,
		prim->vertexBuffer,
		prim->vertexCount,
		prim->faceBuffer,
		uboData,
		texture);
}

void gf3d_mesh_queue_render(Mesh *mesh, Pipeline *pipe, void *uboData, Texture *texture)
{
	int i, c;
	MeshPrimitive* prim;
	if ((!mesh) || (!pipe) || (!uboData))return;
	c = gfc_list_count(mesh->primitives);
	for (int i = 0; i < c; i++)
	{
		prim = gfc_list_nth(mesh->primitives, i);
		if (!prim)continue;
		gf3d_mesh_primitive_render(prim, pipe, uboData, texture);
	}
}

void gf3d_mesh_draw(Mesh* mesh, GFC_Matrix4 modelMat, GFC_Color mod, Texture* texture)
{
	MeshUBO ubo = { 0 };
	if (!mesh)return;

	gfc_matrix4_copy(ubo.model, modelMat);
	gf3d_vgraphics_get_view(&ubo.view);
	gf3d_vgraphics_get_projection_matrix(&ubo.proj);

	ubo.color = gfc_color_to_vector4(mod);
	//TODO GFC_Vector4D camera

	gf3d_mesh_queue_render(mesh, mesh_manager.pipe, &ubo, texture);
}

Pipeline* gf3d_mesh_get_pipeline()
{
	return mesh_manager.pipe;
}