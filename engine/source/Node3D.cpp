#include <fstream>
#include <fstream>
#include <sstream>

#include "Node3D.h"
#include "gameEngine.h"

void Node3D::Start()
{
    // マトリックス標準化処理
    BuildMatrixIdentity(m_rotationMatrix);
    BuildMatrixIdentity(m_translationMatrix);
    BuildMatrixIdentity(m_viewMatrix);
    BuildMatrixIdentity(m_worldMatrix);

    Node::Start();
}

void Node3D::Tick(float deltaTime)
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
    //    y += deltaTime;
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



    Node::Tick(deltaTime);
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

// ModelNode
ModelNode::ModelNode(const std::string& file_name)
{
    g_pEngine->GetRenderer()->LoadXModel(file_name, m_modelId);
}

ModelNode::~ModelNode()
{
    g_pEngine->GetRenderer()->ReleaseXModel(m_modelId);
}

void ModelNode::Start()
{
    Node3D::Start();
}

void ModelNode::Tick(float deltaTime)
{
    Node3D::Tick(deltaTime);

    if (m_isRender)
    {
        Render(deltaTime);
    }
}

void ModelNode::Render(float deltaTime)
{
    g_pEngine->GetRenderer()->SetWorldMatrix(m_worldMatrix);
    g_pEngine->GetRenderer()->RenderXModel(m_modelId);
}


// AnimationNode3D
AnimationNode3D::~AnimationNode3D()
= default;

void AnimationNode3D::Start()
{
    ModelNode::Start();
}

void AnimationNode3D::Tick(float deltaTime)
{
    ModelNode::Tick(deltaTime);
}

void AnimationNode3D::Render(float deltaTime)
{
    g_pEngine->GetRenderer()->UpdateXAnimation(m_modelId, deltaTime, m_worldMatrix);
    g_pEngine->GetRenderer()->RenderXModel(m_modelId);
}

TerrainNode::TerrainNode(const std::string& heightmapFileName, int32_t numVertsPerRow, int32_t numVertsPerCol, int32_t cellSpacing,
    float heightScale)
{
    m_numVertsPerRow = numVertsPerRow;
    m_numVertsPerCol = numVertsPerCol;
    m_cellSpacing = cellSpacing;

    // セル数算出
    m_numCellsPerRow = m_numVertsPerRow - 1;
    m_numCellsPerCol = m_numVertsPerCol - 1;

    // 横と奥行きのサイズ算出
    m_width = m_numCellsPerRow * m_cellSpacing;
    m_depth = m_numCellsPerCol * m_cellSpacing;

    // 頂点数と三角形数算出
    m_numVertices = m_numVertsPerRow * m_numVertsPerCol;
    m_numTriangles = m_numCellsPerRow * m_numCellsPerCol * 2;

    m_heightScale = heightScale;

    // ハイトマップからデータを読み込む
    if (!readRawFile(heightmapFileName))
    {
        std::cout << "readRawFile - FAILED" << std::endl;
    }


    // scale heights
    for (size_t i = 0; i < m_heightmap.size(); i++)
        m_heightmap[i] *= heightScale;

    // compute the vertices
    if (!CreateStaticBuffer())
    {
        ::MessageBox(0, "CreateStaticBuffer - FAILED", 0, 0);
        ::PostQuitMessage(0);
    }
}

TerrainNode::~TerrainNode()
{
}

void TerrainNode::Start()
{
    Node3D::Start();
}

void TerrainNode::Tick(float deltaTime)
{
    Node3D::Tick(deltaTime);

    if (m_isRender)
    {
        this->Render();
    }
}

bool TerrainNode::SetTexture(const std::string& fileName)
{
    return g_pEngine->GetRenderer()->AddTexture2D(fileName, m_texId);
}

int32_t TerrainNode::getHeightmapEntry(int32_t row, int32_t col)
{
    return m_heightmap[row * m_numVertsPerRow + col];
}

void TerrainNode::setHeightmapEntry(int32_t row, int32_t col, int32_t value)
{
    m_heightmap[row * m_numVertsPerRow + col] = value;
}

