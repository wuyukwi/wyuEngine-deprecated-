/*********************************************************************************

  *FileName: D3D9Renderer.cpp
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/02

**********************************************************************************/

#include "D3D9Renderer.h"

#include <deque>

#include "gameEngine.h"



inline DWORD FtoDW(float f)
{
    return *reinterpret_cast<DWORD*>(&f);
}

bool CreateD3DRenderer(RenderInterface** pObj)
{
    if (!*pObj)
        *pObj = new D3D9Renderer;
    else
        return false;

    return true;
}

#define D3DFVF_RHW (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define D3DFVF_2D (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define D3DFVF_3D (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

size_t CreateD3DFVF(int32_t flags)
{
    size_t fvf = 0;

    switch (flags)
    {
    case FVF_RHW:
        fvf = D3DFVF_RHW;
        break;
    case FVF_2D:
        fvf = D3DFVF_2D;
        break;
    case FVF_3D:
        fvf = D3DFVF_3D;
        break;
    default:
        fvf = NULL;
        break;
    }

    return fvf;
}

D3D9Renderer::D3D9Renderer()
{
    m_Color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);

    m_Direct3D = nullptr;
    m_Device = nullptr;
    ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
    ZeroMemory(&m_d3ddm, sizeof(m_d3ddm));
    ZeroMemory(&m_caps, sizeof(m_caps));

    m_renderingScene = false;

    m_activeStaticBuffer = UGP_INVALID;
}

D3D9Renderer::~D3D9Renderer()
= default;


bool D3D9Renderer::Initialize(WinHWND mainWin)
{

    m_screenWidth = g_pEngine->GetConfig()->screenWidth;
    m_screenHeight = g_pEngine->GetConfig()->screenHeight;

    m_mainHandle = mainWin;
    if (!m_mainHandle)
        return false;

    m_fullscreen = g_pEngine->GetConfig()->fullscreen;

    m_Direct3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!m_Direct3D)
        return false;

    // 現在のディスプレイモードを取得
    if (FAILED(m_Direct3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_d3ddm)))
        return false;

    if (FAILED(m_Direct3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_caps)))
        return false;

    DWORD processing = 0;
    if (m_caps.VertexProcessingCaps != 0)
        processing = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
    else
        processing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    if (m_fullscreen)
    {
        m_d3dpp.FullScreen_RefreshRateInHz = m_d3ddm.RefreshRate;
        m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    }
    else
        m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    //m_d3dpp.FullScreen_RefreshRateInHz = m_d3ddm.RefreshRate;
    //m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    m_d3dpp.Windowed = !m_fullscreen;
    m_d3dpp.BackBufferWidth = m_screenWidth;
    m_d3dpp.BackBufferHeight = m_screenHeight;
    m_d3dpp.hDeviceWindow = m_mainHandle;
    m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_d3dpp.BackBufferFormat = m_d3ddm.Format;
    m_d3dpp.BackBufferCount = 1;
    m_d3dpp.EnableAutoDepthStencil = TRUE;
    m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    SetMultiSample(static_cast<int32_t>(g_pEngine->GetConfig()->msaaSamples));


    if (FAILED(
        m_Direct3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_mainHandle, processing, &m_d3dpp, &m_Device)))
        return false;

    if (m_Device == NULL)
        return false;

    OneTimeInit();

    m_texturePath = g_pEngine->GetPathManager()->GetFileFullPath("texture");

#if IMGUI_ENABLE
    SetupImGui(mainWin);
#endif

    return true;
}

void D3D9Renderer::OneTimeInit()
{
    if (!m_Device)
    {
        return;
    }

    // テクスチャフィルターの設定
    m_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
    m_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
    //m_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    //m_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);


    // レンダーステート設定
    m_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    // SetTransparent(RenderState::TRANSPARENCY_ENABLE, TransperentState::SRCALPHA, TransperentState::INVDSTALPHA);
    m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
    m_Device->SetRenderState(D3DRS_SPECULARENABLE, true);

    EnableMultiSample(true);

    // テクスチャステージステート
    m_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
    m_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
    m_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
    m_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
    m_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    CalculateProjMatrix(D3DX_PI / 4, 10.0f, 10000.0f);
}

void D3D9Renderer::Shutdown()
{
    size_t s = 0;

    //　スタティックバファのリリース
    for (s = 0; s < m_staticBufferList.size(); s++)
    {
        SAFE_RELEASE(m_staticBufferList[s].vbPtr);
        SAFE_RELEASE(m_staticBufferList[s].ibPtr);
    }

    // テクスチャリストのリリース
    for (s = 0; s < m_textureList.size(); s++)
    {
        SAFE_DELETE_ARRAY(m_textureList[s].fileName);
        SAFE_RELEASE(m_textureList[s].image);
    }
    // テクスチャリストのリリース
    for (s = 0; s < m_textureList2D.size(); s++)
    {
        SAFE_RELEASE(m_textureList2D[s].image);
    }


    // フォントのリリース
    for (s = 0; s < m_fonts.size(); s++)
    {
        SAFE_RELEASE(m_fonts[s]);
    }

#if IMGUI_ENABLE
    // imgui終了処理
    ImGui::SaveIniSettingsToDisk((g_pEngine->GetPathManager()->GetAssetFolder() + "imgui.ini").c_str());
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif
}

