/*********************************************************************************

  *FileName: XStructures.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/15

**********************************************************************************/

#ifndef _X_ALLOCATE_
#define _X_ALLOCATE_

#include<string>

#include<d3d9.h>
#include<d3dx9.h>

struct stD3DFrameDerived : D3DXFRAME
{
    D3DXMATRIX finalMatrix;  //行列変換後のマトリックス
};

//--------------------------------------------------------------------------------------
// D3DXMESHCONTAINERから派生した構造、各メッシュに保存された固有の情報を追加できます
//--------------------------------------------------------------------------------------
struct stD3DContainerDerived : D3DXMESHCONTAINER
{
    //LPDIRECT3DTEXTURE9 *textures;
    //D3DMATERIAL9 *mat;
    //LPD3DXMESH originalMesh;
    //D3DXMATRIX **boneMatrices;

    //テクスチャ
    LPDIRECT3DTEXTURE9* ppTextures;

    //メッシュ
    LPD3DXMESH pOrigMesh;					// オリジナルメッシュ
    LPD3DXATTRIBUTERANGE pAttributeTable;	// 属性テーブルエントリ
    DWORD NumAttributeGroups;				// ボーンの数
    DWORD NumInfl;							// １つの頂点に影響を及ぼす重みの数
    LPD3DXBUFFER pBoneCombinationBuf;		// ボーンデータが格納されたバッファ
    D3DXMATRIX** ppBoneMatrixPtrs;			// ボーンマトリクス
    D3DXMATRIX* pBoneOffsetMatrices;		// ボーン初期マトリクス
    DWORD NumPaletteEntries;				// ボーン最大値
    bool UseSoftwareVP;						// ソフトウェア使うかどうか
    DWORD iAttributeSW;						// インデックス付けされていないスキニングに必要な場合は、SWとHWの間の分割を示すために使用されます

};

//--------------------------------------------------------------------------------------
// xファイルからメッシュデータ読み込み用
//--------------------------------------------------------------------------------------
class CAllocateHierarchy : public ID3DXAllocateHierarchy
{
private:
    HRESULT AllocateName(LPCSTR Name, LPSTR* pNewName);
    HRESULT GenerateSkinnedMesh(IDirect3DDevice9* pd3dDevice, stD3DContainerDerived* pMeshContainer);

    //ファイルパス保存用
    std::string m_path;

public:
    STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME* ppNewFrame);
    STDMETHOD(CreateMeshContainer)(THIS_
        LPCSTR Name,
        CONST D3DXMESHDATA* pMeshData,
        CONST D3DXMATERIAL* pMaterials,
        CONST D3DXEFFECTINSTANCE* pEffectInstances,
        DWORD NumMaterials,
        CONST DWORD* pAdjacency,
        LPD3DXSKININFO pSkinInfo,
        LPD3DXMESHCONTAINER* ppNewMeshContainer);
    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pInFrame);
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);

    CAllocateHierarchy() = default;

    void SetPath(const std::string& path);
};

#endif