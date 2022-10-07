/*********************************************************************************

  *FileName: DirectInput.cpp
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/12

**********************************************************************************/

#include"DirectInput.h"

//debug用
#if IMGUI_ENABLE
#include "D3D9Renderer.h"
#endif

#define MOUSE_BUFF_SIZE 16

CGameController* gThis = nullptr;

bool CreateDIInput(CInputInterface** pObj, HWND hwnd, HINSTANCE hInst, bool exclusive)
{
    if (!*pObj) *pObj = new CDirectInputSystem(hwnd, hInst, exclusive);
    else return false;

    return true;
}


CKeyboard::CKeyboard(LPDIRECTINPUT8 input, HWND hwnd)
{
    m_pDevice = nullptr;
    // キーボードデバイスを作成、準備、および取得。
    if (input->CreateDevice(GUID_SysKeyboard, &m_pDevice, nullptr) == DI_OK)
    {
        if (m_pDevice->SetDataFormat(&c_dfDIKeyboard) == DI_OK)
        {
            if (m_pDevice->SetCooperativeLevel(hwnd,
                DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) == DI_OK)
            {
                m_pDevice->Acquire();
            }
        }
    }

    // 初期化処理
    memset(m_keys, 0, sizeof(m_keys));
}

bool CKeyboard::ButtonUp(size_t key)
{
    // 変数のキーが放した瞬間だけ、tureを返します。
    return (!(m_keys[key] & 0x80) && m_keys[key] != m_oldKeys[key]);
}

bool CKeyboard::ButtonDown(size_t key, bool trigger)
{
    // 変数のキーが押されていない場合は、falseを返します。
    if ((m_keys[key] & 0x80) == false)
        return false;

    if (trigger)
    {
        if (m_nextFrame == false)
        {
            return true;
        }

        if (m_keyPressStamp[key] == m_pressStamp - 1 || m_keyPressStamp[key] == m_pressStamp)
        {
            m_keyPressStamp[key] = m_pressStamp;
            return false;
        }

        m_keyPressStamp[key] = m_pressStamp;
        m_nextFrame = false;
    }


    return true;
}

POINT CKeyboard::GetPosition(bool delta)
{
    // キーボードは位置状態なし
    POINT p = { 0, 0 };
    return p;
}

POINT CKeyboard::GetZPosition()
{
    // キーボードは位置状態なし
    POINT p = { 0, 0 };
    return p;
}

bool CKeyboard::UpdateDevice()
{
    if (m_pDevice)
    {
        // 入力比較のために古い状態を保存します。
        memcpy(m_oldKeys, m_keys, sizeof(m_keys));

        // 成功するか、不明なエラーが返されるまで、キーボードをポーリングします。
        if (FAILED(m_pDevice->GetDeviceState(sizeof(m_keys), (LPVOID)m_keys)))
        {
            if (FAILED(m_pDevice->Acquire()))
                return false;
            if (FAILED(m_pDevice->GetDeviceState(sizeof(m_keys), (LPVOID)m_keys)))
                return false;
        }
    }

    m_pressStamp++;
    m_nextFrame = true;
    return true;
}

void CKeyboard::Shutdown()
{
    if (m_pDevice)
    {
        m_pDevice->Unacquire();
        m_pDevice->Release();
        m_pDevice = nullptr;
    }
}

CMouse::CMouse(LPDIRECTINPUT8 input, HWND hwnd, bool exclusive)
{
    // マウス初期化処理
    DWORD flags;
    m_hwnd = hwnd;
    m_pDevice = nullptr;

    if (input->CreateDevice(GUID_SysMouse, &m_pDevice, NULL) == DI_OK)
    {
        if (m_pDevice->SetDataFormat(&c_dfDIMouse) == DI_OK)
        {
            if (exclusive) flags = DISCL_FOREGROUND | DISCL_EXCLUSIVE | DISCL_NOWINKEY;
            else flags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;

            if (m_pDevice->SetCooperativeLevel(hwnd, flags) == DI_OK)
            {
                m_pDevice->Acquire();

                m_pressStamp++;
                m_nextFrame = true;
            }
        }
    }
}

bool CMouse::ButtonUp(size_t button)
{
    // 変数のキーが放した瞬間だけ、tureを返します。
    return (!(m_mouseState.rgbButtons[button] & 0x80) && m_mouseState.rgbButtons[button] != m_oldMouseState.rgbButtons[button]);
}

