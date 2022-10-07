/*
   Demo Name:  Game Project 13
      Author:  Allen Sherrod
     Chapter:  Chapter 15
*/


#include"math/Plane.h"


CPlane::CPlane()
{
    a = 0; b = 0; c = 0; d = 0;
}


CPlane::CPlane(float A, float B, float C, float D)
{
    a = A; b = B; c = C; d = D;
}


CPlane::CPlane(Vector3f& n, float D)
{
    a = n.x; b = n.y; c = n.z; d = D;
}


void CPlane::CreatePlaneFromTri(Vector3f& v1, Vector3f& v2, Vector3f& v3)
{
    Vector3f e1 = v3 - v1;
    Vector3f e2 = v2 - v1;

    Normalize(e1);
    Normalize(e2);

    Vector3f normal = CrossProduct(e1, e2);
    Normalize(normal);

    // Save normal and calculate d.
    a = normal.x;
    b = normal.y;
    c = normal.z;
    d = -(a * v1.x + b * v1.y + c * v1.z);
}


//bool CPlane::Intersect(CPlane& p2, CPlane& p3, Vector3f* intersectPoint)
//{
//    Vector3f temp;
//    Vector3f normal(a, b, c);
//    Vector3f p2Normal(p2.a, p2.b, p2.c);
//    Vector3f p3Normal(p3.a, p3.b, p3.c);
//
//    // Get cross product of p2 and p3.
//    temp.CrossProduct(p2Normal, p3Normal);
//
//    // Get the dot prod of this plane with cross of p2 and p3.
//    float dp = normal.DotProduct3(temp);
//    if (dp == 0.0f) return false;
//
//    // Else we must find intersect point.
//    if (intersectPoint)
//    {
//        *intersectPoint = temp * d;
//
//        temp.CrossProduct(p3Normal, normal);
//        *intersectPoint = *intersectPoint + (temp * p2.d);
//
//        temp.CrossProduct(normal, p2Normal);
//        *intersectPoint = *intersectPoint + (temp * p3.d);
//
//        *intersectPoint = *intersectPoint / -dp;
//    }
//
//    return true;
//}
//
//
//bool CPlane::Intersect(CPlane& pl, Vector3f* intersectPoint)
//{
//    Vector3f cross;
//    Vector3f normal(a, b, c);
//    Vector3f plNormal(pl.a, pl.b, pl.c);
//    float length = 0;
//
//    cross.CrossProduct(normal, plNormal);
//    length = cross.DotProduct3(cross);
//
//    if (length < 1e-08f) return false;
//
//    if (intersectPoint)
//    {
//        float l0 = normal.DotProduct3(normal);
//        float l1 = normal.DotProduct3(plNormal);
//        float l2 = plNormal.DotProduct3(plNormal);
//        float det = l0 * l2 - l1 * l1;
//        float invDet = 0;
//
//        if (fabs(det) < 1e-08f) return false;
//
//        invDet = 1 / det;
//        float d0 = (l2 * d - l1 * pl.d) * invDet;
//        float d1 = (l0 * pl.d - l1 * d) * invDet;
//
//        (*intersectPoint) = normal * d0 + plNormal * d1;
//    }
//
//    return true;
//}
//
//
//bool CPlane::Intersect(CPlane& pl, CRay* intersect)
//{
//    Vector3f cross;
//    Vector3f normal(a, b, c);
//    Vector3f plNormal(pl.a, pl.b, pl.c);
//    float length = 0;
//
//    cross.CrossProduct(normal, plNormal);
//    length = cross.DotProduct3(cross);
//
//    if (length < 1e-08f) return false;
//
//    if (intersect)
//    {
//        float l0 = normal.DotProduct3(normal);
//        float l1 = normal.DotProduct3(plNormal);
//        float l2 = plNormal.DotProduct3(plNormal);
//        float det = l0 * l2 - l1 * l1;
//        float invDet = 0;
//
//        if (fabs(det) < 1e-08f) return false;
//
//        invDet = 1 / det;
//        float d0 = (l2 * d - l1 * pl.d) * invDet;
//        float d1 = (l0 * pl.d - l1 * d) * invDet;
//
//        (*intersect).m_origin = normal * d0 + plNormal * d1;
//        (*intersect).m_direction = cross;
//    }
//
//    return true;
//}


//int CPlane::ClassifyPolygon(CPolygon& pol)
//{
//    int frontPolys = 0;
//    int backPolys = 0;
//    int planePolys = 0;
//    int type = 0;
//
//    for (int i = 0; i < 3; i++)
//    {
//        type = ClassifyPoint(pol.m_vertexList[i]);
//
//        switch (type)
//        {
//        case UGP_FRONT:
//            frontPolys++;
//            break;
//
//        case UGP_BACK:
//            backPolys++;
//            break;
//
//        default:
//            frontPolys++;
//            backPolys++;
//            planePolys++;
//            break;
//        }
//    }
//
//    if (planePolys == 3) return UGP_ON_PLANE;
//    else if (frontPolys == 3) return UGP_FRONT;
//    else if (backPolys == 3) return UGP_BACK;
//
//    return UGP_CLIPPED;
//}


int CPlane::ClassifyPoint(Vector3f& v)
{
    float distance = a * v.x + b * v.y + c * v.z + d;

    if (distance > 0.001) return UGP_FRONT;
    if (distance < -0.001) return UGP_BACK;

    return UGP_ON_PLANE;
}


int CPlane::ClassifyPoint(float x, float y, float z)
{
    float distance = a * x + b * y + c * z + d;

    if (distance > 0.001) return UGP_FRONT;
    if (distance < -0.001) return UGP_BACK;

    return UGP_ON_PLANE;
}


void CPlane::Lerp(CPlane& p1, CPlane& p2, float amount)
{
    // First calculate a, b, c, d.
    a = p1.a * (1.0f - amount) + p2.a * amount;
    b = p1.b * (1.0f - amount) + p2.b * amount;
    c = p1.c * (1.0f - amount) + p2.c * amount;
    d = p1.d * (1.0f - amount) + p2.d * amount;

    // Re-normalize plane.
    float length = (float)sqrt((a * a + b * b + c * c));

    if (length == 0.0f) length = 1.0f;
    length = 1 / length;

    // Normalize all but d.
    a = a * length;
    b = b * length;
    c = c * length;
}