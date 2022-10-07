#include "effect.h"

#include "enemy.h"

void Effect::Start()
{
    SpriteAnim::Start();
}

void Effect::Tick(float delta)
{
    SpriteAnim::Tick(delta);
}

//スタティック変数の宣言
std::vector<Effect*> EffectManager::m_effectList;

void EffectManager::Tick(float delta)
{
    for (auto it = m_effectList.begin(); it != m_effectList.end();)
    {
        switch ((*it)->GetEffectType())
        {
        case EffectType::explosion:
            (*it)->PlayAnimation("explosion");
        }
        ++it;
    }
}

Effect* EffectManager::CreateEffect(EffectType type, Vector2f pos, Vector2f size)
{
    Effect* pEffect = nullptr;
    switch (type)
    {
    case EffectType::explosion:
    {
        pEffect = new Effect(0, "game/effect/explosion.png");
        pEffect->Start();
        pEffect->SetPos(pos.x, pos.y);
        pEffect->SetSize(size);
        pEffect->SetTextureFrames(Vector2i{ 0,1 });
        pEffect->AddAnimation("game/effect/explosion.png", Vector2i{ 4,2 },
            "explosion", Vector2i{ 0,4 }, Vector2i{ 0,2 }, 0.1f);
        pEffect->SetEffectType(type);
        m_effectList.push_back(pEffect);
        return pEffect;
    }

    default:
        return pEffect;
    }
}
