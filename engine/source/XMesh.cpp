/*********************************************************************************

  *FileName: XMesh.cpp
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/15

**********************************************************************************/

#include"XMesh.h"

#include <iostream>

#include"XStructures.h"
#include"defines.h"

CXModel::CXModel()
{
    m_pRoot = nullptr;
    m_pAnimControl = nullptr;
    m_boneMatrices = nullptr;
    m_maxBones = 0;
    m_currentContainer = nullptr;
    m_center = D3DXVECTOR3(0, 0, 0);
    m_radius = 0;
    m_currentAni = 0;
    m_numAnimations = 0;
    m_currentTrack = 0;
    m_speed = 1.0f;
    m_currentTime = 0;
    m_transition = 0.25f;
    m_pDevice = nullptr;
    m_pAllocateHier = nullptr;
}

bool CXModel::LoadXFile(const std::string& file_name)
{
    if (!m_pDevice) return false;

    m_pAllocateHier = new CAllocateHierarchy();

    m_pAllocateHier->SetPath(file_name);

    // ファイルからXメッシュをロードします。
    if (FAILED(D3DXLoadMeshHierarchyFromX(file_name.c_str(), D3DXMESH_MANAGED, m_pDevice, m_pAllocateHier, NULL, &m_pRoot, &m_pAnimControl)))
        return false;

    // Xモデルのアニメーションセットの最大数を記録します。
    if (m_pAnimControl)
        m_numAnimations = m_pAnimControl->GetMaxNumAnimationSets();

    // ボーンセットアップ
    if (m_pRoot)
    {
        SetupBoneMatrixPointers(m_pRoot, m_pRoot);

        // Frame階層内のすべてのメッシュの境界球を計算します。
        D3DXFrameCalculateBoundingSphere(m_pRoot, &m_center, &m_radius);
        D3DXCreateSphere(m_pDevice,
            m_radius,
            10,
            10,
            &m_pSphere1,
            nullptr);
        // CreateBounding(m_pRoot, m_pRoot);
    }
    // アニメーション初期化
    SetAnimation(0);

    return true;
}

void CXModel::Shutdown()
{
    if (m_pRoot)
    {
        D3DXFrameDestroy(m_pRoot, m_pAllocateHier);
    }
    m_pRoot = nullptr;

    SAFE_RELEASE(m_pAnimControl)
        SAFE_DELETE(m_pAllocateHier)
}

HRESULT CXModel::SetupBoneMatrixPointers(LPD3DXFRAME pFrameBase, LPD3DXFRAME pFrameRoot)
{
    if (!m_pDevice) E_FAIL;

    if (pFrameBase->pMeshContainer != nullptr)
    {
        stD3DFrameDerived* pFrame = NULL;
        stD3DContainerDerived* pMeshContainer = (stD3DContainerDerived*)pFrameBase->pMeshContainer;

        //スキンメッシュがある場合は、ボーンマトリックスを設定します
        if (pMeshContainer->pSkinInfo != nullptr)
        {
            UINT numBones = pMeshContainer->pSkinInfo->GetNumBones();
            pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX * [numBones];
            for (UINT countBone = 0; countBone < numBones; countBone++)
            {
                pFrame = static_cast<stD3DFrameDerived*>(D3DXFrameFind(pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(countBone)));
                if (pFrame == nullptr) return E_FAIL;

                pMeshContainer->ppBoneMatrixPtrs[countBone] = &pFrame->finalMatrix;
            }
        }
    }

    if (pFrameBase->pFrameSibling != nullptr)
    {
        if (FAILED(SetupBoneMatrixPointers(pFrameBase->pFrameSibling, pFrameRoot)))
            return E_FAIL;
    }

    if (pFrameBase->pFrameFirstChild != nullptr)
    {
        if (FAILED(SetupBoneMatrixPointers(pFrameBase->pFrameFirstChild, pFrameRoot)))
            return E_FAIL;
    }

    return S_OK;
}

void CXModel::Update(float time, const LPD3DXMATRIX matWorld)
{
    m_pDevice->SetTransform(D3DTS_WORLD, matWorld);

    UpdateFrameMatrices(m_pRoot, matWorld);

    if (m_pAnimControl) m_pAnimControl->AdvanceTime(time, nullptr);
}