bool CMouse::ButtonDown(size_t button, bool trigger)
{
    // 変数のキーが押されていない場合は、falseを返します。
    if ((m_mouseState.rgbButtons[button] & 0x80) == false)
        return false;

    if (trigger)
    {
        if (m_nextFrame == false)
        {
            return true;
        }

        if (m_keyPressStamp[button] == m_pressStamp - 1 || m_keyPressStamp[button] == m_pressStamp)
        {
            m_keyPressStamp[button] = m_pressStamp;
            return false;
        }

        m_keyPressStamp[button] = m_pressStamp;
        m_nextFrame = false;
    }


    return true;
}

bool CMouse::UpdateDevice()
{
    if (m_pDevice)
    {
        // 古いデータを保存
        memcpy(&m_oldMouseState, &m_mouseState, sizeof(m_mouseState));

        // デバイスの状態の取得中にエラーが発生した場合は、再取得する
        if (FAILED(m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseState)))
        {
            if (FAILED(m_pDevice->Acquire())) return false;
            if (FAILED(m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseState)))
                return false;
        }

        // マウスの相対位置を取得します。
        GetCursorPos(&m_position);
        ScreenToClient(m_hwnd, &m_position);
    }

    m_pressStamp++;

    return true;
}

POINT CMouse::GetPosition(bool delta)
{
    if (!delta)
        return m_position;

    const POINT p = { m_mouseState.lX,m_mouseState.lY };

    return p;
}

POINT CMouse::GetZPosition()
{
    const POINT p = { m_mouseState.lZ, m_mouseState.lZ };
    return p;
}

void CMouse::Shutdown()
{
    if (m_pDevice)
    {
        m_pDevice->Unacquire();
        m_pDevice->Release();
        m_pDevice = nullptr;
    }
}

CGameController::CGameController()
{
    //xinput
    XInputEnable(true);
}

bool CGameController::ButtonUp(size_t button, size_t user)
{
    return (!(m_XInput[user].KeyState.Gamepad.wButtons & button) &&
        (m_XInput[user].KeyState.Gamepad.wButtons & button) != (m_XInput[user].OldKeyState.Gamepad.wButtons & button));
}

bool CGameController::ButtonDown(size_t button, size_t user, bool trigger)
{
    if (!(m_XInput[user].KeyState.Gamepad.wButtons & button))
        return false;

    if (trigger)
    {
        if (m_nextFrame == false)
        {
            return true;
        }

        if (m_keyPressStamp[user][button] == m_pressStamp - 1 || m_keyPressStamp[user][button] == m_pressStamp)
        {
            m_keyPressStamp[user][button] = m_pressStamp;
            return false;
        }

        m_keyPressStamp[user][button] = m_pressStamp;
        m_nextFrame = false;
    }

    return true;
}

bool CGameController::UpdateDevice()
{
    for (DWORD user_index = 0; user_index < XUSER_MAX_COUNT; user_index++)
    {

        if (m_XInput[user_index].isUse)
        {
            // 古い状態を保存
            memcpy(&m_XInput[user_index].OldKeyState, &m_XInput[user_index].KeyState, sizeof(XINPUT_STATE));

            m_pressStamp++;
            m_nextFrame = true;
        }

        if (XInputGetState(user_index, &m_XInput[user_index].KeyState) == ERROR_SUCCESS)
        {
            // Controller is connected
            m_XInput[user_index].isUse = true;

            //振動の設定
            XInputSetState(user_index, &m_XInput[user_index].Moter);

            //debug用
#if IMGUI_ENABLE
            ImGui::Text("user: %ld", user_index);
            ImGui::Text("wButtons: %d", m_XInput[user_index].KeyState.Gamepad.wButtons);
            ImGui::Text("bLeftTrigger: %d", m_XInput[user_index].KeyState.Gamepad.bLeftTrigger);
            ImGui::Text("bRightTrigger: %d", m_XInput[user_index].KeyState.Gamepad.bRightTrigger);
            ImGui::Text("sThumbLX: %d", m_XInput[user_index].KeyState.Gamepad.sThumbLX);
            ImGui::Text("sThumbLY: %d", m_XInput[user_index].KeyState.Gamepad.sThumbLY);
            ImGui::Text("sThumbRX: %d", m_XInput[user_index].KeyState.Gamepad.sThumbRX);
            ImGui::Text("sThumbRY: %d", m_XInput[user_index].KeyState.Gamepad.sThumbRY);
#endif

        }
        else
        {
            // Controller is not connected
            m_XInput[user_index].isUse = false;
        }
    }

    return true;
}

POINT CGameController::GetPosition(size_t user)
{
    POINT pos;

    pos.x = m_XInput[user].KeyState.Gamepad.sThumbLX;
    pos.y = m_XInput[user].KeyState.Gamepad.sThumbLY;
    return pos;
}

