#include "SceneManager.h"
#include "gameEngine.h"

void SceneManager::Start()
{
    const auto width = g_pEngine->GetConfig()->screenWidth;
    const auto height = g_pEngine->GetConfig()->screenHeight;

    //sceneノードの初期化
    m_fade = new Sprite(0, "fade.jpg");
    m_fade->SetSize(width, height);
    m_fade->SetPos(width / 2, height / 2);
    m_fade->SetColorAlpha(0.0f);

    this->AddChild(m_fade);

    Scene::Start();
}

void SceneManager::ShutDown()
{
    Scene::ShutDown();
    SAFE_DELETE(m_fade);
}

void SceneManager::SetEnableScene(Scene* scene)
{
    m_enableScene = scene;
    m_enableScene->Start();
}

Scene* SceneManager::GetEnableScene()
{
    return m_enableScene;
}

bool SceneManager::SetFade(FADE mode, float time)
{
    if (m_fadeMode != mode)
        m_fadeMode = mode;

    if (m_fadeMode == FADE_IN)
    {
        static float time_count = 0.0f;
        time_count += g_pEngine->GetTimer()->GetElapsed() / time;
        m_fade->SetColorAlpha(time_count);

        if (time_count >= 1.0f)
        {
            m_fadeMode = FADE_IN;
            return true;
        }
        return false;
    }
    else if (m_fadeMode == FADE_OUT)
    {
        static float time_count = 1.0f;
        time_count -= g_pEngine->GetTimer()->GetElapsed() / time;
        m_fade->SetColorAlpha(time_count);

        if (time_count <= 0.0f)
        {
            m_fadeMode = FADE_NONE;
            return true;
        }
        return false;
    }
    return false;
}

SceneManager::FADE SceneManager::GetFadeMode()
{
    return m_fadeMode;
}


void SceneManager::Tick(float delta)
{
    this->GetEnableScene()->Tick(delta);
    Scene::Tick(delta);
}