float TerrainNode::getHeight(float x, float z)
{
    // xz 平面上で平行移動します。
    x = ((float)m_width / 2.0f) + x;
    z = ((float)m_depth / 2.0f) - z;

    // セル間隔を 1 に正規化する
    x /= (float)m_cellSpacing;
    z /= (float)m_cellSpacing;


    // これからは、正の z 軸を「上」方向ではなく「下」方向に進むものとして解釈します。
    // これにより、x と z の小数点以下を切り捨てるするだけで行と列を抽出できます。
    const float col = ::floorf(x);
    const float row = ::floorf(z);

    // 現在いるクワッドの高さを取得します:
    // 
    //  A   B
    //  *---*
    //  | / |
    //  *---*  
    //  C   D

    float A = getHeightmapEntry(row, col);
    float B = getHeightmapEntry(row, col + 1);
    float C = getHeightmapEntry(row + 1, col);
    float D = getHeightmapEntry(row + 1, col + 1);

    //
    // 三角形を見つけます:
    //

    // 現在のセルの左上隅を原点とする変換によって変換します。セル間隔が 1 に正規化された。したがって
    // +x -> 'right' および +z -> 'down' システムの原点に単位平方があります。
    float dx = x - col;
    float dz = z - row;

    auto lerp = [](float a, float b, float t) {
        return a - (a * t) + (b * t);
    };


    float height = 0.0f;
    if (dz < 1.0f - dx)  // 上の三角形 ABC
    {
        const float uy = B - A; // A->B
        const float vy = C - A; // A->C

        // 高さは、{A} から始まるベクトル u と v の頂点の高さに、各ベクトル u と v を補間して求めた高さを加えたものです。
        height = A + lerp(0.0f, uy, dx) + lerp(0.0f, vy, dz);
    }
    else // 下の三角形 ABC
    {
        const float uy = C - D; // D->C
        const float vy = B - D; // D->B

        // 高さは、{D} から始まるベクトル u と v の頂点の高さに、各ベクトル u と v を補間して求めた高さを加えたものです。
        height = D + lerp(0.0f, uy, 1.0f - dx) + lerp(0.0f, vy, 1.0f - dz);
    }

    return height;
}


bool TerrainNode::readRawFile(const std::string& fileName)
{
    // Restriction: RAW file dimensions must be >= to the
    // dimensions of the terrain.  That is a 128x128 RAW file
    // can only be used with a terrain constructed with at most
    // 128x128 vertices.

    if (fileName != "null")
    {

        // A height for each vertex
        std::vector<BYTE> in(m_numVertices);

        std::ifstream inFile(g_pEngine->GetPathManager()->GetFileFullPath(fileName), std::ios_base::binary);

        if (!inFile)
            return false;

        //    std::stringstream buffer;
        //    buffer << inFile.rdbuf();
        //    const std::string readRawFile(buffer.str());
        //
        //#if _DEBUG
        //    std::cout << readRawFile << std::endl;
        //#endif

        inFile.read(
            reinterpret_cast<char*>(&in[0]), // buffer
            in.size());// number of bytes to read into buffer

        inFile.close();

        // copy BYTE vector to int vector
        m_heightmap.resize(m_numVertices);

        for (size_t i = 0; i < in.size(); i++)
            m_heightmap[i] = in[i];

    }
    else
    {
        m_heightmap.resize(m_numVertices);

        for (int32_t i = 0; i < m_numVertices; i++)
            m_heightmap[i] = 0;
    }
    return true;
}