POINT CGameController::GetZPosition(size_t user)
{
    POINT pos;

    pos.x = m_XInput[user].KeyState.Gamepad.sThumbRX;
    pos.y = m_XInput[user].KeyState.Gamepad.sThumbRY;
    return pos;
}

bool CGameController::GetControllerIsUse(size_t user)
{
    return m_XInput[user].isUse;
}

void CGameController::SetMoterSpeed(uint16_t left, uint16_t right, size_t user)
{
    m_XInput[user].Moter.wLeftMotorSpeed = left;
    m_XInput[user].Moter.wRightMotorSpeed = right;
}

void CGameController::Shutdown()
{
    XInputEnable(false);
}

CDirectInputSystem::CDirectInputSystem(HWND hwnd, HINSTANCE hInst, bool exclusive)
{
    // Initialize objects...
    m_keyboard = NULL;
    m_mouse = NULL;
    m_gameController = NULL;

    // Create input system.
    if (DirectInput8Create(hInst, DIRECTINPUT_VERSION,
        IID_IDirectInput8, reinterpret_cast<void**>(&m_system), nullptr) == DI_OK)
    {
        m_keyboard = new CKeyboard(m_system, hwnd);
        m_mouse = new CMouse(m_system, hwnd, exclusive);
        m_gameController = new CGameController();
    }
}

CDirectInputSystem::~CDirectInputSystem()
{
    // Shut everything down.
    CDirectInputSystem::Shutdown();
}

bool CDirectInputSystem::Initialize()
{
    // Everything took place in the constructor.
    // Force keyboard and mouse before considering the
    // system initialized.
    return (m_keyboard && m_mouse);
}

bool CDirectInputSystem::UpdateDevices()
{
    // Get the device state.
    if (m_mouse)  m_mouse->UpdateDevice();
    if (m_keyboard)  m_keyboard->UpdateDevice();
    if (m_gameController)  m_gameController->UpdateDevice();

    return true;
}

bool CDirectInputSystem::KeyUp(size_t key)
{
    if (!m_keyboard) return false;
    return m_keyboard->ButtonUp(key);
}

bool CDirectInputSystem::KeyDown(size_t key, bool trigger)
{
    if (!m_keyboard) return false;
    return m_keyboard->ButtonDown(key, trigger);
}

bool CDirectInputSystem::MouseButtonUp(size_t button)
{
    if (!m_mouse) return false;
    return m_mouse->ButtonUp(button);
}

bool CDirectInputSystem::MouseButtonDown(size_t button, bool trigger)
{
    if (!m_mouse) return false;
    return m_mouse->ButtonDown(button, trigger);
}

bool CDirectInputSystem::ControllerButtonUp(size_t button, size_t user)
{
    if (!m_gameController) return false;
    return m_gameController->ButtonUp(button, user);
}

bool CDirectInputSystem::ControllerButtonDown(size_t button, size_t user, bool trigger)
{
    if (!m_gameController) return false;
    return m_gameController->ButtonDown(button, user, trigger);
}

Vector2i CDirectInputSystem::GetMousePos(bool delta)
{
    const Vector2i null(0, 0);
    if (!m_mouse) return null;

    return Vector2i{ m_mouse->GetPosition(delta).x,m_mouse->GetPosition(delta).y } ;
}

long CDirectInputSystem::GetMouseWheelPos()
{
    if (!m_mouse) return 0;

    const POINT wheel = m_mouse->GetZPosition();
    return wheel.y;
}

POINT CDirectInputSystem::GetLeftStickPos(size_t user)
{
    POINT null = { 0, 0 };
    if (!m_gameController) return null;

    return m_gameController->GetPosition(user);
}

POINT CDirectInputSystem::GetRightStickPos(size_t user)
{
    POINT null = { 0, 0 };
    if (!m_gameController) return null;

    return m_gameController->GetZPosition(user);
}

bool CDirectInputSystem::GetControllerIsUse(size_t user)
{
    return m_gameController->GetControllerIsUse(user);
}

void CDirectInputSystem::SetMoterSpeed(uint16_t left, uint16_t right, size_t user)
{
    m_gameController->SetMoterSpeed(left, right, user);
}

void CDirectInputSystem::Shutdown()
{
    // Delete each object...
    if (m_keyboard)
    {
        m_keyboard->Shutdown();
        delete m_keyboard;
        m_keyboard = nullptr;
    }

    if (m_mouse)
    {
        m_mouse->Shutdown();
        delete m_mouse;
        m_mouse = nullptr;
    }

    if (m_gameController)
    {
        m_gameController->Shutdown();
        delete m_gameController;
        m_gameController = nullptr;
    }

    if (m_system)
    {
        m_system->Release();
        m_system = nullptr;
    }
}
