#include "Sphere.h"

Mesh g_SphereMesh;

#define NUM_SPHERE_POINTS (4)

struct SphereTri
{
    MeshPoly::Color color;
    MeshPoly::Position pos[3];
};

struct SphereQuad
{
    MeshPoly::Color color;
    MeshPoly::Position pos[4];
};

void InitializeSphereMesh()
{
    int nQuads = (NUM_SPHERE_POINTS - 3) * NUM_SPHERE_POINTS;
    int nTris = 2 * NUM_SPHERE_POINTS;
    int nBytes = (nQuads * sizeof(SphereQuad)) + (nTris * sizeof(SphereTri));
    int nWords = nBytes / sizeof(u_long);
    int nPrimBytes = (nQuads * sizeof(POLY_F4)) + (nTris * sizeof(POLY_F3));
    int nPrimWords = nPrimBytes / sizeof(u_long);
    u_long* pMeshPolyBuf = new u_long[nWords];
    u_long* pCur = pMeshPolyBuf;

    // Quads
    for (int i = 1; i < NUM_SPHERE_POINTS - 2; i++)
    {
        int yAngle = (GTE_ONE_OVER_TWO * i) / (NUM_SPHERE_POINTS - 1);
        int nextYAngle = (GTE_ONE_OVER_TWO * (i + 1)) / (NUM_SPHERE_POINTS - 1);

        int curY = -ccos(yAngle);
        int nextY = -ccos(nextYAngle);

        int curColor = curY > 0 ? (curY >> 4) : ((-curY) >> 4);
        int nextColor = nextY > 0 ? (nextY >> 4) : ((-nextY) >> 4);
        int finalColor = (nextColor + curColor) / 2;

        for (int j = 0; j < NUM_SPHERE_POINTS; j++)
        {
            int curIn = (GTE_ONE * j) / NUM_SPHERE_POINTS;
            int nextIn = (GTE_ONE * (j + 1)) / NUM_SPHERE_POINTS;
            
            int curX = ccos(curIn);
            int curZ = csin(curIn);
            int nextX = ccos(nextIn);
            int nextZ = csin(nextIn);

            SphereQuad* pQuad = (SphereQuad*)pCur;
            pQuad->color.r = 255; pQuad->color.g = finalColor; pQuad->color.b = finalColor;
            pQuad->pos[0].vx = curX; pQuad->pos[0].vy = curY; pQuad->pos[0].vz = curZ;
            pQuad->pos[1].vx = nextX; pQuad->pos[1].vy = curY; pQuad->pos[1].vz = nextZ;
            pQuad->pos[2].vx = curX; pQuad->pos[2].vy = nextY; pQuad->pos[2].vz = curZ;
            pQuad->pos[3].vx = nextX; pQuad->pos[3].vy = nextY; pQuad->pos[3].vz = nextZ;
            pCur += sizeof(SphereQuad) / sizeof(u_long);
        }
    }

    SphereTri* pTri = (SphereTri*)pCur;

    // Top tris
    int nextY = ccos(GTE_ONE_OVER_TWO / NUM_SPHERE_POINTS);
    int nextColor = nextY > 0 ? (nextY >> 4) : ((-nextY) >> 4);
    int finalColor = (nextColor + 255) / 2;
    for (int i = 0; i < NUM_SPHERE_POINTS; i++)
    {
        int curIn = (GTE_ONE * i) / NUM_SPHERE_POINTS;
        int nextIn = (GTE_ONE * (i + 1)) / NUM_SPHERE_POINTS;
        int curX = ccos(curIn);
        int curZ = csin(curIn);
        int nextX = ccos(nextIn);
        int nextZ = csin(nextIn);

        pTri->color.r = 255; pTri->color.g = finalColor; pTri->color.b = finalColor;
        pTri->pos[0].vx = curX; pTri->pos[0].vy = nextY; pTri->pos[0].vz = curZ;
        pTri->pos[1].vx = 0; pTri->pos[1].vy = -GTE_ONE; pTri->pos[1].vz = 0;
        pTri->pos[2].vx = nextX; pTri->pos[2].vy = nextY; pTri->pos[2].vz = nextZ;
        pTri++;
    }

    // Bottom tris
    nextY = ccos((GTE_ONE_OVER_TWO * (NUM_SPHERE_POINTS - 1)) / NUM_SPHERE_POINTS);
    nextColor = nextY > 0 ? (nextY >> 4) : ((-nextY) >> 4);
    finalColor = (nextColor + 255) / 2;
    for (int i = 0; i < NUM_SPHERE_POINTS; i++)
    {
        int curIn = (GTE_ONE * i) / NUM_SPHERE_POINTS;
        int nextIn = (GTE_ONE * (i + 1)) / NUM_SPHERE_POINTS;
        int curX = ccos(curIn);
        int curZ = csin(curIn);
        int nextX = ccos(nextIn);
        int nextZ = csin(nextIn);

        pTri->color.r = 255; pTri->color.g = finalColor; pTri->color.b = finalColor;
        pTri->pos[0].vx = curX; pTri->pos[0].vy = nextY; pTri->pos[0].vz = curZ;
        pTri->pos[1].vx = nextX; pTri->pos[1].vy = nextY; pTri->pos[1].vz = nextZ;
        pTri->pos[2].vx = 0; pTri->pos[2].vy = GTE_ONE; pTri->pos[2].vz = 0;
        pTri++;
    }

    g_SphereMesh.AllocateBuffers(nWords, nPrimWords, 2);
    delete[] g_SphereMesh.m_pMeshPolys;
    g_SphereMesh.m_pMeshPolys = pMeshPolyBuf;
    g_SphereMesh.m_pAttrs = new MeshAttr[2];
    g_SphereMesh.m_nAttrs = 1;

    g_SphereMesh.m_pAttrs[0].polyType = MeshPoly::MESHPT_QUAD_FLAT;
    g_SphereMesh.m_pAttrs[0].nPolys = nQuads;
    g_SphereMesh.m_pAttrs[1].polyType = MeshPoly::MESHPT_TRI_FLAT;
    g_SphereMesh.m_pAttrs[1].nPolys = nTris;
    for (int i = 0; i < 2; i++)
    {
        g_SphereMesh.m_pAttrs[i].lightType = MeshPoly::MESHPL_NONE;
        g_SphereMesh.m_pAttrs[i].semitransparentCode = 0xFF;
        g_SphereMesh.m_pAttrs[i].flags = 0;
        g_SphereMesh.m_pAttrs[i].clutId = 0;
        g_SphereMesh.m_pAttrs[i].tpageId = 0;
    }

    for (int i = 0; i < 1; i++)
    {
        SphereQuad* pQuad = ((SphereQuad*)pMeshPolyBuf) + i;
        Debug::PrintColor(&pQuad->color, "Color");
        Debug::PrintVec3Short(&pQuad->pos[0], "Pos 1");
        Debug::PrintVec3Short(&pQuad->pos[1], "Pos 2");
        Debug::PrintVec3Short(&pQuad->pos[2], "Pos 3");
        Debug::PrintVec3Short(&pQuad->pos[3], "Pos 4");
    }
    
    g_SphereMesh.InitPrimBufs();
}
