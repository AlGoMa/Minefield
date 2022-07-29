#include "stdafx.h"
#include "Object.h"

Object::Object(const int aID, const int aPoolID) : 
    m_poolID(aPoolID), m_objectId(aID), m_position({ 0.0f, 0.0f, 0.0f }) 
{
}

Object::Object(void) : 
    m_poolID(0), m_objectId(static_cast<unsigned int>(typeid(Object).hash_code())), m_position({ 0.0f, 0.0f, 0.0f })
{
}

bool Object::Equals(const Object& in_toCompare)
{
    return in_toCompare.GetObjectId() == m_objectId;
}