void CXModel::Render()
{
    if (m_pRoot)
        DrawFrame(m_pRoot);

    /*   m_pSphere1->DrawSubset(0);*/
}

void CXModel::SetAnimation(unsigned int index)
{
    if (index >= m_numAnimations || index == m_currentAni)
        return;

    m_currentAni = index;

    LPD3DXANIMATIONSET set;
    m_pAnimControl->GetAnimationSet(m_currentAni, &set);

    unsigned long nextTrack = (m_currentTrack == 0 ? 1 : 0);

    // 次のトラックを設定します。
    m_pAnimControl->SetTrackAnimationSet(nextTrack, set);
    set->Release();

    // すべてのトラックを取り出します。
    m_pAnimControl->UnkeyAllTrackEvents(m_currentTrack);
    m_pAnimControl->UnkeyAllTrackEvents(nextTrack);

    // 現在のトラック。
    m_pAnimControl->KeyTrackEnable(m_currentTrack, FALSE, m_currentTime + m_transition);
    m_pAnimControl->KeyTrackSpeed(m_currentTrack, 0.0f, m_currentTime, m_transition, D3DXTRANSITION_LINEAR);
    m_pAnimControl->KeyTrackWeight(m_currentTrack, 0.0f, m_currentTime, m_transition, D3DXTRANSITION_LINEAR);

    // 次のトラックをキーにします。
    m_pAnimControl->SetTrackEnable(nextTrack, TRUE);
    m_pAnimControl->KeyTrackSpeed(nextTrack, 1.0f, m_currentTime, m_transition, D3DXTRANSITION_LINEAR);
    m_pAnimControl->KeyTrackWeight(nextTrack, 1.0f, m_currentTime, m_transition, D3DXTRANSITION_LINEAR);

    m_currentTrack = nextTrack;
}

void CXModel::NextAnimation()
{
    unsigned int newAnimationSet = m_currentAni + 1;

    if (newAnimationSet >= m_numAnimations)
        newAnimationSet = 0;

    SetAnimation(newAnimationSet);
}

void CXModel::UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix)
{
    if (pFrameBase == nullptr || pParentMatrix == nullptr) return;
    stD3DFrameDerived* pFrame = (stD3DFrameDerived*)pFrameBase;

    D3DXMatrixMultiply(&pFrame->finalMatrix, &pFrame->TransformationMatrix, pParentMatrix);

    if (pFrame->pFrameSibling != nullptr)
        UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);

    if (pFrame->pFrameFirstChild != nullptr)
        UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->finalMatrix);
}

void CXModel::DrawFrame(LPD3DXFRAME pFrame)
{
    if (!m_pDevice || pFrame == nullptr)
        return;

    LPD3DXMESHCONTAINER pMeshContainer = pFrame->pMeshContainer;

    //　スキンメッシュを描画する
    while (pMeshContainer != nullptr)
    {
        DrawMeshContainer(pMeshContainer, pFrame);
        pMeshContainer = pMeshContainer->pNextMeshContainer;
    }

    DrawFrame(pFrame->pFrameSibling);
    DrawFrame(pFrame->pFrameFirstChild);
}

