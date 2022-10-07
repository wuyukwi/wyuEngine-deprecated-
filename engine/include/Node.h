#pragma once
#include <cstdint>
#include <vector>
#include "math/Vector.h"
#include <memory>
#include <string>
#include "structs.h"
#include <unordered_map>
#include "math/BoundingGeometry.h"

class Node
{
public:
    enum class NodeType
    {
        Sprite,
        SpriteAnim,
        SpriteBackGround,
        SpriteButton
    };

protected:
    std::vector<Node*> m_children{};
    Vector3f m_pos{ 0.0f,0.0f,0.5f };
    Vector3f m_oldPos{ 0 };
    Vector3f m_displacedPos{ 0 };
    Vector2f m_texPos[4];
    int32_t m_key{};
    float m_scalar = 1.f;
    Vector4f m_color = { 1.0f,1.0f,1.0f,1.0f };
    bool m_isDelete = false;
    NodeType m_nodeType;
    float m_rotf = 0.f;
    bool m_isRender = true;

public:
    Node() = default;
    virtual ~Node();

    Node(Node const&) = delete;
    Node& operator=(Node const&) = delete;

    virtual void AddChild(Node* sub_node)
    {
        this->m_children.push_back(sub_node);
    }

    virtual void SetScalar(float Scalar);
    virtual float GetScalar();


    virtual void SetPos(const Vector3f& pos);
    virtual void AddPos(const Vector3f& pos);
    virtual void SetPosX(float x);
    virtual void SetPosY(float y);
    virtual float GetPosX();
    virtual float GetPosY();

    virtual void SetColor(int r, int g, int b, int a);
    virtual void SetColor(int r, int g, int b);
    virtual void SetColor(float r, float g, float b, float a);
    virtual void SetColor(float r, float g, float b);

    virtual void SetIsRender(bool is_render) { m_isRender = is_render; }
    virtual bool GetIsRender() { return m_isRender; }

    virtual void SetColorAlpha(float a);

    virtual void SetRot(float rot);

    virtual void SetIsDelete(bool is_delete) { m_isDelete = is_delete; }
    virtual bool GetIsDelete();

    virtual void Start();

    virtual void Tick(float delta);

    virtual int32_t Getkey();
};

