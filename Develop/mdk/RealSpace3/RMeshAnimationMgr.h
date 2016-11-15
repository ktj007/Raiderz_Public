/**
@brief RMeshAnimationMgr
elu 메쉬의 애니메이션을 관리하는 클래스 입니다
RAnimation , RAnimationManager 의 리소스/관리자를 참조합니다
*/

#pragma once

#include "RAnimation.h"
#include "RAnimationBlendTable.h"
#include "RResourceManager.h"

namespace rs3 {

typedef RHashList<RAnimation*>					RAnimationHashList;
typedef RHashList<RAnimation*>::iterator		RAnimationHashList_Iter;

class RS_API RMeshAnimationMgr : public RResource::LoadingController {
public:
	RMeshAnimationMgr();
	~RMeshAnimationMgr(); 

	RAnimation* Add(const char* name, const char* filename, bool bBackground = false, bool bLoadMonitoring = false, RResource::LoadingController* pLoadingController = NULL);

	void Delete(const char* name);
	void DelAll(); 

	RAnimation* GetAnimation(const char* name);
	RAnimation* GetAnimation(int nID);

	/// 특정 모델 로드시 자동으로 불리는 에니메이션 파일 목록 지원하기
	bool AddXmlDesc(const char* xmlFileName, bool bBackground = false, bool bLoadMonitoring = false, bool bExtern = false, const char* szFilter = "");
	bool SaveToXml(const char* szFileName);

	RAnimationBlendTable& GetAnimationBlendTable() { return m_AnimationBlendTable; }
	BLENDDATA GetAnimationBlendType( const char* source, const char* target );
	BLENDDATA GetAnimationBlendType( int sourceId, int targetId );

	bool IsLoadingCompleted();

	bool SaveAnimation(const char* szFileName);
	bool SaveAnimationEvent(const char* szFileName);

	bool LoadAnimationInformationsToAnimationMgr(const string& EluFileName,bool bExistAutoAnimation,bool bBackgournd);
public:
	RAnimationHashList		m_NodeHashList;
	RAnimationBlendTable	m_AnimationBlendTable;

private:
	int	m_nIDCount;

	virtual void	OnLoadingComplete(RResource*);
};

}