void D3D9Renderer::SetClearCol(float r, float g, float b, float a)
{
    m_Color = D3DXCOLOR(r, g, b, a);
}

void D3D9Renderer::StartRender(bool bColor, bool bDepth, bool bStencil)
{
    if (!m_Device)
        return;

    unsigned int buffers = 0;
    if (bColor) buffers |= D3DCLEAR_TARGET;
    if (bDepth) buffers |= D3DCLEAR_ZBUFFER;
    if (bStencil) buffers |= D3DCLEAR_STENCIL;

    if (FAILED(m_Device->Clear(0, NULL, buffers, m_Color, 1.0f, 0)))
        return;

    if (FAILED(m_Device->BeginScene()))
        return;

    m_renderingScene = true;
}

void D3D9Renderer::ClearBuffers(bool bColor, bool bDepth, bool bStencil)
{
    if (!m_Device)
        return;

    unsigned int buffers = 0;
    if (bColor) buffers |= D3DCLEAR_TARGET;
    if (bDepth) buffers |= D3DCLEAR_ZBUFFER;
    if (bStencil) buffers |= D3DCLEAR_STENCIL;

    if (m_renderingScene) m_Device->EndScene();
    if (FAILED(m_Device->Clear(0, NULL, buffers, m_Color, 1, 0)))
        return;

    if (m_renderingScene)
        if (FAILED(m_Device->BeginScene()))
            return;
}

void D3D9Renderer::EndRendering()
{
    if (!m_Device)
        return;

    m_Device->EndScene();
    m_Device->Present(nullptr, nullptr, nullptr, nullptr);

    m_renderingScene = false;
}

void D3D9Renderer::SetMaterial(stMaterial* mat)
{
    if (!mat || !m_Device)
        return;

    const D3DMATERIAL9 m = {
        {
            mat->diffuseR, mat->diffuseG,mat->diffuseB, mat->diffuseA
        },
        {
            mat->ambientR, mat->ambientG,mat->ambientB, mat->ambientA
        },
        {
            mat->specularR, mat->specularG,mat->specularB, mat->specularA
        },
        {
            mat->emissiveR, mat->emissiveG,mat->emissiveB, mat->emissiveA
        },
        mat->power
    };

    m_Device->SetMaterial(&m);
}

void D3D9Renderer::SetLight(stLight& light, uint32_t index)
{
    if (!m_Device || index < 0) return;

    D3DLIGHT9 l;

    l.Ambient.a = light.Ambient.a;
    l.Ambient.r = light.Ambient.r;
    l.Ambient.g = light.Ambient.g;
    l.Ambient.b = light.Ambient.b;
    l.Attenuation0 = light.Attenuation0;
    l.Attenuation1 = light.Attenuation1;
    l.Attenuation2 = light.Attenuation2;
    l.Diffuse.a = light.Diffuse.a;
    l.Diffuse.r = light.Diffuse.r;
    l.Diffuse.g = light.Diffuse.g;
    l.Diffuse.b = light.Diffuse.b;
    l.Direction.x = light.Direction.x;
    l.Direction.y = light.Direction.y;
    l.Direction.z = light.Direction.z;
    l.Falloff = light.Falloff;
    l.Phi = light.Phi;
    l.Position.x = light.Position.x;
    l.Position.y = light.Position.y;
    l.Position.z = light.Position.z;
    l.Range = light.Range;
    l.Specular.a = light.Specular.a;
    l.Specular.r = light.Specular.r;
    l.Specular.g = light.Specular.g;
    l.Specular.b = light.Specular.b;
    l.Theta = light.Theta;

    if (light.Type == LightType::POINT)
    {
        l.Type = D3DLIGHT_POINT;
    }
    else if (light.Type == LightType::SPOT)
    {
        l.Type = D3DLIGHT_SPOT;
    }
    else if (light.Type == LightType::DIRECTIONAL)
    {
        l.Type = D3DLIGHT_DIRECTIONAL;
    }

    m_Device->SetLight(index, &l);
    m_Device->LightEnable(index, TRUE);
}

void D3D9Renderer::DisableLight(size_t index)
{
    if (!m_Device)
        return;

    m_Device->LightEnable(index, FALSE);
}

bool D3D9Renderer::SetMultiSample(int32_t samples)
{
    D3DMULTISAMPLE_TYPE type = D3DMULTISAMPLE_TYPE(samples);

    if (type != D3DMULTISAMPLE_TYPE(0))
    {
        if (m_Direct3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_d3ddm.Format, true, type, NULL)
            == D3D_OK)
        {
            m_d3dpp.MultiSampleType = type;
        }
        else
        {
#ifdef _DEBUG
            std::cout << "SetMultiSample " << samples << " ERROR" << std::endl;
#endif
            return false;
        }
    }

    return true;
}

void D3D9Renderer::EnableMultiSample(bool samples)
{
    if (samples)
    {
        m_Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
    }
    else
    {
        m_Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, false);
    }
}

