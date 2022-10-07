#include "enemy.h"
#include "bullet.h"

void Enemy::Start()
{
    SpriteAnim::Start();
}

void Enemy::Tick(float delta)
{
    SpriteAnim::Tick(delta);
}

void Enemy::SetHp(float hp)
{
    m_hp += hp;
}

void Enemy::CreateBullet()
{
    const auto pBullet = BulletManager::CreateBullet(BulletUser::Enemy, BulletType::ThunderBall, this->GetPos());

    if (pBullet != nullptr)
    {
        pBullet->SetShootingAngle(0.0f, 800);
        this->AddChild(pBullet);
    }
}

EnemyManager::EnemyManager()
{
}

void EnemyManager::Start()
{
}

void EnemyManager::Tick(float delta, std::vector<Enemy*>& pEnemyList)
{
    static float time = 0.0f;
    time += delta;

    for (auto it = pEnemyList.begin(); it != pEnemyList.end();)
    {
        (*it)->SetColor(255, 255, 255);

        switch ((*it)->GetEnemyType())
        {
        case EnemyType::DEATH:
        {
            //まだ生きてる
            if ((*it)->GetHp() >= 0.0f)
            {
                if (BulletManager::Collision(*it, BulletUser::Enemy))
                {
                    (*it)->SetHp(-1.0f);
                    (*it)->SetColor(100, 100, 100);
                }
                static float pos = 0.0f;
                pos += 0.1f;
                (*it)->PlayAnimation("Scan");
                (*it)->SetPos(900.0f, pos);

                if (time >= 1.0f)
                {
                    time = 0.0f;
                    (*it)->CreateBullet();
                }
            }
            else if ((*it)->GetHp() <= 0.0f)
            {
                if ((*it)->PlayAnimation("explosion"))
                {
                    (*it)->SetIsDelete(true);
                }
            }
            break;
        }

        }
        if ((*it)->GetIsDelete())
        {
            //　erase関数は次のエレメントのイテレータを返す
            it = pEnemyList.erase(it);
        }
        else
        {
            ++it;
        }
    }

}

Enemy* EnemyManager::CreateEnemy(EnemyType type, Vector2f pos)
{
    Enemy* pEnemy = nullptr;
    switch (type)
    {
    case EnemyType::DEATH:
    {
        pEnemy = new Enemy(0, "game/enemy/Scan.png");
        pEnemy->Start();
        pEnemy->SetPos(pos.x, pos.y);
        pEnemy->AddAnimation("game/enemy/Scan.png", Vector2i{ 8,1 },
            "Scan", Vector2i{ 0,8 }, Vector2i{ 0,1 }, 0.05f);
        pEnemy->AddAnimation("game/effect/explosion.png", Vector2i{ 4,2 },
            "explosion", Vector2i{ 0,4 }, Vector2i{ 0,2 }, 0.1f);
        pEnemy->SetEnemyType(EnemyType::DEATH);
        pEnemy->SetHp(100.0f);
        return pEnemy;
    }

    default:
        return pEnemy;
    }
}
