#ifndef _XMODULE_H
#define _XMODULE_H

#include "XEvent.h"
#include "XModuleID.h"
#include <map>
#include <set>
using namespace std;

class XModuleContainer;
class XObject;

/// 아무것이나 담을 수 있는 모듈 기본 클래스
class XModule {
	friend XModuleContainer;

protected:

	enum E_OWNER_TYPE
	{
		E_OWNER_OBJECT = 0,
		E_OWNER_ACTOR,

		E_OWNER_NPC,
		E_OWNER_NET_PLAYER,
		E_OWNER_MY_PLAYER
	};
	XObject*					m_pOwner;										///< 이모듈의 소유자(여기서는 XObject 클래스)
	virtual E_OWNER_TYPE		DecideOwnerType();								///< 모듈 소유자의 타입을 결정하는 함수


	XModuleContainer*			m_pContainer;									///< 이모듈을 담고있는 container
	void						Subscribe(int nMessageID);						///< 메세지 구독

public:
								XModule(XObject* pOwner=NULL);					///< 생성자
	virtual						~XModule();										///< 파괴자
	void						Active(bool bActive = true);					///< (비)활성화 시킨다
	void						Update(float fDelta){ OnUpdate(fDelta); }		///< 업데이트
	void						PostUpdate(float fDelta){ OnPostUpdate(fDelta); }	///< 업데이트 이후에 발동되는 업데이트
	XEventResult				Event(XEvent& msg)	{ return OnEvent(msg); }	///< 이벤트 처리
	XModuleContainer*			GetContainer()		{ return m_pContainer;}		///< 컨테이너 반환
	XObject*					GetOwner()			{ return m_pOwner; }		///< 소유자 오브젝트 반환
	virtual int					GetID() = 0;
	void						Initialize()		{ OnInitialized(); }
	void						Finalize()			{ OnFinalized(); }
	bool						IsActivate();


protected:
	
	// 필요한 부분은 하위 모듈에서 상속받는다.
	virtual void				OnAdd()					{}						///< 컨테이너에 추가될 때 호출
	virtual void				OnRemove()				{}						///< 컨테이너에서 제거될 때 호출
	virtual void				OnActivate()			{}						///< 활성화될 때 호출
	virtual void				OnDeactivate()			{}						///< 비활성화될 때 호출
	virtual void				OnUpdate(float fDelta)	{}						///< 매 틱마다 호출
	virtual void				OnPostUpdate(float fDelta)	{}					///< 매 틱마다 호출, 업데이트 이후 발동
	virtual void				OnInitialized()			{}						///< 초기화될 때 호출
	virtual void				OnFinalized()			{}						///< 마무리될 때 호출
	virtual XEventResult		OnEvent(XEvent& msg){ return MR_IGNORED; }		///< 메세지 핸들러
	virtual void				OnSubscribeEvent()	{}							///< 받을 메세지를 지정한다.
};

/// 모듈을 담을 수 있는 컨테이너 클래스
class XModuleContainer 
{
private:
	typedef map<int, XModule*>	XModuleMap;
	typedef vector<XModule*>	XModuleVector;
	friend XModule;

	XModuleMap					m_Modules;
	XModuleVector				m_ActiveModules;

	std::set<int>				m_ModuleMessageTypes[MAX_MODULE_ID];
	void						SubscribeEvent(int nModuleID, int nMessageID);


public:

	XModuleContainer();
	virtual ~XModuleContainer();

	//////////////////////////////////////////////////////////////////////////
	// bInitialActive : 활성화 초기치
	//
	void						AddModule(XModule *pModule, bool bInitialActive = true);
	void						RemoveModule(XModule *pModule);

	//////////////////////////////////////////////////////////////////////////
	// 등록된 모듈중 nID 모듈을 찾는다
	//
	XModule*					GetModule(int nID);

	void						ActiveModule(int nID, bool bActive = true);	// 특정 모듈을 (비)활성화 시킨다
	bool						IsActiveModule(int nID);

	void						UpdateModules(float fDelta);
	void						PostUpdateModules(float fDelta);
	void						Event(XEvent& msg);
};

#define DECLARE_ID(_ID)			int GetID() { return _ID; }




#endif