#pragma once

#include "GTalentDef.h"
using namespace cscommon_server;

class GEntityActor;
class GTalentInfo;
class GTalent;


/// 충돌, 판정 관련 모음 클래스
/// TODO: GCollision 안에서 아예 GActor::IsHittable을 체크하면 성능이 더 좋아진다. - birdkr
class GCollision
{
protected:
	static bool IsCheckableOverlappedCol(GEntityActor* pAttacker, GEntityActor* pVictim);
public:
	static bool HitTest(GEntityActor* pThisEntity, MSphere& tarSphere);
	
	static bool HitTest(GEntityActor* pThisEntity, GEntityActor* pTarEntity, float fToler = 0.0f);

	static bool HitTest1(GEntityActor* pThisEntity, GTalentInfo* pTalentInfo, int nTalentColIndex, const vec3& vOwnerPos, const vec3& vOwnerDir, int8& nHitCapsuleGroup, int8& nHitCapsuleindex);
	static bool HitTest2(GEntityActor* pThisEntity, MCapsule& tarCapsule, int8& noutHitCapsuleGroup, int8& noutHitCapsuleindex);		// nHitCapsuleindex는 판정에 성공한 충돌캡슐의 인덱스
	static bool HitTest3(GEntityActor* pThisEntity, const MVector3& rayOrigin, const MVector3& rayDir, float &outDistance);

	static void HitTestTalent(GTalent* pAttackTalent, int nHitColIndex, set<MUID>* psetAlreadyVictim, vector<TALENT_TARGET_INFO>& outsetVictim);	// psetAlreadyVictim는 NULL일 수 있다.
};


