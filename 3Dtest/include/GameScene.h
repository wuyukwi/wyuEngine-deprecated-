#pragma once

#include <string>

#include "light.h"
#include "Scene.h"
#include "sceneType.h"
#include "Node3D.h"

class GameScene :public Scene
{
public:
    ~GameScene() override;

    void Start() override;

    void Tick(float delta) override;

    void ShutDown() override;

    SceneType GetNextScene() const;

private:
    std::map <std::string, Sprite*> m_nodeList{};

    CameraNode* m_camera = nullptr;
    Node3D* m_model = nullptr;
    AnimationNode3D* m_ani3D = nullptr;
    TerrainNode* m_terrain = nullptr;
    stLight m_light{};

    SceneType m_nextScene = SceneType::Game_Scene;
    float m_time = 0.0f;
    int32_t m_timeCount = 0;
};


