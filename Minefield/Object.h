#pragma once
#include "Utilities.h"
#include <typeinfo>

struct Vector3
{
    float x, y, z;

    Vector3(float in_x, float in_y, float in_z) : x(in_x), y(in_y), z(in_z) {}
    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

    /// <summary>
    /// Subtraction operator overrided. 
    /// </summary>
    /// <param name="in_vector"></param>
    /// <returns></returns>
    Vector3 operator-(const Vector3& in_vector) const { return Vector3(x - in_vector.x, y - in_vector.y, z - in_vector.z); }
    /// <summary>
    /// Vector magnitude.
    /// </summary>
    /// <returns>float. Length of vector</returns>
    float Magnitude(void) const { return static_cast<float>(rsqrt(Dot(*this, *this))); }
    /// <summary>
    /// Squared vector magnitude.
    /// </summary>
    /// <returns>float. Squared length of this vector</returns>
    float SqrMagnitude(void) const { return Dot(*this, *this); }

#pragma region Static Functions
    /// <summary>
    /// Dot product. Multiplies sum of vector elements products.
    /// </summary>
    /// <param name="in_vectorR">Vector3. Right vector3</param>
    /// <param name="in_vectorL">Vector3. Left vector3</param>
    /// <returns>float. Dot product result.</returns>
    static float Dot(const Vector3& in_vectorR, const Vector3& in_vectorL)
    {
        return in_vectorL.x * in_vectorR.x + in_vectorL.y * in_vectorR.y + in_vectorL.z * in_vectorR.z;
    }
    /// <summary>
    /// Squared Distance between two vectors.
    /// </summary>
    /// <param name="in_vectorA">Vector3. Origin vector</param>
    /// <param name="in_vectorB">Vector3. Destiny vector</param>
    /// <returns>float. Distance</returns>
    static float SqrDistance(const Vector3& in_vectorA, const Vector3& in_vectorB)
    {
        return (in_vectorA - in_vectorB).SqrMagnitude();
    }
    /// <summary>
    /// Square root Distance between two vectors.
    /// </summary>
    /// <param name="in_vectorA">Vector3. Origin vector</param>
    /// <param name="in_vectorB">Vector3. Destiny vector</param>
    /// <returns>float. Distance</returns>
    static float Distance(const Vector3& in_vectorA, const Vector3& in_vectorB)
    {
        return (in_vectorA - in_vectorB).Magnitude();
    }
#pragma endregion
};

class Object
{
public:
    Object(const int aID, const int aPoolID);
    Object(void);
    virtual ~Object(void) {}

    /* Setters */
    /// <summary>
    /// Sets pool ID object belongs to
    /// </summary>
    /// <param name="aPoolID">const int. Pool ID</param>
    void SetObjectPoolID(const int aPoolID) { m_poolID = aPoolID; }
    /// <summary>
    /// Sets object ID
    /// </summary>
    /// <param name="aID">const int. New ID</param>
    void SetObjectID(const unsigned int aID) { m_objectId = aID; }
    /* Getters */
    /// <summary>
    /// Returns Object ID.
    /// </summary>
    /// <returns>int. Unique ID</returns>
    inline const int& GetObjectPoolID(void) const { return m_poolID; }
    /// <summary>
    /// Returns Object ID.
    /// </summary>
    /// <returns>int. Unique ID</returns>
    inline const unsigned int& GetObjectId(void) const { return m_objectId; }
    /// <summary>
    /// Returns mine position.
    /// </summary>
    /// <returns>Vector3. Mine vector3 pos</returns>
    inline virtual const Vector3& GetPosition(void) const { return m_position; }
    /// <summary>
    /// Virtual function. Sets position
    /// </summary>
    /// <param name="aPosition">Vector3. New vector3 position</param>
    inline virtual void           SetPosition(const Vector3 aPosition) { m_position = aPosition; }
    /// <summary>
    /// Compares ID againts other object ID.
    /// </summary>
    /// <param name="in_toCompare">Object&. Object reference</param>
    /// <returns>bool. True if they are equal, fasel otherwise</returns>
    virtual bool Equals(const Object& atoCompare);

private:
    int m_poolID;
    unsigned int m_objectId;
    Vector3 m_position;
};