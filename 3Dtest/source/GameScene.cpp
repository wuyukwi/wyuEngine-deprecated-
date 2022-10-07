#include <algorithm>
#include <filesystem>
#include <functional>

#include "GameScene.h"
#include "main.h"
#include "json11.hpp"
#include "light.h"

using namespace std;
using namespace json11;

GameScene::~GameScene()
= default;


void GameScene::Start()
{
    m_time = 0.0f;
    m_camera = new CameraNode();
    m_camera->SetPos(Vector3f{ -210.0f,120.0f,-130.0f });
    this->AddChild(m_camera);

    m_model = new Node3D("model/bigship1.x");
    this->AddChild(m_model);

    m_ani3D = new AnimationNode3D("model/man/man.x");
    this->AddChild(m_ani3D);

    const float width = g_pEngine->GetConfig()->screenWidth;
    const float height = g_pEngine->GetConfig()->screenHeight;

    Vector3f dir(1.0f, -1.0f, 1.0f);
    Vector4f col(1.0f, 1.0f, 1.0f, 1.0f);
    stLight light;
    light.InitDirectionalLight(dir, col);

    g_pEngine->GetRenderer()->SetLight(light, 0);
    g_pEngine->GetRenderer()->CalculateProjMatrix(PI / 4, 10.0f, 10000.0f);

    Scene::Start();
}

