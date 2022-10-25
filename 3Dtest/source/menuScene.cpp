#include "gameEngine.h"
#include "json11.hpp"
#include "MenuScene.h"

MenuScene::~MenuScene()
{

}

void MenuScene::Start()
{
    const auto width = g_pEngine->GetConfig()->screenWidth;
    const auto height = g_pEngine->GetConfig()->screenHeight;

    m_pBackGround = new SpriteButton(0, "menu/title.png");
    m_pBackGround->SetSize(width, height);
    m_pBackGround->SetPos(width / 2, height / 2);
    this->AddChild(m_pBackGround);

    m_pStartButton = new SpriteButton(1, "menu/new_game.png");
    m_pStartButton->SetScalar(0.6f);
    m_pStartButton->SetPos(width - width / 8, height / 2 + height / 10);
    m_pStartButton->SetBounding(true);
    this->AddChild(m_pStartButton);

    /*   m_pSettingButton = new SpriteButton(1, "menu/game_setting.png");
       m_pSettingButton->SetScalar(0.6f);
       m_pSettingButton->SetPos(width - width / 8, height / 2 + height / 10 * 2);
       m_pSettingButton->SetBounding(true);
       this->AddChild(m_pSettingButton);*/

    m_pExitButton = new SpriteButton(1, "menu/exit_game.png");
    m_pExitButton->SetScalar(0.6f);
    m_pExitButton->SetPos(width - width / 8, height / 2 + height / 10 * 2);
    m_pExitButton->SetBounding(true);
    this->AddChild(m_pExitButton);

    //test = new SpriteAnim(1, "menu/test.png");
    //test->SetFrames(Vector2i{ 10,5 });
    ////test->SetSize(370, 587);
    //test->SetScalar(0.3f);
    //test->SetPos(width / 2, height / 2);
    //test->AddAnimation("test", Vector2i{ 0,10 }, Vector2i{ 0,5 }, 0.05f);
    //test->SetTextureFrames(Vector2i{ 0,0 });
    //m_pStartButton->AddChild(test);

    g_pEngine->GetSoundManager()->LoadJsonFile("SoundList.json");
    g_pEngine->GetSoundManager()->Play("game_bgm");

    // g_pEngine->GetRenderer()->SetD3D9RenderState(RenderState::SHADE_WIRETRI);
     // g_pEngine->GetRenderer()->SetD3D9RenderState(RenderState::SHADE_POINTS);
    Scene::Start();
}

