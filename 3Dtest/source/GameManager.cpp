#include "GameManager.h"
#include "gameEngine.h"

GameManager::~GameManager()
= default;

void GameManager::Start()
{

    SceneManager::Start();

    // sceneの初期化
    m_menuScene = new MenuScene();
    m_gameScene = new GameScene();

    this->SetEnableScene(m_menuScene);
    m_enableSceneType = SceneType::Menu_Scene;

    //this->SetEnableScene(m_gameScene);
    //m_enableSceneType = SceneType::Game_Scene;
}

void GameManager::Tick(float delta)
{

#if IMGUI_ENABLE
    using namespace ImGui;
    Begin("SceneManager");

    static float color[4] = { 1.0f,1.0f,1.0f,0.0f };
    ColorEdit4("color RGBA", &color[0]);
    m_fade->SetColor(color[0], color[1], color[2], color[3]);

    static bool reset;
    if (Button("game scene"))
    {
        m_enableScene->ShutDown();
        this->SetEnableScene(m_gameScene);
        m_enableSceneType = SceneType::Game_Scene;
    }
    if (Button("menu scene"))
    {
        m_enableScene->ShutDown();
        this->SetEnableScene(m_menuScene);
        m_enableSceneType = SceneType::Menu_Scene;
    }

    if (m_enableSceneType == SceneType::Menu_Scene)
    {
        Text("Menu_Scene");
    }
    else if (m_enableSceneType == SceneType::Game_Scene)
    {
        Text("Game_Scene");
    }



    End();
#endif

    if (m_enableSceneType == SceneType::Menu_Scene)
    {
        if (m_menuScene->GetNextScene() == SceneType::Game_Scene)
        {
            if (SetFade(FADE_IN, 1))
            {
                m_menuScene->ShutDown();
                this->SetEnableScene(m_gameScene);
                m_enableSceneType = SceneType::Game_Scene;
            }
        }
    }

    if (m_enableSceneType == SceneType::Game_Scene)
    {
        if (this->GetFadeMode() != FADE_NONE)
        {
            if (SetFade(FADE_OUT, 1))
            {

            }
        }

    }

    SceneManager::Tick(delta);

}

void GameManager::ShutDown()
{
    SceneManager::ShutDown();

    m_menuScene->ShutDown();
    SAFE_DELETE(m_menuScene)
        m_gameScene->ShutDown();
    SAFE_DELETE(m_gameScene)
}