void D3D9Renderer::SetD3D9RenderState(RenderState state)
{
    if (!m_Device)
        return;

    switch (state)
    {
    case RenderState::CULL_NONE:
        m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        break;
    case RenderState::CULL_CW:
        m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
        break;
    case RenderState::CULL_CCW:
        m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
        break;
    case RenderState::SHADE_POINTS:
        m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
        break;
    case RenderState::SHADE_SOLIDTRI:
        m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        break;
    case RenderState::SHADE_WIRETRI:
        m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        break;
    case RenderState::SHADE_WIREPOLY:
        break;
    case RenderState::TRANSPARENCY_NONE:
        break;
    case RenderState::TRANSPARENCY_ENABLE:
        break;
    default:
        break;
    }
}

void D3D9Renderer::SetDepthTesting(RenderState state)
{
    if (!m_Device)
        return;

    switch (state)
    {
    case RenderState::DEPTH_NONE:
        m_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        m_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
        break;
    case RenderState::DEPTH_READONLY:
        m_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        m_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
        break;
    case RenderState::DEPTH_READWRITE:
        m_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
        m_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
        break;
    default:
        break;
    }
}

/*
D3DBLEND_ZERO——blendFactor = (0, 0, 0, 0)
D3DBLEND_ONE——blendFactor = (1, 1, 1, 1)
D3DBLEND_SRCCOLOR——blendFactor = (rs, gs, bs, as)
D3DBLEND_INVSRCCOLOR——blendFactor = (1 - rs, 1 - gs, 1 - bs, 1 - as)
D3DBLEND_SRCALPHA——blendFactor = (as, as, as, as)
D3DBLEND_INVSRCALPHA——blendFactor = (1 - as, 1 - as, 1 - as, 1 - as)
D3DBLEND_DESTALPHA——blendFactor = (ad, ad, ad, ad)
D3DBLEND_INVDESTALPHA——blendFactor = (1 - ad, 1 - ad, 1 - ad, 1 - ad)
D3DBLEND_DESTCOLOR——blendFactor = (rd, gd, bd, ad)
D3DBLEND_INVDESTCOLOR——blendFactor = (1 - rd, 1 - gd, 1 - bd, 1 - ad)
D3DBLEND_SRCALPHASAT——blendFactor = (f, f, f, 1), f = min(as, 1 – ad)
D3DBLEND_BOTHINVSRCALPHA——（1 - as, 1 - as, 1 - as, 1 - as, ）（as, as, as, as）。D3DRS_SRCBLEND。
*/
void D3D9Renderer::SetTransparent(RenderState state, TransperentState src, TransperentState dst)
{
    if (!m_Device) return;

    if (state == RenderState::TRANSPARENCY_NONE)
    {
        m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
        return;
    }

    if (state == RenderState::TRANSPARENCY_ENABLE)
    {
        m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

        switch (src)
        {
        case TransperentState::ZERO:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
            break;

        case TransperentState::ONE:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
            break;

        case TransperentState::SRCCOLOR:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
            break;

        case TransperentState::INVSRCCOLOR:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVSRCCOLOR);
            break;

        case TransperentState::SRCALPHA:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            break;

        case TransperentState::INVSRCALPHA:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA);
            break;

        case TransperentState::DSTALPHA:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTALPHA);
            break;

        case TransperentState::INVDSTALPHA:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTALPHA);
            break;

        case TransperentState::DSTCOLOR:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
            break;

        case TransperentState::INVDSTCOLOR:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR);
            break;

        case TransperentState::SRCALPHASAT:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHASAT);
            break;

        case TransperentState::BOTHSRCALPHA:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BOTHSRCALPHA);
            break;

        case TransperentState::INVBOTHSRCALPHA:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BOTHINVSRCALPHA);
            break;

        case TransperentState::BLENDFACTOR:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
            break;

        case TransperentState::INVBLENDFACTOR:
            m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVBLENDFACTOR);
            break;

        default:
            m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
            return;
        }

        switch (dst)
        {
        case TransperentState::ZERO:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
            break;

        case TransperentState::ONE:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
            break;

        case TransperentState::SRCCOLOR:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
            break;

        case TransperentState::INVSRCCOLOR:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
            break;

        case TransperentState::SRCALPHA:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
            break;

        case TransperentState::INVSRCALPHA:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
            break;

        case TransperentState::DSTALPHA:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
            break;

        case TransperentState::INVDSTALPHA:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTALPHA);
            break;

        case TransperentState::DSTCOLOR:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR);
            break;

        case TransperentState::INVDSTCOLOR:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR);
            break;

        case TransperentState::SRCALPHASAT:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHASAT);
            break;

        case TransperentState::BOTHSRCALPHA:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_BOTHSRCALPHA);
            break;

        case TransperentState::INVBOTHSRCALPHA:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_BOTHINVSRCALPHA);
            break;

        case TransperentState::BLENDFACTOR:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_BLENDFACTOR);
            break;

        case TransperentState::INVBLENDFACTOR:
            m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
            break;

        default:
            m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
            break;
        }
    }
}


