#ifndef _MOCK_PLAYER_OBJECT_H_
#define _MOCK_PLAYER_OBJECT_H_

#include "GPlayerObject.h"
#include "MockEntityPlayer.h"

// 내부 Entity가 MockEntityPlayer인 MockPlayerObject
class MockPlayerObject : public GPlayerObject
{
private:
	MockEntityPlayer* m_pMockEntityPlayer;
public:
	MockPlayerObject(MUID& uid):GPlayerObject(uid), m_pMockEntityPlayer(NULL)  { }
	virtual ~MockPlayerObject()	{}
	virtual GEntityPlayer* GetEntity() 
	{ 
		if (m_pMockEntityPlayer) return m_pMockEntityPlayer;
		return m_pMyEntity;
	}
	MockEntityPlayer* GetMockEntity() { return m_pMockEntityPlayer; }
	virtual void Create()					
	{ 
		m_pMockEntityPlayer = new MockEntityPlayer();
		m_pMockEntityPlayer->Create(m_UID);
		m_pMyEntity = m_pMockEntityPlayer;
		m_bCreated = true;
	}
	void Create(GEntityPlayer* pMyEntity)
	{
		m_pMyEntity = pMyEntity;
		m_bCreated = true;
	}
	virtual void Destroy()
	{
		GPlayerObject::Destroy();
		m_pMockEntityPlayer = NULL;
	}
};

#endif //_MOCK_PLAYER_OBJECT_H_
