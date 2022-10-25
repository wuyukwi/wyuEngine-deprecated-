/*
   Demo Name:  Game Project 12
      Author:  Allen Sherrod
     Chapter:  Chapter 13
*/

#include "XStructures.h"
#include "defines.h"

HRESULT CAllocateHierarchy::AllocateName(LPCSTR Name, LPSTR* pNewName)
{
    UINT cbLength;

    if (Name != nullptr)
    {
        cbLength = (UINT)strlen(Name) + 1;
        *pNewName = new CHAR[cbLength];
        if (*pNewName == nullptr)
            return E_OUTOFMEMORY;
        memcpy(*pNewName, Name, cbLength * sizeof(CHAR));
    }
    else
    {
        *pNewName = nullptr;
    }

    return S_OK;
}

// ------------------------------------------------ --------------------------------------
// スキンメッシュをロードするとき、またはメソッドを変更するときにCreateMeshContainerによって呼び出されます。
// この関数は、メッシュコンテナのpSkinInfoを使用して、目的の描画可能なメッシュとボーンの組み合わせテーブルを生成します。
// ------------------------------------------------ --------------------------------------
HRESULT CAllocateHierarchy::GenerateSkinnedMesh(IDirect3DDevice9* pd3dDevice, stD3DContainerDerived* pMeshContainer)
{
    D3DCAPS9 d3dCaps;
    pd3dDevice->GetDeviceCaps(&d3dCaps);

    if (pMeshContainer->pSkinInfo == nullptr)
        return S_OK;

    SAFE_RELEASE(pMeshContainer->MeshData.pMesh)
        SAFE_RELEASE(pMeshContainer->pBoneCombinationBuf)

        // ボーンの最適化
        if (FAILED(pMeshContainer->pSkinInfo->ConvertToBlendedMesh(
            pMeshContainer->pOrigMesh,// 入力メッシュ
            D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE,
            pMeshContainer->pAdjacency,// メッシュの隣接データ(入力)
            NULL,// メッシュの隣接データ(出力)
            NULL,// ポリゴンの新規インデックスのバッファ
            NULL,// 頂点の新規インデックスのバッファ
            &pMeshContainer->NumInfl,// １つの頂点に影響を及ぼす重みの数
            &pMeshContainer->NumAttributeGroups,// ボーンの数
            &pMeshContainer->pBoneCombinationBuf,// ボーンデータが格納されたバッファ
            &pMeshContainer->MeshData.pMesh)))// 変換後のメッシュ
            return E_FAIL;
    return S_OK;
}

//--------------------------------------------------------------------------------------
// Name: CD3DAllocate::CreateFrame()
// Desc: Frameを作成して，初期化する
//----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME* ppNewFrame)
{
    HRESULT hr = S_OK;

    *ppNewFrame = nullptr;

    auto pFrame = new stD3DFrameDerived;
    if (pFrame == nullptr)
    {
        hr = E_OUTOFMEMORY;
        delete pFrame;
        return hr;
    }

    hr = AllocateName(Name, &pFrame->Name);
    if (FAILED(hr))
    {
        delete pFrame;
        return hr;
    }

    // frameのマトリックスを初期化します
    D3DXMatrixIdentity(&pFrame->TransformationMatrix);
    D3DXMatrixIdentity(&pFrame->finalMatrix);

    pFrame->pMeshContainer = nullptr;
    pFrame->pFrameSibling = nullptr;
    pFrame->pFrameFirstChild = nullptr;

    *ppNewFrame = pFrame;
    pFrame = nullptr;

    return S_OK;
}

