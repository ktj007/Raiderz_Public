#ifndef _XSCENE_MANAGER_H
#define _XSCENE_MANAGER_H

#include "RSceneManager.h"
using namespace rs3;

extern const char *RSID_XCHARACTER;

class XSceneManager : public RSceneManager
{
private:
	MDeclareRTTI;
public:
	XSceneManager();
	virtual ~XSceneManager();


	//////////////////////////////////////////////////////////////////////////
	// 씬 매니저 생성시
	//
	virtual bool				Create(RDevice* pDevice) override;


	//////////////////////////////////////////////////////////////////////////
	// 업데이트 / 렌더링
	//
	virtual void				Update();
	virtual void				RenderPrimaryCamera();				// 프라이머리 카메라 랜더링


	//////////////////////////////////////////////////////////////////////////
	// 리소스 지우기
	//
	virtual void				Clear();


	//////////////////////////////////////////////////////////////////////////
	// 팩토리
	//
	virtual RSceneNode*				CreateSceneNode(const char* pChar);
	virtual RSceneNodeController*	CreateSceneNodeController(const char* _pSzControllerName);

	void						DeleteActor(XCharacter* pActor);
	void						ReserveActorToDelete(XCharacter* pActor);


	std::vector<XCharacter*>	m_vecReserveRemoveToDelete;
};

#endif // _XSCENE_MANAGER_H