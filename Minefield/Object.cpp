#include "stdafx.h"

#include "Object.h"


bool Object::GetActive()
{
    return m_bitFlags & OBF_ACTIVE;
}

void Object::SetActive(bool aActive)
{
    if(aActive)
    {
        m_bitFlags &= OBF_ACTIVE;
    }
    else
    {
        m_bitFlags &= ~OBF_ACTIVE;
    }
}

bool Object::GetInvulnerable()
{
    return (m_bitFlags & OBF_INVULNERABLE) ? true : false;
}
