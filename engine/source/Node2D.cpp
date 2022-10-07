#include "Node2D.h"
#include "gameEngine.h"

Sprite::~Sprite()
{
#if _DEBUG
    std::cout << "~Sprite()" << std::endl;
#endif
    g_pEngine->GetRenderer()->ReleaseStaticBuffer(m_staticId);
    g_pEngine->GetRenderer()->ReleaseTexture(m_texId);
    std::cout << "~Sprite()" << std::endl;
}

Sprite::Sprite(int32_t key, const std::string& tex_name) :
    m_texName(tex_name)
{
#if _DEBUG
    std::cout << "Sprite()" << std::endl;
#endif
    this->m_key = key;
    this->m_nodeType = NodeType::Sprite;
    g_pEngine->GetRenderer()->AddTexture2D(m_texName, m_texId, m_xySize);
    m_texPos[0] = { 0.0f,0.0f };
    m_texPos[1] = { 0.0f,1.0f };
    m_texPos[2] = { 1.0f,0.0f };
    m_texPos[3] = { 1.0f,1.0f };
}

void Sprite::SetSizeX(int32_t x)
{
    m_xySize.x = static_cast<float>(x);
}

void Sprite::SetSizeY(int32_t y)
{
    m_xySize.y = static_cast<float>(y);
}

void Sprite::SetSize(float x, float y)
{
    m_xySize.x = x; m_xySize.y = y;
}

void Sprite::SetSize(int32_t x, int32_t y)
{
    m_xySize.x = static_cast<float>(x); m_xySize.y = static_cast<float>(y);
}

void Sprite::SetSize(uint32_t x, uint32_t y)
{
    m_xySize.x = static_cast<float>(x); m_xySize.y = static_cast<float>(y);
}

void Sprite::SetSize(const Vector2f& vec)
{
    m_xySize.x = vec.x;
    m_xySize.y = vec.y;
}

void Sprite::SetMove(float move, float angle)
{
    m_pos.x += sinf(PI * angle) * move;
    m_pos.y += cosf(PI * angle) * move;
}

void Sprite::SetPos(float x, float y)
{
    m_pos.x = x; m_pos.y = y;
}

void Sprite::SetPos(int32_t x, int32_t y)
{
    m_pos.x = static_cast<float>(x); m_pos.y = static_cast<float>(y);
}

void Sprite::SetPos(uint32_t x, uint32_t y)
{
    m_pos.x = static_cast<float>(x); m_pos.y = static_cast<float>(y);
}

void Sprite::SetPos(const Vector2f& vec)
{
    m_pos.x = vec.x;
    m_pos.y = vec.y;
}

Vector2f Sprite::GetPos()
{
    return Vector2f{ m_pos.x,m_pos.y };
}


void Sprite::AddPos(const Vector2f& pos)
{
    m_pos += pos;
}

void Sprite::SetBounding(bool enable)
{
    m_enableBounding = enable;
}

void Sprite::SetTexturePos(const Vector2f& u, const Vector2f& v)
{
    m_texPos[0] = { u.begin,v.begin };
    m_texPos[1] = { u.begin,v.end };
    m_texPos[2] = { u.end,v.begin };
    m_texPos[3] = { u.end,v.end };
}

Vector2f Sprite::GetTexturePos()
{
    return m_texPos[0];
}

void Sprite::SetZbuffer(float z)
{
    m_pos.z = z;
}

void Sprite::SetScreenSize(float min_x, float max_x, float min_y, float max_y)
{
    m_screenSize = Vector4f{ min_x ,max_x,min_y,max_y };
}

void Sprite::Start()
{
    m_oldPos = m_pos;

    g_pEngine->GetRenderer()->CreateStaticBuffer(
        FVF_RHW, PrimType::TRIANGLE_STRIP,
        4, NULL, sizeof(NodeVertex2D),
        reinterpret_cast<void**>(m_vertexBuffer), nullptr, &m_staticId);

    Node::Start();
}

