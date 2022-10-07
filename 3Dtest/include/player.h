#pragma once

#include <vector>
#include "Node2D.h"
#include "bullet.h"

enum class PlayerProperty
{
    None,
    Red,
    Blue
};

class Player : public SpriteAnim
{
public:
    Player(int32_t key, const std::string& tex_name)
        :SpriteAnim(key, tex_name)
    {
        m_enableBounding = true;
    }

    virtual void Start() override;
    virtual void Tick(float delta) override;

    virtual void SetMode()
    {
        if (m_property == PlayerProperty::Red)
        {
            m_propertyNext = PlayerProperty::Blue;
        }
        else if (m_property == PlayerProperty::Blue)
        {
            m_propertyNext = PlayerProperty::Red;
        }
    }

    virtual void CreateBullet()
    {
        const auto bullet = new Bullet(0, "game/bk-0.png", BulletUser::Enemy);
        m_bullets.push_back(bullet);
        this->AddChild(bullet);
    }

    virtual PlayerProperty GetPlayerProperty()
    {
        return  m_property;
    }

    virtual void SetShootInterval(float interval)
    {
        m_shootInterval = interval;
    }

    virtual float GetShootInterval()
    {
        return m_shootInterval;
    }

private:
    float m_shootInterval;
    std::vector<Bullet*> m_bullets;
    PlayerProperty m_property = PlayerProperty::Red;
    PlayerProperty m_propertyNext = PlayerProperty::None;
};

