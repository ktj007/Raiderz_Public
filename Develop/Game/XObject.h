#ifndef _XOBJECT_H
#define _XOBJECT_H

#include "MUID.h"
#include "MRTTI.h"
#include "XModule.h"
#include "XEvent.h"
#include "CSChrInfo.h"
#include "MReferenceCounted.h"
#include "XObjectID.h"
#include "XEventID.h"

class XObjectManager;
class XModuleEntity;
class XModuleMotion;
class XModuleAction;
class XModuleMovable;
class XModuleCollision;
class XModuleMyControl;
class XModuleNetControl;
class XModuleNonControl;
class XModuleTalent;
class XModuleMessageQueue;
class XModuleActorControl;
class XModuleBaseData;
class XCharacter;
class XModuleEffect;
class XModuleBuff;
class XModuleUI;
class XModulePost;
class XNetPlayer;
class XMyPlayer;
class XNonPlayer;
class XCutScenePlayer;
class XActor;
class XPlayer;
class XBPartNonPlayer;
class XModuleBParts;

/// 게임 상에 존재하는 모든 오브젝트의 부모 클래스
class XObject : public MReferenceCounted
{
	MDeclareRootRTTI;
private:
	friend class XObjectManager;	
protected:
	MUID				m_UID;
	ENTITY_TYPEID		m_nTypeID;
	bool				m_bCreated;
	bool				m_bDeleteMe;		///< true로 세팅되어 있으면 Update후 Manager가 삭제시킨다.
	XModuleContainer	m_Modules;

	vec3				m_vFacingDir;		///< 

	virtual void		RegisterModules()			{}
	virtual void		UnregisterModules()			{}
	virtual bool		OnCreate()					{return true;}
	virtual void		OnDestroy()					{}
	virtual void		OnRender()					{}
	virtual void		OnPostRender()				{}
	virtual void		OnUpdate(float fDelta);
	virtual void		OnPostUpdate(float fDelta);
	virtual void		OnEvent(XEvent& msg);
public:
						XObject(MUID uid) : m_UID(uid), m_bCreated(false), m_nTypeID(ETID_ENTITY), m_bDeleteMe(false) {}
	virtual				~XObject();

	bool				Create();
	void				Destroy();
	void				Update(float fDelta);
	void				PostUpdate(float fDelta);
	void				Render();
	void				Event(XEvent& msg);
	void				DeleteMe()					{ m_bDeleteMe = true; }
	bool				IsDeleteMe()				{ return m_bDeleteMe; }
	const MUID&			GetUID()					{ return m_UID; }
	ENTITY_TYPEID		GetEntityType()					{ return m_nTypeID; }
	virtual XObjectID	GetType() = 0;

	XModule*			GetModule(int nID);
	bool				IsActiveModule(int nID);
	void				ActiveModule(int nID, bool bActive);
	void				AddModule(XModule *pModule, bool bInitialActive);

	vec3				GetPosition();
	vec3				GetDirection();
	vec3				GetUpvector();
	mat4				GetWorldTransform();
	void				SetPosition(const vec3& pos);
	void				SetDirection(const vec3& dir);
	void				SetDirectionSmoothly(const vec3& dir, float fRotateSpeed);
	void				SetUpVector(vec3& up);
	void				PostDelayEvent(XEvent& msg, unsigned int nDelayTime);
	void				RemoveDelayEvent(XEventID nEventID);
	float				GetScale();
	virtual bool		IsHuge()					{ return false; }

	void				SetTarget( const MUID& target);
	const MUID&			GetTarget();
	void				SetExtraTarget( const MUID& target);
	const MUID&			GetExtraTarget();

	void				SetFacingDir(vec3& dir) { m_vFacingDir = dir; }
	vec3				GetFacingDir() { return m_vFacingDir; }

	void				SetVisible(bool bVisible);
	bool				IsVisible();

	float				GetRadius();				///< Radius
	virtual float		GetValidInteractionDistance()		{ return 0.0f; }

	bool				IsModuleEntity();
	bool				IsModuleAction();
	bool				IsModuleMotion();
	bool				IsModuleTalent();
	bool				IsModuleEffect();

	XCharacter*			GetActor();
	XCharacter*			GetCollisionActor();
	virtual bool		IsPlayer() { return false; }
	virtual bool		IsNPC() { return false; }

	virtual vec3		GetNamePos();
	virtual const wchar_t*	GetName() { return L""; }
	virtual wstring		GetCurrentAnimationName() { return L""; }

	virtual bool		Reload();
public:
	// 편의를 위해 모듈 가져오는 인터페이스를 따로 만든다.
	virtual XModuleEntity*			GetModuleEntity();
	virtual XModuleMotion*			GetModuleMotion();
	virtual XModuleAction*			GetModuleAction();
	virtual XModuleMovable*			GetModuleMovable();
	virtual XModuleCollision*		GetModuleCollision();
	virtual XModuleUI*				GetModuleUI();
	virtual XModuleMyControl*		GetModuleMyControl()		{ return NULL; }
	virtual XModuleNetControl*		GetModuleNetControl()		{ return NULL; }
	virtual XModuleNonControl*		GetModuleNonControl()		{ return NULL; }
	virtual XModuleTalent*			GetModuleTalent()			{ return NULL; }
	virtual XModuleMessageQueue*	GetModuleMessageQueue()		{ return NULL; }
	virtual XModuleActorControl*	GetModuleActorControl()		{ return NULL; }
	virtual XModuleEffect*			GetModuleEffect()			{ return NULL; }
	virtual XModuleBuff *			GetModuleBuff()				{ return NULL; }
	virtual XModulePost*			GetModulePost()				{ return NULL; }
	virtual XModuleBaseData*		GetModuleBaseData()			{ return NULL; }
	virtual XModuleBParts*			GetModuleBParts()			{ return NULL; }

	// 편의와 안전성을 위한 형변환 함수
	XMyPlayer*			AsMyPlayer();
	XNetPlayer*			AsNetPlayer();
	XPlayer*			AsPlayer();
	XNonPlayer*			AsNPC();
	XCutScenePlayer*	AsCutScenePlayer();
	XActor*				AsActor();
	XBPartNonPlayer* 	AsBPartNPC();
};

// 형변환 편의 함수
inline XMyPlayer* AsMyPlayer(XObject* pObject)
{
	if (pObject) return pObject->AsMyPlayer();
	return NULL;
}
inline XNetPlayer* AsNetPlayer(XObject* pObject)
{
	if (pObject) return pObject->AsNetPlayer();
	return NULL;
}
inline XNonPlayer* AsNPC(XObject* pObject)
{
	if (pObject) return pObject->AsNPC();
	return NULL;
}
inline XCutScenePlayer* AsCutScenePlayer(XObject* pObject)
{
	if (pObject) return pObject->AsCutScenePlayer();
	return NULL;
}
inline XActor* AsActor(XObject* pObject)
{
	if (pObject) return pObject->AsActor();
	return NULL;
}
inline XBPartNonPlayer* AsBPartNPC(XObject* pObject)
{
	if (pObject) return pObject->AsBPartNPC();
	return NULL;
}


#endif