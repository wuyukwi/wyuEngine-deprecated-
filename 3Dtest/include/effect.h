#pragma once
#include "Node2D.h"

enum class EffectType
{
    explosion,
};

class Effect : public SpriteAnim
{
public:
    Effect(int32_t key, const std::string& tex_name)
        :SpriteAnim(key, tex_name)
    {
    }

    void Start() override;

    void Tick(float delta) override;

    virtual void SetEffectType(EffectType type) { m_type = type; }
    virtual EffectType GetEffectType() { return m_type; }

protected:
    EffectType m_type;
};



class EffectManager
{
public:
    EffectManager() = default;

    static void Tick(float delta);

    static Effect* CreateEffect(EffectType type, Vector2f pos, Vector2f size);


protected:
    static std::vector<Effect*>m_effectList;
};


