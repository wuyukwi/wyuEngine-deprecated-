/*********************************************************************************

  *FileName: XMesh.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/15

**********************************************************************************/

#ifndef _X_ANIMATED_MODEL_H_
#define _X_ANIMATED_MODEL_H_

#include <string>
#include <unordered_map>

#include"XStructures.h"

class CXModel
{
public:
    CXModel();
    ~CXModel() { }

    void SetDevice(LPDIRECT3DDEVICE9 device, D3DCAPS9 caps)
    {
        m_pDevice = device;
        m_caps = caps;
    }

    bool LoadXFile(const std::string& file_name);
    void Shutdown();

    void GetBoundingSphere(D3DXVECTOR3* center, float* radius)
    {
        if (center) *center = m_center;
        if (radius) *radius = m_radius;
    }

    void Update(float time, const LPD3DXMATRIX matWorld);

    void Render();

    void SetAnimation(unsigned int index);
    void NextAnimation();

    unsigned int GetCurrentAnimation() { return m_currentAni; }
    unsigned int GetTotalAnimations() { return m_numAnimations; }

    float GetAnimationSpeed() { return m_speed; }
    void IncreaseAnimationSpeed(float amt) { m_speed += amt; }
    void DecreaseAnimationSeep(float amt)
    {
        if (m_speed > 0.1f) m_speed -= amt;
    }

    float GetTransitionSpeed() { return m_transition; }
    void DecreaseTransitionSpeed(float amt) { m_transition += amt; }
    void IncreaseTransitionSeep(float amt)
    {
        if (m_transition > 0.1f) m_transition -= amt;
    }

private:
    HRESULT SetupBoneMatrixPointers(LPD3DXFRAME pFrameBase, LPD3DXFRAME pFrameRoot);
    void UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix);
    void DrawFrame(LPD3DXFRAME frame);
    void DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase);

    bool CreateBounding(LPD3DXFRAME pFrameBase, LPD3DXFRAME pFrameRoot);
    void DrwaBounding(const std::string& name);

private:
    // D3D デバイス
    LPDIRECT3DDEVICE9 m_pDevice;
    D3DCAPS9 m_caps;

    // ルートフレームとアニメーションコントローラー。
    LPD3DXFRAME m_pRoot;
    LPD3DXANIMATIONCONTROLLER m_pAnimControl;
    CAllocateHierarchy* m_pAllocateHier;

    // ボーン.
    D3DXMATRIX* m_boneMatrices;
    unsigned int m_maxBones;

    // 現在のメッシュコンテナ。
    stD3DContainerDerived* m_currentContainer;

    // バウンディングスフィア。
    std::unordered_map<std::string, LPD3DXMESH> m_pSphere;
    LPD3DXMESH m_pSphere1;
    D3DXVECTOR3 m_center;
    float m_radius;

    // バウンディングボックス
    std::unordered_map<std::string, LPD3DXMESH> m_pBox;
    D3DXVECTOR3 m_min;
    D3DXVECTOR3 m_max;

    // アニメーション変数。
    unsigned int m_currentAni;
    unsigned int m_numAnimations;
    unsigned long m_currentTrack;
    float m_speed;
    float m_currentTime;
    float m_transition;
};

#endif