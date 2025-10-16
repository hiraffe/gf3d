#include "simple_logger.h"

#include "simple_json.h"

#include "gf3d_vgraphics.h"
#include "gf3d_obj_load.h"
#include "gfc_color.h"
#include "gf3d_camera.h"

#include "gf3d_mesh.h"

#define MESH_ATTRIBUTE_COUNT 3

extern int __DEBUG;

typedef struct
{
    Mesh* mesh_list;
    Uint32                                  mesh_count;
    Uint32                                  chain_length;     /**<length of swap chain*/
    VkDevice                                device;           /**<logical vulkan device*/
    Pipeline*                               pipe;             /**<the pipeline associated with mesh rendering*/
    Pipeline*                               sky_pipe;         /**<the pipeline associated with skybox rendering*/
    VkBuffer                                faceBuffer;
    VkDeviceMemory                          faceBufferMemory;
    VkVertexInputAttributeDescription       attributeDescriptions[MESH_ATTRIBUTE_COUNT];
    VkVertexInputBindingDescription         bindingDescription;
    float                                   drawOrder;
    Texture* defaultTexture;
} MeshManager;

static MeshManager mesh_manager = { 0 };

void gf3d_mesh_manager_close()
{
    int i;
    for (i = 0; i < mesh_manager.mesh_count; i++)
    {
        gf3d_mesh_delete(&mesh_manager.mesh_list[i]);
    }
    if (mesh_manager.mesh_list)
    {
        free(mesh_manager.mesh_list);
    }

    memset(&mesh_manager, 0, sizeof(MeshManager));
    if (__DEBUG)slog("Mesh manager closed");
}

void gf3d_mesh_init(Uint32 mesh_max)
{
    Uint32 count = 0;
    if (mesh_max == 0)
    {
        slog("cannot intilizat mesh manager for 0 meshs");
        return;
    }
    mesh_manager.chain_length = gf3d_swapchain_get_chain_length();
    mesh_manager.mesh_list = (Mesh*)gfc_allocate_array(sizeof(Mesh), mesh_max);
    mesh_manager.mesh_count = mesh_max;
    mesh_manager.device = gf3d_vgraphics_get_default_logical_device();


    gf3d_mesh_get_attribute_descriptions(&count);
    mesh_manager.pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/model_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        gf3d_mesh_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(NULL),
        count,
        sizeof(MeshUBO),
        VK_INDEX_TYPE_UINT16
    );
    mesh_manager.sky_pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/sky_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        gf3d_mesh_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(NULL),
        count,
        sizeof(SkyUBO),
        VK_INDEX_TYPE_UINT16
    );

    mesh_manager.defaultTexture = gf3d_texture_load("images/default.png");
    if (__DEBUG)slog("mesh manager initiliazed");
    atexit(gf3d_mesh_manager_close);
}

void gf3d_mesh_delete(Mesh* mesh)
{
    int i, c;
    MeshPrimitive* prim;
    if (!mesh)return;
    c = gfc_list_count(mesh->primitives);
    for (i = 0; i < c; i++)
    {
        prim = gfc_list_get_nth(mesh->primitives, i);
        if (!prim) continue;
        gf3d_mesh_primitive_free(prim);
    }
    if (mesh->primitives) gfc_list_delete(mesh->primitives);
    memset(mesh, 0, sizeof(Mesh));
}

Mesh* gf3d_mesh_new()
{
    int i;
    for (i = 0; i < mesh_manager.mesh_count; i++)
    {
        if (mesh_manager.mesh_list[i]._refCount)continue;
        mesh_manager.mesh_list[i]._refCount = 1;
        return &mesh_manager.mesh_list[i];
    }
    slog("gf3d_mesh_new: no free slots for new meshes");
    return NULL;
}

Mesh* gf3d_mesh_get_by_filename(const char* filename)
{
    int i;
    if (!filename)return NULL;
    for (i = 0; i < mesh_manager.mesh_count; i++)
    {
        if (!mesh_manager.mesh_list[i]._refCount)continue;
        if (gfc_line_cmp(mesh_manager.mesh_list[i].filename, filename) == 0)
        {
            return &mesh_manager.mesh_list[i];
        }
    }
    return NULL;
}


Mesh* gf3d_mesh_load(const char* filename)
{
    MeshPrimitive* primitive;
    Mesh* mesh;
    ObjData* obj;

    if (!filename) return NULL;
    obj = gf3d_obj_load_from_file(filename); //Parse data from file
    if (!obj)
    {
        slog("Failed to parse obj file %s", filename);
        return NULL;
    }
    mesh = gf3d_mesh_new();
    if (!mesh)
    {
        gf3d_obj_free(obj);
        slog("Failed to make new mesh");
        return NULL;
    }

    primitive = gf3d_mesh_primitive_new();

    if (!primitive)
    {
        gf3d_obj_free(obj);
        gf3d_mesh_free(mesh);
        slog("Failed to make new meshPrim");
        return NULL;
    }

    mesh->primitives = gfc_list_new();
    gfc_list_append(mesh->primitives, primitive);

    primitive->objData = obj;

    gf3d_mesh_primitive_create_vertex_buffer(primitive);
    gf3d_mesh_primitive_create_face_buffer(primitive);
    gfc_line_cpy(mesh->filename, filename);

    return mesh;
}

MeshPrimitive* gf3d_mesh_primitive_new()
{
    MeshPrimitive* prim = NULL;
    prim = (MeshPrimitive*)gfc_allocate_array(sizeof(MeshPrimitive), 1);

    if (!prim)
    {
        slog("Couldnt make a new mesh primitive memory");
        return NULL;
    }


    return prim;

}

