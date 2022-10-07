/*
   Demo Name:  Game Project 12
      Author:  Allen Sherrod
     Chapter:  Chapter 13
*/

#ifndef CROUTE_H
#define CROUTE_H

#include"math/Vector.h"

#define STRIAGHT_PATH 0
#define CURVE_PATH 1

class CPath
{
public:
    CPath();
    CPath(int Type, Vector3f start, Vector3f cnt1,
        Vector3f cnt2, Vector3f end);
    ~CPath();

    void SetPath(int Type, Vector3f start, Vector3f cnt1,
        Vector3f cnt2, Vector3f end);
    void Shutdown();

    int m_type;          // Type of path this is.

    Vector3f m_startPos; // Start location of this path.
    Vector3f m_control1; // Control point one (only for curve paths).
    Vector3f m_control2; // Control point two (only for curve paths).
    Vector3f m_endPos;   // Destination location.

    float m_start;       // Starting lenght for this path animation.
    float m_total;       // Total length distance of this animation path.

    CPath* m_next;       // Pointer to the next path in our link list.
};

class CRoute
{
public:
    CRoute();
    ~CRoute();

    bool AddPath(int Type, Vector3f start, Vector3f cnt1,
        Vector3f cnt2, Vector3f end);

    void GetPosition(float time, Vector3f& objPos);

    Vector3f CalcBezierCurvePos(Vector3f start, Vector3f c1,
        Vector3f c2, Vector3f end,
        float Scalar);
    Vector3f CalcStriaghtPath(Vector3f start,
        Vector3f end, float Scalar);
    void Shutdown();

    CPath* m_path;
    float m_startTime;
};

#endif