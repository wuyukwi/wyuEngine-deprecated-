#include "Node.h"
#include "gameEngine.h"

Node::~Node()
{

}

void Node::SetScalar(float scalar)
{
    m_scalar = scalar;
}

float Node::GetScalar()
{
    return m_scalar;
}

void Node::SetPos(const Vector3f& pos)
{
    m_pos = pos;
}

void Node::AddPos(const Vector3f& pos)
{
    m_pos += pos;
}


void Node::SetPosX(float x)
{
    m_pos.x = x;
}

void Node::SetPosY(float y)
{
    m_pos.y = y;
}

float Node::GetPosX()
{
    return m_pos.x;
}

float Node::GetPosY()
{
    return m_pos.y;
}


void Node::SetColor(int r, int g, int b, int a)
{
    m_color.r = static_cast<float>(r) / 255.0f;
    m_color.g = static_cast<float>(g) / 255.0f;
    m_color.b = static_cast<float>(b) / 255.0f;
    m_color.a = static_cast<float>(a) / 255.0f;
}

void Node::SetColor(int r, int g, int b)
{
    m_color.r = static_cast<float>(r) / 255.0f;
    m_color.g = static_cast<float>(g) / 255.0f;
    m_color.b = static_cast<float>(b) / 255.0f;
}

void Node::SetColor(float r, float g, float b, float a)
{
    m_color = Vector4f{ r,g,b,a };
}

void Node::SetColor(float r, float g, float b)
{
    m_color.r = r;
    m_color.g = g;
    m_color.b = b;
}

void Node::SetColorAlpha(float a)
{
    m_color.a = a;
}

void Node::SetRot(float rot)
{
    m_rotf = rot;
}

bool Node::GetIsDelete()
{
    return m_isDelete;
}

void Node::Start()
{
    for (auto itr = m_children.begin(); itr < m_children.end(); ++itr)
    {
        (*itr)->Start();
    }
}

void Node::Tick(float deltaTime)
{
    // 前の位置と移動した距離を保存する
    m_displacedPos = m_pos - m_oldPos;
    m_oldPos = m_pos;

    //　子供ノードをティックする
    for (auto it = m_children.begin(); it < m_children.end();)
    {
        // エレメントが削除された場合はvectorから削除
        if (*it == nullptr)
        {
            //　erase関数は次のエレメントのイテレータを返す
            it = m_children.erase(it);
        }
        else if ((*it)->GetIsDelete())
        {
            SAFE_DELETE(*it)
                //　erase関数は次のエレメントのイテレータを返す
                it = m_children.erase(it);
        }
        else
        {
            (*it)->AddPos(m_displacedPos);
            (*it)->Tick(deltaTime);

            ++it;
        }
    }

}

int32_t Node::Getkey()
{
    return m_key;
}





