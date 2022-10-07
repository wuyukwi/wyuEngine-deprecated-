/*********************************************************************************

  *FileName: Camera.cpp
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/15

**********************************************************************************/

#include"Camera.h"
#include<string>

CCamera::CCamera()
{
    m_pos = Vector3f(100.0f, 50.0f, 20.0f);
    m_right = Vector3f(1.0f, 0.0f, 0.0f);
    m_up = Vector3f(0.0f, 1.0f, 0.0f);
    m_look = Vector3f(0.0f, 0.0f, 1.0f);
    m_cameraType = FOLLOW;

    // マトリックス標準化処理
    BuildMatrixIdentity(m_matView);
}

void CCamera::SetCamera(Vector3f pos, Vector3f right, Vector3f up, Vector3f look)
{
    this->m_pos = pos;
    this->m_right = right;
    this->m_up = up;
    this->m_look = look;
}

void CCamera::walk(float units)
{
    // 着地オブジェクトはxz軸のみ移動します
    if (m_cameraType == FOLLOW)
        VectorAdd(m_pos, m_pos, Vector3f(m_look.x, 0.0f, m_look.z) * units);

    else if (m_cameraType == FREE)
        VectorAdd(m_pos, m_pos, m_look * units);
}

void CCamera::strafe(float units)
{
    // 着地オブジェクトはxz軸のみ移動します
    if (m_cameraType == FOLLOW)
        VectorAdd(m_pos, m_pos, Vector3f(m_right.x, 0.0f, m_right.z) * units);

    else if (m_cameraType == FREE)
        VectorAdd(m_pos, m_pos, m_right * units);
}

void CCamera::fly(float units)
{
    // 着地オブジェクトはy軸のみ移動します
    if (m_cameraType == FOLLOW)
        m_pos.y += units;
    else if (m_cameraType == FREE)
        VectorAdd(m_pos, m_pos, m_up * units);
}

void CCamera::pitch(float angle)
{
    Matrix4x4f T;

    BuildMatrixRotationAxis(T, m_right, angle);

    BuildVec3TransformCoord(m_up, m_up, T);
    BuildVec3TransformCoord(m_look, m_look, T);
}

void CCamera::yaw(float angle)
{
    Matrix4x4f T;

    // マトリックス標準化処理
    BuildMatrixIdentity(T);

    // 着地オブジェクトに対して世界y（0、1、0）を中心に回転します
    if (m_cameraType == FOLLOW)
        BuildMatrixRotationY(T, angle);

    // フリーカメラは自分のupベクトルの回って回転します
    if (m_cameraType == FREE)
        BuildMatrixRotationAxis(T, m_up, angle);

    BuildVec3TransformCoord(m_right, m_right, T);
    BuildVec3TransformCoord(m_look, m_look, T);
}

void CCamera::roll(float angle)
{
    // フリータイプの場合だけroll出来ます
    if (m_cameraType == FREE)
    {
        Matrix4x4f T;

        // マトリックス標準化処理
        BuildMatrixIdentity(T);

        BuildMatrixRotationAxis(T, m_look, angle);

        BuildVec3TransformCoord(m_right, m_right, T);
        BuildVec3TransformCoord(m_up, m_up, T);
    }
}

Matrix4x4f* CCamera::GetCameraView()
{
    // 	// カメラの軸を互いに直交にします
    Normalize(m_look);

    CrossProduct(m_up, m_look, m_right);
    Normalize(m_up);

    CrossProduct(m_right, m_up, m_look);
    Normalize(m_right);

    //  ビューマトリックスを作成します。
    const float x = -DotProduct(m_right, m_pos);
    const float y = -DotProduct(m_up, m_pos);
    const float z = -DotProduct(m_look, m_pos);

    m_matView.data[0][0] = m_right.x; m_matView.data[0][1] = m_up.x; m_matView.data[0][2] = m_look.x; m_matView.data[0][3] = 0.0f;
    m_matView.data[1][0] = m_right.y; m_matView.data[1][1] = m_up.y; m_matView.data[1][2] = m_look.y; m_matView.data[1][3] = 0.0f;
    m_matView.data[2][0] = m_right.z; m_matView.data[2][1] = m_up.z; m_matView.data[2][2] = m_look.z; m_matView.data[2][3] = 0.0f;
    m_matView.data[3][0] = x;         m_matView.data[3][1] = y;      m_matView.data[3][2] = z;        m_matView.data[3][3] = 1.0f;

    return &m_matView;
}