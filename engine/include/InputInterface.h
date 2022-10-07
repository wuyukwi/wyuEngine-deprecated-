/*********************************************************************************

  *FileName: InputInterface.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/12

**********************************************************************************/

#pragma once
#include "math/Vector.h"

class CInputInterface
{
public:
    CInputInterface() = default;
    virtual ~CInputInterface() = default;

    virtual bool Initialize() = 0;
    virtual bool UpdateDevices() = 0;

    virtual bool KeyUp(size_t key) = 0;
    virtual bool KeyDown(size_t key, bool trigger = false) = 0;

    virtual bool MouseButtonUp(size_t button) = 0;
    virtual bool MouseButtonDown(size_t button, bool trigger = false) = 0;
    virtual Vector2i GetMousePos(bool delta = false) = 0;
    virtual long GetMouseWheelPos() = 0;

    virtual bool ControllerButtonUp(size_t button, size_t user = 0) = 0;
    virtual bool ControllerButtonDown(size_t button, size_t user = 0, bool trigger = false) = 0;
    virtual POINT GetLeftStickPos(size_t user = 0) = 0;
    virtual POINT GetRightStickPos(size_t user = 0) = 0;
    virtual bool GetControllerIsUse(size_t user = 0) = 0;
    virtual void SetMoterSpeed(uint16_t left, uint16_t right, size_t user = 0) = 0;

    virtual void Shutdown() = 0;
};

class CDeviceInterface
{
public:
    CDeviceInterface() {}
    virtual ~CDeviceInterface() {}

    virtual bool UpdateDevice() = 0;

    virtual bool ButtonUp(size_t key) = 0;
    virtual bool ButtonDown(size_t key, bool trigger = false) = 0;

    virtual POINT GetPosition(bool delta = false) = 0;
    virtual POINT GetZPosition() = 0;

    virtual void Shutdown() = 0;

};

