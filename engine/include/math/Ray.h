#pragma once

#include "math/Vector.h"

class CRay
{
public:
    CRay(Vector3f origin, Vector3f direction) :m_origin(origin), m_direction(direction) {}

    /**
     * \brief レイの相交判定
     * \param pos ベクトルの位置
     * \param radius 半径
     * \param dist 距離
     * \return 当たってるかどうか
     */
    bool Intersect(Vector3f& pos, float radius, float* dist) const;
    bool Intersect(const Vector3f& p1, Vector3f& p2, Vector3f& p3, bool cull, float* dist);
    //  bool Intersect(Vector3f& p1, Vector3f& p2, Vector3f& p3, bool cull, float length, float* dist);
      /*  bool Intersect(CPlane& pl, bool cull, Vector3f* intersectPoint, float* dist);
        bool Intersect(CPlane& pl, bool cull, float length, Vector3f* intersectPoint, float* dist);*/

    Vector3f m_origin;
    Vector3f m_direction;
};