void Sprite::Tick(float delta)
{
    if (m_isUpdata)
    {
        const Vector2f size = m_xySize * m_scalar;

        const float lenght = sqrtf(size.x * size.x + size.y * size.y) / 2;
        const float angle = atan2f(size.x, size.y);

        if (m_pos.x - size.x / 2 < m_screenSize.x)
        {
            m_pos.x = m_screenSize.x + size.x / 2;
        }
        else if (m_pos.x + size.x / 2 > m_screenSize.y)
        {
            m_pos.x = m_screenSize.y - size.x / 2;
        }

        if (m_pos.y - size.y / 2 < m_screenSize.z)
        {
            m_pos.y = m_screenSize.z + size.y / 2;
        }
        if (m_pos.y + size.y / 2 > m_screenSize.w)
        {
            m_pos.y = m_screenSize.w - size.y / 2;
        }

        Vector2f topLeft = { m_pos.x + sinf(m_rotf + (PI + angle)) * lenght,
            m_pos.y + cosf(m_rotf + (PI + angle)) * lenght };


        Vector2f lowerLeft = { m_pos.x + sinf(m_rotf - angle) * lenght,
                   m_pos.y + cosf(m_rotf - angle) * lenght };

        const Vector2f topRight = { m_pos.x + sinf(m_rotf + (PI - angle)) * lenght,
           m_pos.y + cosf(m_rotf + (PI - angle)) * lenght };

        Vector2f lowerRight = { m_pos.x + sinf(m_rotf + angle) * lenght,
                  m_pos.y + cosf(m_rotf + angle) * lenght };


        const auto color = COLOR_RGBA(m_color.r, m_color.g, m_color.b, m_color.a);
        m_vertexBuffer[0] = NodeVertex2D(topLeft.x, topLeft.y, m_pos.z, color, m_texPos[0].u, m_texPos[0].v);
        m_vertexBuffer[1] = NodeVertex2D(lowerLeft.x, lowerLeft.y, m_pos.z, color, m_texPos[1].u, m_texPos[1].v);
        m_vertexBuffer[2] = NodeVertex2D(topRight.x, topRight.y, m_pos.z, color, m_texPos[2].u, m_texPos[2].v);
        m_vertexBuffer[3] = NodeVertex2D(lowerRight.x, lowerRight.y, m_pos.z, color, m_texPos[3].u, m_texPos[3].v);

        g_pEngine->GetRenderer()->SetStaticBuffer(m_staticId, reinterpret_cast<void**>(m_vertexBuffer));
    }

    if (m_enableBounding)
    {
        this->CreateFromPoints(&m_vertexBuffer[0], 4);
    }

    if (m_isRender)
    {
        g_pEngine->GetRenderer()->SetTextureAlphaBlend(true);
        g_pEngine->GetRenderer()->ApplyTexture(1, m_texId);
        g_pEngine->GetRenderer()->RenderStaticBuffer(m_staticId);
        g_pEngine->GetRenderer()->ApplyTexture(0, -1);
        g_pEngine->GetRenderer()->SetTextureAlphaBlend(false);

    }

    Node::Tick(delta);
}

void Sprite::SetTexture(std::string& tex_name)
{
    g_pEngine->GetRenderer()->AddTexture2D(tex_name, m_texId);

    m_texName = tex_name;
}

SpriteAnim::~SpriteAnim()
{
    std::cout << "~SpriteAnim()" << std::endl;
}

void SpriteAnim::Start()
{
    Sprite::Start();
}

void SpriteAnim::Tick(float delta)
{
    Sprite::Tick(delta);
}

bool SpriteAnim::AddAnimation(const std::string& tex_name, Vector2i frames, const std::string& animation_name, Vector2i frames_x, Vector2i frames_y, float frame_rate)
{
    if (frames_x.end > frames.x || frames_y.end > frames.y)
    {
        return false;
    }

    Animation2D temp;
    Vector2f size;

    g_pEngine->GetRenderer()->AddTexture2D(tex_name, temp.texId, size);
    temp.frames = frames;
    this->SetSize(size.x / static_cast<float>(frames.u), size.y / static_cast<float>(frames.v));
    temp.frameOffset.u = 1.0f / static_cast<float>(frames.u);
    temp.frameOffset.v = 1.0f / static_cast<float>(frames.v);
    temp.animation_name = animation_name;
    temp.frames_u = frames_x;
    temp.frames_v = frames_y;
    temp.frame_rate = frame_rate;

    m_animation.push_back(temp);

    return true;

}

void SpriteAnim::SetTextureFrames(const Vector2i& frames)
{
    const Vector2f frame_count = Vector2f(static_cast<float>(frames.u), static_cast<float>(frames.v));

    m_texPos[0].u = m_frameOffset.u * frame_count.u;
    m_texPos[0].v = m_frameOffset.v * frame_count.v;

    m_texPos[1].u = m_frameOffset.u * frame_count.u;
    m_texPos[1].v = m_frameOffset.v * (frame_count.v + 1);

    m_texPos[2].u = m_frameOffset.u * (frame_count.u + 1);
    m_texPos[2].v = m_frameOffset.v * frame_count.v;

    m_texPos[3].u = m_frameOffset.u * (frame_count.u + 1);
    m_texPos[3].v = m_frameOffset.v * (frame_count.v + 1);
}

