#pragma once

#include "Object.h"
#include "Random.h"
#include <vector>

class Mine : public Object
{
public:
    Mine(const int aMineID, const int aPoolID);
    ~Mine(void);

    /* Internal enums*/
    enum ObjectBitFlags : unsigned char
    {
        OBF_ACTIVE = 0x0001,
        OBF_INVULNERABLE = 0x0002,
        OBF_SELFDESTROYED = 0x0004,
        OBF_INVALIDATED = 0x0008
    };

    /// <summary>
    /// Reduce health by specific amount.
    /// </summary>
    /// <param name="aDamage"></param>
    void  TakeDamage(const float aDamage);
    /// <summary>
    /// Collects targets.
    /// </summary>
    void  FindCurrentTargets(void);
    /// <summary>
    /// Performes explotion if applicable.
    /// </summary>
    void  Explode(void);

    /* Setters */
    /// <summary>
    /// Sets mine active or inactive.
    /// </summary>
    /// <param name="aActive">bool. If mine would be active or inactive</param>
    void SetActive(bool aActive) { m_bitFlags = (aActive ? m_bitFlags | OBF_ACTIVE : m_bitFlags & ~OBF_ACTIVE); }
    /// <summary>
    /// Sets vulnerability flag.
    /// </summary>
    /// <param name="aVulnerabilty">bool. If it would be vunerable or not</param>
    void SetVunerabilty(bool aVulnerabilty) { m_bitFlags = (aVulnerabilty ? m_bitFlags | OBF_INVULNERABLE : m_bitFlags & ~OBF_INVULNERABLE); }
    /// <summary>
    /// Sets Team, using Pool ID as equivalence.
    /// </summary>
    /// <param name="aTeam">int. Team ID</param>
    void SetTeam(const int aTeam) { SetObjectPoolID(aTeam); }
    /// <summary>
    /// Sets health value.
    /// </summary>
    /// <param name="in_health">float. New health</param>
    inline void SetHealth(const float aHealth) { m_health = aHealth; }
    /// <summary>
    /// Sets mine destructive radius.
    /// </summary>
    /// <param name="in_radius">float. Radius</param>
    inline void SetDestructiveRadius(const float aRadius) { m_destructiveRadius = aRadius; };
    /// <summary>
    /// Invalidates mine
    /// </summary>
    inline void SetInvalid(void) { m_bitFlags = OBF_INVALIDATED; }
    /// <summary>
    /// Set mine as destroyed (after self destroy)
    /// </summary>
    inline void SetSelfDestroy(void) { m_bitFlags |= OBF_SELFDESTROYED; }
    /* Getters */
    /// <summary>
    /// Returns Mine team ID
    /// </summary>
    /// <returns>int. Team ID</returns>
    inline const int GetTeam(void) const { return GetObjectPoolID(); }
    /// <summary>
    /// Returns current object vulnerability state.
    /// </summary>
    /// <returns>bool. If mines is vunerable or not</returns>
    inline bool IsInvulnerable(void) const { return (m_bitFlags & OBF_INVULNERABLE) == OBF_INVULNERABLE; }
    /// <summary>
    /// Returns object primarialy state
    /// </summary>
    /// <returns>bool. If mine is active or not</returns>
    inline bool IsActive(void) const { return m_bitFlags & OBF_ACTIVE; }
    /// <summary>
    /// If mine is already destroyed.
    /// </summary>
    /// <returns>bool. If mine was destroyed or not</returns>
    inline bool IsDestroyed(void) const { return (m_bitFlags & OBF_SELFDESTROYED) == OBF_SELFDESTROYED; }
    /// <summary>
    /// If mine is already destroyed.
    /// </summary>
    /// <returns>bool. If mine was destroyed or not</returns>
    inline bool IsInvalid(void) const { return (m_bitFlags & OBF_INVALIDATED) == OBF_INVALIDATED; }
    /// <summary>
    /// Returns Mine targest.
    /// </summary>
    /// <returns>int. Number of targets</returns>
    inline int GetNumberOfTargets(void) const { return static_cast<int>(m_targetList.size()); }

private:
    float m_destructiveRadius;
    float m_health;
    float m_explosiveYield;
    unsigned char m_bitFlags;
    std::vector<Mine*> m_targetList;
};
