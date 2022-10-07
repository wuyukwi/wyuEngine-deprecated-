/*********************************************************************************

  *FileName: DirectInput.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/12

**********************************************************************************/

#ifndef _DIRECTINPUT_H_
#define _DIRECTINPUT_H_

#include<dinput.h>
#include<XInput.h>
#include<cstdint>
#include"InputInterface.h"
#include"defines.h"

class CKeyboard : public CDeviceInterface
{
public:
    CKeyboard(LPDIRECTINPUT8 input, HWND hwnd);
    ~CKeyboard() override { Shutdown(); }

    bool UpdateDevice() override;

    bool ButtonUp(size_t key) override;
    bool ButtonDown(size_t key, bool trigger = false) override;

    POINT GetPosition(bool delta = false) override;
    POINT GetZPosition() override;

    void Shutdown() override;

    // Keyboard device.
    bool m_nextFrame = true;
    LPDIRECTINPUTDEVICE8 m_pDevice;
    char m_keys[KEYS_SIZE];
    char m_oldKeys[KEYS_SIZE];

    size_t m_pressStamp = 0;
    size_t m_keyPressStamp[KEYS_SIZE] = { 0 }; // 各キーが事前にシードされた最後のフレームにスタンプを付けます。
};

class CMouse : public CDeviceInterface
{
public:
    CMouse(LPDIRECTINPUT8 input, HWND hwnd, bool exclusive);
    ~CMouse() override { Shutdown(); }

    bool UpdateDevice() override;

    bool ButtonUp(size_t key) override;
    bool ButtonDown(size_t key, bool trigger = false) override;

    POINT GetPosition(bool delta = false) override;
    POINT GetZPosition() override;

    void Shutdown() override;

    // Mouse device.
    LPDIRECTINPUTDEVICE8 m_pDevice;
    DIMOUSESTATE m_mouseState;
    DIMOUSESTATE m_oldMouseState;
    bool m_button[3];

    size_t m_pressStamp = 0;
    size_t m_keyPressStamp[3] = { 0 }; // 各キーが事前にシードされた最後のフレームにスタンプを付けます。
    bool m_nextFrame = true;

    // マウスのの位置
    POINT m_position;

    // 親ウィンドウのハンドル。
    HWND m_hwnd;

    // 最大x（幅）およびy（高さ）制限
    bool m_restrict;
    int m_width;
    int m_height;
};

struct Controller_State
{
    XINPUT_STATE KeyState;			//ジョイパットの情報
    XINPUT_STATE OldKeyState;		//ジョイパットの古い情報
    XINPUT_VIBRATION Moter;			//ジョイパッドのモーター
    float MoterTime;
    bool isUse;
};

class CGameController
{
public:
    CGameController();
    ~CGameController() { Shutdown(); }

    bool UpdateDevice();

    bool ButtonUp(size_t key, size_t user = 0);
    bool ButtonDown(size_t key, size_t user = 0, bool trigger = false);

    POINT GetPosition(size_t user);
    POINT GetZPosition(size_t user);

    bool GetControllerIsUse(size_t user = 0);

    void SetMoterSpeed(uint16_t left, uint16_t right, size_t user = 0);

    void Shutdown();

protected:
    bool m_nextFrame = true;
    size_t m_pressStamp = 0;
    size_t m_keyPressStamp[XUSER_MAX_COUNT][KEYS_SIZE]{ }; // 各キーが事前にシードされた最後のフレームにスタンプを付けます。

    //xinput
    Controller_State m_XInput[XUSER_MAX_COUNT]{};
};




class CDirectInputSystem : public CInputInterface
{
public:
    CDirectInputSystem(HWND hwnd, HINSTANCE hInst, bool exclusive);
    ~CDirectInputSystem() override;

    // Init devics, update devices states.
    bool Initialize() override;
    bool UpdateDevices() override;

    // Keyboard functions.
    bool KeyUp(size_t key) override;
    bool KeyDown(size_t key, bool trigger = false) override;

    // Mouse functions.
    bool MouseButtonUp(size_t button) override;
    bool MouseButtonDown(size_t button, bool trigger = false) override;

    /**
     * \brief マウスのposを取得関数
     * \param delta trueの場合は毎フレーム移動した値を返す
     * \return マウスのｘ、ｙ軸のpos
     */
    Vector2i GetMousePos(bool delta = false) override;
    long GetMouseWheelPos() override;

    // Game controller functions.
    bool ControllerButtonUp(size_t button, size_t user = 0) override;
    bool ControllerButtonDown(size_t button, size_t user = 0, bool trigger = false) override;

    // Get controller main (left) and right stick position.
    POINT GetLeftStickPos(size_t user = 0) override;
    POINT GetRightStickPos(size_t user = 0) override;

    bool GetControllerIsUse(size_t user = 0) override;
    void SetMoterSpeed(uint16_t left, uint16_t right, size_t user = 0) override;

    void Shutdown() override;

protected:
    LPDIRECTINPUT8 m_system{};

    // Devices.
    CKeyboard* m_keyboard;
    CMouse* m_mouse;
    CGameController* m_gameController;
};

bool CreateDIInput(CInputInterface** pObj, HWND hwnd, HINSTANCE hInst, bool exclusive);

#endif
