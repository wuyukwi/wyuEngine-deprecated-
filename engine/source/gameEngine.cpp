#include "gameEngine.h"

bool gameEngine::InitializeEngine(const std::string& config_file_path, const std::string& config_file_name, SceneManager* scene_manager)
{
    m_Config = new GfxConfiguration();
    m_pConfigManager = new ConfigManager{};
    m_pConfigManager->Initialize(config_file_path, config_file_name, m_Config);


    m_pTimer = new Timer();

    m_WinApp = new WindowsApplication();

    m_pSceneManager = scene_manager;

    if (!m_WinApp->Initialize(m_Config))
    {
        return false;
    }
    if (!CreateD3DRenderer(&m_Renderer))
    {
        return false;
    }

    if (!m_Renderer->Initialize(m_WinApp->GetMainWindow()))
    {
        return false;
    }
    m_Renderer->SetClearCol(1.0f, 1.0f, 1.0f, 1.0f);

    if (!m_Renderer->CreateText("Arial", 0, true, 18, m_arialID))
    {
        return false;
    }

    if (!CreateDIInput(&m_Input, m_WinApp->GetMainWindow(), m_WinApp->GetHinstance(), false) || !m_Input->Initialize())
    {
        return false;
    }

    if (!CreateDMSound(&m_Sound) || !LoadSoundScript())
    {
        return false;
    }

    m_pSceneManager->Start();

    return true;
}

void gameEngine::ShutdownEngine()
{

    if (m_pSceneManager)
    {
        m_pSceneManager->ShutDown();
        delete m_pSceneManager;
        m_pSceneManager = nullptr;
    }

    // エンジン終了処理
    if (m_Renderer)
    {
        m_Renderer->Shutdown();
        delete m_Renderer;
        m_Renderer = nullptr;
    }

    // Input終了処理
    if (m_Input)
    {
        m_Input->Shutdown();
        delete m_Input;
        m_Input = nullptr;
    }

    if (m_Sound)
    {
        m_Sound->Shutdown();
        delete m_Sound;
        m_Sound = nullptr;
    }

    if (m_WinApp)
    {
        m_WinApp->Shutdown();
        delete m_WinApp;
        m_WinApp = nullptr;
    }

    SAFE_DELETE(m_pConfigManager);
    SAFE_DELETE(m_pTimer);
    SAFE_DELETE(m_Config);
}

void gameEngine::Run() const
{
    while (!m_WinApp->IsQuit())
    {
#if IMGUI_ENABLE
        m_Renderer->StartImGuiFrame();
#endif

        // 描画開始
        m_Renderer->StartRender(true, true, false);

        m_pTimer->Tick();
        m_WinApp->Tick();
        m_Input->UpdateDevices();
        m_pSceneManager->Tick(m_pTimer->GetElapsed());

#if IMGUI_ENABLE
        // InGuiの描画
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
#endif

        // 描画終了
        m_Renderer->EndRendering();
    }

}

Timer* gameEngine::GetTimer() const
{
    return m_pTimer;
}

RenderInterface* gameEngine::GetRenderer() const
{
    return m_Renderer;
}

GfxConfiguration* gameEngine::GetConfig() const
{
    return m_Config;
}

ConfigManager* gameEngine::GetPathManager() const
{
    return m_pConfigManager;
}

CInputInterface* gameEngine::GetInput() const
{
    return m_Input;
}

SoundInterface* gameEngine::GetSoundManager() const
{
    return m_Sound;
}

WindowsApplication* gameEngine::GetWinapp() const
{
    return m_WinApp;
}

