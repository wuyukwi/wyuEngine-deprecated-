#pragma once
#include "Node.h"

struct Animation2D
{
    Vector2i frames{ 0 };          //フレーム数
    Vector2f frameOffset = { 0 };  //フレームの間隔
    std::string animation_name;//アニメーションの名前
    Vector2i frames_u{ 0 };//フレームｘ軸の範囲
    Vector2i frames_v{ 0 };//フレームＹ軸の範囲
    float frame_rate{};//アニメーションの再生スピード
    int32_t texId{};//テクスチャのid
};

class Sprite :public Node, public BoundingBox2D
{
protected:
    NodeVertex2D m_vertexBuffer[4];
    std::string m_texName;
    int32_t m_texId = -1;
    int32_t m_staticId = -1;
    bool m_enableBounding = false;
    bool m_isUpdata = true;
    Vector2f m_xySize{ 0 };

public:

    Sprite() = default;
    ~Sprite() override;

    Sprite(int32_t key, std::string tex_name);

    virtual Vector2f GetSize() { return m_xySize; }
    virtual float GetSizeX() { return m_xySize.x; }
    virtual float GetSizeY() { return m_xySize.y; }
    Vector4f m_screenSize{ -1000.0f,5000.0f,-1000.0f,4000.0f };

    virtual void SetSizeX(int32_t x);
    virtual void SetSizeY(int32_t y);
    virtual void SetSize(float x, float y);
    virtual void SetSize(int32_t x, int32_t y);
    virtual void SetSize(uint32_t x, uint32_t y);
    virtual void SetSize(const Vector2f& vec);
    virtual void SetMove(float move, float angle);
    virtual void AddPos(const Vector2f& pos);
    virtual void SetPos(float x, float y);
    virtual void SetPos(int32_t x, int32_t y);
    virtual void SetPos(uint32_t x, uint32_t y);
    virtual void SetPos(const Vector2f& pos);
    virtual Vector2f GetPos();
    virtual void SetZbuffer(float z);

    virtual void SetScreenSize(float min_x, float max_x, float min_y, float max_y);

    virtual void SetTexturePos(const Vector2f& u, const Vector2f& v);
    virtual Vector2f GetTexturePos();

    virtual void SetBounding(bool enable);
    virtual void SetTexture(std::string& tex_name);

    virtual void Start() override;
    virtual void Tick(float delta) override;
};

class SpriteAnim :public Sprite
{
public:
    SpriteAnim(int32_t key, const std::string& tex_name)
        :Sprite(key, tex_name) {
        this->m_nodeType = NodeType::SpriteAnim;
        this->m_isRender = false;
    }

    ~SpriteAnim() override;


    void Start() override;
    void Tick(float delta) override;

    /**
     * \brief アニメーション設定関数
     * \param tex_name テクスチャのパス
     * \param frames フレームの数(x,y)
     * \param animation_name アニメーションの名前
     * \param frames_x x軸の何フレームから何フレームまでの範囲
     * \param frames_y y軸の範囲
     * \param frame_rate アニメーションのスピード
     * \return 設定する値当てるかどうか
     */
    virtual bool AddAnimation(const std::string& tex_name, Vector2i frames, const std::string& animation_name, Vector2i frames_x, Vector2i frames_y, float frame_rate);
    virtual void SetTextureFrames(const Vector2i& frames);
    virtual bool PlayAnimation(const std::string& animation_name, float speed = -1.0f);
    virtual int32_t GetAnimationCount() { return m_animationCount; }

private:
    bool UpdataAnim(float speed);

    Vector2i m_frames{ 0 };          //フレーム数
    Vector2f m_frameOffset = { 0 };  //フレームの間隔

    Animation2D m_enableAnimation;  //再生しているアニメーション
    int32_t m_enableAnimationID = -1;   //アニメーションのID
    std::vector<Animation2D> m_animation;//アニメーションの配列

    float m_frameRateCount = 0.0f;   //フレーム更新計算用
    Vector2i m_frameCount = { 0 };//現在のフレーム
    int32_t m_animationCount = 0;//アニメーション再生した回数

};

class SpriteBackGround :public Sprite
{
protected:
    Vector2f m_rollSpeed = { 0,0 };
public:

    SpriteBackGround() = default;
    ~SpriteBackGround() override;

    SpriteBackGround(int32_t key, const std::string& tex_name) :Sprite(key, tex_name)
    {
        m_nodeType = NodeType::SpriteBackGround;
    }

    virtual void SetRollSpeed(const Vector2f& speed) { m_rollSpeed = speed; }
    virtual void SetRollSpeed(float u, float v) { m_rollSpeed = Vector2f{ u,v }; }

    void Tick(float delta) override
    {
        m_texPos[0] += m_rollSpeed * delta;
        m_texPos[1] += m_rollSpeed * delta;
        m_texPos[2] += m_rollSpeed * delta;
        m_texPos[3] += m_rollSpeed * delta;

        Sprite::Tick(delta);
    }
};

class SpriteButton :public Sprite
{
protected:
    Vector4f m_colorSelect = { 0.0f };
    Vector2f m_mousePos = { 0.0f };
    bool m_isSelect = false;

    virtual void UpdateButton()
    {

    }

public:

    SpriteButton() = default;
    ~SpriteButton() override {}

    SpriteButton(int32_t key, const std::string& tex_name) :Sprite(key, tex_name)
    {
        m_nodeType = NodeType::SpriteButton;
        m_enableBounding = true;
    }

    void Tick(float delta) override
    {
        m_isSelect = this->isPointInside(m_mousePos);
        Sprite::Tick(delta);
    }

    virtual void SetMousePos(Vector2f mouse_pos)
    {
        m_mousePos = mouse_pos;
    }

    virtual bool IsMouseSelect()
    {
        return m_isSelect;
    }

};
