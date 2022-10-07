#include "WindowsApplication.h"
#include "engine.h"

bool WindowsApplication::m_IsQuit = false;

bool WindowsApplication::Initialize(GfxConfiguration* cfg)
{
    m_Config = cfg;

#if IMGUI_ENABLE
    ImGui_ImplWin32_EnableDpiAwareness();
#endif

    //HICON hIcon = (HICON)LoadImage(GetModuleHandle(nullptr), "small.ico", IMAGE_ICON, 0, 0, 0);

    // ウインドウクラスの構造体の初期化
    WNDCLASSEX wcex;
    ZeroMemory(&wcex, sizeof(WNDCLASSEX));
    wcex.cbSize = sizeof(WNDCLASSEX);					// WNDCLASSEXのメモリサイズ
    wcex.style = CS_CLASSDC;							// ウインドウのスタイル
    wcex.lpfnWndProc = WindowProc;						// ウインドウプロシージャ
    wcex.cbClsExtra = 0;								// ０にする (通常は使用しない)
    wcex.cbWndExtra = 0;								// ０にする (通常は使用しない)
    wcex.hInstance = GetModuleHandle(nullptr);				//   インスタンスハンドル
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);		// タスクバーのアイコン
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);			// マウスカーソル
    wcex.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));// クライアント領域の背景色
    wcex.lpszMenuName = nullptr;							// メニューバー
    wcex.lpszClassName = m_Config->winClassName;				// ウインドウクラスの名前
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    // ウィンドウクラスの登録
    RegisterClassEx(&wcex);

    m_hinstance = wcex.hInstance;


    // クライアント領域を指定のサイズに調整
    RECT rect = { 0, 0, static_cast<long>(m_Config->screenWidth), static_cast<long>(m_Config->screenHeight) };		// 画面サイズの構造体
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    // ウインドウを作成
    if (m_Config->fullscreen)
    {
        m_hwnd = CreateWindowEx(
            0,								        // 拡張ウインドウスタイル
            m_Config->winClassName,		            // ウインドウスタイルの名前
            m_Config->appName.c_str(),		            // ウインドウの名前
            WS_OVERLAPPEDWINDOW,	// ウインドウスタイル
            0,					            // ウインドウの左上X座標
            0,					            // 　　〃　　の左上Y座標
            rect.right - rect.left,		//幅
            rect.bottom - rect.top,		//高さ
            nullptr,							// 親ウインドウのハンドル
            nullptr,							// メニューハンドルまたは子ウインドウID
            m_hinstance,				                // インスタンスハンドル
            nullptr);							// ウインドウ作成データ
    }
    else
    {
        m_hwnd = CreateWindowEx(
            0,								// 拡張ウインドウスタイル
            m_Config->winClassName,		            // ウインドウスタイルの名前
            m_Config->appName.c_str(),		            // ウインドウの名前
            WS_OVERLAPPEDWINDOW,	// ウインドウスタイル
            0,					            // ウインドウの左上X座標
            0,					            // 　　〃　　の左上Y座標
            rect.right - rect.left,		//幅
            rect.bottom - rect.top,		//高さ
            nullptr,							// 親ウインドウのハンドル
            nullptr,							// メニューハンドルまたは子ウインドウID
            m_hinstance,				                // インスタンスハンドル
            nullptr);							// ウインドウ作成データ
    }


    if (m_hwnd)
    {
        // ウィンドウの表示
        ShowWindow(m_hwnd, SW_SHOWDEFAULT);
        UpdateWindow(m_hwnd);
    }

    return true;
}

void WindowsApplication::Shutdown()
{
    m_IsQuit = true;
}

void WindowsApplication::Tick()
{
    // ウィンドウズメッセージハンドル
    MSG msg;

    // メッセージループ
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        // 仮想メッセージを文字メッセージへ変換
        TranslateMessage(&msg);

        // ウィンドウプロシージャへメッセージを送信
        DispatchMessage(&msg);
    }

}

bool WindowsApplication::IsQuit()
{
    return m_IsQuit;
}

#if IMGUI_ENABLE
// imgui_impl_win32.cppからの前方宣言メッセージハンドラ
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
LRESULT WindowsApplication::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#if IMGUI_ENABLE
    //メッセージをimguiに渡す
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;
#endif

    switch (message)
    {
    case WM_CLOSE:
    case WM_DESTROY:
        PostQuitMessage(0);// ウィンドウ破棄のメッセージ
        m_IsQuit = true;
        // WM_QUITメッセージを送る
        return 0;
    default: break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);// 規定の処理を返す
}
