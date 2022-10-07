/*********************************************************************************

  *FileName: DirectSound.cpp
			コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/12

**********************************************************************************/

#include"DirectSound.h"
#include"defines.h"

HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

bool CreateDMSound(SoundInterface** pObj)
{
	if (!*pObj) *pObj = new CDMSoundObject();
	else return false;

	return true;
}

CDMSoundObject::CDMSoundObject()
{
	m_pXAudio2 = nullptr;
	pMasterVoice = nullptr;
	m_comInit = false;
}


bool CDMSoundObject::Initialize(std::string& filepath)
{
	m_filePath = filepath;

	if (!m_comInit)
	{
		// COMライブラリの初期化
		CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
		m_comInit = true;
	}

	if (!m_pXAudio2)
	{
		//XAudio2 エンジンのインスタンスを作成します。
		if (FAILED(XAudio2Create(&m_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
		{
			// COMライブラリの終了処理
			CoUninitialize();

			return false;
		}
	}

	if (!pMasterVoice)
	{
		// マスター音声を作成します。
		if (FAILED(m_pXAudio2->CreateMasteringVoice(&pMasterVoice)))
		{
			if (m_pXAudio2)
			{
				// XAudio2オブジェクトの開放
				m_pXAudio2->Release();
				m_pXAudio2 = nullptr;
			}

			// COMライブラリの終了処理
			CoUninitialize();

			return false;
		}
	}

	return true;
}

bool CDMSoundObject::AddSound(const char* sound_file, int numRepeats, const char* command)
{
	// コマンド既にある場合はtrueを返す
	for (auto it = m_audioState.begin(); it < m_audioState.end(); ++it)
	{
		if (it->command == command)
		{
			return true;
		}
	}

	if (sound_file == nullptr)
	{
		return false;
	}

	AudioState temp;

	DWORD dwChunkSize = 0;
	DWORD dwChunkPosition = 0;
	DWORD dwFiletype;
	WAVEFORMATEXTENSIBLE wfx;
	XAUDIO2_BUFFER buffer;

	// ファイルネーム保存
	temp.fileName = m_filePath + sound_file;

	// コマンド保存
	temp.command = command;

	// バッファのクリア
	memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

	// サウンドデータファイルの生成
	const HANDLE h_file = CreateFile(temp.fileName.c_str(), GENERIC_READ, FILE_SHARE_READ,
		nullptr, OPEN_EXISTING, 0, nullptr);
	if (h_file == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	if (SetFilePointer(h_file, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを先頭に移動
		return false;
	}

	// WAVEファイルのチェック
	if (FAILED(CheckChunk(h_file, 'FFIR', &dwChunkSize, &dwChunkPosition)))
	{
		return false;
	}
	if (FAILED(ReadChunkData(h_file, &dwFiletype, sizeof(DWORD), dwChunkPosition)))
	{
		return false;
	}
	if (dwFiletype != 'EVAW')
	{
		return false;
	}

	// フォーマットチェック
	if (FAILED(CheckChunk(h_file, ' tmf', &dwChunkSize, &dwChunkPosition)))
	{
		return false;
	}
	if (FAILED(ReadChunkData(h_file, &wfx, dwChunkSize, dwChunkPosition)))
	{
		return false;
	}

	// オーディオデータ読み込み
	if (FAILED(CheckChunk(h_file, 'atad', &temp.SizeAudio, &dwChunkPosition)))
	{
		return false;
	}
	temp.pDataAudio = static_cast<BYTE*>(malloc(temp.SizeAudio));
	if (FAILED(ReadChunkData(h_file, temp.pDataAudio, temp.SizeAudio, dwChunkPosition)))
	{
		return false;
	}

	// ソースボイスの生成
	if (FAILED(m_pXAudio2->CreateSourceVoice(&temp.pSourceVoice, &(wfx.Format))))
	{
		return S_FALSE;
	}

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = temp.SizeAudio;
	buffer.pAudioData = temp.pDataAudio;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = temp.Repeats = numRepeats;

	// オーディオバッファの登録
	temp.pSourceVoice->SubmitSourceBuffer(&buffer);

	// ファイルをクローズ
	CloseHandle(h_file);

	m_audioState.push_back(temp);

	return true;
}

bool CDMSoundObject::SetupSoundParameters(int id, float dopplerFactor, float rolloffFactor, float minDist, float maxDist)
{
	//// Set listener.
	//m_listenerParams.flDopplerFactor = dopplerFactor;
	//m_listenerParams.flRolloffFactor = rolloffFactor;
	//m_audioListener->SetAllParameters(&m_listenerParams,DS3D_IMMEDIATE);

	//// Set buffer.
	//m_bufferParams.flMinDistance = minDist;
	//m_bufferParams.flMaxDistance = maxDist;
	//m_audioBuffer->SetAllParameters(&m_bufferParams,DS3D_IMMEDIATE);

	return true;
}

bool CDMSoundObject::Play(const char* command)
{
	// セグメント再生(再生中なら停止)
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// コマンドを探す
	for (auto it = m_audioState.begin(); it < m_audioState.end(); ++it)
	{
		if (it->command == command)
		{
			// バッファの値設定
			memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
			buffer.AudioBytes = it->SizeAudio;
			buffer.pAudioData = it->pDataAudio;
			buffer.Flags = XAUDIO2_END_OF_STREAM;
			buffer.LoopCount = it->Repeats;

			// 状態取得
			it->pSourceVoice->GetState(&xa2state);
			if (xa2state.BuffersQueued != 0)
			{// 再生中
				// 一時停止
				it->pSourceVoice->Stop(0);

				// オーディオバッファの削除
				it->pSourceVoice->FlushSourceBuffers();
			}

			// オーディオバッファの登録
			it->pSourceVoice->SubmitSourceBuffer(&buffer);

			// 再生
			it->pSourceVoice->Start(0);

			return true;
		}
	}

	// コマンド見つからない
	return false;
}

void CDMSoundObject::Play(int32_t id)
{
	if (id >= static_cast<int>(m_audioState.size()) || id < 0)
	{
		return;
	}
	// セグメント再生(再生中なら停止)
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer = {};

	buffer.AudioBytes = m_audioState[id].SizeAudio;
	buffer.pAudioData = m_audioState[id].pDataAudio;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = m_audioState[id].Repeats;

	// 状態取得
	m_audioState[id].pSourceVoice->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_audioState[id].pSourceVoice->Stop(0);

		// オーディオバッファの削除
		m_audioState[id].pSourceVoice->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	m_audioState[id].pSourceVoice->SubmitSourceBuffer(&buffer);

	// 再生
	m_audioState[id].pSourceVoice->Start(0);
}

void CDMSoundObject::UpdateSoundPosition(int id, float x, float y, float z)
{
}

//=============================================================================
// セグメント停止(ラベル指定) -(全て)
//=============================================================================
bool CDMSoundObject::Stop(const char* command)
{
	XAUDIO2_VOICE_STATE xa2state;

	// コマンドを探す
	for (auto it = m_audioState.begin(); it < m_audioState.end(); ++it)
	{
		if (it->command == command)
		{
			// 状態取得
			it->pSourceVoice->GetState(&xa2state);
			if (xa2state.BuffersQueued != 0)
			{// 再生中
				// 一時停止
				it->pSourceVoice->Stop(0);

				// オーディオバッファの削除
				it->pSourceVoice->FlushSourceBuffers();
			}
			return true;
		}
		if (_stricmp(command, "all") == 0)
		{
			for (const auto& state : m_audioState)
			{
				if (state.pSourceVoice)
				{
					// 全て一時停止
					state.pSourceVoice->Stop(0);
				}
			}
			return true;
		}
	}

	// コマンド見つからない
	return false;
}

void CDMSoundObject::Stop(int32_t id)
{
	if (id >= static_cast<int>(m_audioState.size()))
	{
		return;
	}

	if (id <= 0)
	{
		for (const auto& state : m_audioState)
		{
			if (state.pSourceVoice)
			{
				// 全て一時停止
				state.pSourceVoice->Stop(0);
			}
		}
	}

	// セグメント再生(再生中なら停止)
	XAUDIO2_VOICE_STATE xa2state;

	// 状態取得
	m_audioState[id].pSourceVoice->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_audioState[id].pSourceVoice->Stop(0);

		// オーディオバッファの削除
		m_audioState[id].pSourceVoice->FlushSourceBuffers();
	}

}

void CDMSoundObject::Shutdown()
{
	// Release all resources.

	for (auto& state : m_audioState)
	{
		if (state.pSourceVoice)
		{
			// 一時停止
			state.pSourceVoice->Stop(0);

			// ソースボイスの破棄
			state.pSourceVoice->DestroyVoice();
			state.pSourceVoice = nullptr;

			// オーディオデータの開放
			free(state.pDataAudio);
			state.pDataAudio = nullptr;
		}
	}

	// マスターボイスの破棄
	if (pMasterVoice)
	{
		pMasterVoice->DestroyVoice();
		pMasterVoice = NULL;
	}

	// XAudio2オブジェクトの開放
	if (m_pXAudio2)
	{
		SAFE_RELEASE(m_pXAudio2);
	}

	// COMライブラリの終了処理
	CoUninitialize();
	m_comInit = false;
}
//=============================================================================
// チャンクのチェック
//=============================================================================
HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;

	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを先頭に移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクデータの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// ファイルタイプの読み込み
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// ファイルポインタをチャンクデータ分移動
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == format)
		{
			*pChunkSize = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if (dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}

//=============================================================================
// チャンクデータの読み込み
//=============================================================================
HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを指定位置まで移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// データの読み込み
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}

CDirectMusicSystem::CDirectMusicSystem()
{
	m_soundList = NULL;
}

bool CDirectMusicSystem::Initialize(const char* filepath)
{
	return true;
}

bool CDirectMusicSystem::AddSound(const char* filename, int numRepeats, int* id)
{
	return true;
}

bool CDirectMusicSystem::SetupSoundParameters(int id, float dopplerFactor, float rolloffFactor, float min, float max)
{
	return true;
}

void CDirectMusicSystem::Play(int id)
{
}

void CDirectMusicSystem::UpdateSoundPosition(int id, float x, float y, float z)
{
}

void CDirectMusicSystem::Stop(int id)
{
}

void CDirectMusicSystem::Shutdown()
{
}