void MenuScene::Tick(float delta)
{
    const Vector2f mouse_pos = Vector2f{ static_cast<float>(g_pEngine->GetInput()->GetMousePos().x)
        ,static_cast<float>(g_pEngine->GetInput()->GetMousePos().y) };

    m_pStartButton->SetMousePos(mouse_pos);
    // m_pSettingButton->SetMousePos(mouse_pos);
    m_pExitButton->SetMousePos(mouse_pos);

    static auto pInput = g_pEngine->GetInput();
    static float alpha = 1.0f;

#if IMGUI_ENABLE
    using namespace ImGui;
    Begin("MenuScene");
    /* static float zbuffer[2];
     SliderFloat2("zbuffer", &zbuffer[0], 0.0f, 1.0f);
     m_pBackGround->SetZbuffer(zbuffer[0]);
     m_pStartButton->SetZbuffer(zbuffer[1]);*/

    static float scalar = m_pStartButton->GetScalar();
    SliderFloat("scalar", &scalar, 0.0f, 10.0f);
    m_pStartButton->SetScalar(scalar);

    static float rot = 0.f;
    SliderFloat("rot", &rot, -PI, PI);
    m_pBackGround->SetRot(rot);

    static float volume = 0.0f;
    SliderFloat("volume", &volume, -50.0f, 40.0f);
    // g_pEngine->GetSoundManager()->SetVolume(volume);
    Text("volume :%f", g_pEngine->GetSoundManager()->GetVolume());
    g_pEngine->GetSoundManager()->SetVolume(volume, "game_bgm");

    static float color[4] = { 255,255,255,255 };
    ColorEdit4("color RGBA", &color[0]);
    m_pBackGround->SetColor(color[0], color[1], color[2], color[3]);
    Text("mouse pos : %.0f,%.0f", mouse_pos.x, mouse_pos.y);
    Text("mouse left button : %d", pInput->MouseButtonDown(MOUSE_LEFT_BUTTON));
    //Text("mouse left button trigger: %d", pInput->MouseButtonDown(MOUSE_LEFT_BUTTON, true));
    //std::cout << "1 " << pInput->MouseButtonDown(MOUSE_LEFT_BUTTON, true) << std::endl;
   // std::cout << "2 " << pInput->MouseButtonDown(MOUSE_LEFT_BUTTON, true) << std::endl;

    //Text("isPointInside %d ", m_pStartButton->IsMouseSelect());
    //static bool play = false;
    //Checkbox("PlayAnimation", &play);
    //test->PlayAnimation("test");

    Text("alpha %f", alpha);

    End();
#endif

    static bool fadeIn = true, fadeOut = false;
    static float fadeSpeed = 0.01f;

    if (fadeIn)
    {
        alpha += fadeSpeed;
        if (alpha >= 1.0f)
        {
            fadeIn = false;
            fadeOut = true;
        }
    }

    if (fadeOut)
    {
        alpha -= fadeSpeed;
        if (alpha <= 0.0f)
        {
            fadeOut = false;
            fadeIn = true;
        }
    }

    static int32_t Select = 0;

    if (Select == 0)
    {
        m_pStartButton->SetColorAlpha(alpha);

        if (pInput->ControllerButtonDown(XINPUT_GAMEPAD_DPAD_DOWN, 0, true) || pInput->KeyDown(DIK_S, true))
        {
            Select = 1;
            alpha = 1.0f;
            m_pStartButton->SetColorAlpha(1.0f);
        }
        else if (pInput->ControllerButtonDown(XINPUT_GAMEPAD_DPAD_UP, 0, true) || pInput->KeyDown(DIK_W, true))
        {
            Select = 1;
            alpha = 1.0f;
            m_pStartButton->SetColorAlpha(1.0f);
        }

        if (pInput->ControllerButtonDown(XINPUT_GAMEPAD_A, 0, true) || pInput->KeyDown(DIK_SPACE, true))
        {
            m_nextScene = SceneType::Game_Scene;
        }
    }
    else if (Select == 1)
    {
        m_pExitButton->SetColorAlpha(alpha);
        if (Select == 1 && pInput->ControllerButtonDown(XINPUT_GAMEPAD_DPAD_DOWN, 0, true) || pInput->KeyDown(DIK_S, true))
        {
            Select = 0;
            alpha = 1.0f;
            m_pExitButton->SetColorAlpha(1.0f);
        }
        else if (Select == 1 && pInput->ControllerButtonDown(XINPUT_GAMEPAD_DPAD_UP, 0, true) || pInput->KeyDown(DIK_W, true))
        {
            Select = 0;
            alpha = 1.0f;
            m_pExitButton->SetColorAlpha(1.0f);
        }

        if (pInput->ControllerButtonDown(XINPUT_GAMEPAD_A, 0, true) || pInput->KeyDown(DIK_SPACE, true))
        {
            g_pEngine->GetWinapp()->Shutdown();
        }
    }



    if (m_pStartButton->IsMouseSelect())
    {
        m_pStartButton->SetColor(0, 0, 0);

        if (pInput->MouseButtonDown(MOUSE_LEFT_BUTTON, true))
        {
            m_nextScene = SceneType::Game_Scene;
        }
    }
    else
    {
        m_pStartButton->SetColor(255, 255, 255);
    }

    //static bool down;
    //if (m_pSettingButton->IsMouseSelect())
    //{
    //    m_pSettingButton->SetColor(0, 0, 0);

    //    if (!pInput->MouseButtonDown(MOUSE_LEFT_BUTTON))
    //    {
    //        down = true;
    //    }

    //    if (down)
    //    {
    //        if (pInput->MouseButtonDown(MOUSE_LEFT_BUTTON))
    //        {
    //            m_pSettingButton->SetPos(mouse_pos.x, mouse_pos.y);
    //        }

    //    }
    //}
    //else
    //{
    //    m_pSettingButton->SetColor(255, 255, 255);
    //    down = false;
    //}


    if (m_pExitButton->IsMouseSelect())
    {
        m_pExitButton->SetColor(0, 0, 0);
        if (g_pEngine->GetInput()->MouseButtonUp(MOUSE_LEFT_BUTTON))
        {
            g_pEngine->GetWinapp()->Shutdown();
        }
    }
    else
    {
        m_pExitButton->SetColor(255, 255, 255);
    }


    Scene::Tick(delta);
}

void MenuScene::ShutDown()
{
    SAFE_DELETE(m_pBackGround)
        SAFE_DELETE(m_pStartButton)
        SAFE_DELETE(m_pSettingButton)
        SAFE_DELETE(m_pExitButton)
        SAFE_DELETE(test)
        m_nextScene = SceneType::Menu_Scene;
    g_pEngine->GetSoundManager()->StopALL();
    Scene::ShutDown();
}

SceneType MenuScene::GetNextScene() const
{
    return m_nextScene;
}

