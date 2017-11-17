#include "modelply.h"
#include <cmath>
#include <string>
#include <stdio.h>
#include <string.h>

ModelPLY::ModelPLY() :
    m_facesTriangles(NULL),
    m_facesQuads(nullptr),
    m_normals(nullptr)
{
}

ModelPLY::~ModelPLY()
{
    delete m_facesTriangles;
    delete m_normals;
    delete m_facesQuads;
}

int ModelPLY::ModelPLY::load(char *filename)
{
    m_totalConnectedTriangles = 0;
    m_totalConnectedQuads = 0;
    m_totalConnectedPoints = 0;

    char* pch = strstr(filename,".ply");

    if (pch != NULL)
    {
        FILE* file = fopen(filename,"r");

        fseek(file,0,SEEK_END);
        long fileSize = ftell(file);

        try
        {
            m_vertexBuffer = (float*) malloc (ftell(file));
        }
        catch (char* )
        {
            return -1;
        }
        if (m_vertexBuffer == NULL) return -1;
        fseek(file,0,SEEK_SET);

        m_facesTriangles = (float*) malloc(fileSize*sizeof(float));
        m_normals  = (float*) malloc(fileSize*sizeof(float));

        if (file)
        {
            int i = 0;
            int temp = 0;
            int quads_index = 0;
            int triangle_index = 0;
            int normal_index = 0;
            char buffer[1000];


            fgets(buffer,300,file);			// ply


            // READ HEADER
            // -----------------

            // Find number of vertexes
            while (  strncmp( "element vertex", buffer,strlen("element vertex")) != 0  )
            {
                fgets(buffer,300,file);			// format
            }
            strcpy(buffer, buffer+strlen("element vertex"));
            sscanf(buffer,"%i", &this->m_totalConnectedPoints);


            // Find number of vertexes
            fseek(file,0,SEEK_SET);
            while (  strncmp( "element face", buffer,strlen("element face")) != 0  )
            {
                fgets(buffer,300,file);			// format
            }
            strcpy(buffer, buffer+strlen("element face"));
            sscanf(buffer,"%i", &this->m_totalFaces);


            // go to end_header
            while (  strncmp( "end_header", buffer,strlen("end_header")) != 0  )
            {
                fgets(buffer,300,file);			// format
            }

            //----------------------


            // read verteces
            i =0;
            for (int iterator = 0; iterator < this->m_totalConnectedPoints; iterator++)
            {
                fgets(buffer,300,file);

                sscanf(buffer,"%f %f %f", &m_vertexBuffer[i], &m_vertexBuffer[i+1], &m_vertexBuffer[i+2]);
                i += 3;
            }

            // read faces
            i =0;
            for (int iterator = 0; iterator < this->m_totalFaces; iterator++)
            {
                fgets(buffer,300,file);

                if (buffer[0] == '3')
                {

                    int vertex1 = 0, vertex2 = 0, vertex3 = 0;
                    //sscanf(buffer,"%i%i%i\n", vertex1,vertex2,vertex3 );
                    buffer[0] = ' ';
                    sscanf(buffer,"%i%i%i", &vertex1,&vertex2,&vertex3 );
                    /*vertex1 -= 1;
                            vertex2 -= 1;
                            vertex3 -= 1;
    */
                    //  vertex == punt van vertex lijst
                    // m_vertexBuffer -> xyz xyz xyz xyz
                    printf("%f %f %f ", m_vertexBuffer[3*vertex1], m_vertexBuffer[3*vertex1+1], m_vertexBuffer[3*vertex1+2]);

                    m_facesTriangles[triangle_index] = m_vertexBuffer[3*vertex1];
                    m_facesTriangles[triangle_index+1] = m_vertexBuffer[3*vertex1+1];
                    m_facesTriangles[triangle_index+2] = m_vertexBuffer[3*vertex1+2];
                    m_facesTriangles[triangle_index+3] = m_vertexBuffer[3*vertex2];
                    m_facesTriangles[triangle_index+4] = m_vertexBuffer[3*vertex2+1];
                    m_facesTriangles[triangle_index+5] = m_vertexBuffer[3*vertex2+2];
                    m_facesTriangles[triangle_index+6] = m_vertexBuffer[3*vertex3];
                    m_facesTriangles[triangle_index+7] = m_vertexBuffer[3*vertex3+1];
                    m_facesTriangles[triangle_index+8] = m_vertexBuffer[3*vertex3+2];

                    float coord1[3] = {m_facesTriangles[triangle_index], m_facesTriangles[triangle_index+1],m_facesTriangles[triangle_index+2]};
                    float coord2[3] = {m_facesTriangles[triangle_index+3],m_facesTriangles[triangle_index+4],m_facesTriangles[triangle_index+5]};
                    float coord3[3] = {m_facesTriangles[triangle_index+6],m_facesTriangles[triangle_index+7],m_facesTriangles[triangle_index+8]};
                    float *norm = calculateNormal( coord1, coord2, coord3 );

                    m_normals[normal_index] = norm[0];
                    m_normals[normal_index+1] = norm[1];
                    m_normals[normal_index+2] = norm[2];
                    m_normals[normal_index+3] = norm[0];
                    m_normals[normal_index+4] = norm[1];
                    m_normals[normal_index+5] = norm[2];
                    m_normals[normal_index+6] = norm[0];
                    m_normals[normal_index+7] = norm[1];
                    m_normals[normal_index+8] = norm[2];

                    normal_index += 9;

                    triangle_index += 9;
                    m_totalConnectedTriangles += 3;
                }


                i += 3;
            }


            fclose(file);
        }

        else { printf("File can't be opened\n"); }
    } else {
        printf("File does not have a .PLY extension. ");
    }
    return 0;
}

void ModelPLY::ModelPLY::draw()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3,GL_FLOAT,	0,m_facesTriangles);
    glNormalPointer(GL_FLOAT, 0, Normals);
    glDrawArrays(GL_TRIANGLES, 0, m_totalConnectedTriangles);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

float *ModelPLY::ModelPLY::calculateNormal(float *coord1, float *coord2, float *coord3)
{
    /* calculate Vector1 and Vector2 */
    float va[3], vb[3], vr[3], val;
    va[0] = coord1[0] - coord2[0];
    va[1] = coord1[1] - coord2[1];
    va[2] = coord1[2] - coord2[2];

    vb[0] = coord1[0] - coord3[0];
    vb[1] = coord1[1] - coord3[1];
    vb[2] = coord1[2] - coord3[2];

    /* cross product */
    vr[0] = va[1] * vb[2] - vb[1] * va[2];
    vr[1] = vb[0] * va[2] - va[0] * vb[2];
    vr[2] = va[0] * vb[1] - vb[0] * va[1];

    /* normalization factor */
    val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );

    float norm[3];
    norm[0] = vr[0]/val;
    norm[1] = vr[1]/val;
    norm[2] = vr[2]/val;


    return norm;
}



