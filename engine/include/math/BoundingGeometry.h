#pragma once

#include "math/Vector.h"
#include "math/Ray.h"
#include "math/Plane.h"
#include "structs.h"

class BoundingBase2D
{

    virtual void CreateFromPoints(NodeVertex2D* pointList, int32_t numPoints) = 0;
    virtual bool isPointInside(const Vector2f& v) = 0;

};

class BoundingBox2D : public BoundingBase2D
{
public:

    void CreateFromPoints(NodeVertex2D* pointList, int32_t numPoints) override;
    bool isPointInside(const Vector2f& v) override;
    bool isBoxCollision(const BoundingBox2D* box);
    bool isBoxCollisionIn(const BoundingBox2D* box);

private:
    Vector2f m_min{ 0 }, m_max{ 0 };
    Vector2f m_Vec[4]; //順番: 左上、左下、右上、右下
};

class BoundingBase3D
{
public:
    BoundingBase3D()
    {
    }

    virtual ~BoundingBase3D()
    {
    }

    virtual void CreateFromPoints(Vector3f* pointList, int numPoints) = 0;
    virtual bool isPointInside(Vector3f& v) = 0;

    virtual bool Intersect(CRay ray, float* dist) = 0;
    virtual bool Intersect(CRay ray, float length, float* dist) = 0;

    virtual void GetPlanes(CPlane* planes) = 0;
    virtual bool isRayInside(CRay& ray, float length) = 0;
};


class CBoundingBox : public BoundingBase3D
{
public:
    CBoundingBox()
    {
    }

    ~CBoundingBox()
    {
    }

    void CreateFromPoints(Vector3f* pointList, int numPoints) override;
    bool isPointInside(Vector3f& v) override;

    bool Intersect(CRay ray, float* dist) override;
    bool Intersect(CRay ray, float length, float* dist) override;

    void GetPlanes(CPlane* planes) override;
    bool isRayInside(CRay& ray, float length) override;

    Vector3f m_min, m_max;
};


class CBoundingSphere : public BoundingBase3D
{
public:
    CBoundingSphere() : m_radius(0)
    {
    }

    ~CBoundingSphere()
    {
    }

    void CreateFromPoints(Vector3f* pointList, int numPoints);
    bool isPointInside(Vector3f& v);

    bool Intersect(CRay ray, float* dist);
    bool Intersect(CRay ray, float length, float* dist);

    void GetPlanes(CPlane* planes)
    {
    }

    bool isRayInside(CRay& ray, float length);

    Vector3f m_center;
    float m_radius;
};

bool BoxToBoxIntersect(const CBoundingBox& bb1, const CBoundingBox& bb2);
bool SphereToSphereIntersect(CBoundingSphere& bs1, CBoundingSphere& bs2);

