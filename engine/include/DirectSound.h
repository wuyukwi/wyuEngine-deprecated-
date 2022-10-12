/*********************************************************************************

  *FileName: DirectSound.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/12

**********************************************************************************/

#pragma once

#include<Windows.h>
#include <unordered_map>

#define XAUDIO2_HELPER_FUNCTIONS
#include <xaudio2.h>

#include"json11.hpp"
#include"SoundManagerInterface.h"

struct AudioState
{
    std::string name;	//名前
    std::string path;	//ファイルの相対パス
    IXAudio2SourceVoice* pSourceVoice = nullptr; // ソースボイス
    XAUDIO2_BUFFER buffer{};
    BYTE* pDataAudio = nullptr; // オーディオデータ
};

class XAudioSoundManager : public SoundManagerInterface
{
public:
    XAudioSoundManager();
    ~XAudioSoundManager() override = default;

    bool Initialize(const std::string& path) override;

    bool Play(const std::string& name) override;
    bool Stop(const std::string& name) override;
    void StopALL() override;

    bool LoadJsonFile(const std::string& file_name) override;

    void SoundMapReleased() override;

    float GetVolume(const std::string& name = "null") override;
    void SetVolume(float Decibels, const std::string& name = "null") override;

    void Shutdown() override;

private:
    bool LoadSound(const json11::Json& json);

    std::string m_filePath;
    bool m_comInit;

    IXAudio2* m_pXAudio2 = nullptr; // XAudio2オブジェクトへのインターフェイス
    IXAudio2MasteringVoice* m_pMasterVoice = nullptr; // マスターボイス
    // IXAudio2SubmixVoice* m_pSFXSubmixVoice; // SFX ボイス
    // IXAudio2SubmixVoice* m_pBGMSubmixVoice; // BGM ボイス

    std::unordered_map<std::string, AudioState> m_audioMap;
};


bool CreateDMSound(SoundManagerInterface** pObj);

