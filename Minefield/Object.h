#pragma once


class Object
{
public:
    Object() { }

    ~Object() {}

    int GetObjectId() { return m_objectId; }

    bool GetActive();
    void SetActive(bool aActive);

    bool GetInvulnerable();
    void SetInvulnerable(bool aInvulnerable);

    virtual float* GetPosition() { return 0; }
    virtual void SetPosition(float*) {}

    unsigned int m_objectId;
    int m_team;

    enum ObjectBitFlags
    {
        OBF_ACTIVE = 0x0000,
        OBF_INVULNERABLE = 0x0002,
    };

    int m_bitFlags;
};