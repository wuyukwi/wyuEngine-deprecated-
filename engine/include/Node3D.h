#include "Node.h"
#include "math/Matrix.h"

class Node3D :public Node
{
public:
    Node3D(const std::string& file_name);
    ~Node3D();

    virtual void Start() override;
    virtual void Tick(float delta) override;

    virtual Matrix4x4f GetWorldMatrix();
    virtual void SetWorldMatrix(Matrix4x4f& mat);

    virtual void SetTranslation(float x, float y, float z);
    virtual void SetTranslation(const Vector3f& translation);
    virtual void AddTranslation(float x, float y, float z);
    virtual void AddTranslation(const Vector3f& translation);
    virtual Vector3f GetTranslation() const;

    virtual void SetRotation(float x, float y, float z);
    virtual void SetRotation(Vector3f rotation);
    virtual void AddRotation(float x, float y, float z);
    virtual void AddRotation(Vector3f rotation);
    virtual Vector3f GetRotation() const;


protected:
    virtual void Render(float delta);

    int32_t m_modelId;
    Matrix4x4f  m_worldMatrix{}; // ワールドマトリックス
    Matrix4x4f  m_viewMatrix{};  // ビューマトリックス

    Vector3f m_translation{ 0 }; // オブジェクトの平行移動。
    Vector3f m_rotation{ 0 };   // オブジェクトの回転。

    Matrix4x4f m_translationMatrix{}; // Translation matrix.
    Matrix4x4f m_rotationMatrix{};    // Rotation matrix.

};

class AnimationNode3D :public Node3D
{
public:
    AnimationNode3D(const std::string& file_name) :Node3D(file_name) {}
    ~AnimationNode3D();

    void Start() override;
    void Tick(float delta) override;

protected:
    void Render(float delta) override;

    float m_animationSpeed = 0.0f;

};

class CameraNode :public Node
{
public:
    enum CameraType { FOLLOW, FREE };

    CameraNode();
    ~CameraNode() = default;

    void SetCamera(Vector3f pos, Vector3f right, Vector3f up, Vector3f look);

    void SetCameraType(CameraType type) { this->m_cameraType = type; }

    void strafe(float units); // left/right
    void fly(float units);    // up/down
    void walk(float units);   // forward/backward

    void pitch(float angle); // rotate on right vector
    void yaw(float angle);   // rotate on up vector
    void roll(float angle);  // rotate on look vector

    Vector3f GetCameraPos() { return m_pos; }
    Vector3f GetCameraRight() { return m_right; }
    Vector3f GetCameraUp() { return m_up; }
    Vector3f GetCameraLook() { return m_look; }

    Matrix4x4f GetCameraView() { return m_matView; }

    virtual void Start() override;
    virtual void Tick(float delta) override;

private:

    Vector3f m_right, m_up, m_look;
    CameraType  m_cameraType;
    Matrix4x4f m_matView;
};
