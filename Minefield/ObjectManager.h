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

    /// <summary>
    /// Initializes size of collection based on inputs
    /// </summary>
    /// <param name="aPools">int. Number of pools</param>
    /// <param name="aObjectPerPool">int. Number of elements per pool</param>
    virtual void    Init(const int aPools, const int aObjectPerPool);
    /// <summary>
    /// Cleans up collection
    /// </summary>
    virtual void    Dispose(void) {};
    /// <summary>
    /// Adds new element to collection
    /// </summary>
    /// <param name="apObject">const TClass*. Element to be inserted</param>
    virtual void    AddObject(const TClass* apObject) = 0;
    /// <summary>
    /// Add new element to collection
    /// </summary>
    /// <param name="objectId">int. Object ID</param>
    /// <param name="poolID">int. Pool ID where object will be pushed</param>
    virtual void    AddObject(const int objectId, const int poolID) = 0;
    /// <summary>
    /// Removes element from collection by raw pointer.
    /// </summary>
    /// <param name="apObject">const TClass*. Object to be deleted</param>
    virtual void    RemoveObject(const TClass* apObject) = 0;
    /// <summary>
    /// Removes element from collection by ID
    /// </summary>
    /// <param name="aObjectID">int. Object ID</param>
    virtual void    RemoveById(const int aObjectID) = 0;
    /// <summary>
    /// Removed element from collection by Index
    /// </summary>
    /// <param name="aIndex">int. Absolut index</param>
    virtual void    RemoveByIndex(const int aIndex) = 0;
    /// <summary>
    /// Returns specific element by ID
    /// </summary>
    /// <param name="aObjectID">int. ID to look for</param>
    /// <returns>TClass*. Object raw pointer, can be NULL</returns>
    virtual TClass* GetObjectByID(const int aObjectID) = 0;
    /// <summary>
    /// Returns specific element by Index
    /// </summary>
    /// <param name="aObjectID">int. Index to look for</param>
    /// <returns>TClass*. Object raw pointer, can be NULL</returns>
    virtual TClass* GetObjectByIndex(const int aIndex) = 0;

    /// <summary>
    /// Returns number of element constructed.
    /// </summary>
    /// <returns>int. Number of existing objects</returns>
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