void GameScene::Tick(float delta)
{
    auto* pInput = g_pEngine->GetInput();
    /*   const Vector2f mouse_pos = Vector2f{ static_cast<float>(pInput->GetMousePos().x)
          ,static_cast<float>(pInput->GetMousePos().y) };*/

    const Vector2f L_pos = Vector2f{ static_cast<float>(pInput->GetLeftStickPos().x) / 32767.0f
        ,static_cast<float>(pInput->GetLeftStickPos().y) / 32767.0f };
    const Vector2f R_pos = Vector2f{ static_cast<float>(pInput->GetRightStickPos().x) / 32767.0f
      ,static_cast<float>(pInput->GetRightStickPos().y) / 32767.0f };

    m_time += delta;
    static float timeCount = 0.0f;
    timeCount += delta;
    if (timeCount > 1.0f)
    {
        timeCount = 0.0f;
        m_timeCount++;
    }

#if IMGUI_ENABLE
    ImGui::Begin("\t|TIME|\t");
    ImGui::Text("\tTIME :%.3f\t", m_time);
    ImGui::Text("\tTIME COUNT :%d\t", m_timeCount);
    ImGui::Text("\tFPS :%.d\t", g_pEngine->GetTimer()->GetFps());

    //static float move;
    //static float angle;
    //ImGui::SliderFloat("move", &move, -1.0f, 1.0f);
    //ImGui::SliderFloat("angle", &angle, -1.0f, 1.0f);

    //ImGui::Text("leftStick pos x: %f y: %f", L_pos.x, L_pos.y);
    //ImGui::Text("rightStick pos x: %f y: %f", R_pos.x, R_pos.y);

    //ImGui::Text("XINPUT_GAMEPAD_A : %d", pInput->ControllerButtonDown(XINPUT_GAMEPAD_A));
    //ImGui::Text("XINPUT_GAMEPAD_A T: %d", pInput->ControllerButtonDown(XINPUT_GAMEPAD_A, 0, true));
    //ImGui::Text("pInput->KeyDown(DIK_J T: %d", pInput->KeyDown(DIK_J, true));

    //static int aup = 0, bup = 0, cup = 0;
    //if (pInput->ControllerButtonDown(XINPUT_GAMEPAD_A))
    //    aup++;

    //if (pInput->ControllerButtonDown(XINPUT_GAMEPAD_A, 0, true))
    //    bup++;

    //if (pInput->KeyDown(DIK_J, true))
    //    cup++;

    //static int32_t moter_lr[2] = { 0 };
    //ImGui::SliderInt2("moter", &moter_lr[0], 0, 100000);
    //pInput->SetMoterSpeed(moter_lr[0], moter_lr[1]);

    //ImGui::Text("XINPUT_GAMEPAD_A Down: %d", aup);
    //ImGui::Text("XINPUT_GAMEPAD_A Down t: %d", bup);
    //ImGui::Text("pInput->KeyDown(DIK_J T: %d", cup);




    auto look = m_camera->GetCameraLook();
    auto pos = m_camera->GetCameraPos();
    auto right = m_camera->GetCameraRight();
    auto up = m_camera->GetCameraUp();

    ImGui::Text("look(%3f,%3f,%3f", look.x, look.y, look.z);
    ImGui::Text("right(%3f,%3f,%3f", right.x, right.y, right.z);
    ImGui::Text("up(%3f,%3f,%3f", up.x, up.y, up.z);
    ImGui::Text("pos(%3f,%3f,%3f", pos.x, pos.y, pos.z);

    static float ts[3];
    ImGui::SliderFloat3("ts", &ts[0], -100.0f, 100.0f);
    m_model->SetTranslation(ts[0], ts[1], ts[2]);
    m_ani3D->SetTranslation(ts[0], ts[1], ts[2]);
    Vector3f Ts = m_model->GetTranslation();
    ImGui::Text("ts %f %f %f", Ts.x, Ts.y, Ts.z);

    static float rot[3];
    ImGui::SliderFloat3("rot", &rot[0], -100.0f, 100.0f);
    m_model->SetRotation(rot[0], rot[1], rot[2]);
    m_ani3D->SetRotation(rot[0], rot[1], rot[2]);
    Vector3f Rot = m_model->GetRotation();
    ImGui::Text("ts %f %f %f", Rot.x, Rot.y, Rot.z);

    //static float walk;
    //ImGui::SliderFloat("walk", &walk, -1.0f, 1.0f);
    //m_camera->walk(walk);

    //static float fly;
    //ImGui::SliderFloat("fly", &fly, -1.0f, 1.0f);
    //m_camera->walk(fly);

    //static float picth;
    //ImGui::SliderFloat("picth", &picth, -1.0f, 1.0f);
    //m_camera->pitch(picth);

    //static float yaw;
    //ImGui::SliderFloat("yaw", &yaw, -1.0f, 1.0f);
    //m_camera->yaw(picth);

    ImGui::End();
#endif

    static float walk_speed = 30.0f * delta;
    static float rot_speed = 0.5f * delta;
    if (pInput->KeyDown(DIK_W))
    {

        m_camera->walk(walk_speed);

    }

    if (pInput->KeyDown(DIK_S))
    {

        m_camera->walk(-walk_speed);

    }

    pInput->GetMousePos();

    if (pInput->KeyDown(DIK_A))
    {
        m_camera->strafe(-walk_speed);
    }

    if (pInput->KeyDown(DIK_D))
    {
        m_camera->strafe(walk_speed);
    }


    if (pInput->KeyDown(DIK_R))
    {
        m_camera->fly(walk_speed);
    }

    if (pInput->KeyDown(DIK_F))
    {
        m_camera->fly(-walk_speed);
    }


    if (pInput->KeyDown(DIK_UP))
    {
        m_camera->pitch(rot_speed);
    }
    if (pInput->KeyDown(DIK_DOWN))
    {
        m_camera->pitch(-rot_speed);
    }
    if (pInput->KeyDown(DIK_LEFT))
    {
        m_camera->yaw(rot_speed);
    }
    if (pInput->KeyDown(DIK_RIGHT))
    {
        m_camera->yaw(-rot_speed);
    }

    if (pInput->MouseButtonDown(MOUSE_RIGHT_BUTTON))
    {
        m_camera->yaw(pInput->GetMousePos(true).x * -delta);
        m_camera->pitch(pInput->GetMousePos(true).y * -delta);
    }



    Scene::Tick(delta);
}

void GameScene::ShutDown()
{
    if (!m_nodeList.empty())
    {
        for (auto& i : m_nodeList)
        {
            SAFE_DELETE(i.second)
        }
    }
    m_nodeList.clear();

    Scene::ShutDown();
}

SceneType GameScene::GetNextScene() const
{
    return m_nextScene;
}
