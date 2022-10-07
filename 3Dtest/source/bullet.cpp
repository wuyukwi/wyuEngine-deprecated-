#include "bullet.h"

void Bullet::SetShootingAngle(float angle, float speed)
{
    m_angle = angle;
    m_speed = speed;
}

void Bullet::Start()
{
    SpriteAnim::Start();
}

void Bullet::Tick(float delta)
{
    SetMove(m_speed * delta, m_angle);
    SpriteAnim::Tick(delta);
}

std::vector<Bullet*> BulletManager::m_bulletEnemy;
std::vector<Bullet*> BulletManager::m_bulletPlayer;
BulletManager::BulletManager()
{
}


void BulletManager::Tick(float delta)
{
    for (auto it = m_bulletPlayer.begin(); it != m_bulletPlayer.end();)
    {
        if ((*it)->GetPosY() <= 0.0f)
        {
            (*it)->SetIsDelete(true);
            //　erase関数は次のエレメントのイテレータを返す
            it = m_bulletPlayer.erase(it);
        }
        else
        {
            switch ((*it)->GetBulletType())
            {
            case BulletType::ThunderBall:
                (*it)->PlayAnimation("ThunderBall");
                break;
            }
            ++it;
        }
    }

    for (auto it = m_bulletEnemy.begin(); it != m_bulletEnemy.end();)
    {
        if ((*it)->GetPosY() <= 0.0f || (*it)->GetPosY() >= 1080.0f)
        {
            (*it)->SetIsDelete(true);
            //　erase関数は次のエレメントのイテレータを返す
            it = m_bulletEnemy.erase(it);
        }
        else
        {
            switch ((*it)->GetBulletType())
            {
            case BulletType::ThunderBall:
                (*it)->PlayAnimation("ThunderBall");
                break;
            }
            ++it;
        }
    }
}

Bullet* BulletManager::CreateBullet(BulletUser user, BulletType type, Vector2f pos)
{
    Bullet* pBullet = nullptr;
    switch (type)
    {
    case BulletType::ThunderBall:
    {
        pBullet = new Bullet(0, "game/bullet/eff_plasma_000_blue.PNG", user);
        pBullet->Start();
        pBullet->SetPos(pos.x, pos.y);
        pBullet->SetShootingAngle(1.0f, 800);
        pBullet->AddAnimation("game/bullet/eff_plasma_000_blue.PNG", Vector2i{ 4,2 },
            "ThunderBall", Vector2i{ 0,4 }, Vector2i{ 0,2 }, 0.05f);

        pBullet->SetBulletType(BulletType::ThunderBall);
    }

    }

    if (user == BulletUser::Player)
    {
        m_bulletPlayer.push_back(pBullet);
    }
    else if (user == BulletUser::Enemy)
    {
        m_bulletEnemy.push_back(pBullet);
    }

    return pBullet;
}

bool BulletManager::Collision(Sprite* node, BulletUser user)
{

    if (user == BulletUser::Player)
    {
        for (auto it = m_bulletEnemy.begin(); it != m_bulletEnemy.end(); ++it)
        {
        }
    }
    else if (user == BulletUser::Enemy)
    {
        for (auto it = m_bulletPlayer.begin(); it != m_bulletPlayer.end();)
        {
            if (node->isBoxCollision(*it))
            {
                //node->SetIsDelete(true);
                return true;
            }
            else
            {
                ++it;
            }
        }
    }

    return false;
}
