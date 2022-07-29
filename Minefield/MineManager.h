#pragma once
#include "ObjectManager.h"

class Mine;
struct Vector3;

class MineManager :
    public ObjectManager<Mine>
{
public:
    const Mine* AddMineObject(const unsigned int aObjectId, const Vector3 aPosition, const int aTeam);
    int         GetNumberOfObjectForTeam(int aTeam);
    Mine*       GetObjectWithMostEnemyTargets(const int aTeam);
    
    static MineManager& GetInstance(void) {
        static MineManager instance;
        return instance;
    }

    /* Overrided functions */
    virtual void  AddObject(const Mine* apObject) override;
    virtual void  AddObject(const int objectId, const int poolID) override;
    virtual void  RemoveObject(const Mine* apObject) override;
    virtual void  RemoveById(const int aObjectID) override;
    virtual void  RemoveByIndex(const int aIndex) override;
    virtual Mine* GetObjectByID(const int aObjectID) override;
    virtual Mine* GetObjectByIndex(const int aIndex) override;
    virtual void  Dispose(void) override;

protected:
    MineManager(void);
    ~MineManager(void);
};

