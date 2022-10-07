#pragma once

#include "math/Vector.h"
#include "math/Ray.h"
#include "math/Polygon.h"
#include "math/Plane.h"

class CPlane
{
public:
    CPlane();
    CPlane(float A, float B, float C, float D);
    CPlane(Vector3f& n, float D);

    void CreatePlaneFromTri(Vector3f& v1, Vector3f& v2,
        Vector3f& v3);

    void SetPlaneNormal(float A, float B, float C)
    {
        a = A; b = B; c = C;
    }
    void SetPlaneIntercept(float D) { d = D; }

    bool Intersect(CPlane& p2, CPlane& p3,
        Vector3f* intersectPoint);
    bool Intersect(CPlane& pl, Vector3f* intersectPoint);
    bool Intersect(CPlane& pl, CRay* intersect);
    bool Intersect(Vector3f& p1, Vector3f& p2, Vector3f& p3);

    //int ClassifyPolygon(CPolygon& pol);
    int ClassifyPoint(Vector3f& v);
    int ClassifyPoint(float x, float y, float z);

    void Lerp(CPlane& p1, CPlane& p2, float amount);

    float GetDistance(Vector3f& v)
    {
        return a * v.x + b * v.y + c * v.z + d;
    }

    float GetDistance(float x, float y, float z)
    {
        return a * x + b * y + c * z + d;
    }

    float a, b, c, d;
};
