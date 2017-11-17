#ifndef MODELPLY_H
#define MODELPLY_H


class ModelPLY
{

public:

    ModelPLY();
    ~ModelPLY();
    int load(char *filename);
    void draw();
    float* calculateNormal( float *coord1, float *coord2, float *coord3 );


private:

    float* m_facesTriangles;
    float* m_facesQuads;
    float* m_vertexBuffer;
    float* m_normals;

    int m_totalConnectedTriangles;
    int m_totalConnectedQuads;
    int m_totalConnectedPoints;
    int m_totalFaces;

};

#endif // MODELPLY_H
