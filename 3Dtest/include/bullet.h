#pragma once
#include "Node2D.h"

struct BulletProperty
{
    float m_bulletAngle;
    float m_bulletSpeed;

};

enum class BulletType
{
    ThunderBall,
};

enum class BulletUser
{
    Player,
    Enemy
};

class Bullet : public SpriteAnim
{
public:
    Bullet(int32_t key, const std::string& tex_name, BulletUser user)
        :SpriteAnim(key, tex_name)
    {
        m_enableBounding = true;
        m_user = user;
    }

    virtual void SetShootingAngle(float angle, float speed);

    void Start() override;

    void Tick(float delta) override;

    virtual void SetBulletType(BulletType type) { m_type = type; }
    virtual BulletType GetBulletType() { return m_type; }

    virtual BulletUser GetBulletUser() { return m_user; }

protected:
    float m_angle;
    float m_speed;

    BulletType m_type;
    BulletUser m_user;
};

class BulletManager
{
public:
    BulletManager();

    static void Tick(float delta);

    static Bullet* CreateBullet(BulletUser user, BulletType type, Vector2f pos);

    static bool Collision(Sprite* node, BulletUser user);

protected:
    static std::vector<Bullet*>m_bulletPlayer;
    static std::vector<Bullet*>m_bulletEnemy;
};