int D3D9Renderer::AddTexture2D(const std::string& file_name, int& texId)
{
    if (file_name.empty() || !m_Device)
        return UGP_FAIL;

    // テクスチャ既に読み込んでるのかをチェック
    for (size_t count = 0; count < m_textureList2D.size(); count++)
    {
        if (m_textureList2D[count].fileName == file_name)
        {
            texId = static_cast<int>(count);
            m_textureList2D[count].ret++;
            return UGP_OK;
        }
    }

    stD3D9Texture2D texture;

    // 透明色
    D3DCOLOR colorkey = 0xff000000;

    if (D3DXCreateTextureFromFileEx(m_Device,
        g_pEngine->GetPathManager()->GetTextureFullPath(file_name).c_str(),
        0, 0, 0, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
        D3DX_DEFAULT, colorkey, &texture.info, nullptr,
        &texture.image) != D3D_OK)
    {
        //assert(("texture load error", 0));
        return false;
    }

    // 自動生成されたミップマップサブレベルに使用されるフィルタータイプを設定します
    texture.image->SetAutoGenFilterType(D3DTEXF_LINEAR);
    texture.image->GenerateMipSubLevels();

    texture.fileName = file_name;
    texture.ret++;

    // テクスチャリリースされたのかをチェック
    for (size_t count = 0; count < m_textureList2D.size(); count++)
    {
        if (m_textureList2D[count].fileName == "null")
        {
            texId = static_cast<int>(count);
            m_textureList2D[count] = texture;
            return UGP_OK;
        }
    }

    texId = static_cast<int>(m_textureList2D.size());
    this->m_textureList2D.push_back(texture);
    return UGP_OK;
}

bool D3D9Renderer::AddTexture2D(const std::string& file_name, int& texId, Vector2f& size)
{
    if (file_name.empty() || !m_Device)
        return UGP_FAIL;

    // テクスチャ既に読み込んでるのかをチェック
    for (size_t count = 0; count < m_textureList2D.size(); count++)
    {
        if (m_textureList2D[count].fileName == file_name)
        {
            texId = static_cast<int>(count);
            m_textureList2D[count].ret++;
            size = Vector2f{ static_cast<float>(m_textureList2D[count].info.Width),static_cast<float>(m_textureList2D[count].info.Height) };
            return UGP_OK;
        }
    }

    stD3D9Texture2D texture;

    // 透明色
    D3DCOLOR colorkey = 0xff000000;

    if (D3DXCreateTextureFromFileEx(m_Device,
        g_pEngine->GetPathManager()->GetTextureFullPath(file_name).c_str(),
        0, 0, 0, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
        D3DX_DEFAULT, colorkey, &texture.info, nullptr,
        &texture.image) != D3D_OK)
    {
        //assert(("texture load error", 0));
        std::cout << g_pEngine->GetPathManager()->GetTextureFullPath(file_name).c_str() << std::endl;
        return false;
    }

    // 自動生成されたミップマップサブレベルに使用されるフィルタータイプを設定します
    texture.image->SetAutoGenFilterType(D3DTEXF_LINEAR);
    texture.image->GenerateMipSubLevels();

    size.x = static_cast<float>(texture.info.Width);
    size.y = static_cast<float>(texture.info.Height);

    texture.fileName = file_name;
    texture.ret++;

    // テクスチャリリースされたのかをチェック
    for (size_t count = 0; count < m_textureList2D.size(); count++)
    {
        if (m_textureList2D[count].fileName.empty())
        {
            texId = static_cast<int>(count);
            m_textureList2D[count] = texture;
            return UGP_OK;
        }
    }

    texId = static_cast<int>(m_textureList2D.size());
    this->m_textureList2D.push_back(texture);
    return UGP_OK;
}

//
void D3D9Renderer::SetTextureFilter(size_t index, TextureState filter, FilterType type)
{
    if (!m_Device)
        return;

    D3DSAMPLERSTATETYPE fil = D3DSAMP_MINFILTER;
    int t = D3DTEXF_POINT;

    switch (filter)
    {
    case MIN_FILTER:
        fil = D3DSAMP_MINFILTER;
        break;
    case MAG_FILTER:
        fil = D3DSAMP_MAGFILTER;
        break;
    case MIP_FILTER:
        fil = D3DSAMP_MIPFILTER;
        break;
    default:
        break;
    }

    switch (type)
    {
    case POINT_TYPE:
        t = D3DTEXF_POINT;
        break;
    case LINEAR_TYPE:
        t = D3DTEXF_LINEAR;
        break;
    case ANISOTROPIC_TYPE:
        t = D3DTEXF_ANISOTROPIC;
        break;
    default:
        break;
    }

    m_Device->SetSamplerState(index, fil, t);
}

void D3D9Renderer::SetTextureAlphaBlend(bool enable)
{
    if (!enable)
    {
        m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        return;
    }
    // テクスチャ画像のアルファ透明度をオンに設定します
    m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    //                                                      //ALPHAだけ

}

void D3D9Renderer::SetMultiTexture()
{
    if (!m_Device)
        return;

    m_Device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    m_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    m_Device->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
    m_Device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_Device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_Device->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
}

