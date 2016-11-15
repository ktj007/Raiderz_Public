#ifndef _GENTITY_H
#define _GENTITY_H

#include "GModule.h"
#include "GDef.h"
#include "MReferenceCounted.h"
#include "CSChrInfo.h"

class GField;
class GEntityMgr;
class GEntityActor;
class GFieldInfo;

/// 엔티티의 최상위 클래스
/// 모든 개체는 이것을 상속해야한다.
class GEntity : public MReferenceCounted
{
	friend class GEntityMgr;

public:
	GEntity();
	virtual ~GEntity();
	virtual bool Create(MUID& uid);
	virtual void Destroy();
	void Update(float fDelta);	
	void Message(GMessage& msg);

	// 모듈별 업데이트
	virtual void UpdateCombat(float fDelta) {}
	virtual void UpdateAI(float fDelta)		{}
	virtual void UpdateBuff(float fDelta)	{}
	virtual void UpdateMovable(float fDelta)		{}

	// EntityMgr여 나를 delete 해주세요..라는 뜻..
	void DeleteMe()			{ m_bDeleteMe = true; }
	bool IsDeleteMe()		{ return m_bDeleteMe; }

	// 필드 들어가기
	void EnterToField(GField* pField, const vec3& pos, const vec3& dir=vec3(0.0f, 1.0f, 0.0f));		
	// 필드에서 나오기
	void LeaveFromField();		

	// do 함수 시리즈 정리 필요. - bird
	virtual void doDie();		// TODO: actor로 이주
	virtual void OnRebirth();	// TODO: actor로 이주
	virtual bool Damage(MUID& uidAttacker, int nDamage) { return true; } 	// TODO: actor로 이주
	virtual void doAttack(MUID uidTarget, int nDamage) {}						// TODO: actor로 이주
	bool		 IsBack(const vec3& vecTargetPos);	// TODO: actor로 이주
	bool				IsAILodEnableFiled() const; // TODO: npc로 이주
	
	// setter
	virtual bool		SetPos(const vec3& pos);
	void				SetDir(const vec3& dir);
	void				SetFacingDir(const vec3& dir);
	const MUID			GetUID(void) const	{ return m_UID; }
	MUID				GetFieldUID() const;
	const ENTITY_TYPEID	GetTypeID() const	{ return m_nTypeID; }
	
	// getter
	bool				IsType(uint32 mask) const;
	vec3				GetPos() const		{ return m_vPos; }
	vec3				GetDir() const		{ return m_vDir; }
	virtual vec3		GetFacingDir() const{ return m_vFacingDir; }
	GField*				GetField() const	{ return m_pField; }
	const GFieldInfo*	GetFieldInfo() const;
	int					GetFieldID() const;
	bool				IsInField() const	{ return (m_pField != NULL); }
	bool				IsDead() const		{ return m_bDead; }
	bool				IsNPC()  const;
	bool				IsPlayer()  const;
	bool				IsActor()  const;
	bool				IsBuffEntity() const;
	bool				IsBPart() const;

	// util
	float				DistanceTo(GEntity* pEntity);

	void SetActivateUpdate(bool isActivate);
	bool				IsActivateUpdate() const;

protected:
	virtual void OnUpdate(float fDelta);
	virtual void OnMessage(GMessage& msg);
	virtual void OnEnter()		{}
	virtual void OnLeave()		{}
	virtual void OnDestroy()	{}
	virtual void OnEnterToSharedField()		{}
	virtual void OnEnterToDynamicField()	{}
	virtual void RegisterModules()			{}						///< 모듈 등록
	virtual void UnregisterModlues()		{}						///< 모듈 해제
	bool				IsActiveModule(int nID);
	GModule*			GetModule(int nID);
	

private:
	

private:
protected:
	GModuleContainer	m_Modules;
	MUID				m_UID;
	uint32				m_nType;
	ENTITY_TYPEID		m_nTypeID;
	vec3				m_vPos;
	vec3				m_vDir;
	vec3				m_vFacingDir;		// 실제 바라보는 방향
	bool				m_bCreated;

	bool				m_bDead;

	GField*				m_pField;
	bool				m_bDeleteMe;		// 이것이 true이면 EntityMgr의 Update에서 직접 delete해준다.

	bool				m_isActivateUpdate;	///< EntityMgr에서 업데이트 허용 여부
};

#endif
