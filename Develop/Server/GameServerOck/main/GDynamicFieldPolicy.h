#ifndef _G_DYNAMIC_GROUP_POLICY_H_
#define _G_DYNAMIC_GROUP_POLICY_H_

//#include "GDynamicFieldObserver.h"

class GDynamicFieldMaster;
class GEntityPlayer;

enum DYNAMICFIELD_POLICY_TYPE
{
	DPT_NONE = -1,
	DPT_TRIAL = 0,
	DPT_INN = 1,
	DPT_BATTLEARENA = 2,
};

class GDynamicFieldPolicy//: public GDynamicFieldObserver
{
protected:
	GDynamicFieldMaster* m_pOwner;
	bool m_isVirginField;			// 입장 전에 파괴되는 것을 막기위한 플래그

private:
	bool CheckExistPlayer();

protected:
	virtual void OnCreate() { }
	virtual void OnDestroy() { }

	virtual void OnUpdate(float fDelta) {}
	virtual bool OnEnterPlayer(GEntityPlayer* pPlayer) { return true; }
	virtual void OnLeavePlayer(GEntityPlayer* pPlayer) { }	

public:
	GDynamicFieldPolicy(GDynamicFieldMaster* pOwner);
	virtual ~GDynamicFieldPolicy();

	virtual DYNAMICFIELD_POLICY_TYPE GetType() { return DPT_NONE; }

	void Create();
	void Destroy();

	virtual FIELD_ENTER_FAIL_TYPE IsEnterable();

	void Update(float fDelta);
	bool EnterPlayer(GEntityPlayer* pPlayer);
	void LeavePlayer(GEntityPlayer* pPlayer);

	// GDynamicFieldObserver ===================
	virtual void OnQuestSynch(MUID uidPlayer, MUID uidField, const vector<int>* pVecQuestID) {}
	virtual void OnQuestComplete(GEntityPlayer* pPlayer, int nQuestId) {}
	virtual void OnQuestDrop(GEntityPlayer* pPlayer, int nQuestId) {}
	virtual void OnQuestEraseRunner(MUID uidPlayer) {}
	virtual bool IsExpiredCounting(void) {return false;	}
	// GDynamicFieldObserver ===================

	virtual void ChangePlayerUID(MUID uidOlePlayer, MUID uidNewPlayer)	{}
};

class GDynamicFieldPolicyMLeaf : public GDynamicFieldPolicy, public MMemPool<GDynamicFieldPolicyMLeaf>
{
public:
	GDynamicFieldPolicyMLeaf(GDynamicFieldMaster* pOwner)
		: GDynamicFieldPolicy(pOwner) {}
	virtual ~GDynamicFieldPolicyMLeaf() {}
};

#endif
