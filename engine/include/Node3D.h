#include "Node.h"
#include "math/Matrix.h"

class Node3D :public Node
{
public:
    Node3D() = default;
    ~Node3D() override = default;

    virtual void Start() override;
    virtual void Tick(float deltaTime) override;

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

    Matrix4x4f  m_worldMatrix{}; // ワールドマトリックス
    Matrix4x4f  m_viewMatrix{};  // ビューマトリックス

    Vector3f m_translation{ 0 }; // オブジェクトの平行移動。
    Vector3f m_rotation{ 0 };   // オブジェクトの回転。

    Matrix4x4f m_translationMatrix{}; // Translation matrix.
    Matrix4x4f m_rotationMatrix{};    // Rotation matrix.

};

class ModelNode :public Node3D
{
public:
    ModelNode(const std::string& file_name);
    ~ModelNode() override;

    void Start() override;
    void Tick(float deltaTime) override;

protected:
    virtual void Render(float deltaTime);
    int32_t m_modelId{};
};

class AnimationNode3D :public ModelNode
{
public:
    AnimationNode3D(const std::string& file_name) :ModelNode(file_name) {}
    ~AnimationNode3D() override;

    void Start() override;
    void Tick(float deltaTime) override;

protected:
    void Render(float deltaTime) override;

    float m_animationSpeed = 0.0f;

};

class TerrainNode :public Node3D
{
public:
    /**
     * \brief ハイトマップからメッシュフィールド作成
     * \param heightmapFileName ファイル名
     * \param numVertsPerRow 行の頂点数
     * \param numVertsPerCol 縦の頂点数
     * \param cellSpacing 頂点の間隔
     * \param heightScale 高さのスケール
     */
    TerrainNode(const std::string& heightmapFileName,
        int32_t numVertsPerRow,
        int32_t numVertsPerCol,
        int32_t cellSpacing,
        float heightScale);

    ~TerrainNode() override;

    void Start() override;
    void Tick(float deltaTime) override;

    virtual bool SetTexture(const std::string& fileName);

    int32_t  getHeightmapEntry(int32_t row, int32_t col);
    void setHeightmapEntry(int32_t row, int32_t col, int32_t value);

    float getHeight(float x, float z);

protected:
    int32_t m_numVertsPerRow;// 行の頂点数
    int32_t m_numVertsPerCol;// 列の頂点数
    int32_t m_cellSpacing;// 頂点の間隔

    int32_t m_numCellsPerRow;// 行のセルの数
    int32_t m_numCellsPerCol;// 列のセルの数
    int32_t m_width;// 横のサイズ
    int32_t m_depth;// 奥行きのサイズ
    int32_t m_numVertices;// 頂点数
    int32_t m_numTriangles;// 三角形の数

    float m_heightScale;// ハイトマップのスケール

    // ハイトマップ格納配列
    std::vector<int32_t> m_heightmap{};

    // スタティックバッファのid
    int32_t m_staticId;
    // テクスチャのid
    int32_t m_texId = -1;

    // helper methods
    // ハイトマップからデータを読み込む関数
    bool  readRawFile(const std::string& fileName);
    bool  CreateStaticBuffer();
    bool  lightTerrain(Vector3f& directionToLight);
    float computeShade(int cellRow, int cellCol, Vector3f& directionToLight);
    void Render();
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
    virtual void Tick(float deltaTime) override;

private:

    Vector3f m_right, m_up, m_look;
    CameraType  m_cameraType;
    Matrix4x4f m_matView;
};
