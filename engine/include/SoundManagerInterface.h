/*********************************************************************************

  *FileName: SoundManagerInterface.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/12

**********************************************************************************/

#pragma once
#include <string>

class SoundManagerInterface
{
public:
    SoundManagerInterface() = default;
    virtual ~SoundManagerInterface() = default;

    virtual bool Initialize(const std::string& path) = 0;

    virtual bool Play(const std::string& name) = 0;
    virtual bool Stop(const std::string& name) = 0;
    virtual void StopALL() = 0;

    virtual bool LoadJsonFile(const std::string& file_name) = 0;

    virtual void SoundMapReleased() = 0;

    virtual float GetVolume(const std::string& name = "null") = 0;
    virtual void SetVolume(float Decibels, const std::string& name = "null") = 0;

    virtual void Shutdown() = 0;
};


