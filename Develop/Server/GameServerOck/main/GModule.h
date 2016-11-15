#ifndef _GMODULE_H
#define _GMODULE_H

#include "GMessage.h"
#include "MTypes.h"
#include "GModuleID.h"
#include <map>
#include <set>
using namespace std;


class GModuleContainer;
class GEntity;

/// 아무것이나 담을 수 있는 모듈 기본 클래스
class GModule {
	friend GModuleContainer;
protected:
	GModuleContainer*		m_pContainer;	///< 이모듈을 담고있는 container
	GEntity*				m_pOwner;		///< 이모듈의 소유자(여기서는 GEntitySync 클래스)
	void					Subscribe(int nMessageID);
public:
							GModule(GEntity* pOwner=NULL);								///< 생성자
	virtual					~GModule();												///< 파괴자
	void					Active(bool bActive = true);							///< (비)활성화 시킨다
	void					Update(float fDelta)	{ OnUpdate(fDelta); }			///< 업데이트
	GMessageResult			Message(GMessage& msg)	{ return OnMessage(msg); }			///< 메세지 처리
	GModuleContainer*		GetContainer()			{ return m_pContainer;}			///< 컨테이너 반환
	GEntity*				GetOwner()				{ return m_pOwner; }	///< 소유자 오브젝트 반환 - 재활용할때는 이걸 빼줘야 함
	virtual int				GetID() = 0;
	void					Initialize()			{ OnInitialized(); }
	void					Finalize()				{ OnFinalized(); }
	bool					IsActivate();
	MUID					GetOwnerUID();

protected:
	// 필요한 부분은 하위 모듈에서 상속받는다.
	virtual void			OnAdd()					{}						///< 컨테이너에 추가될 때 호출
	virtual void			OnRemove()				{}						///< 컨테이너에서 제거될 때 호출
	virtual void			OnActivate()			{}						///< 활성화될 때 호출
	virtual void			OnDeactivate()			{}						///< 비활성화될 때 호출
	virtual void			OnUpdate(float fDelta)	{}						///< 매 틱마다 호출
	virtual void			OnInitialized()			{}						///< 초기화될 때 호출
	virtual void			OnFinalized()			{}						///< 마무리될 때 호출
	virtual GMessageResult	OnMessage(GMessage& msg){ return MR_IGNORED; }	///< 메세지 핸들러
	virtual void			OnSubscribeMessage()	{}						///< 받을 메세지를 지정한다.

#define SERVER_MODULE	L"MODULE"
	/// 클래스 이름 얻기
	virtual const wchar_t* GetClassName(void){ return SERVER_MODULE; }

};

/// 모듈을 담을 수 있는 컨테이너 클래스
class GModuleContainer 
{
private:
	typedef map<int, GModule*>	GModuleMap;
	friend GModule;

	GModuleMap			m_Modules;
	vector<GModule*>	m_ActiveModules;

	std::set<int>	m_ModuleMessageTypes[MAX_MODULE_ID];
	void			SubscribeMessage(int nModuleID, int nMessageID);
public:
	GModuleContainer();
	virtual ~GModuleContainer();

	// bInitialActive : 활성화 초기치
	void AddModule(GModule *pModule, bool bInitialActive = true);
	void RemoveModule(GModule *pModule);

	// 등록된 모듈중 nID 모듈을 찾는다
	GModule* GetModule(int nID);

	void ActiveModule(int nID, bool bActive = true);	// 특정 모듈을 (비)활성화 시킨다
	bool IsActiveModule(int nID);

	void Message(GMessage& msg);
};

#define DECLARE_ID(_ID)		int GetID() { return _ID; }




#endif