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

	if (!CreateDMSound(&m_Sound))
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


std::string GetPath(const std::string& filename)
{

	size_t found = filename.find_last_of("\\");

	std::string cwd = filename.substr(0, found + 1);

	return cwd;
}
