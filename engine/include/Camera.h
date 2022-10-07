/*********************************************************************************

  *FileName: Camera.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/15

**********************************************************************************/

#ifndef CCAMERA_H
#define CCAMERA_H

#include"math/Vector.h"
#include"math/Matrix.h"

class CCamera
{
public:
    enum CameraType { FOLLOW, FREE };

    CCamera();
    ~CCamera() {}

    void SetCamera(Vector3f pos, Vector3f right, Vector3f up, Vector3f look);

    void SetCameraType(CameraType type) { this->m_cameraType = type; }

    void strafe(float units); // left/right
    void fly(float units);    // up/down
    void walk(float units);   // forward/backward

    void pitch(float angle); // rotate on right vector
    void yaw(float angle);   // rotate on up vector
    void roll(float angle);  // rotate on look vector

    Vector3f* GetCameraPos() { return &m_pos; }
    Vector3f* GetCameraRight() { return &m_right; }
    Vector3f* GetCameraUp() { return &m_up; }
    Vector3f* GetCameraLook() { return &m_look; }

    Matrix4x4f* GetCameraView();

private:

    Vector3f m_pos;
    Vector3f m_right, m_up, m_look;
    CameraType  m_cameraType;
    Matrix4x4f m_matView;
};

#endif