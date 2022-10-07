#pragma once
#include <cstdint>
#include <ctime>

class Timer
{
private:
    uint32_t m_timeCount;
    uint32_t m_frame;
    uint32_t m_fps;
    float m_elapsed;
    float m_currentTime;
    float m_lastTime;
    float m_frameTime;

public:
    Timer() :m_timeCount(0), m_frame(0), m_fps(0), m_elapsed(0), m_currentTime(0), m_lastTime(0), m_frameTime(0)
    {
        m_currentTime = static_cast<float>(clock());
    }
    ~Timer() = default;

    void Tick()
    {
        m_currentTime = static_cast<float>(clock());
        m_elapsed = (m_currentTime - m_lastTime) * 0.001f;
        m_lastTime = m_currentTime;

        m_frameTime += m_elapsed;
        if (m_frameTime > 1.0f)
        {
            ++m_timeCount;

            m_fps = m_frame;
            m_frame = 0;
            m_frameTime = 0.0f;
        }
        else
        {
            ++m_frame;
        }

    }

    void Reset()
    {
        m_timeCount = 0;
    }

    uint32_t GetTimeCount() const
    {
        return m_timeCount;
    }

    float GetElapsed() const
    {
        return m_elapsed;
    }

    uint32_t GetFps() const
    {
        return m_fps;
    }

    float GetTime() const
    {
        return m_currentTime;
    }

};

