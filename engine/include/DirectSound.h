/*********************************************************************************

  *FileName: DirectSound.h
			コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/12

**********************************************************************************/

#pragma once

#include<Windows.h>
#include <string>
#include <vector>

#include <xaudio2.h>

#include"SoundInterface.h"

struct AudioState
{
	int Repeats = 0;	//ループ
	std::string command;	//コマンド
	std::string fileName;	//ファイル名
	IXAudio2SourceVoice* pSourceVoice = nullptr; // ソースボイス
	BYTE* pDataAudio = nullptr; // オーディオデータ
	DWORD SizeAudio = 0; // オーディオデータサイズ
};

class CDMSoundObject : public SoundInterface
{
public:
	CDMSoundObject();
	~CDMSoundObject() override = default;

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
	std::vector<AudioState> m_audioState;
};

class CDirectMusicSystem : public SoundSystemInterface
{
public:
	CDirectMusicSystem();
	~CDirectMusicSystem() override = default;

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

