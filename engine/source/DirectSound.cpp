/*********************************************************************************

  *FileName: DirectSound.cpp
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/12

**********************************************************************************/

#include <fstream>
#include <iostream>
#include <sstream>

#include"DirectSound.h"
#include"defines.h"

HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

using namespace std;
using namespace json11;

bool CreateDMSound(SoundManagerInterface** pObj)
{
    if (!*pObj) *pObj = new XAudioSoundManager();
    else return false;

    return true;
}

XAudioSoundManager::XAudioSoundManager()
{
    m_pXAudio2 = nullptr;
    m_pMasterVoice = nullptr;
    m_comInit = false;
}


bool XAudioSoundManager::Initialize(const string& filepath)
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

    if (!m_pMasterVoice)
    {
        // マスター音声を作成します。
        if (FAILED(m_pXAudio2->CreateMasteringVoice(&m_pMasterVoice)))
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


bool XAudioSoundManager::Play(const std::string& name)
{
    XAUDIO2_VOICE_STATE xa2state;

    // サウンドマップから探す
    const auto iterator = m_audioMap.find(name);

    //　見つかった場合
    if (iterator != m_audioMap.end())
    {
        // 状態取得
        iterator->second.pSourceVoice->GetState(&xa2state);
        if (xa2state.BuffersQueued != 0)
        {// 再生中
            // 一時停止
            iterator->second.pSourceVoice->Stop(0);

            // オーディオバッファの削除
            iterator->second.pSourceVoice->FlushSourceBuffers();
        }

        // オーディオバッファの登録
        iterator->second.pSourceVoice->SubmitSourceBuffer(&iterator->second.buffer);

        // 再生
        iterator->second.pSourceVoice->Start(0);

        return true;
    }

    // 見つからない場合
    return false;
}

bool XAudioSoundManager::Stop(const string& name)
{
    XAUDIO2_VOICE_STATE xa2state;

    // サウンドマップから探す
    const auto iterator = m_audioMap.find(name);

    //　見つかった場合
    if (iterator != m_audioMap.end())
    {
        // 状態取得
        iterator->second.pSourceVoice->GetState(&xa2state);
        if (xa2state.BuffersQueued != 0)
        {// 再生中
            // 一時停止
            iterator->second.pSourceVoice->Stop(0);

            // オーディオバッファの削除
            iterator->second.pSourceVoice->FlushSourceBuffers();
        }

        return true;
    }

    // 見つからない場合
    return false;
}

void XAudioSoundManager::StopALL()
{
    for (const auto& sound : m_audioMap)
    {
        //ソースボイスある場合
        if (sound.second.pSourceVoice)
        {
            // 一時停止
            sound.second.pSourceVoice->Stop(0);
        }
    }
}

bool XAudioSoundManager::LoadJsonFile(const string& file_name)
{
    // stringにjsonファイルを読み込む
    const ifstream asset_json_file(m_filePath + file_name);
    if (!asset_json_file)
    {
#if _DEBUG
        cout << "!asset_json_file\n" << m_filePath + file_name << endl;
#endif
        return false;
    }

    stringstream buffer;
    buffer << asset_json_file.rdbuf();
    const string asset_json_text(buffer.str());

    // jsonオブジェクトに解析し
    string error;
    const Json SoundJson = Json::parse(asset_json_text, error);
    if (!error.empty())
    {
#if _DEBUG
        cout << error << endl;
#endif
    }

    if (!SoundJson["BGM"].is_null())
    {
        // サウンド読み込む
        for (auto& bgm : SoundJson["BGM"].array_items())
        {
            if (!bgm.is_null())
            {
                LoadSound(bgm);
            }
        }
    }
    if (!SoundJson["SFX"].is_null())
    {
        // サウンド読み込む
        for (auto& sfx : SoundJson["SFX"].array_items())
        {
            if (sfx.is_null())
            {
                LoadSound(sfx);
            }
        }
    }

    return true;
}

void XAudioSoundManager::SoundMapReleased()
{
    for (auto& sound : m_audioMap)
    {
        //ソースボイスある場合
        if (sound.second.pSourceVoice)
        {
            // 一時停止
            sound.second.pSourceVoice->Stop(0);

            // ソースボイスの破棄
            sound.second.pSourceVoice->DestroyVoice();
            sound.second.pSourceVoice = nullptr;

            // オーディオデータの開放
            free(sound.second.pDataAudio);
            sound.second.pDataAudio = nullptr;
        }
    }
    //マップをクリアする
    m_audioMap.clear();
}

float XAudioSoundManager::GetVolume(const string& name)
{
    float volume = 0.0f;
    // 名前指定してない場合は全体の音量取得する
    if (name == "null")
    {
        m_pMasterVoice->GetVolume(&volume);
        return volume;
    }

    // サウンドマップから探す
    const auto iterator = m_audioMap.find(name);

    //　見つかった場合
    if (iterator != m_audioMap.end())
    {
        iterator->second.pSourceVoice->GetVolume(&volume);
        return volume;
    }

    // 見つからない場合
    return -0.1f;
}

void XAudioSoundManager::SetVolume(float Decibels, const std::string& name)
{
    // 名前指定してない場合は全体の音量調整する
    if (name == "null")
    {
        m_pMasterVoice->SetVolume(XAudio2DecibelsToAmplitudeRatio(Decibels));
    }

    // サウンドマップから探す
    const auto iterator = m_audioMap.find(name);

    //　見つかった場合
    if (iterator != m_audioMap.end())
    {
        iterator->second.pSourceVoice->SetVolume(XAudio2DecibelsToAmplitudeRatio(Decibels));
    }
}

void XAudioSoundManager::Shutdown()
{
    SoundMapReleased();

    // マスターボイスの破棄
    if (m_pMasterVoice)
    {
        m_pMasterVoice->DestroyVoice();
        m_pMasterVoice = nullptr;
    }

    // XAudio2オブジェクトの開放
    if (m_pXAudio2)
    {
        SAFE_RELEASE(m_pXAudio2)
    }

    // COMライブラリの終了処理
    CoUninitialize();
    m_comInit = false;
}


bool XAudioSoundManager::LoadSound(const Json& json)
{
    AudioState temp;

    DWORD dwChunkSize = 0;
    DWORD dwChunkPosition = 0;
    DWORD dwFiletype;
    WAVEFORMATEXTENSIBLE wfx;
    DWORD SizeAudio = 0; // オーディオデータサイズ

    // 名前を保存
    temp.name = json["name"].string_value();

    // パスを保存
    temp.path = m_filePath + json["path"].string_value();

    // バッファのクリア
    memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
    //memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

    // サウンドデータファイルの生成
    const HANDLE h_file = CreateFile(temp.path.c_str(), GENERIC_READ, FILE_SHARE_READ,
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
    if (FAILED(CheckChunk(h_file, 'atad', &SizeAudio, &dwChunkPosition)))
    {
        return false;
    }
    temp.pDataAudio = static_cast<BYTE*>(malloc(SizeAudio));
    if (FAILED(ReadChunkData(h_file, temp.pDataAudio, SizeAudio, dwChunkPosition)))
    {
        return false;
    }

    // ソースボイスの生成
    if (FAILED(m_pXAudio2->CreateSourceVoice(&temp.pSourceVoice, &(wfx.Format))))
    {
        return false;
    }

    // バッファの値設定
    temp.buffer.AudioBytes = SizeAudio;
    temp.buffer.pAudioData = temp.pDataAudio;
    temp.buffer.Flags = XAUDIO2_END_OF_STREAM;
    temp.buffer.LoopCount = json["LoopCount"].int_value();

    // オーディオバッファの登録
    temp.pSourceVoice->SubmitSourceBuffer(&temp.buffer);


    //m_pXAudio2->CreateSubmixVoice(&m_pSFXSubmixVoice, 1, 44100, 0, 0, 0, 0);
    //XAUDIO2_SEND_DESCRIPTOR SFXSend = { 0, m_pSFXSubmixVoice };
    //XAUDIO2_VOICE_SENDS SFXSendList = { 1, &SFXSend };
    //VoiceCallback voiceCallback;

    //IXAudio2SourceVoice* pSFXSourceVoice;
    //m_pXAudio2->CreateSourceVoice(&pSFXSourceVoice, (WAVEFORMATEX*)&wfx,
    //	0, XAUDIO2_DEFAULT_FREQ_RATIO, &voiceCallback, &SFXSendList, NULL);


    // ファイルをクローズ
    CloseHandle(h_file);

    //　音声マップに保存
    m_audioMap.insert(pair<string, AudioState>(temp.name, temp));

    return true;
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

