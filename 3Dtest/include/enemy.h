#pragma once
#include "bullet.h"
#include "Node2D.h"

enum class EnemyType
{
    DEATH,
};

class Enemy :public SpriteAnim
{
public:
    Enemy(int32_t key, const std::string& tex_name)
        :SpriteAnim(key, tex_name)
    {
        m_enableBounding = true;
    }

    virtual void Start() override;
    virtual void Tick(float delta) override;

    virtual void SetEnemyType(EnemyType type) { m_type = type; }
    virtual EnemyType GetEnemyType() { return m_type; }
    virtual void SetHp(float hp);

    virtual float GetHp() { return m_hp; }

    virtual void CreateBullet();

protected:

    EnemyType m_type;
    float m_hp = 0.0f;

    std::vector<Bullet*>m_bulletEnemy;
};

class EnemyManager
{
public:
    EnemyManager();

    void Start();

    static void Tick(float delta, std::vector<Enemy*>& m_pEnemyList);

    static Enemy* CreateEnemy(EnemyType type, Vector2f pos);

protected:
};

