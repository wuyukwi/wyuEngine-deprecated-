#pragma once

#include "SceneManager.h"
#include "GameScene.h"
#include "MenuScene.h"

class GameManager :public SceneManager
{
public:


    GameManager() : m_menuScene(nullptr), m_gameScene(nullptr)
    {
    }

    ~GameManager() override;

    void Start() override;

    void Tick(float delta) override;

    void ShutDown() override;

private:
    MenuScene* m_menuScene;
    GameScene* m_gameScene;

    SceneType m_enableSceneType;
};