bool TerrainNode::CreateStaticBuffer()
{
    // 頂点の生成を開始する座標
    const int32_t startX = -m_width / 2;
    const int32_t startZ = m_depth / 2;

    // 頂点の生成を終了する座標
    const int32_t endX = m_width / 2;
    const int32_t endZ = -m_depth / 2;

    // ある頂点から次の頂点までのテクスチャ座標の増分サイズを計算します。
    float uCoordIncrementSize = 1.0f / (float)m_numCellsPerRow;
    float vCoordIncrementSize = 1.0f / (float)m_numCellsPerCol;

    const auto vertex = new NodeVertex3D[m_numVertices];

    int32_t i = 0;
    for (int z = startZ; z >= endZ; z -= m_cellSpacing)
    {
        int j = 0;
        for (int x = startX; x <= endX; x += m_cellSpacing)
        {
            // ネストされたループのどこにいるかに基づいて
            // 頂点バッファーとハイトマップへの正しいインデックスを計算します。
            int index = i * m_numVertsPerRow + j;

            //v[index] = NodeVertex3D(
            //    Vector3f{ (float)x,(float)m_heightmap[index],(float)z },
            //    Vector2f{ (float)j * uCoordIncrementSize,(float)i * vCoordIncrementSize });

            vertex[index] = NodeVertex3D(
                Vector3f{ (float)x,(float)m_heightmap[index],(float)z },
                Vector2f{ float(j % 2),float(i % 2) });

            j++; // 次の列
        }
        i++; // 次の行
    }
    for (size_t i = 0; i < m_numVertices; i++)
    {
        std::cout << "m_numVertices" << i << ":" << "(" << vertex[i].tex.u << "," << vertex[i].tex.v << ")" << std::endl;
    }

    const int32_t numIndices = m_numTriangles * 3;
    const auto indices = new int16_t[numIndices];

    // クワッドを構成する 2 つの三角形を表す 6 つのインデックスのグループの先頭へのインデックス
    int32_t baseIndex = 0;

    // ループして各クワッドの三角形を計算します
    for (int32_t col = 0; col < m_numCellsPerCol; col++)
    {
        for (int32_t row = 0; row < m_numCellsPerRow; row++)
        {
            indices[baseIndex] = col * m_numVertsPerRow + row;
            indices[baseIndex + 1] = col * m_numVertsPerRow + row + 1;
            indices[baseIndex + 2] = (col + 1) * m_numVertsPerRow + row;

            indices[baseIndex + 3] = (col + 1) * m_numVertsPerRow + row;
            indices[baseIndex + 4] = col * m_numVertsPerRow + row + 1;
            indices[baseIndex + 5] = (col + 1) * m_numVertsPerRow + row + 1;

            // 次の列
            baseIndex += 6;
        }
    }

    //indices[0] = 0;
    //indices[1] = 1;
    //indices[2] = 2;
    //indices[3] = 2;
    //indices[4] = 1;
    //indices[5] = 3;

    //for (size_t i = 0; i < numIndices; i++)
    //{
    //    std::cout << "baseIndex" << i << ":" << indices[i] << std::endl;
    //}

    g_pEngine->GetRenderer()->CreateStaticBuffer(
        FVF_3D, PrimType::TRIANGLE_LIST,
        m_numVertices, numIndices, sizeof(NodeVertex3D),
        reinterpret_cast<void**>(vertex), indices, &m_staticId);

    return true;
}

void TerrainNode::Render()
{
    g_pEngine->GetRenderer()->SetWorldMatrix(m_worldMatrix);
    g_pEngine->GetRenderer()->SetTextureAlphaBlend(true);
    g_pEngine->GetRenderer()->ApplyTexture(1, m_texId);
    g_pEngine->GetRenderer()->RenderStaticBuffer(m_staticId);
    g_pEngine->GetRenderer()->ApplyTexture(0, -1);
    g_pEngine->GetRenderer()->SetTextureAlphaBlend(false);
}

// CameraNode
CameraNode::CameraNode()
{
    m_pos = Vector3f(0.0f, 0.0f, 0.0f);
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
        m_pos += Vector3f(m_look.x, 0.0f, m_look.z) * units;

    else if (m_cameraType == FREE)
        m_pos += m_look * units;
}

void CameraNode::strafe(float units)
{
    // 着地オブジェクトはxz軸のみ移動します
    if (m_cameraType == FOLLOW)
        m_pos += Vector3f(m_right.x, 0.0f, m_right.z) * units;

    else if (m_cameraType == FREE)
        m_pos += m_right * units;
}

void CameraNode::fly(float units)
{
    // 着地オブジェクトはy軸のみ移動します
    if (m_cameraType == FOLLOW)
        m_pos.y += units;
    else if (m_cameraType == FREE)
        m_pos += m_up * units;
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
