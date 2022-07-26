#pragma once

#include "Object.h"
#include "Random.h"

#include <vector>

class Mine : public Object
{
public:
    Mine();
    ~Mine();

    float* GetPosition() { return m_position; }
    void SetPosition(float* aPosition);

    float GetDistance(float aPositionA[3], float aPositionB[3]);
    void FindCurrentTargets();
    int GetNumberOfEnemyTargets();
    int GetTeam() { return m_team; }
    void Explode();
    void TakeDamage(float aDamage);

    float* m_position;
    float m_destructiveRadius;
    float m_health;
    float m_explosiveYield;

    std::vector<Object*> m_targetList;
};