void D3D9Renderer::ApplyTexture(size_t index, int texId)
{
    if (!m_Device)
        return;

    if (texId < 0)
    {
        m_Device->SetTexture(0, nullptr);
        return;
    }

    if (index == 0)
    {
        m_Device->SetTexture(0, m_textureList[texId].image);
    }
    if (index == 1)
    {
        m_Device->SetTexture(0, m_textureList2D[texId].image);
    }
}

void D3D9Renderer::SaveScreenShot(const char* file)
{
    if (!file)
        return;

    LPDIRECT3DSURFACE9 surface = NULL;
    D3DDISPLAYMODE disp;

    m_Device->GetDisplayMode(0, &disp);
    m_Device->CreateOffscreenPlainSurface(disp.Width, disp.Height, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &surface, NULL);

    m_Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &surface);
    D3DXSaveSurfaceToFile(file, D3DXIFF_PNG, surface, NULL, NULL);

    SAFE_RELEASE(surface)
}

void D3D9Renderer::EnablePointSprites(float size, float min, float a, float b, float c)
{
    if (!m_Device) return;

    m_Device->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
    m_Device->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
    m_Device->SetRenderState(D3DRS_POINTSIZE, FtoDW(size));
    m_Device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(min));
    m_Device->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(a));
    m_Device->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(b));
    m_Device->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(c));
}

void D3D9Renderer::DisablePointSprites()
{
    m_Device->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
    m_Device->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
}

bool D3D9Renderer::CreateText(const char* font, int weight, bool italic, int size, int& id)
{
    LPD3DXFONT temp;

    if (FAILED(D3DXCreateFont(m_Device, size, 0, weight, 1, italic,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, font,
        &temp)))
        return false;

    id = m_fonts.size();
    m_fonts.push_back(temp);

    return true;
}

bool D3D9Renderer::DisplayText(int id, long x, long y, int r, int g, int b, const char* text, ...)
{
    RECT FontPosition = { x, y, m_screenWidth, m_screenWidth };
    char message[1024];
    va_list argList;

    if (id >= static_cast<int>(m_fonts.size()))
        return false;
    // 全てのテキスト取得し、messageに保存する
    va_start(argList, text);
    vsprintf(message, text, argList);
    va_end(argList);

    m_fonts[id]->DrawText(NULL, message, -1, &FontPosition, DT_SINGLELINE, D3DCOLOR_ARGB(255, r, g, b));

    return true;
}

bool D3D9Renderer::DisplayText(int id, long x, long y, unsigned long color, const char* text, ...)
{
    RECT FontPosition = { x, y, m_screenWidth, m_screenWidth };
    char message[1024];
    va_list argList;

    if (id >= static_cast<int>(m_fonts.size()))
        return false;

    // 全てのテキスト取得し、messageに保存する
    va_start(argList, text);
    vsprintf(message, text, argList);
    va_end(argList);

    m_fonts[id]->DrawText(NULL, message, -1, &FontPosition, DT_SINGLELINE, color);

    return true;
}


void D3D9Renderer::CalculateProjMatrix(float fov, float zn, float zf)
{
    if (!m_Device)
        return;

    D3DXMATRIX projection;

    D3DXMatrixPerspectiveFovLH(&projection, fov,
        static_cast<float>(m_screenWidth) / static_cast<float>(m_screenHeight), zn, zf);

    m_Device->SetTransform(D3DTS_PROJECTION, &projection);
}

void D3D9Renderer::CalculateOrthoMatrix(float zn, float zf)
{
    if (!m_Device)
        return;

    D3DXMATRIX ortho;

    D3DXMatrixOrthoLH(&ortho, (float)m_screenWidth, (float)m_screenHeight, zn, zf);

    m_Device->SetTransform(D3DTS_PROJECTION, &ortho);
}

int D3D9Renderer::CreateStaticBuffer(VertexType vType,
    PrimType primType, int32_t totalVerts,
    int32_t totalIndices, int32_t stride, void** data,
    int16_t* indices, int32_t* staticId)
{
    void* ptr;
    stD3D9StaticBuffer staticBuffer;

    staticBuffer.numVerts = totalVerts;
    staticBuffer.numIndices = totalIndices;
    staticBuffer.primType = primType;
    staticBuffer.stride = stride;
    staticBuffer.fvf = CreateD3DFVF(vType);

    if (totalIndices > 0)
    {
        if (FAILED(m_Device->CreateIndexBuffer(
            sizeof(int16_t) * totalIndices,
            D3DUSAGE_WRITEONLY,
            D3DFMT_INDEX16,
            D3DPOOL_DEFAULT,
            &staticBuffer.ibPtr, NULL)))
        {
            return UGP_FAIL;
        }

        if (FAILED(staticBuffer.ibPtr->Lock(0, 0, &ptr, 0)))
            return UGP_FAIL;

        memcpy(ptr, indices, sizeof(int16_t) * totalIndices);
        staticBuffer.ibPtr->Unlock();
    }
    else
    {
        staticBuffer.ibPtr = nullptr;
    }

    if (FAILED(m_Device->CreateVertexBuffer(
        totalVerts * stride,
        D3DUSAGE_WRITEONLY,
        staticBuffer.fvf,
        D3DPOOL_DEFAULT,
        &staticBuffer.vbPtr, NULL)))
        return UGP_FAIL;

    if (FAILED(staticBuffer.vbPtr->Lock(0, 0, &ptr, 0)))
        return UGP_FAIL;

    memcpy(ptr, data, totalVerts * stride);
    staticBuffer.vbPtr->Unlock();

    *staticId = m_staticBufferList.size();
    m_staticBufferList.push_back(staticBuffer);

    return UGP_OK;
}