//--------------------------------------------------------------------------------------
// メッシュ読み込んで、必要なデータを保存する
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateMeshContainer(
    LPCSTR Name,
    CONST D3DXMESHDATA* pMeshData,
    CONST D3DXMATERIAL* pMaterials,
    CONST D3DXEFFECTINSTANCE* pEffectInstances,
    DWORD NumMaterials,
    CONST DWORD* pAdjacency,
    LPD3DXSKININFO pSkinInfo,
    LPD3DXMESHCONTAINER* ppNewMeshContainer)
{
    HRESULT hr;
    stD3DContainerDerived* pMeshContainer = nullptr;
    UINT NumFaces;
    UINT iMaterial; //マテリアルカウント
    UINT countBone, numBones;
    LPDIRECT3DDEVICE9 pd3dDevice = nullptr;

    LPD3DXMESH pMesh = nullptr;

    if (pMeshData->Type != D3DXMESHTYPE_MESH)
    {
        //D3DXMESHTYPE_PMESHはプログレッシブメッシュです。
        //D3DXMESHTYPE_PATCHMESHはパッチメッシュです。
        //このコードでは、通常のメッシュのみを処理します。
        hr = E_FAIL;
        goto e_Exit;
    }

    // メッシュデータ構造からpMeshインターフェイスポインタを取得します
    pMesh = pMeshData->pMesh;

    // FVF見つからないから終わります
    if (pMesh->GetFVF() == 0)
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    //オーバーロードされた構造を割り当ててD3DXMESHCONTAINERとして返します
    pMeshContainer = new stD3DContainerDerived;
    if (pMeshContainer == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }
    memset(pMeshContainer, 0, sizeof(stD3DContainerDerived));

    // ネームをコピーします。
    hr = AllocateName(Name, &pMeshContainer->Name);
    if (FAILED(hr))
        goto e_Exit;

    // D3D デバイス取得.
    pMesh->GetDevice(&pd3dDevice);

    // 面数を保存
    NumFaces = pMesh->GetNumFaces();

    // 頂点法線ない場合は追加します。
    if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
    {
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

        // メッシュをクローンしてFVFに法線を追加します
        hr = pMesh->CloneMeshFVF(pMesh->GetOptions(),
            pMesh->GetFVF() | D3DFVF_NORMAL,
            pd3dDevice, &pMeshContainer->MeshData.pMesh);
        if (FAILED(hr))
            goto e_Exit;

        // 使用するメッシュコンテナから新しいpMeshポインタを取得します
        // 注：pMeshへの参照がまだないため、pMeshをリリースしません
        pMesh = pMeshContainer->MeshData.pMesh;

        // 法線を作成します
        D3DXComputeNormals(pMesh, nullptr);
    }
    else
    {
        pMeshContainer->MeshData.pMesh = pMesh;
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

        pMesh->AddRef();
    }

    // マテリアル情報保存用のメモリを割り当てます。
    pMeshContainer->NumMaterials = max(1, NumMaterials);
    pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
    pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
    pMeshContainer->pAdjacency = new DWORD[NumFaces * 3];
    if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL))
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // 隣接情報保存します
    memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces * 3);

    // メモリクリア
    memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);

    // マテリアル情報が提供されている場合は、それらをコピーします
    if (NumMaterials > 0)
    {
        memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials);

        for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++)
        {
            if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != nullptr)
            {
                if (FAILED(D3DXCreateTextureFromFileA(pd3dDevice,
                    (m_path + pMeshContainer->pMaterials[iMaterial].pTextureFilename).c_str(),
                    &pMeshContainer->ppTextures[iMaterial])))
                    pMeshContainer->ppTextures[iMaterial] = nullptr;//テクスチャない場合はnullptr

                // ロード後テクスチャネームをnullptrにします。
                pMeshContainer->pMaterials[iMaterial].pTextureFilename = nullptr;
            }
        }
    }
    else //提供されていない場合は、デフォルトを使用します
    {
        pMeshContainer->pMaterials[0].pTextureFilename = nullptr;
        memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
    }

    // スキンメッシュ情報がある場合は、必要なデータを保存してから、HWスキン用にセットアップします
    if (pSkinInfo != nullptr)
    {
        // 最初にSkinInfoとオリジナルメッシュデータを保存します
        pMeshContainer->pSkinInfo = pSkinInfo;
        pSkinInfo->AddRef();

        pMeshContainer->pOrigMesh = pMesh;
        pMesh->AddRef();

        // 頂点を図形空間からBonesの空間に移動するには、offset行列の配列が必要になります
        numBones = pSkinInfo->GetNumBones();
        pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[numBones];
        if (pMeshContainer->pBoneOffsetMatrices == nullptr)
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }

        // 各ボーンoffset行列を取得します
        for (countBone = 0; countBone < numBones; countBone++)
        {
            pMeshContainer->pBoneOffsetMatrices[countBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(countBone));
        }

        // GenerateSkinnedMesh関数は、一般的なスキン情報を取得し、HW対応バージョンに変換します
        hr = GenerateSkinnedMesh(pd3dDevice, pMeshContainer);
        if (FAILED(hr))
            goto e_Exit;
    }

    *ppNewMeshContainer = pMeshContainer;
    pMeshContainer = nullptr;

e_Exit:
    SAFE_RELEASE(pd3dDevice);

    // メモリクリア
    if (pMeshContainer != nullptr)
    {
        DestroyMeshContainer(pMeshContainer);
    }

    return hr;
}

HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pInFrame)
{
    SAFE_DELETE_ARRAY(pInFrame->Name)
        SAFE_DELETE(pInFrame)
        return S_OK;
}

HRESULT CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
    UINT countMaterial;

    stD3DContainerDerived* pMeshContainer = (stD3DContainerDerived*)pMeshContainerBase;

    SAFE_DELETE_ARRAY(pMeshContainer->Name)
        SAFE_DELETE_ARRAY(pMeshContainer->pAdjacency)
        SAFE_DELETE_ARRAY(pMeshContainer->pMaterials)
        SAFE_DELETE_ARRAY(pMeshContainer->pBoneOffsetMatrices)

        // 割り当てられたすべてのテクスチャを解放します
        if (pMeshContainer->ppTextures != nullptr)
        {
            for (countMaterial = 0; countMaterial < pMeshContainer->NumMaterials; countMaterial++)
            {
                SAFE_RELEASE(pMeshContainer->ppTextures[countMaterial])
            }
        }
    SAFE_DELETE_ARRAY(pMeshContainer->ppTextures)
        SAFE_DELETE_ARRAY(pMeshContainer->ppBoneMatrixPtrs)
        SAFE_RELEASE(pMeshContainer->pBoneCombinationBuf)
        SAFE_RELEASE(pMeshContainer->MeshData.pMesh)
        SAFE_RELEASE(pMeshContainer->pSkinInfo)
        SAFE_RELEASE(pMeshContainer->pOrigMesh)
        SAFE_DELETE(pMeshContainer)
        return S_OK;
}

void CAllocateHierarchy::SetPath(const std::string& path)
{
    size_t found = path.find_last_of("/");

    m_path = path.substr(0, found + 1);
}
