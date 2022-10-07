#pragma once
#include "Windows.h"
#include "ApplicationInterface.h"

class WindowsApplication :public ApplicationInterface
{
public:
    WindowsApplication() = default;
    ~WindowsApplication() override = default;

    bool Initialize(GfxConfiguration* cfg) override;
    void Shutdown() override;

    void Tick() override;

    virtual bool IsQuit();

    HWND GetMainWindow() const { return m_hwnd; };
    HINSTANCE GetHinstance() const { return m_hinstance; }

private:

    // the WindowProc function prototype
    static LRESULT CALLBACK WindowProc(HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam);

    HWND m_hwnd = nullptr;
    HINSTANCE m_hinstance = nullptr;

    GfxConfiguration* m_Config;

    static bool m_IsQuit;
};

