/*********************************************************************************

  *FileName: SoundInterface.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/12

**********************************************************************************/

#pragma once
#include <string>

class SoundSystemInterface
{
public:
    SoundSystemInterface() {}
    virtual ~SoundSystemInterface() {}

    virtual bool Initialize(const char* filepath) = 0;
    virtual bool AddSound(const char* soundfile, int numRepeats, int* id) = 0;
    virtual bool SetupSoundParameters(int id, float dopplerFactor, float rolloffFactor, float minDist, float maxDist) = 0;

    virtual void Play(int id) = 0;
    virtual void UpdateSoundPosition(int id, float x, float y, float z) = 0;
    virtual void Stop(int id) = 0;
    virtual void Shutdown() = 0;
};

class SoundInterface
{
public:
    SoundInterface() {}
    virtual ~SoundInterface() {}

    virtual bool Initialize(std::string& filepath) = 0;
    virtual bool AddSound(const char* soundfile, int numRepeats, const char* command) = 0;
    virtual bool SetupSoundParameters(int id, float dopplerFactor, float rolloffFactor, float minDist, float maxDist) = 0;

    virtual bool Play(const char* command) = 0;
    virtual void Play(int32_t id) = 0;
    virtual void UpdateSoundPosition(int id, float x, float y, float z) = 0;
    virtual bool Stop(const char* command) = 0;
    virtual void Stop(int32_t id) = 0;
    virtual void Shutdown() = 0;
};


