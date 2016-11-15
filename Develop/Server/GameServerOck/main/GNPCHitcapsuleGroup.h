#pragma once

#include "CSHitCapsuleIndexMgr.h"
#include "CSChrInfo.h"

class GEntityNPC;
class GNPCInfo;

class GNPCHitcapsuleGroup : public MTestMemPool<GNPCHitcapsuleGroup>
{
public:
	GNPCHitcapsuleGroup(GEntityNPC* pOwner);
	~GNPCHitcapsuleGroup(void);

	// 히트캡슐그룹 초기화
	void Init( GNPCInfo* pNPCInfo );
	// 현재 히트캡슐그룹 인덱스 반환
	int8 GetCapsuleGroupIndex();

	// 트리거(콜트,스크립트)에 의한 히트캡슐그룹 변경
	bool ChangeCapsuleGroup_ByTrigger( int8 val );
	// 탤런트 사용에 의한 히트캡슐그룹 변경
	bool ChangeCapsuleGroup_ByTalent( int8 val );
	// 탤런트 사용완료에 의한 히트캡슐그룹 변경취소
	void ChangeCapsuleGroup_ByTalentComplete();
	// 모드변경에 의한 히트캡슐그룹 변경
	void ChangeCapsuleGroup_ByMode(NPC_MODE nMode);

private:
	// 유효한 캡슐그룹 ID인지 확인
	bool IsValidCapsuleGroupID( int8 val ) const;
	// 캡슐그룹 변경을 알리는 패킷
	void RouteChangeCapsule(int8 nHitCapsuleGroup);
	
private:
	GEntityNPC* m_pOwner;
	CSHitCapsuleIndexMgr		m_HitCapsuleIndexMgr;
};
