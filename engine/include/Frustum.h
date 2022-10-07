#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#define MAX_SIDES       6

class CFrustum
{
public:
    void CalculateFrustum(float* mv, float* proj);
    bool isPointVisiable(float x, float y, float z);
    bool isSphereVisiable(float x, float y, float z, float radius);
    bool isBoxVisiable(float x, float y, float z, float size);

protected:
    void NormalizeFrustum();

protected:
    //CPlane Frustum[MAX_SIDES];
};

#endif