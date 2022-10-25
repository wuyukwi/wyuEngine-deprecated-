/*********************************************************************************

  *FileName: D3D9Renderer.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/02

**********************************************************************************/

#ifndef _D3D_RENDERER_H_
#define _D3D_RENDERER_H_

// ビルド時の警告対処用マクロ
#define DIRECTINPUT_VERSION	(0x0800)

#include <Windows.h>
#include <d3d9.h>
#include "RenderInterface.h"
#include"XMesh.h"

#if IMGUI_ENABLE
#include "imgui.h"
#include "backends\imgui_impl_win32.h"
#include "backends\imgui_impl_dx9.h"
#endif

struct stD3D9StaticBuffer
{
    stD3D9StaticBuffer() : vbPtr(0), ibPtr(0), numVerts(0),
        numIndices(0), stride(0), fvf(0),
        primType(PrimType::NULL_TYPE) {}

    LPDIRECT3DVERTEXBUFFER9 vbPtr;
    LPDIRECT3DINDEXBUFFER9 ibPtr;
    int32_t numVerts;
    int32_t numIndices;
    int32_t stride;
    int32_t fvf;
    PrimType primType;
};

struct stD3D9Texture
{
    stD3D9Texture() : fileName(nullptr), width(0), height(0), image(nullptr) {}

    char* fileName;
    float width, height;
    LPDIRECT3DTEXTURE9 image;
};

struct stD3D9Texture2D
{
    stD3D9Texture2D() :image(nullptr), ret(0) {}

    std::string fileName;
    LPDIRECT3DTEXTURE9 image;
    int32_t ret;
    D3DXIMAGE_INFO info{};
};

class D3D9Renderer : public RenderInterface
{
public:
    D3D9Renderer();
    ~D3D9Renderer() override;

    bool Initialize(WinHWND mainWin) override;

#if IMGUI_ENABLE
    bool SetupImGui(WinHWND mainWin) override;
    void StartImGuiFrame(bool enable = true) override;
#endif

    void Shutdown() override;

    void SetClearCol(float r, float g, float b, float a) override;
    void StartRender(bool bColor, bool bDepth, bool bStencil) override;
    void ClearBuffers(bool bColor, bool bDepth, bool bStencil) override;
    void EndRendering() override;

    void SetMaterial(Material& mat) override;
    void SetLight(stLight& light, uint32_t index) override;
    void DisableLight(size_t index) override;

    bool SetMultiSample(int32_t samples)  override;
    void EnableMultiSample(bool samples) override;

    void SetD3D9RenderState(RenderState state) override;
    void SetDepthTesting(RenderState state) override;
    void SetTransparent(RenderState state, TransperentState src, TransperentState dst) override;

    int AddTexture2D(const std::string& file_name, int& texId) override;
    bool AddTexture2D(const std::string& file_name, int& texId, Vector2f& size) override;
    void SetTextureFilter(size_t index, TextureState filter, FilterType type) override;
    void SetTextureAlphaBlend(bool enable) override;
    void SetMultiTexture() override;
    void SetDetailMapping() override;
    void ApplyTexture(size_t index, int texId) override;
    void SaveScreenShot(const char* file) override;

    virtual void EnablePointSprites(float size, float min, float a, float b, float c) override;
    virtual void DisablePointSprites() override;

    bool CreateText(const char* font, int weight, bool italic, int size, int& id) override;
    bool DisplayText(int id, long x, long y, int r, int g, int b, const char* text, ...) override;
    bool DisplayText(int id, long x, long y, unsigned long color, const char* text, ...) override;

    void CalculateProjMatrix(float fov, float n, float f) override;
    void CalculateOrthoMatrix(float n, float f) override;

    void EnableFog(float start, float end, FOG_TYPE type, unsigned long color, bool rangeFog) override;
    void DisableFog() override;

    // スタティックバファ作成関数
    int CreateStaticBuffer(VertexType, PrimType,
        int32_t totalVerts, int32_t totalIndices,
        int32_t stride, void** data, int16_t* indices,
        int32_t* staticId) override;

    void SetStaticBuffer(int32_t staticId, void** data) const override;

    bool LoadXModel(const std::string& file_name, int32_t& xModelId) override;

    void ReleaseAllStaticBuffers() override;
    void ReleaseAllXModels() override;
    int ReleaseStaticBuffer(int staticID) override;
    int ReleaseXModel(int xModelId) override;
    void ReleaseTexture(int32_t texid) override;

    int RenderStaticBuffer(int32_t staticId) override;
    int RenderXModel(int xModelId) override;

    void GetXModelBoundingSphere(int xModelId, Vector3f* origin, float* radius) override
    {
        if (xModelId < 0 || m_xModelsList.empty())
            return;

        D3DXVECTOR3 center;
        float r;

        m_xModelsList[xModelId].GetBoundingSphere(&center, &r);

        if (origin)
        {
            origin->x = center.x;
            origin->y = center.y;
            origin->z = center.z;
        }

        if (radius)
            *radius = r;
    }

    void UpdateXAnimation(int xModelId, float time, Matrix4x4f& mat) override;

    void SetViewMatrix(Matrix4x4f& mat) override;
    void SetWorldMatrix(Matrix4x4f& mat) override;
    void GetProjectMatrix(Matrix4x4f& mat) override;
private:
    void OneTimeInit() override;

private:
    D3DXCOLOR m_Color;
    LPDIRECT3D9 m_Direct3D;
    LPDIRECT3DDEVICE9 m_Device;
    D3DDISPLAYMODE m_d3ddm{};
    D3DPRESENT_PARAMETERS m_d3dpp{};
    D3DCAPS9 m_caps{};

    bool m_renderingScene;

    std::vector<stD3D9StaticBuffer> m_staticBufferList;
    int m_activeStaticBuffer;

    std::vector<stD3D9Texture> m_textureList;
    std::vector<stD3D9Texture2D> m_textureList2D;
    std::string m_texturePath;

    std::vector<LPD3DXFONT> m_fonts;

    std::vector<CXModel> m_xModelsList;

    D3DXMATRIX m_projection;
    D3DXMATRIX m_view;
};

bool CreateD3DRenderer(RenderInterface** pObj);

#endif