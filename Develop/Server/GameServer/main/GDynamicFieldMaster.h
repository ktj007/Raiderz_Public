#ifndef _GDYNAMIC_FIELD_GROUP_H_
#define _GDYNAMIC_FIELD_GROUP_H_

#include "GFieldInfo.h"
#include "SDef.h"
#include "MMemPool.h"

class GDynamicFieldPolicy;
class GDynamicFieldPolicy_TrialField;
class GDynamicFieldPolicy_InnRoom;
class GDynamicFieldPolicy_BattleArena;
class GFieldMgr;
class GDynamicField;
class GEntityPlayer;

typedef map<int, GDynamicField*> MAP_DYNAMICFIELD;

class GDynamicFieldMaster
{
	class SelfDestructor : public MTestMemPool<SelfDestructor>
	{
	private:
		GDynamicFieldMaster* m_pOwner;
		bool m_bReadyToDestroy;
	private:
		void ReadyToDestroy();
		void Destory();
	public:
		SelfDestructor(GDynamicFieldMaster* pOwner);
		void Update();
		
	};
private:
	friend SelfDestructor;
	friend GEntityPlayer;
	friend GFieldMgr;
	friend GDynamicFieldPolicy;
	friend GDynamicFieldPolicy_TrialField;
	friend GDynamicFieldPolicy_InnRoom;
	friend GDynamicFieldPolicy_BattleArena;

private:
	MUID				m_UID;
	MUID				m_uidCreator;
	MAP_DYNAMICFIELD	m_mapFieldByID;
	DYNAMIC_FIELD_TYPE	m_eType;
	FIELD_GROUP_INFO*	m_pInfo;
	set<MUID>			m_setEntryPlayer;
	bool				m_bDeleteMe;
	SelfDestructor*		m_pSelfDestructor;

private:
	bool				KickAllPlayers();
	void				InsertPlayer(GEntityPlayer* pPlayer);
	void				ErasePlayer(GEntityPlayer* pPlayer);
	bool				IsEnteredPlayer(const MUID& uidPlayer);
	bool				IsPlayerExistInField(MUID uidField);
	bool				OnEnterPlayer(GEntityPlayer* pPlayer);
	void				OnLeavePlayer(GEntityPlayer* pPlayer);	
	void				RunSelfDestructor();

protected:
	GDynamicFieldPolicy* m_pPolicy;	// 처리 정책
	

public:
	GDynamicFieldMaster(const MUID& uidCreator, const DYNAMIC_FIELD_TYPE eType);
	virtual ~GDynamicFieldMaster();

	void				Create(MUID uidNew, FIELD_GROUP_INFO* pInfo);
	void				Insert(GDynamicField* pField);
	void				Destroy();
	void				DeleteMe();
		
	GDynamicField*		Get(int nFieldID);
	MAP_DYNAMICFIELD&	GetFieldContainer()				{ return m_mapFieldByID; }
	int					GetQty()						{ return (int)m_mapFieldByID.size(); }
	int					GetPlayerQty();
	MUID				GetUID()						{ return m_UID; }
	FIELD_GROUP_INFO*	GetInfo()						{ return m_pInfo; }
	DYNAMIC_FIELD_TYPE	GetType()						{ return m_eType; }
	int					GetGroupID()					{ if (m_pInfo) return m_pInfo->nID;		return -1;}
	
	FIELD_ENTER_FAIL_TYPE				IsEnterable();
	// 해당 필드에서 전투 중인지 여부
	bool				IsNowCombat() const; 
	
	bool				HasPreservedEnterPlayers();

	void				Update(float fDelta);

	void				SetPolicy(GDynamicFieldPolicy* pPolicy);
	GDynamicFieldPolicy* GetPolicy(void)	{ return m_pPolicy;	}

	bool IsChallengerQuestField();
};

class GDynamicFieldMasterMLeaf : public GDynamicFieldMaster, public MMemPool<GDynamicFieldMasterMLeaf>
{
public:
	GDynamicFieldMasterMLeaf(const MUID& uidCreator, const DYNAMIC_FIELD_TYPE eType)
		: GDynamicFieldMaster(uidCreator, eType) {}
	virtual ~GDynamicFieldMasterMLeaf() {}
};

#endif//_GDYNAMIC_FIELD_GROUP_H_
