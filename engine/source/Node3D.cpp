#include "Node3D.h"
#include "gameEngine.h"

Node3D::Node3D(const std::string& file_name)
{
    g_pEngine->GetRenderer()->LoadXModel(file_name, m_modelId);
}

Node3D::~Node3D()
{
    g_pEngine->GetRenderer()->ReleaseXModel(m_modelId);
}

void Node3D::Start()
{
    // マトリックス標準化処理
    BuildMatrixIdentity(m_rotationMatrix);
    BuildMatrixIdentity(m_translationMatrix);
    BuildMatrixIdentity(m_viewMatrix);
    BuildMatrixIdentity(m_worldMatrix);

    Node::Start();
}

void Node3D::Tick(float delta)
{
    // 平行移動マトリックスの更新
    BuildMatrixTranslation(m_translationMatrix, m_translation.x, m_translation.y, m_translation.z);

    // 回転マトリックスの更新
    Matrix4x4f rotX, rotY;
    BuildMatrixIdentity(rotX);
    BuildMatrixIdentity(rotY);
    BuildMatrixRotationX(rotX, m_rotation.x);
    BuildMatrixRotationY(rotY, m_rotation.y);
    BuildMatrixRotationZ(m_rotationMatrix, m_rotation.z);
    MatrixMultiBy(m_rotationMatrix, m_rotationMatrix, rotX);
    MatrixMultiBy(m_rotationMatrix, m_rotationMatrix, rotY);

    // ワールドマトリックスの更新
    MatrixMultiBy(m_worldMatrix, m_rotationMatrix, m_translationMatrix);

    //g_pEngine->GetRenderer()->SetWorldMatrix(m_worldMatrix);
    //    static float y = 0.0f;
    //    D3DXMATRIX yRot;
    //    D3DXMatrixRotationY(&yRot, y);
    //    y += delta;
    //
    //    if (y >= 6.28f)
    //        y = 0.0f;
    //
    //    // D3DXMATRIX World = yRot;
    //
    //    Matrix4x4f temp = {
    //yRot.m[0][0], yRot.m[0][1], yRot.m[0][2], yRot.m[0][3],
    //yRot.m[1][0], yRot.m[1][1], yRot.m[1][2], yRot.m[1][3],
    //yRot.m[2][0], yRot.m[2][1], yRot.m[2][2], yRot.m[2][3],
    //yRot.m[3][0], yRot.m[3][1], yRot.m[3][2], yRot.m[3][3],
    //    };

    if (m_isRender)
    {
        Render(delta);
    }

    Node::Tick(delta);
}

Matrix4x4f Node3D::GetWorldMatrix()
{
    return m_worldMatrix;
}

void Node3D::SetWorldMatrix(Matrix4x4f& mat)
{
    m_worldMatrix = mat;
}

void Node3D::SetTranslation(float x, float y, float z)
{
    m_translation.x = x;
    m_translation.y = y;
    m_translation.z = z;
}

void Node3D::SetTranslation(const Vector3f& translation)
{
    m_translation = translation;
}

void Node3D::AddTranslation(float x, float y, float z)
{
    m_translation.x += x;
    m_translation.y += y;
    m_translation.z += z;
}

void Node3D::AddTranslation(const Vector3f& translation)
{
    m_translation += translation;
}

Vector3f Node3D::GetTranslation() const
{
    return m_translation;
}

void Node3D::SetRotation(float x, float y, float z)
{
    m_rotation.x = x;
    m_rotation.y = y;
    m_rotation.z = z;
}

void Node3D::SetRotation(Vector3f rotation)
{
    m_rotation = rotation;
}

void Node3D::AddRotation(float x, float y, float z)
{
    m_rotation.x += x;
    m_rotation.y += y;
    m_rotation.z += z;
}

void Node3D::AddRotation(Vector3f rotation)
{
    m_rotation += rotation;
}

Vector3f Node3D::GetRotation() const
{
    return m_rotation;
}

void Node3D::Render(float delta)
{
    g_pEngine->GetRenderer()->SetWorldMatrix(m_worldMatrix);
    g_pEngine->GetRenderer()->RenderXModel(m_modelId);
}


AnimationNode3D::~AnimationNode3D()
= default;

void AnimationNode3D::Start()
{
    Node3D::Start();
}

void AnimationNode3D::Tick(float delta)
{
    Node3D::Tick(delta);
}

void AnimationNode3D::Render(float delta)
{
    g_pEngine->GetRenderer()->UpdateXAnimation(m_modelId, delta, m_worldMatrix);
    g_pEngine->GetRenderer()->RenderXModel(m_modelId);
}

CameraNode::CameraNode()
{
    m_pos = Vector3f(4.0f, 4.0f, -13.0f);
    m_right = Vector3f(1.0f, 0.0f, 0.0f);
    m_up = Vector3f(0.0f, 1.0f, 0.0f);
    m_look = Vector3f(0.0f, 0.0f, 1.0f);

    m_cameraType = FOLLOW;

    // マトリックス標準化処理
    BuildMatrixIdentity(m_matView);
}

void CameraNode::SetCamera(Vector3f pos, Vector3f right, Vector3f up, Vector3f look)
{
    this->m_pos = pos;
    this->m_right = right;
    this->m_up = up;
    this->m_look = look;
}

void CameraNode::walk(float units)
{
    // 着地オブジェクトはxz軸のみ移動します
    if (m_cameraType == FOLLOW)
        VectorAdd(m_pos, m_pos, Vector3f(m_look.x, 0.0f, m_look.z) * units);

    else if (m_cameraType == FREE)
        VectorAdd(m_pos, m_pos, m_look * units);
}

void CameraNode::strafe(float units)
{
    // 着地オブジェクトはxz軸のみ移動します
    if (m_cameraType == FOLLOW)
        VectorAdd(m_pos, m_pos, Vector3f(m_right.x, 0.0f, m_right.z) * units);

    else if (m_cameraType == FREE)
        VectorAdd(m_pos, m_pos, m_right * units);
}

void CameraNode::fly(float units)
{
    // 着地オブジェクトはy軸のみ移動します
    if (m_cameraType == FOLLOW)
        m_pos.y += units;
    else if (m_cameraType == FREE)
        VectorAdd(m_pos, m_pos, m_up * units);
}

void CameraNode::pitch(float angle)
{
    Matrix4x4f T;

    BuildMatrixRotationAxis(T, m_right, angle);

    BuildVec3TransformCoord(m_up, m_up, T);
    BuildVec3TransformCoord(m_look, m_look, T);
}

void CameraNode::yaw(float angle)
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

void CameraNode::roll(float angle)
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


void CameraNode::Start()
{
    Node::Start();
}

void CameraNode::Tick(float delta)
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

    g_pEngine->GetRenderer()->SetViewMatrix(m_matView);

    Node::Tick(delta);
}
