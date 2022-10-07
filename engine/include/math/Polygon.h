/*
   Demo Name:  Game Project 13
      Author:  Allen Sherrod
     Chapter:  Chapter 15
*/

#ifndef __UGP_POLYGON_H_
#define __UGP_POLYGON_H_

#include"Vector.h"
#include"Ray.h"
#include"Plane.h"
#include"MathDefines.h"

class CPolygon
{
public:
    CPolygon() : m_flag(0) { }

    void SetFlag(unsigned int fl) { m_flag = fl; }
    unsigned int GetFlag() { return m_flag; }
    void Copy(CPolygon& pol);

    /*  void Clip(CPlane& pl, CPolygon* front, CPolygon* back);

      bool Intersect(CRay& ray, bool cull, float* dist);*/

    unsigned int m_flag;
    Vector3f m_vertexList[3];
};

#endif