//
// ZOS Software Engineer Applicant Test 2.4.0
//
// Submitted by: Alonso González Martínez
//
// Date: 29/07/2022
//
// Time taken: 24hrs
//
// Notes:
// Mines.h & Mines.cpp 
// I reworked class entirely adding more setters and getters, renaming as well member called Team by a Pool ID to match with my design. I'll elaborate further more about that.
// I removed bitflags member from Object to place it into Mines since it made me more sense mine monitored it itself since, from my point of view, it had nothing to with Object.
// I added two more states(Self destroy and invalidated) to a specific porpuse. One of them, since I'm using stack memory to contruct mines, insted of raw pointers, and I needed a proper
// way to determine mines should no be consider as target, once invalidated, by another mine during explotion stage.
// I removed distance function since a new Vector3 struct handles that operations. During targeting pass, distance is calculated using squared length to avoid square roots improving performance.
// During explotion pass, there mines will be update in different ways. If it's a mine that's exploting, next state will be SELFDESTROYED and eventually INVALIDATED. If it's a mine taking 
// damage, it has one more loop to explode itself and makes damange before new state is assgined (SELFDESTROYED and INVALIDATED).
// 
// Object.h & Object.cpp
// I created a new struct called Vector3 to handle position operations in a handy way. Hence, Position type is a Vector3 instead of float*. I made small changes here adding some comments, 
// setters and getters. One remark, m_poolID is used for different purposes. The first, to identify the pool object was stored and, as a Mine object, to set up the team it belongs.
// 
// ObjectManager.h
// I made this class a abstract template class having the flexibility to jump, if necessary, between different object types. In this case, I did that in order to work with Mine class objects 
// primarily and avoid casting. It only has two de defined functions and the rest are pure virtual. One to get a object index and a getter to retrieve the exact number of objects spawned. Why to get Indexes? Im using a unordered_map to store objects by pools to speed up searching and deleting.
// Deleted:
// ObjectManager(const ObjectManager&) = delete;
// ObjectManager& operator=(const ObjectManager&) = delete;
// 
// To avoid singleton copies.
// 
// 
// MineManger.h & MineManager.cpp
// Child class where base class is object. Here is where mine management occurs
// Search element:
// There are two options to search object (by ID - object ID and Index). Searching by ID iterates over the map until it finds it within a pool. Narrows down search subtantially since instead of looking in a huge set of elements, does it by pool.
// Searching by Index is faster since based on provided index (absolute position), determines the pool ID and relative index.
// Add element:
// Im using stack memory instead of heap for safety and reduce memory overhead since it doesn't generate copies when is pushed into the array list. Plus, containers size is set before insertion.
// Delete element:
// Uses same logic to search element and then erase it from pool.
// 
// In general, I added multiple protections when pointers are used, indexes out of range and unnecesary loops. Also, I decided to use algorithm STL since, in my opinion, they're more safer iterating over collections, it never affects the size or storage allocation of them, it's more readable and prevents access violations.
// 
// I defined a new command whose purpose is to change how ID are generated either using already defined method or using hash class which takes as input (mineIndex * (teamIndex + 1)). The latter, avoid duplicated ID; ergo, could affect time since, potentially, would be more mines.
// 
#include "stdafx.h"
#ifdef _WIN32
#include "Windows.h"
#include <process.h>
#endif
#include "MineManager.h"
#include "Mine.h"
#ifdef __linux
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "Minefield.h"
#endif

int g_numberOfTeams = 5;
int g_numberOfMinesPerTeam = 1500;
bool g_useHashIDs = false;

#ifdef _WIN32
class QueryPerformanceTimer
{
public:
    QueryPerformanceTimer()
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);

        m_inverseFrequency = 1000000.0 / (double)frequency.QuadPart;
    }

    void Start()
    {
        QueryPerformanceCounter(&m_start);
    }

    double Get()
    {
        QueryPerformanceCounter(&m_stop);

        double time = (double)(m_stop.QuadPart - m_start.QuadPart) * m_inverseFrequency;

        m_start = m_stop;

        // time value is in micro seconds
        return time;
    }

    LARGE_INTEGER m_start;
    LARGE_INTEGER m_stop;
    double m_inverseFrequency;
};
#endif

#ifdef __linux
class QueryPerformanceTimer
{
public:
    QueryPerformanceTimer()
    {

    }

    void Start()
    {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        m_start = now.tv_sec + now.tv_nsec / 1000000000.0;
    }

    double Get()
    {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        m_stop = now.tv_sec + now.tv_nsec / 1000000000.0;

        double time = m_stop - m_start;

        m_start = m_stop;

        // time value is in micro seconds
        return time;
    }

    double m_start;
    double m_stop;
};
#endif

class ScopedQueryPerformanceTimer
{
public:
    ScopedQueryPerformanceTimer(const char* aMsg = NULL)
    {
        m_msg = aMsg;
        m_timer.Start();
    }

    ~ScopedQueryPerformanceTimer()
    {
        double timeUsed = m_timer.Get();
        printf("%s %f\n", m_msg, timeUsed / 1000.0);
    }

    QueryPerformanceTimer m_timer;
    const char* m_msg;
};

static int s_numberOfWorkerThreadsActive = 0;
static int s_numberOfWorkerThreadsStarted = 0;
static int s_currentMineIndex = 0;
static Mutex s_lock;

namespace
{
    const int NextIndex(void) {
        MutexLock lock(s_lock);

        int index = s_currentMineIndex;

        s_currentMineIndex++;

        return index;
    }

