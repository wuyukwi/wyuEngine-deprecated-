/*********************************************************************************

  *FileName: vertex.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/05

**********************************************************************************/

#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "math/Vector.h"

struct stVector
{
    stVector() : x(0), y(0), z(0) {}
    float x, y, z;
};

struct GUIVertex
{
    float x, y, z, rhw;
    unsigned long color;
    float tu, tv;
};

struct NodeVertex2D
{
    float x, y, z, rhw;
    unsigned long color;
    float tu, tv;

    void scalar(float scalar)
    {
        this->x *= scalar;
        this->y *= scalar;
    }
    NodeVertex2D()
    {
        this->x = 0.0f; this->y = 0.0f; this->z = 0.0f; this->rhw = 1.0f;
        this->color = 0xFFFFFFFF;
        this->tu = 0.0f; this->tv = 0.0f;
    }

    NodeVertex2D(float x, float y, float tu, float tv)
    {
        this->x = x; this->y = y; this->z = 1.0f; this->rhw = 1.0f;
        this->color = 0xFFFFFFFF;
        this->tu = tu; this->tv = tv;
    }
    NodeVertex2D(float x, float y, float z, float tu, float tv)
    {
        this->x = x; this->y = y; this->z = z; this->rhw = 1.0f;
        this->color = 0xFFFFFFFF;
        this->tu = tu; this->tv = tv;
    }

    NodeVertex2D(float x, float y, float z, unsigned long color, float tu, float tv)
    {
        this->x = x; this->y = y; this->z = z; this->rhw = 1.0f;
        this->color = color;
        this->tu = tu; this->tv = tv;
    }
};

struct NodeVertex3D
{
    Vector3f pos{};
    Vector3f nor{};
    Vector2f tex{};

    NodeVertex3D()
        = default;

    NodeVertex3D(Vector3f pos, Vector2f tex)
    {
        this->pos = pos;
        this->nor = Vector3f{ 0.0f,1.0f,0.0f };
        this->tex = tex;
    }

    NodeVertex3D(Vector3f pos, Vector3f nor, Vector2f tex)
    {
        this->pos = pos;
        this->nor = nor;
        this->tex = tex;
    }
};


// A general structure for our models.
struct stModelVertex
{
    float x, y, z;
    float nx, ny, nz;
    unsigned long color;
    float tu, tv;
};
#endif // _VERTEX_H_