#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#pragma once

#include "Mutex.h"
#include <unordered_map>
#include <vector>
#include <memory>

const int cMaximumNumberOfObjects = 1000000;

template<typename TClass>
class ObjectManager
{
public:
    // To avoid extra copies.
    ObjectManager(const ObjectManager&) = delete;
    ObjectManager& operator=(const ObjectManager&) = delete;

    virtual void    Init(const int aPools, const int aObjectPerPool);
    virtual void    Dispose(void) {};
    virtual void    AddObject(const TClass* apObject) = 0;
    virtual void    AddObject(const int objectId, const int poolID) = 0;
    virtual void    RemoveObject(const TClass* apObject) = 0;
    virtual void    RemoveById(const int aObjectID) = 0;
    virtual void    RemoveByIndex(const int aIndex) = 0;
    virtual TClass* GetObjectByID(const int aObjectID) = 0;
    virtual TClass* GetObjectByIndex(const int aIndex) = 0;

    inline const int GetNumberOfObjects(void) const { return m_numberOfObjects; }

protected:
    ObjectManager(void);
    virtual ~ObjectManager(void);

    struct ObjectIndexData {
        int m_poolID = 0;
        int m_actualIndex = 0;
    };

    /// <summary>
    /// Returns object relative index based on absolute index.
    /// </summary>
    /// <param name="in_absIndex">int. Absolute index</param>
    /// <returns>ObjectIndexData. Object relative position data</returns>
    const ObjectIndexData GetObjectIndex(const int aAbsIndex)
    {
        int poolID((int)((aAbsIndex / (float)(m_objectPerPool * m_numberOfPools)) * m_numberOfPools));

        return { poolID, aAbsIndex - (m_objectPerPool * poolID) };
    }

    Mutex m_lock;
    std::unordered_map<int, std::vector<TClass>> m_mObject;
    int m_numberOfObjects;
    int m_numberOfPools;
    int m_objectPerPool;
};

template<typename TClass>
ObjectManager<TClass>::ObjectManager()
    : m_numberOfObjects(0)
    , m_numberOfPools(0)
    , m_objectPerPool(0)
{
}

template<typename TClass>
ObjectManager<TClass>::~ObjectManager()
{
    Dispose();
}

template<typename TClass>
void ObjectManager<TClass>::Init(const int in_pools, const int in_objectPerPool)
{
    m_numberOfPools = in_pools;
    m_objectPerPool = in_objectPerPool;

    for (int i = 0; i < in_pools; i++)
    {
        /* Setting pool id. This step is required in order to reduce allocation overhead*/
        m_mObject[i].reserve(in_objectPerPool);
    }
}

//template<typename TClass>
//void ObjectManager<TClass>::Dispose(void)
//{
//    m_mObject.clear();
//}

#endif // OBJECTMANAGER_H