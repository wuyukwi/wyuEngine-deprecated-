#include "player.h"

void Player::Start()
{
    SpriteAnim::Start();


}

void Player::Tick(float delta)
{
    static float timer = 0.0f;

    if (m_property != m_propertyNext)
    {
        if (m_propertyNext == PlayerProperty::Red)
        {
            this->SetColor(0.5f, 0.5f, 0.5f);
            timer += delta;
            if (timer >= 1.0f)
            {
                m_property = m_propertyNext;
                this->SetColor(0.0f, 0.0f, 0.0f);
                timer = 0.0f;
            }
        }
        else if (m_propertyNext == PlayerProperty::Blue)
        {
            this->SetColor(0.5f, 0.5f, 0.5f);
            timer += delta;
            if (timer >= 1.0f)
            {
                m_property = m_propertyNext;
                this->SetColor(1.0f, 1.0f, 1.0f);
                timer = 0.0f;
            }
        }


    }

    SpriteAnim::Tick(delta);
}