bool gameEngine::LoadScript(const char* path_file)
{
    m_pScript = new ScriptLoader{};

    if (!m_pScript->LoadScriptFile(path_file))
        return false;
    //// ループして、指定された各コマンドを実行します。
    //for (int i = 0; i < m_pScript->GetTotalLines(); i++)
    //{
    //    char command[MAX_COMMAND_SIZE];
    //    m_pScript->ParseCommand(command);

    //    if (_stricmp(command, "WINDOW_CLASS") == 0)
    //    {
    //        char temp[MAX_COMMAND_SIZE];
    //        m_pScript->ParseStringParam(temp);
    //        memcpy(&m_Config->winClassName, temp, sizeof(temp));
    //    }
    //    else if (_stricmp(command, "WINDOW_NAME") == 0)
    //    {
    //        char temp[MAX_COMMAND_SIZE];
    //        m_pScript->ParseStringParam(temp);
    //        memcpy(&m_Config->appName, temp, sizeof(temp));
    //    }
    //    else if (_stricmp(command, "WIN_WIDTH") == 0)
    //    {
    //        m_Config->screenWidth = m_pScript->ParseIntParam();
    //    }
    //    else if (_stricmp(command, "WIN_HEIGHT") == 0)
    //    {
    //        m_Config->screenHeight = m_pScript->ParseIntParam();
    //    }
    //    else if (_stricmp(command, "FULLSCREEN") == 0)
    //    {
    //        m_Config->fullscreen = m_pScript->ParseBoolParam();
    //    }
    //    else if (_stricmp(command, "MULTI_SAMPLE") == 0)
    //    {
    //        m_Config->msaaSamples = m_pScript->ParseIntParam();
    //    }
    //    else if (_stricmp(command, "SOUNDS_PATH") == 0)
    //    {
    //        char temp[MAX_COMMAND_SIZE];
    //        m_pScript->ParseStringParam(temp);
    //        memcpy(&m_Config->soundPath, temp, sizeof(temp));
    //    }
    //    else if (_stricmp(command, "SOUNDS_FILE") == 0)
    //    {
    //        char temp[MAX_COMMAND_SIZE];
    //        m_pScript->ParseStringParam(temp);
    //        memcpy(&m_Config->soundFile, temp, sizeof(temp));
    //    }
    //    else if (_stricmp(command, "#") == 0)
    //    {
    //        // 何にもしません
    //    }
    //    else
    //    {
    //        // 何にもしません
    //    }
    //    // スクリプトの次の行に移動します。
    //    m_pScript->MoveToNextLine();
    //}

    //// すべてのリソースを解放します。このスクリプトは、初期化中にのみ実行されるように設計されています。
    //m_pScript->Shutdown();

    return false;
}

bool gameEngine::LoadSoundScript()
{
    m_pScript = new ScriptLoader{};
    std::string soundFolder = m_pConfigManager->GetSoundFolder();

    m_Sound->Initialize(soundFolder);

    std::string soundtxt = "/Phat Phrog Studios - Autopilot Engaged.wav";
    m_Sound->AddSound(soundtxt.c_str(), -1, "command");

    //m_pScript->LoadScriptFile(soundtxt.c_str());

    //int repeats = 0;
    //int id = 0;

    //// ループして、指定された各コマンドを実行します。
    //for (int i = 0; i < m_pScript->GetTotalLines(); i++)
    //{
    //    char fileName[MAX_COMMAND_SIZE];
    //    char command[MAX_COMMAND_SIZE];
    //    m_pScript->ParseCommand(command);

    //    if (_stricmp(command, "MENU_SOUND") == 0)
    //    {
    //        m_pScript->ParseStringParam(fileName);
    //        repeats = m_pScript->ParseIntParam();
    //        m_Sound->AddSound(fileName, repeats, command);
    //    }
    //    else if (_stricmp(command, "SHOT_SOUND") == 0)
    //    {
    //        m_pScript->ParseStringParam(fileName);
    //        repeats = m_pScript->ParseIntParam();
    //        m_Sound->AddSound(fileName, repeats, command);
    //    }

    //    m_pScript->MoveToNextLine();
    //}

    //// すべてのリソースを解放します。このスクリプトは、初期化中にのみ実行されるように設計されています。
    //m_pScript->Shutdown();
    return true;
}

std::string GetPath(const std::string& filename)
{

    size_t found = filename.find_last_of("\\");

    std::string cwd = filename.substr(0, found + 1);

    return cwd;
}
