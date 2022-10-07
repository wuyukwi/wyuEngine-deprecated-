#pragma once
#include "engine.h"

class gameEngine
{
public:
    gameEngine() = default;
    ~gameEngine() = default;

    /**
     * \brief ゲームエンジン初期化関数
     * \param config_file_path /　configファイルのパス
     * \param scene_manager SceneManagerクラスのポインター
     * \return 成功: True 失敗：False
     */
    bool InitializeEngine(const std::string& config_file_path, const std::string& config_file_name, SceneManager* scene_manager);

    //エンジンの終了処理
    void ShutdownEngine();

    //エンジン起動関数、終了まではずっとループする。
    void Run() const;

    Timer* GetTimer() const;
    RenderInterface* GetRenderer() const;
    GfxConfiguration* GetConfig() const;
    ConfigManager* GetPathManager() const;
    CInputInterface* GetInput() const;
    SoundInterface* GetSoundManager() const;
    WindowsApplication* GetWinapp()const;

private:
    bool LoadScript(const char* path_file);
    bool LoadSoundScript();

    ConfigManager* m_pConfigManager{}; //パス管理クラス、configファイルから読み込む
    ScriptLoader* m_pScript{};//Scriptファイル読み込み用クラス、現在未使用
    Timer* m_pTimer{};//タイマークラス
    SceneManager* m_pSceneManager{};//シンーマネージャー
    WindowsApplication* m_WinApp{};//ウィンドウズアプリケーションクラス
    RenderInterface* m_Renderer{};//描画全般管理するクラス
    CInputInterface* m_Input{};//inputマネージャー
    SoundInterface* m_Sound{};//音声マネージャー
    GfxConfiguration* m_Config{};//設定保存用クラス

    int32_t m_arialID;       //フォントid
};

//グローバル変数
extern gameEngine* g_pEngine;

std::string GetPath(const std::string& filename);