    void FindTargets(void* aIgnored)
    {
        {
            MutexLock lock(s_lock);
            s_numberOfWorkerThreadsActive++;
            s_numberOfWorkerThreadsStarted++;
        }
        bool done = false;
        while (!done)
        {
            int index = NextIndex();

            if (index < MineManager::GetInstance().GetNumberOfObjects())
            {
                Mine* pMineObject = MineManager::GetInstance().GetObjectByIndex(index);

                if (NULL != pMineObject)
                {
                    pMineObject->FindCurrentTargets();
                }
            }
            else
            {
                done = true;
            }
        }
        {
            MutexLock lock(s_lock);
            s_numberOfWorkerThreadsActive--;
        }
    }
}

class WorkerThread
{
public:

    WorkerThread()
    {
    }

    ~WorkerThread()
    {
    }

    void FindTargetsForAllMines()
    {
#ifdef __linux
        pthread_t threadId = 0;

        pthread_attr_t attributes;
        pthread_attr_init(&attributes);

        pthread_create(&threadId, &attributes, (void* (*)(void*))FindTargets, NULL);
#elif _WIN32
        _beginthread(FindTargets, 0, NULL);
#endif
    }
};

int main(int aArgc, char* aArgv[])
{
    int numberOfWorkerThreads = 12;
    int randomSeed = 654321;
    if (aArgc > 1)
    {
        randomSeed = atoi(aArgv[1]);
        SetRandomSeed(randomSeed);
    }
    if (aArgc > 2)
    {
        numberOfWorkerThreads = atoi(aArgv[2]);
    }
    if (aArgc > 3)
    {
        g_numberOfTeams = atoi(aArgv[3]);
    }
    if (aArgc > 4)
    {
        g_numberOfMinesPerTeam = atoi(aArgv[4]);
    }
    if (aArgc > 5)
    {
        g_useHashIDs = atoi(aArgv[5]) > 0;
    }

    printf("Random seed: %d\n", randomSeed);
    printf("Number of worker threads: %d\n", numberOfWorkerThreads);
    printf("Number of teams: %d  \n", g_numberOfTeams);
    printf("Number of mines per team: %d\n", g_numberOfMinesPerTeam);

    {
        ScopedQueryPerformanceTimer timer("Time taken in milliseconds:");

        MineManager::GetInstance().Init(g_numberOfTeams, g_numberOfMinesPerTeam);

        // Let's add lots of mine objects to the system before starting things up
        for (int i = 0; i < g_numberOfTeams; i++)
        {
            for (int j = 0; j < g_numberOfMinesPerTeam; j++)
            {
                Vector3 position{ GetRandomFloat32_Range(-1000.0f, 1000.0f),
                                   GetRandomFloat32_Range(-1000.0f, 1000.0f),
                                   GetRandomFloat32_Range(-1000.0f, 1000.0f) };

                unsigned int objectId(g_useHashIDs ?
                    static_cast<unsigned int>(std::hash<unsigned int>()(j * (i + 1))) :  GetRandomUInt32() % (g_numberOfMinesPerTeam * 10));

                const Mine* cachedMine(MineManager::GetInstance().AddMineObject(objectId, position, i));

                printf("Object id %d position (%0.3f, %0.3f, %0.3f) active %s invulnerable %s\n", cachedMine->GetObjectId(),
                    cachedMine->GetPosition().x, cachedMine->GetPosition().y, cachedMine->GetPosition().z, cachedMine->IsActive() ? "Y" : "N", cachedMine->IsInvulnerable() ? "Y" : "N");
            }
        }

        printf("Number of objects in system %u\n", MineManager::GetInstance().GetNumberOfObjects());

        std::vector<WorkerThread> workerThreadList;
        workerThreadList.reserve(numberOfWorkerThreads);

        for (int i = 0; i < numberOfWorkerThreads; i++)
        {
            workerThreadList.emplace_back();
        }

        int numberOfTurns = 0;
        bool targetsStillFound = true;

        while (targetsStillFound)
        {
            numberOfTurns++;
            targetsStillFound = false;
            s_numberOfWorkerThreadsStarted = 0;
            s_currentMineIndex = 0;

            for (int i = 0; i < numberOfWorkerThreads; i++)
            {
                workerThreadList[i].FindTargetsForAllMines();
            }

            do
            {
                // sleep until all worker threads have finished doing their thing
#ifdef __linux
                usleep(1000);
#elif _WIN32
                Sleep(1);
#endif
            } while (s_numberOfWorkerThreadsActive > 0 || s_numberOfWorkerThreadsStarted == 0);

            for (int i = 0; i < g_numberOfTeams; i++)
            {
                Mine* pMine = MineManager::GetInstance().GetObjectWithMostEnemyTargets(i);

                int enemyTargets = NULL != pMine ? pMine->GetNumberOfTargets() : 0;

                if (0 < enemyTargets)
                {
                    pMine->Explode();

                    targetsStillFound = true;

                    if (5 > numberOfTurns)
                    {
                        printf("Turn %d: Team %d picks Mine with object id %d (with %d targets) to explode\n", numberOfTurns, i,
                            pMine->GetObjectId(), enemyTargets);
                    }
                }
            }
        }

        int winningTeam = 0;
        int winningObjectCount = 0;
        for (int i = 0; i < g_numberOfTeams; i++)
        {
            int noOfTargets = MineManager::GetInstance().GetNumberOfObjectForTeam(i);

            printf("Team %d has %d mines remaining\n", i, noOfTargets);

            if (noOfTargets > winningObjectCount)
            {
                winningObjectCount = noOfTargets;
                winningTeam = i;
            }
        }

        printf("Team %d WINS after %d turns!!\n", winningTeam, numberOfTurns);

        workerThreadList.clear();

        MineManager::GetInstance().Dispose();
    }

#ifdef __linux
    usleep(-1);
#elif _WIN32
    Sleep(-1);
#endif

    return 0;
}

