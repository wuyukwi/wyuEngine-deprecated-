#pragma once

#include "Node2D.h"
#include "map"
#include "defines.h"

class Scene
{
private:
    std::multimap<int32_t, Node*> m_sceneNodeMap;
    uint32_t m_id = 0;

public:
    virtual ~Scene() = default;

    virtual void AddChild(Node* node) {
        m_sceneNodeMap.insert({ node->Getkey(), node });
    }
    virtual void SetSceneId(uint32_t id) { m_id = id; }
    virtual uint32_t GetSceneId() { return m_id; }

    virtual void Start()
    {
        for (auto it = m_sceneNodeMap.begin(); it != m_sceneNodeMap.end(); ++it)
        {
            it->second->Start();
        }
    }

    virtual void Tick(float delta)
    {
        for (auto it = m_sceneNodeMap.begin(); it != m_sceneNodeMap.end();)
        {
            // エレメントがdeleteされた場合はmapから削除
            if (it->second == nullptr)
            {
                //　erase関数は次のエレメントのイテレータを返す
                it = m_sceneNodeMap.erase(it);
            }
            else if (it->second->GetIsDelete())
            {
                //　erase関数は次のエレメントのイテレータを返す
                SAFE_DELETE(it->second)
                    it = m_sceneNodeMap.erase(it);
            }
            else
            {
                it->second->Tick(delta);

                ++it;
            }
        }
    }

    virtual void ShutDown()
    {
        if (!m_sceneNodeMap.empty())
        {
            m_sceneNodeMap.clear();
        }
    }
};
