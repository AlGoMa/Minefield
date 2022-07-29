#include "stdafx.h"

#ifdef __linux
#include <math.h>
#endif
#include "Mine.h"
#include "MineManager.h"

Mine::Mine(const int aMineID, const int aPoolID) : 
    m_destructiveRadius(0.0f)
  , m_health(100.0f)
  , m_explosiveYield(500)
  , m_bitFlags(0)
  , Object(aMineID, aPoolID) 
{
}

Mine::~Mine()
{
}

// Invulnerable mines do not take damage, but can be manually exploded if they are active
void Mine::FindCurrentTargets()
{
    if (IsActive())
    {
        m_targetList.clear();

        for (int i = 0; i < MineManager::GetInstance().GetNumberOfObjects(); ++i)
        {
            Mine* pObject = MineManager::GetInstance().GetObjectByIndex(i);

            if (NULL != pObject && !Equals(*pObject))
            {
                float distance = Vector3::SqrDistance(pObject->GetPosition(), GetPosition());
                if (pObject->IsInvulnerable() || distance > (m_destructiveRadius * m_destructiveRadius))
                {
                    break;
                }

                /* Dismiss allied mines when, throwing a coin into the air, it gets the desired value. 
                In other words, if we get equal or less than 5%, allied mine will be save for at least a turn*/
                if(pObject->GetTeam() == GetTeam() && GetRandomFloat32() <= 0.05f)
                {
                    break;
                }

                m_targetList.push_back(pObject);
            }
        }
    }
}

void Mine::Explode()
{
    if (!IsDestroyed())
    {
        for (unsigned int i = 0; i < m_targetList.size(); ++i)
        {
            Mine* cachedMine(m_targetList[i]);

            if (NULL != cachedMine && !cachedMine->IsInvalid())
            {
                float distance = Vector3::SqrDistance(cachedMine->GetPosition(), GetPosition());

                // damage is inverse-squared of distance
                float factor = 1.0f - (distance / (m_destructiveRadius * m_destructiveRadius));
                float damage = (factor * factor) * m_explosiveYield;
                cachedMine->TakeDamage(damage);
            }
        }

        SetSelfDestroy();

        // Destroy self
        if (m_health > 0)
        {
           TakeDamage(m_health);
        }
    }
}

void Mine::TakeDamage(const float aDamage)
{
    m_health -= aDamage;

    if (m_health <= 0.0f)
    {
        Explode();
        
        MineManager::GetInstance().RemoveObject(this);
        
        SetInvalid();
    }
}
