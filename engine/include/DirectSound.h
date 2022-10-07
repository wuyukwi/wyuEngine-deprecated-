/*********************************************************************************

  *FileName: DirectSound.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/12

**********************************************************************************/

#ifndef _DIRECTSOUND_H_
#define _DIRECTSOUND_H_

#define INITGUID

#include<Windows.h>
#include <string>
#include <vector>
#include"SoundInterface.h"

#include <XAudio2.h>
struct AUDIO_STATE
{
    int Repeats;
    std::string command;
    std::string fileName;
    IXAudio2SourceVoice* pSourceVoice; // ソースボイス
    BYTE* pDataAudio; // オーディオデータ
    DWORD SizeAudio; // オーディオデータサイズ
};

class CDMSoundObject : public SoundInterface
{
public:
    CDMSoundObject();
    ~CDMSoundObject() override { Shutdown(); }

    bool Initialize(std::string& filepath) override;
    bool AddSound(const char* sound_file, int numRepeats, const char* command) override;
    bool SetupSoundParameters(int id, float dopplerFactor, float rolloffFactor, float minDist, float maxDist) override;

    bool Play(const char* command) override;
    void Play(int32_t id) override;
    void UpdateSoundPosition(int id, float x, float y, float z) override;
    bool Stop(const char* command) override;
    void Stop(int32_t id) override;

    void Shutdown() override;

private:
    std::string m_filePath;
    bool m_comInit;
    IXAudio2* m_pXAudio2; // XAudio2オブジェクトへのインターフェイス
    IXAudio2MasteringVoice* pMasterVoice; // マスターボイス
    std::vector<AUDIO_STATE> m_audioState;
};

class CDirectMusicSystem : public SoundSystemInterface
{
public:
    CDirectMusicSystem();
    ~CDirectMusicSystem() override { Shutdown(); }

    bool Initialize(const char* filepath) override;
    bool AddSound(const char* soundfile, int numRepeats, int* id) override;
    bool SetupSoundParameters(int id, float dopplerFactor, float rolloffFactor, float minDist, float maxDist) override;

    void Play(int id) override;
    void UpdateSoundPosition(int id, float x, float y, float z) override;
    void Stop(int id) override;
    void Shutdown() override;

private:
    CDMSoundObject* m_soundList;
};

bool CreateDMSound(SoundInterface** pObj);

#endif _DIRECTSOUND_H_