bool SpriteAnim::PlayAnimation(const std::string& animation_name, float speed)
{
    if (m_enableAnimation.animation_name != animation_name)
    {
        m_isRender = false;
        for (auto iterator = m_animation.begin(); iterator != m_animation.end(); ++iterator)
        {
            if (iterator->animation_name == animation_name)
            {
                // アニメーションデータ更新
                m_frames = iterator->frames;
                m_frameOffset = iterator->frameOffset;
                m_frameCount.u = iterator->frames_u.begin;
                m_frameCount.v = iterator->frames_v.begin;
                m_enableAnimationID = static_cast<int>(iterator - m_animation.begin());
                m_texId = iterator->texId;
                m_enableAnimation = *iterator;

                // 初期化
                m_frameRateCount = 0.0f;
                m_frameCount = 0;
                UpdataAnim(speed);
            }
        }
    }
    else
    {
        return UpdataAnim(speed);
    }

    return false;
}

bool SpriteAnim::UpdataAnim(float speed)
{
    // 経過時間の加算処理
    m_frameRateCount += g_pEngine->GetTimer()->GetElapsed();
    static bool test;

#if IMGUI_ENABLE
    ImGui::Begin("UpdataAnim()");
    ImGui::Text("m_frameRateCount :%f", m_frameRateCount);
    ImGui::Text("m_frameCount :%d,%d", m_frameCount.u, m_frameCount.v);
    ImGui::Text("m_texPos[0] :%f,%f", m_texPos[0].u, m_texPos[0].v);
    ImGui::Text("m_texPos[1] :%f,%f", m_texPos[1].u, m_texPos[1].v);
    ImGui::Text("m_texPos[2] :%f,%f", m_texPos[2].u, m_texPos[2].v);
    ImGui::Text("m_texPos[3] :%f,%f", m_texPos[3].u, m_texPos[3].v);
    ImGui::Text("m_size : %f,%f", m_xySize.x, m_xySize.y);
    ImGui::Text("test %d", test);
    ImGui::End();
#endif

    float frame_rate;
    if (speed <= 0.0f)
    {
        frame_rate = m_animation[m_enableAnimationID].frame_rate;
    }
    else
    {
        frame_rate = speed;
    }

    // 一定の時間経過したらアニメーションを更新する
    if (m_frameRateCount >= frame_rate)
    {
        m_frameRateCount = 0.0f;

        const Vector2f frame_count = Vector2f(static_cast<float>(m_frameCount.u), static_cast<float>(m_frameCount.v));

        // テクスチャ位置更新
        m_texPos[0].u = m_frameOffset.u * frame_count.u;
        m_texPos[0].v = m_frameOffset.v * frame_count.v;

        m_texPos[1].u = m_frameOffset.u * frame_count.u;
        m_texPos[1].v = m_frameOffset.v * (frame_count.v + 1);

        m_texPos[2].u = m_frameOffset.u * (frame_count.u + 1);
        m_texPos[2].v = m_frameOffset.v * frame_count.v;

        m_texPos[3].u = m_frameOffset.u * (frame_count.u + 1);
        m_texPos[3].v = m_frameOffset.v * (frame_count.v + 1);

        // uが1フレーム進める
        m_frameCount.u++;
        // uの最後のフレームに進んだら
        if (m_frameCount.u >= m_animation[m_enableAnimationID].frames_u.end)
        {
            // uを最初の位置に戻す
            m_frameCount.u = m_animation[m_enableAnimationID].frames_u.begin;

            // vが1フレーム進める
            m_frameCount.v++;
            // vの最後のフレームに進んだら
            if (m_frameCount.v >= m_animation[m_enableAnimationID].frames_v.end)
            {
                // vを最初の位置に戻す
                m_frameCount.v = m_animation[m_enableAnimationID].frames_v.begin;
                m_animationCount++;
                test = true;
                return test;
            }

        }
        m_isRender = true;
    }

    test = false;
    return test;
}

SpriteBackGround::~SpriteBackGround()
{
    std::cout << "SpriteBackGround ~" << std::endl;
}
