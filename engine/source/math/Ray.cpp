#include"math/Ray.h"


bool CRay::Intersect(Vector3f& pos, float radius, float* dist) const
{
    //　方向を算出
    const Vector3f RSDir = pos - m_origin;
    
    //　長さを算出
    const float RayToSphereLength = DotProduct(RSDir, RSDir);

    //　内積を算出
    const float IntersectPoint = DotProduct(RSDir, m_direction);

    // 当たってない場合
    if (IntersectPoint < 0) return false;

    const float SquaredPoint = (radius * radius) - RayToSphereLength + (IntersectPoint * IntersectPoint);

    // 0より小さい場合は当たってない
    if (SquaredPoint < 0) return false;

    // 当たった場合は距離を保存する
    if (dist)
        *dist = IntersectPoint - sqrt(SquaredPoint);

    return true;
}


bool CRay::Intersect(const Vector3f& p1, Vector3f& p2, Vector3f& p3,
    bool cull, float* dist)
{
    //ベクトルを保存します。
    const Vector3f vecAB = p2 - p1;
    const Vector3f vecAC = p3 - p1;

    //p3-p1vecとraydirの間の外積を取得します。
    const Vector3f cross = CrossProduct(m_direction, vecAC);

    // Calculate the determinate.
    const float det = DotProduct(vecAB, cross);

    // If either true, no collision.
    if (cull && det < 0.0001f) return false;
    else if (det < 0.0001f && det > -0.0001f) return false;

    const Vector3f rayPointVec = m_origin - p1;
    const float test1 = DotProduct(rayPointVec, cross);

    // Ray behind, no collision.
    if (test1 < 0.0f || test1 > det) return false;

    const Vector3f cross2 = CrossProduct(rayPointVec, vecAB);
    const float test2 = DotProduct(m_direction, cross);

    // No collision.
    if (test2 < 0.0f || test1 + test2 > det) return false;

    // Get collision dist.
    const float inverseDet = 1.0f / det;

    if (dist)
    {
        *dist = DotProduct(vecAC, cross2);
        *dist *= inverseDet;
    }

    return true;
}


//bool CRay::Intersect(Vector3f& p1, Vector3f& p2, Vector3f& p3,
//    bool cull, float length, float* dist)
//{
//    // Store these vectors between the points for future use.
//    Vector3f vecAB = p2 - p1;
//    Vector3f vecAC = p3 - p1;
//
//    // Get the corss product bewteen the p3 - p1 vec and the ray dir.
//    Vector3f cross;
//    cross.CrossProduct(m_direction, vecAC);
//
//    // Calculate the determinate.
//    float det = vecAB.DotProduct3(cross);
//
//    // If either true, no collision.
//    if (cull && det < 0.0001f) return false;
//    else if (det < 0.0001f && det > -0.0001f) return false;
//
//    Vector3f rayPointVec = m_origin - p1;
//    float test1 = rayPointVec.DotProduct3(cross);
//
//    // Ray behind, no collision.
//    if (test1 < 0.0f || test1 > det) return false;
//
//    Vector3f cross2;
//    cross2.CrossProduct(rayPointVec, vecAB);
//    float test2 = m_direction.DotProduct3(cross2);
//
//    // No collision.
//    if (test2 < 0.0f || test1 + test2 > det) return false;
//
//    // Get distance. If smaller than ray length then collision.
//    float inverseDet = 1.0f / det;
//
//    if (dist)
//    {
//        *dist = vecAC.DotProduct3(cross2);
//        *dist *= inverseDet;
//
//        if (*dist > length) return false;
//    }
//    else
//    {
//        float l = vecAC.DotProduct3(cross2) * inverseDet;
//        if (l > length) return false;
//    }
//
//    return true;
//}
//
//
//bool CRay::Intersect(CPlane& pl, bool cull, Vector3f* intersectPoint, float* dist)
//{
//    Vector3f normal(pl.a, pl.b, pl.c);
//
//    float rayD = normal.DotProduct3(m_direction);
//
//    // If true then ray is parallel to plane, no intersection.
//    if (fabs(rayD) < 0.00001f) return false;
//    if (cull && rayD > 0.0f) return false;
//
//    float originD = -(normal.DotProduct3(m_origin) + pl.d);
//    float intersectDist = originD / rayD;
//
//    // If less then 0, no intersection.  Behind ray origin.
//    if (intersectDist < 0.001f) return false;
//
//    // Else we have a hit.
//    if (dist) *dist = intersectDist;
//    if (intersectPoint)
//        *intersectPoint = m_origin + (m_direction * intersectDist);
//
//    return true;
//}
//
//
//bool CRay::Intersect(CPlane& pl, bool cull, float length,
//    Vector3f* intersectPoint, float* dist)
//{
//    Vector3f normal(pl.a, pl.b, pl.c);
//
//    float rayD = normal.DotProduct3(m_direction);
//
//    // If true then ray is parallel to plane, no intersection.
//    if (fabs(rayD) < 0.00001f) return false;
//    if (cull && rayD > 0.0f) return false;
//
//    float originD = -(normal.DotProduct3(m_origin) + pl.d);
//
//    float intersectDist = originD / rayD;
//
//    // If less then 0, no intersection.  Behind ray origin.
//    if (intersectDist < 0.001f || intersectDist > length) return false;
//
//    // Else we have a hit.
//    if (dist) *dist = intersectDist;
//    if (intersectPoint)
//        *intersectPoint = m_origin + (m_direction * intersectDist);
//
//    return true;
//}