//--------------------------------------------------------------------------------------
// Name: DrawMeshContainer()
// Desc: スキンメッシュの描画 
//--------------------------------------------------------------------------------------
void CXModel::DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase)
{
    stD3DContainerDerived* pMeshContainer = (stD3DContainerDerived*)pMeshContainerBase;
    stD3DFrameDerived* pFrame = (stD3DFrameDerived*)pFrameBase;
    UINT iMaterial;
    UINT NumBlend;
    UINT iAttrib;
    DWORD AttribIdPrev;
    LPD3DXBONECOMBINATION pBoneComb;

    UINT CountMatrixIndex;
    D3DXMATRIXA16 matTemp;

    // スキンあるかどうかをチェック
    if (pMeshContainer->pSkinInfo != nullptr)
    {
        AttribIdPrev = UNUSED32;
        // ポインター安全変換
        pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

        // デバイスのデフォルトのvtx処理（通常はHW）を使用して描画します
        for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
        {
            NumBlend = 0;
            for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
            {
                if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
                {
                    NumBlend = i;
                }
            }

            if (m_caps.MaxVertexBlendMatrices >= NumBlend + 1)
            {
                // まず、ブレンドウェイトの現在のセットのワールドマトリックスを計算し、フレームの正確なカウントを取得します
                for (DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
                {
                    CountMatrixIndex = pBoneComb[iAttrib].BoneId[i];
                    if (CountMatrixIndex != UINT_MAX)
                    {
                        D3DXMatrixMultiply(&matTemp, &pMeshContainer->pBoneOffsetMatrices[CountMatrixIndex],
                            pMeshContainer->ppBoneMatrixPtrs[CountMatrixIndex]);
                        m_pDevice->SetTransform(D3DTS_WORLDMATRIX(i), &matTemp);
                    }
                }

                m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);

                // このfacesのsubsetに使用されているマテリアルを検索します
                if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
                {
                    m_pDevice->SetMaterial(&pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D);
                    m_pDevice->SetTexture(0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);
                    AttribIdPrev = pBoneComb[iAttrib].AttribId;
                }

                // 正しいマテリアルとマトリックスがロードされたので、subsetを描画します
                pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
            }
        }
        this->DrwaBounding(pMeshContainer->Name);
        m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);
    }
    else  // 普通のメッシュ、マテリアルを設定した後に描画するだけ
    {
        // m_pDevice->SetTransform(D3DTS_WORLD, &pFrame->finalMatrix);

        for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
        {
            m_pDevice->SetMaterial(&pMeshContainer->pMaterials[iMaterial].MatD3D);
            m_pDevice->SetTexture(0, pMeshContainer->ppTextures[iMaterial]);
            pMeshContainer->MeshData.pMesh->DrawSubset(iMaterial);

            this->DrwaBounding(pMeshContainer->Name);
        }
    }
}

bool CXModel::CreateBounding(LPD3DXFRAME pFrameBase, LPD3DXFRAME pFrameRoot)
{
    if (pFrameBase->pMeshContainer != nullptr)
    {
        if (pFrameBase->pMeshContainer->MeshData.pMesh != nullptr)
        {
            LPD3DXMESH sphere, box;
            uint8_t* v = nullptr;
            pFrameBase->pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, (void**)&v);

            /*     D3DXComputeBoundingSphere(
                     (D3DXVECTOR3*)v,
                     pFrameBase->pMeshContainer->MeshData.pMesh->GetNumVertices(),
                     pFrameBase->pMeshContainer->MeshData.pMesh->GetNumBytesPerVertex(),
                     &m_center,
                     &m_radius
                 );

                 D3DXCreateSphere(m_pDevice,
                     m_radius,
                     10,
                     10,
                     &sphere,
                     nullptr);

                 m_pSphere.insert(std::pair<std::string, LPD3DXMESH>(pFrameBase->Name, sphere));*/

            D3DXComputeBoundingBox(
                (D3DXVECTOR3*)v,
                pFrameBase->pMeshContainer->MeshData.pMesh->GetNumVertices(),
                pFrameBase->pMeshContainer->MeshData.pMesh->GetNumBytesPerVertex(),
                &m_min,
                &m_max);

            D3DXCreateBox(m_pDevice,
                m_max.x - m_min.x,
                m_max.y - m_min.y,
                m_max.z - m_min.z,
                &box,
                nullptr);
            m_pBox.insert(std::pair<std::string, LPD3DXMESH>(pFrameBase->pMeshContainer->Name, box));

            pFrameBase->pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
        }
    }

    if (pFrameBase->pFrameSibling != nullptr)
    {
        if (CreateBounding(pFrameBase->pFrameSibling, pFrameRoot))
            return false;
    }

    if (pFrameBase->pFrameFirstChild != nullptr)
    {
        if (CreateBounding(pFrameBase->pFrameFirstChild, pFrameRoot))
            return false;
    }

    return true;
}

void CXModel::DrwaBounding(const std::string& name)
{
    const auto it_sphere = m_pSphere.find(name);
    if (it_sphere != m_pSphere.end())
    {
        it_sphere->second->DrawSubset(0);
    }

    const auto it_box = m_pBox.find(name);
    if (it_box != m_pBox.end())
    {
        it_box->second->DrawSubset(0);
    }

}
