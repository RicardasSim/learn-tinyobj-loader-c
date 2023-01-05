#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

typedef struct s_ctx {
    char *p_buffer[2];
    unsigned int num;
} S_ctx;

static void fileReaderCallback( void *context,
                                const char *file_name,
                                const int mtl_flag,
                                const char *obj_file_name,
                                char **content_buffer,
                                size_t *size_of_content )
{
    printf( "fileReaderCallback().\n" );
    printf( "mtl flag %d\n", mtl_flag );
    printf( "file name %s\n", file_name );
    printf( "obj file name %s\n\n", obj_file_name );

    FILE *fp;

    fp = fopen( file_name, "rb" );

    if ( !fp )
    {
        printf( "Error: failed to open file \"%s\" !\n", file_name );
        *content_buffer = NULL;
        *size_of_content = 0;
        return;
    }

    fseek( fp, 0L, SEEK_END );

    size_t file_size = ftell(fp);

    char *buffer = malloc( file_size );

    if ( !buffer )
    {
        printf( "Error: cannot allocate memory!\n" );
        *content_buffer = NULL;
        *size_of_content = 0;
        return;
    }

    fseek( fp, 0, SEEK_SET );

    fread( buffer, 1, file_size, fp );

    fclose(fp);

    *content_buffer = buffer;
    *size_of_content = file_size;

    ((S_ctx*) context)->p_buffer[((S_ctx*) context)->num] = buffer;
    ((S_ctx*) context)->num++;
}

int main()
{

    char *file_name = {"planes.obj"};

    tinyobj_attrib_t attrib;
    tinyobj_shape_t *shapes;
    size_t num_shapes;
    tinyobj_material_t *materials;
    size_t num_materials;
    void *context;
    unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;

    S_ctx sctx = {0};

    printf("Begin\n--------------------\n\n");

    context = &sctx;

    int retv = tinyobj_parse_obj( &attrib,
                                  &shapes,
                                  &num_shapes,
                                  &materials,
                                  &num_materials,
                                  file_name,
                                  fileReaderCallback,
                                  context,
                                  flags );

    if ( retv != TINYOBJ_SUCCESS )
    {
        printf("Error: tinyobj parse error.\n");
        return 1;
    }

    printf("Info\n--------------------\n\n");

    printf( "Number of vertices: %u\n", attrib.num_vertices );
    printf( "Number of normals: %u\n", attrib.num_normals );
    printf( "Number of texcoords: %u\n", attrib.num_texcoords );
    printf( "Number of vertex indices: %u\n", attrib.num_faces );
    printf( "Number of faces: %u\n", attrib.num_face_num_verts );
    printf( "Number of materials: %ld\n", num_materials );
    printf( "Number of shapes: %ld\n\n", num_shapes );

    /*
    Vertices

    for ( unsigned int i = 0; i < attrib.num_vertices; ++i )
    {
        float x = attrib.vertices[ i*3 ];
        float y = attrib.vertices[ i*3 + 1 ];
        float z = attrib.vertices[ i*3 + 2 ];
    }

    Normals

    for ( unsigned int i = 0; i < attrib.num_normals; ++i )
    {
        float x = attrib.normals[ i*3 ];
        float y = attrib.normals[ i*3 + 1 ];
        float z = attrib.normals[ i*3 + 2 ];
    }

    Texcoords

    for ( unsigned int i = 0; i < attrib.num_texcoords; ++i )
    {
        float a = attrib.texcoords[ i*2 ];
        float b = attrib.texcoords[ i*2 + 1 ];
    }

    indexes v vt vn

    for ( unsigned int i = 0; i < attrib.num_faces; ++i )
    {
        tinyobj_vertex_index_t vert_idx = attrib.faces[i];

        int indx_v = vert_idx.v_idx;
        int indx_vt = vert_idx.vt_idx;
        int indx_vn = vert_idx.vn_idx;

        float x = attrib.vertices[indx_v*3];
        float y = attrib.vertices[indx_v*3+1];
        float z = attrib.vertices[indx_v*3+2];

        float a = attrib.texcoords[indx_vt*2];
        float b = attrib.texcoords[indx_vt*2 + 1];

        float x = attrib.normals[indx_vn*3];
        float y = attrib.normals[indx_vn*3 + 1];
        float z = attrib.normals[indx_vn*3 + 2];
    }

    */

    for ( size_t i = 0; i < num_shapes; ++i )
    {
        printf( "Shape [%ld]:\n", i );
        printf( "name: %s\n", shapes[i].name );

        unsigned int offset = shapes[i].face_offset;

        unsigned int length = shapes[i].length;

        printf( "face offset: %u\n", offset );
        printf( "length: %u\n\n", length );

        /*

        ...

        */

        printf("\n\n");
    }

    // ********** Cleanup **********

    printf("Cleanup\n--------------------\n\n");

    tinyobj_attrib_free( &attrib );
    tinyobj_shapes_free( shapes, num_shapes );
    tinyobj_materials_free( materials, num_materials );

    printf("num: %u\n", sctx.num );

    for ( uint32_t i = 0; i < sctx.num; ++i )
    {
        if ( sctx.p_buffer[i] )
        {
            printf("free buffer (%u).\n", i );
            free( sctx.p_buffer[i] );
        }
    }

    printf("\nBye Bye !\n");

    return 0;
}
