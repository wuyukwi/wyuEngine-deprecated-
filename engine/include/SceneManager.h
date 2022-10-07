#pragma once
#include "Scene.h"

class SceneManager : public Scene
{
public:
    enum  FADE
    {
        FADE_NONE = 0,		//何もしていない状態
        FADE_IN,			//フェードイン状態
        FADE_OUT			//フェードアウト状態
    };

    ~SceneManager() override = default;

    void Start() override;
    void ShutDown() override;

    virtual void SetEnableScene(Scene* scene);
    virtual Scene* GetEnableScene();
    virtual bool SetFade(FADE mode, float time);
    virtual FADE GetFadeMode();

    void Tick(float delta) override;

protected:
    Scene* m_enableScene = nullptr;
    Sprite* m_fade = nullptr;
    FADE m_fadeMode = FADE_NONE;
};