void D3D9Renderer::SetStaticBuffer(int32_t staticId, void** data) const
{
    if (static_cast<size_t>(staticId) >= m_staticBufferList.size() || staticId < 0)
        return;

    void* ptr;

    m_staticBufferList[staticId].vbPtr->Lock(0, 0, &ptr, 0);

    memcpy(ptr, data, (m_staticBufferList[staticId].numVerts * m_staticBufferList[staticId].stride));

    m_staticBufferList[staticId].vbPtr->Unlock();
}

bool D3D9Renderer::LoadXModel(const std::string& file_name, int32_t& xModelId)
{
    if (file_name.empty())
        return false;

    CXModel temp;
    temp.SetDevice(m_Device, m_caps);
    if (!temp.LoadXFile(g_pEngine->GetPathManager()->GetFileFullPath(file_name)))
        return false;

    xModelId = m_xModelsList.size();
    m_xModelsList.push_back(temp);

    return true;
}

// 描画関数
int D3D9Renderer::RenderStaticBuffer(int32_t staticId)
{
    if (staticId >= static_cast<int>(m_staticBufferList.size()))
        return UGP_FAIL;

    if (m_activeStaticBuffer != staticId)
    {
        if (m_staticBufferList[staticId].ibPtr != nullptr)
            m_Device->SetIndices(m_staticBufferList[staticId].ibPtr);

        m_Device->SetStreamSource(0, m_staticBufferList[staticId].vbPtr, 0, m_staticBufferList[staticId].stride);

        m_Device->SetFVF(m_staticBufferList[staticId].fvf);

        m_activeStaticBuffer = staticId;
    }

    if (m_staticBufferList[staticId].ibPtr != nullptr)
    {
        switch (m_staticBufferList[staticId].primType)
        {
        case PrimType::POINT_LIST:
            if (FAILED(m_Device->DrawPrimitive(D3DPT_POINTLIST,
                0, m_staticBufferList[staticId].numVerts)))
                return UGP_FAIL;
            break;

        case PrimType::TRIANGLE_LIST:
            if (FAILED(m_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,
                0, m_staticBufferList[staticId].numVerts,
                0, m_staticBufferList[staticId].numIndices / 3)))
                return UGP_FAIL;
            break;

        case PrimType::TRIANGLE_STRIP:
            if (FAILED(m_Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0,
                0, m_staticBufferList[staticId].numVerts,
                0, m_staticBufferList[staticId].numIndices / 3)))
                return UGP_FAIL;
            break;

        case PrimType::TRIANGLE_FAN:
            if (FAILED(m_Device->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0,
                0, m_staticBufferList[staticId].numVerts / 2,
                0, m_staticBufferList[staticId].numIndices)))
                return UGP_FAIL;
            break;

        case PrimType::LINE_LIST:
            if (FAILED(m_Device->DrawIndexedPrimitive(D3DPT_LINELIST, 0,
                0, m_staticBufferList[staticId].numVerts / 2,
                0, m_staticBufferList[staticId].numIndices)))
                return UGP_FAIL;
            break;

        case PrimType::LINE_STRIP:
            if (FAILED(m_Device->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0,
                0, m_staticBufferList[staticId].numVerts,
                0, m_staticBufferList[staticId].numIndices)))
                return UGP_FAIL;
            break;

        case PrimType::NULL_TYPE: break;
        default:
            return UGP_FAIL;
        }
    }
    else
    {
        switch (m_staticBufferList[staticId].primType)
        {
        case PrimType::POINT_LIST:
            if (FAILED(m_Device->DrawPrimitive(D3DPT_POINTLIST, 0,
                m_staticBufferList[staticId].numVerts)))
                return UGP_FAIL;
            break;

        case PrimType::TRIANGLE_LIST:
            if (FAILED(m_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0,
                static_cast<int>(m_staticBufferList[staticId].numVerts / 3))))
                return UGP_FAIL;
            break;

        case PrimType::TRIANGLE_STRIP:
            if (FAILED(m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0,
                static_cast<int>(m_staticBufferList[staticId].numVerts / 2))))
                return UGP_FAIL;
            break;

        case PrimType::TRIANGLE_FAN:
            if (FAILED(m_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0,
                static_cast<int>(m_staticBufferList[staticId].numVerts / 2))))
                return UGP_FAIL;
            break;

        case PrimType::LINE_LIST:
            if (FAILED(m_Device->DrawPrimitive(D3DPT_LINELIST, 0,
                m_staticBufferList[staticId].numVerts / 2)))
                return UGP_FAIL;
            break;

        case PrimType::LINE_STRIP:
            if (FAILED(m_Device->DrawPrimitive(D3DPT_LINESTRIP, 0,
                m_staticBufferList[staticId].numVerts)))
                return UGP_FAIL;
            break;

        default:
            return UGP_FAIL;
        }
    }

    return UGP_OK;
}