VkVertexInputAttributeDescription* gf3d_mesh_get_attribute_descriptions(Uint32* count)
{
    mesh_manager.attributeDescriptions[0].binding = 0;
    mesh_manager.attributeDescriptions[0].location = 0;
    mesh_manager.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh_manager.attributeDescriptions[0].offset = offsetof(Vertex, vertex);

    mesh_manager.attributeDescriptions[1].binding = 0;
    mesh_manager.attributeDescriptions[1].location = 1;
    mesh_manager.attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh_manager.attributeDescriptions[1].offset = offsetof(Vertex, normal);

    mesh_manager.attributeDescriptions[2].binding = 0;
    mesh_manager.attributeDescriptions[2].location = 2;
    mesh_manager.attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    mesh_manager.attributeDescriptions[2].offset = offsetof(Vertex, texel);
    if (count)*count = MESH_ATTRIBUTE_COUNT;
    return mesh_manager.attributeDescriptions;
}

VkVertexInputBindingDescription* gf3d_mesh_get_bind_description()
{
    mesh_manager.bindingDescription.binding = 0;
    mesh_manager.bindingDescription.stride = sizeof(Vertex);
    mesh_manager.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return &mesh_manager.bindingDescription;
}

void gf3d_mesh_primitive_free(Mesh* mesh) {}

void gf3d_mesh_free(Mesh* mesh)
{
    if (!mesh)return;
    mesh->_refCount--;
    if (mesh->_refCount <= 0)gf3d_mesh_delete(mesh);
}

void gf3d_mesh_primitive_create_vertex_buffer(MeshPrimitive* prim)
{
    void* data = NULL;
    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    Vertex* verticies;
    Uint32 vcount;
    size_t bufferSize;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

    if (!prim)
    {
        slog("No mesh primitize provided");
        return;
    }

    verticies = prim->objData->faceVertices;
    vcount = prim->objData->face_vert_count;
    bufferSize = sizeof(Vertex) * vcount;
    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, verticies, (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &prim->vertexBuffer, &prim->vertexBufferMemory);

    gf3d_buffer_copy(stagingBuffer, prim->vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

    prim->vertexCount = vcount;

}

void gf3d_mesh_primitive_create_face_buffer(MeshPrimitive* prim)
{
    void* data = NULL;
    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    Face* faces;
    Uint32 fcount;
    size_t bufferSize;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

    if (!prim)
    {
        slog("No mesh primitize provided");
        return;
    }

    faces = prim->objData->outFace;
    fcount = prim->objData->face_count;
    bufferSize = sizeof(Face) * fcount;
    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, faces, (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &prim->faceBuffer, &prim->faceBufferMemory);

    gf3d_buffer_copy(stagingBuffer, prim->faceBuffer, bufferSize);

    prim->faceCount = fcount;

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

}

void gf3d_mesh_queue_render(Mesh* mesh, Pipeline* pipe, void* uboData, Texture* texture)
{
    int i, c;
    MeshPrimitive* prim;
    if ((!mesh) || (!pipe) || (!uboData)) return;
    c = gfc_list_count(mesh->primitives);
    for (i = 0; i < c; i++)
    {
        prim = gfc_list_nth(mesh->primitives, i);
        if (!prim) continue;
        gf3d_mesh_primitive_queue_render(prim, pipe, uboData, texture);
    }


}

void gf3d_mesh_primitive_queue_render(MeshPrimitive* prim, Pipeline* pipe, void* uboData, Texture* texture)
{
    if ((!prim) || (!pipe) || (!uboData)) return;
    if (!texture) texture = mesh_manager.defaultTexture;
    gf3d_pipeline_queue_render(
        pipe,
        prim->vertexBuffer,
        prim->vertexCount,
        prim->faceBuffer,
        uboData,
        texture);
}

void gf3d_mesh_draw(Mesh* mesh, GFC_Matrix4 modelMat, GFC_Color mod, Texture* texture, GFC_Vector3D lightPos, GFC_Color lightColor)
{
    MeshUBO ubo = { 0 };
    //slog("In draw");
    if (!mesh) return;
    gfc_matrix4_copy(ubo.model, modelMat);
    gf3d_vgraphics_get_view(&ubo.view);
    gf3d_vgraphics_get_projection_matrix(&ubo.proj);

    ubo.color = gfc_color_to_vector4f(mod);
    ubo.lightColor = gfc_color_to_vector4f(lightColor);
    ubo.lightPos = gfc_vector3dw(lightPos, 1.0);
    ubo.camera = gfc_vector3dw(gf3d_camera_get_position(), 1.0);

    gf3d_mesh_queue_render(mesh, mesh_manager.pipe, &ubo, texture);
}

void gf3d_mesh_sky_draw(Mesh* mesh, GFC_Matrix4 modelMat, GFC_Color mod, Texture* texture)
{
    SkyUBO ubo = { 0 };

    if (!mesh) return;
    gfc_matrix4_copy(ubo.model, modelMat);
    gf3d_vgraphics_get_view(&ubo.view);

    ubo.view[0][3] = 0;
    ubo.view[1][3] = 0;
    ubo.view[2][3] = 0;
    ubo.view[3][0] = 0;
    ubo.view[3][0] = 0; //1
    ubo.view[3][0] = 0; //2

    gf3d_vgraphics_get_projection_matrix(&ubo.proj);
    ubo.color = gfc_color_to_vector4f(mod);
    gf3d_mesh_queue_render(mesh, mesh_manager.sky_pipe, &ubo, texture);
}


Pipeline* gf3d_mesh_get_pipeline()
{
    return mesh_manager.pipe;
}


