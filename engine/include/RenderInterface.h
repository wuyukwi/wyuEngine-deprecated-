/*********************************************************************************

  *FileName: RenderInterface.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/02

**********************************************************************************/

#ifndef _RENDERINTERFACE_H_
#define _RENDERINTERFACE_H_

#include <vector>
#include "defines.h"
#include "structs.h"
#include "material.h"
#include "light.h"
#include "math/Matrix.h"
#include "Camera.h"

class RenderInterface
{
public:
    RenderInterface() : m_screenWidth(0), m_screenHeight(0), m_fullscreen(false), m_mainHandle(nullptr), m_near(0), m_far(0) { }
    virtual ~RenderInterface() {}

    virtual bool Initialize(WinHWND mainWin) = 0;
#if IMGUI_ENABLE
    virtual bool SetupImGui(WinHWND mainWin) = 0;
    virtual void StartImGuiFrame(bool enable = true) = 0;
#endif

    virtual void OneTimeInit() = 0;
    virtual void Shutdown() = 0;

    virtual void SetClearCol(float r, float g, float b, float a) = 0;

    virtual void StartRender(bool bColor, bool bDepth, bool bStencil) = 0;
    virtual void ClearBuffers(bool bColor, bool bDepth, bool bStencil) = 0;
    virtual void EndRendering() = 0;

    virtual void SetMaterial(Material& mat) = 0;
    virtual void SetLight(stLight& light, uint32_t index) = 0;
    virtual void DisableLight(size_t index) = 0;

    virtual bool SetMultiSample(int32_t samples) = 0;
    virtual void EnableMultiSample(bool samples) = 0;

    virtual void SetD3D9RenderState(RenderState state) = 0;
    virtual void SetDepthTesting(RenderState state) = 0;
    virtual void SetTransparent(RenderState state, TransperentState src, TransperentState dst) = 0;

    virtual int AddTexture2D(const std::string& file_name, int& texId) = 0;
    virtual bool AddTexture2D(const std::string& file_name, int& texId, Vector2f& size) = 0;
    virtual void SetTextureFilter(size_t index, TextureState filter, FilterType type) = 0;
    virtual void SetTextureAlphaBlend(bool enable) = 0;
    virtual void SetMultiTexture() = 0;
    virtual void SetDetailMapping() = 0;
    virtual void ApplyTexture(size_t index, int texId) = 0;
    virtual void SaveScreenShot(const char* file) = 0;

    virtual void EnablePointSprites(float size, float min, float a, float b, float c) = 0;
    virtual void DisablePointSprites() = 0;

    virtual bool CreateText(const char* font, int weight, bool italic, int size, int& id) = 0;
    virtual bool DisplayText(int id, long x, long y, int r, int g, int b, const char* text, ...) = 0;
    virtual bool DisplayText(int id, long x, long y, unsigned long color, const char* text, ...) = 0;

    virtual void EnableFog(float start, float end, FOG_TYPE type, unsigned long color, bool rangeFog) = 0;
    virtual void DisableFog() = 0;

    virtual void CalculateProjMatrix(float fov, float n, float f) = 0;
    virtual void CalculateOrthoMatrix(float n, float f) = 0;

    virtual int CreateStaticBuffer(VertexType, PrimType,
        int32_t totalVerts, int32_t totalIndices,
        int32_t stride, void** data, int16_t* indices,
        int32_t* staticId) = 0;

    virtual void SetStaticBuffer(int32_t staticId, void** data) const = 0;

    virtual bool LoadXModel(const std::string& file_name, int32_t& xModelId) = 0;

    virtual void ReleaseAllStaticBuffers() = 0;
    virtual void ReleaseAllXModels() = 0;
    virtual int ReleaseStaticBuffer(int staticID) = 0;
    virtual int ReleaseXModel(int xModelId) = 0;
    virtual void ReleaseTexture(int32_t texid) = 0;

    virtual int RenderStaticBuffer(int32_t staticId) = 0;
    virtual int RenderXModel(int xModelId) = 0;

    virtual void GetXModelBoundingSphere(int xModelId, Vector3f* origin, float* radius) = 0;

    virtual void UpdateXAnimation(int xModelId, float time, Matrix4x4f& mat) = 0;

    virtual void SetViewMatrix(Matrix4x4f& mat) = 0;
    virtual void SetWorldMatrix(Matrix4x4f& mat) = 0;
    virtual void GetProjectMatrix(Matrix4x4f& mat) = 0;

protected:
    int m_screenWidth;
    int m_screenHeight;
    bool m_fullscreen;

    WinHWND m_mainHandle;

    float m_near;
    float m_far;

};

#endif