void D3D9Renderer::EnableFog(float start, float end, FOG_TYPE type, unsigned long color, bool rangeFog)
{
    if (!m_Device)
        return;

    m_Device->SetRenderState(D3DRS_FOGENABLE, true);
    m_Device->SetRenderState(D3DRS_FOGCOLOR, color);

    m_Device->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&start));
    m_Device->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&end));

    if (type == VERTEX_FOR)
    {
        m_Device->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
    }
    else
    {
        m_Device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
    }

    if (m_caps.RasterCaps & D3DPRASTERCAPS_FOGRANGE)
    {
        if (rangeFog)
        {
            m_Device->SetRenderState(D3DRS_RANGEFOGENABLE, true);
        }
        else
        {
            m_Device->SetRenderState(D3DRS_RANGEFOGENABLE, false);
        }
    }
}

void D3D9Renderer::DisableFog()
{
    if (!m_Device)
        return;

    m_Device->SetRenderState(D3DRS_FOGENABLE, false);
}

void D3D9Renderer::SetDetailMapping()
{
    if (!m_Device)
        return;

    m_Device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    m_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTEXOPCAPS_SELECTARG1);
    m_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    m_Device->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
    m_Device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADDSIGNED);
    m_Device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_Device->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
}

void D3D9Renderer::ReleaseAllStaticBuffers()
{
    for (uint32_t s = 0; s < m_staticBufferList.size(); s++)
    {
        ReleaseStaticBuffer(s);
    }

    if (!m_staticBufferList.empty())
    {
        m_staticBufferList.clear();
    }
}

void D3D9Renderer::ReleaseAllXModels()
{
    for (auto& models : m_xModelsList)
    {
        models.Shutdown();
    }

    m_xModelsList.clear();
}

int D3D9Renderer::ReleaseStaticBuffer(int staticID)
{
    if (staticID < 0 || staticID >= static_cast<int>(m_staticBufferList.size()))
        return UGP_FAIL;

    m_staticBufferList[staticID].fvf = 0;
    m_staticBufferList[staticID].numIndices = 0;
    m_staticBufferList[staticID].numVerts = 0;
    //m_staticBufferList[staticID].primType = 0;
    m_staticBufferList[staticID].stride = 0;

    if (m_staticBufferList[staticID].vbPtr)
    {
        m_staticBufferList[staticID].vbPtr->Release();
        m_staticBufferList[staticID].vbPtr = nullptr;
    }

    if (m_staticBufferList[staticID].ibPtr)
    {
        m_staticBufferList[staticID].ibPtr->Release();
        m_staticBufferList[staticID].ibPtr = nullptr;
    }


    return UGP_OK;
}

void D3D9Renderer::UpdateXAnimation(int xModelId, float time, Matrix4x4f& mat)
{
    D3DXMATRIX temp = {
        mat.data[0][0], mat.data[0][1], mat.data[0][2], mat.data[0][3],
        mat.data[1][0], mat.data[1][1], mat.data[1][2], mat.data[1][3],
        mat.data[2][0], mat.data[2][1], mat.data[2][2], mat.data[2][3],
        mat.data[3][0], mat.data[3][1], mat.data[3][2], mat.data[3][3],
    };

    m_xModelsList[xModelId].Update(time, &temp);
}

int D3D9Renderer::RenderXModel(int xModelId)
{
    m_xModelsList[xModelId].Render();

    return UGP_OK;
}

int D3D9Renderer::ReleaseXModel(int xModelId)
{
    m_xModelsList[xModelId].Shutdown();


    return UGP_OK;
}

void D3D9Renderer::ReleaseTexture(int32_t texid)
{
    if ((--m_textureList2D.at(texid).ret) <= 0)
    {
        m_textureList2D[texid].fileName.clear();
        m_textureList2D[texid].image->Release();
        m_textureList2D[texid].image = nullptr;
    }
}

void D3D9Renderer::SetViewMatrix(Matrix4x4f& mat)
{
    if (!m_Device)
        return;
    m_view = {
        mat.data[0][0], mat.data[0][1], mat.data[0][2], mat.data[0][3],
        mat.data[1][0], mat.data[1][1], mat.data[1][2], mat.data[1][3],
        mat.data[2][0], mat.data[2][1], mat.data[2][2], mat.data[2][3],
        mat.data[3][0], mat.data[3][1], mat.data[3][2], mat.data[3][3],
    };

    m_Device->SetTransform(D3DTS_VIEW, &m_view);
}

