#include "stdafx.h"
#include "MineManager.h"
#include "Mine.h"
#include <algorithm>

MineManager::MineManager()
{
}

MineManager::~MineManager()
{
    Dispose();
}

const Mine* MineManager::AddMineObject(const unsigned int aObjectId, const Vector3 aPosition, const int aTeam)
{
    MutexLock lock(m_lock);

    /* If not team defined, then added, otherwise, retrieve*/
    auto& cachedTeam = m_mObject[aTeam];

    /* Verify if object exists */
    Mine* resultObj(GetObjectByID(aObjectId));

    /* If so, then it is remove from previous pool to be respawned into a different one. */
    if (NULL != resultObj)
    {
        RemoveObject(resultObj);
    }

    /* Create a new mine*/
    if (m_numberOfObjects < cMaximumNumberOfObjects)
    {
        /* To avoid extra copies */
        AddObject(aObjectId, aTeam);

        resultObj = &cachedTeam.back();

        resultObj->SetTeam(aTeam);
        resultObj->SetPosition(aPosition);
        resultObj->SetDestructiveRadius(GetRandomFloat32_Range(100.0f, 1000.0f));
        resultObj->SetActive(GetRandomFloat32() < 0.95f);
        resultObj->SetVunerabilty(GetRandomFloat32() < 0.1f);
    }

    return resultObj;
}

Mine* MineManager::GetObjectWithMostEnemyTargets(const int aTeam)
{
    Mine* out_pObject = NULL;

    auto cachedTeam = m_mObject.find(aTeam);

    if (std::end(m_mObject) != cachedTeam && (*cachedTeam).second.size() > 0)
    {
        out_pObject = &(*std::max_element((*cachedTeam).second.begin(), (*cachedTeam).second.end(), [](const Mine& aObjectLeft, const Mine& aObjectRight) {
                return aObjectLeft.GetNumberOfTargets() < aObjectRight.GetNumberOfTargets();
            }));
    }

    return out_pObject;
}

int MineManager::GetNumberOfObjectForTeam(int aTeam)
{
    auto cachedTeam = m_mObject.find(aTeam);

    return std::end(m_mObject) != cachedTeam ? static_cast<int>((*cachedTeam).second.size()) : 0;
}

void MineManager::AddObject(const Mine* in_object)
{
    if (NULL != in_object)
    {
        m_numberOfObjects++;
        m_mObject[in_object->GetObjectPoolID()].push_back(*in_object);
    }
    else
    {
        STATIC_ASSERT("Null ptr")
    }
}

void MineManager::AddObject(const int objectId, const int poolID)
{
    m_numberOfObjects++;

    m_mObject[poolID].emplace_back(objectId, poolID);
}

void MineManager::RemoveObject(const Mine* in_object)
{
    if (NULL != in_object)
    {
        auto& cachedPool(m_mObject[in_object->GetObjectPoolID()]);

        const auto& it(std::find_if(cachedPool.begin(), cachedPool.end(), [&](const Mine& object) {
                return in_object->GetObjectId() == object.GetObjectId();
            }));

        if (std::end(cachedPool) != it)
        {
            m_numberOfObjects--;
            cachedPool.erase(it);
        }
    }
    else
    {
        STATIC_ASSERT("Null ptr")
    }
}

void MineManager::RemoveById(const int in_objectID)
{
    for (auto& cachedMap : m_mObject)
    {
        const auto& it(std::find_if(cachedMap.second.begin(), cachedMap.second.end(), [&](const Mine& object) {
                return in_objectID == object.GetObjectId();
            }));

        if (std::end(cachedMap.second) != it)
        {
            cachedMap.second.erase(it);
            m_numberOfObjects--;
            break;
        }
    }
}

void MineManager::RemoveByIndex(const int in_index)
{
    if (in_index < m_numberOfObjects)
    {
        const auto indexData = GetObjectIndex(in_index);

        auto& cachedMap(m_mObject[indexData.m_poolID]);

        if (indexData.m_actualIndex < static_cast<int>(cachedMap.size()))
        {
            m_numberOfObjects--;
            cachedMap.erase(cachedMap.begin() + indexData.m_actualIndex);
        }
    }
}

Mine* MineManager::GetObjectByID(const int in_objectID)
{
    Mine* out_result = NULL;

    for (auto& cachedMap : m_mObject)
    {
        const auto& it = std::find_if(cachedMap.second.begin(), cachedMap.second.end(), [&](const Mine& object) {
                return in_objectID == object.GetObjectId();
            });

        if (std::end(cachedMap.second) != it)
            out_result = &(*it);
    }

    return out_result;
}

Mine* MineManager::GetObjectByIndex(const int in_Index)
{
    Mine* out_result = NULL;

    if (in_Index < m_numberOfObjects)
    {
        const auto indexData = GetObjectIndex(in_Index);

        auto& cachedMap(m_mObject[indexData.m_poolID]);

        if (indexData.m_actualIndex < static_cast<int>(cachedMap.size()))
        {
            out_result = &cachedMap[indexData.m_actualIndex];
        }
    }
    else
    {
        STATIC_ASSERT("Null ptr")
    }

    return out_result;
}

void MineManager::Dispose(void)
{
    for (auto& keyVal : m_mObject)
    {
        keyVal.second.clear();
    }

    m_mObject.clear();
}