void D3D9Renderer::SetWorldMatrix(Matrix4x4f& mat)
{
    if (!m_Device)
        return;

    D3DXMATRIX temp = {
        mat.data[0][0], mat.data[0][1], mat.data[0][2], mat.data[0][3],
        mat.data[1][0], mat.data[1][1], mat.data[1][2], mat.data[1][3],
        mat.data[2][0], mat.data[2][1], mat.data[2][2], mat.data[2][3],
        mat.data[3][0], mat.data[3][1], mat.data[3][2], mat.data[3][3],
    };

    m_Device->SetTransform(D3DTS_WORLD, &temp);
}

void D3D9Renderer::GetProjectMatrix(Matrix4x4f& mat)
{
    if (!m_Device)
        return;

    mat.data[0][0] = m_projection._11;
    mat.data[0][1] = m_projection._12;
    mat.data[0][2] = m_projection._13;
    mat.data[0][3] = m_projection._14;

    mat.data[1][0] = m_projection._21;
    mat.data[1][1] = m_projection._22;
    mat.data[1][2] = m_projection._23;
    mat.data[1][3] = m_projection._24;

    mat.data[2][0] = m_projection._31;
    mat.data[2][1] = m_projection._32;
    mat.data[2][2] = m_projection._33;
    mat.data[2][3] = m_projection._34;

    mat.data[3][0] = m_projection._41;
    mat.data[3][1] = m_projection._42;
    mat.data[3][2] = m_projection._43;
    mat.data[3][3] = m_projection._44;
}

#if IMGUI_ENABLE
bool D3D9Renderer::SetupImGui(WinHWND mainWin)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::LoadIniSettingsFromDisk((g_pEngine->GetPathManager()->GetAssetFolder() + "imgui.ini").c_str());
    const ImGuiIO& io = ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
   // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    if (!ImGui_ImplWin32_Init(mainWin))
        return false;
    if (!ImGui_ImplDX9_Init(m_Device))
        return false;

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !

    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF((font_path / "ClashDisplay-Variable.ttf").generic_string().c_str(), 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
   // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    io.Fonts->AddFontFromFileTTF((g_pEngine->GetPathManager()->GetFontFolder() + "GenEiNuGothic-EB.ttf").c_str(),
        16.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());



    return true;
}

void D3D9Renderer::StartImGuiFrame(bool enable)
{
    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    static bool show_imgui_window = false;
    static bool show_demo_window = false;
    static bool show_another_window = false;
    static bool show_gui_debugwindow = true;

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    if (show_imgui_window)
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!", &show_imgui_window); // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);
        ImGui::Checkbox("Debug Window", &show_gui_debugwindow);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit4("clear color", (float*)&m_Color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))
            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("another window", &show_another_window);
        // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    if (show_gui_debugwindow)
    {
        const char* test_desc[] =
        {
            "SHADE_POINTS",
            "SHADE_SOLIDTRI",
            "SHADE_WIRETRI",
            "CULL_NONE",
            "CULL_CW",
            "CULL_CCW",
            "DEPTH_NONE",
            "DEPTH_READONLY",
            "DEPTH_READWRITE",
        };
        static int type = 0;

        switch (type)
        {
        case 0:
            SetD3D9RenderState(RenderState::SHADE_SOLIDTRI);
            break;
        case 1:
            SetD3D9RenderState(RenderState::SHADE_POINTS);
            break;
        case 2:
            SetD3D9RenderState(RenderState::SHADE_WIRETRI);
            break;
        case 3:
            SetD3D9RenderState(RenderState::CULL_NONE);
            break;
        case 4:
            SetD3D9RenderState(RenderState::CULL_CW);
            break;
        case 5:
            SetD3D9RenderState(RenderState::CULL_CCW);
            break;
        case 6:
            SetD3D9RenderState(RenderState::DEPTH_NONE);
            break;
        case 7:
            SetD3D9RenderState(RenderState::DEPTH_READONLY);
            break;
        case 8:
            SetD3D9RenderState(RenderState::DEPTH_READWRITE);
            break;
        default:
            break;
        }




        ImGui::Begin(u8"GUI Debugテスト手数料", &show_gui_debugwindow);

        static std::deque<float> fps_data;
        float fps = ImGui::GetIO().Framerate;
        ImGui::Text((const char*)u8"平均レンダリング間隔 %.3f ms/frame (%.1f FPS)",
            1000.0f / fps,
            fps);

        fps_data.push_back(fps);
        if (fps_data.size() > 100) fps_data.pop_front();

        auto getData = [](void* data, int index)->float {
            return static_cast<decltype(fps_data)*>(data)->at(index);
        };

        ImGui::PlotLines("fps", getData, (void*)&fps_data, fps_data.size(), 0, "FPS", 0.0f);

        //ImGui::SetNextItemWidth(200);
        ImGui::Combo("Render state", &type, test_desc, IM_ARRAYSIZE(test_desc));
        //ImGui::DragInt("Lines", &display_lines, 0.2f, 1, 100);

        static bool msaa = 1;
        ImGui::Text("Multisample%d :%d", m_d3dpp.MultiSampleType, msaa);
        ImGui::Checkbox("Multisample4", &msaa);
        EnableMultiSample(msaa);

        ImGui::End();
    }
}
#endif
