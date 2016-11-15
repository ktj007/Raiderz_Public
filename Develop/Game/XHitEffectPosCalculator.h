#pragma once

#include "CSMeshInfo.h"

class XObject;
class XModuleCollision;

class XHitEffectPosCalculator
{
private:
	bool CheckCalcEnabled( XObject* pAttacker, XTalentInfo* pTalentInfo, CSMeshInfo* cAttackerMeshInfo, CSMeshInfo* pVictimMeshInfo );
	void GetObjectMaxSizeInfo(XObject* pObject, XModuleCollision* pObjCollision,CSMeshInfo* cMeshInfo, float& fOutArea, float& fOutHeight);

	// 기존과 다르게 탤런트 히트 캡슐 뼈대로 계산합니다.
	vec3 CalHitPositionByTalentHitCapsule(XModuleCollision* pVictimModuleCollision, XObject* pAttacker, XTalentInfo* pTalentInfo, vec3 vBasisPos, vector<MCapsule>& vecColCapsule, vec3& vOutNearTalentHit, vec3& vOutNearCol, float* pfTalentHitMax_z = NULL, float* pfTalentHitMin_z = NULL);
	vec3 CalHitPositionByModelPick(XObject* pAttacker, XModuleCollision* pVictimModuleCollision, MCapsule AttackerCapsule, vec3 vRetHitPos, float* pfTalentHitMax_z = NULL, float* pfTalentHitMin_z = NULL);
	vec3 CalHitPositionByColCapsule(XObject* pAttacker, vector<MCapsule>& vecColCapsule, XTalentInfo* pTalentInfo, vec3 vRetHitPos);
	bool CalHitPositionByVirtualHitCapsule(XObject* pAttacker, MCapsule& AttackerCapsule, vector<MCapsule>& vecColCapsule, XTalentInfo* pTalentInfo, vec3& vRetHitPos, MCapsule* pOutVirtualHitCapsule = NULL);
	bool CalHitPositionByHitEffectPos(XModuleCollision* pVictimModuleCollision, XObject* pAttacker, XTalentInfo* pTalentInfo, vec3& vOutHitPos);

	void CompareToTalentHitCapule( XTalentInfo* pTalentInfo, XObject* pAttacker, RPICKINFO &pickInfo, vec3 &vRetHitPos );
	void GetVictimHitEffectCapsule(XModuleCollision* pVictimModuleCollision, CSMeshInfo* pVictimMeshInfo, XObject* pAttacker, MCapsule AttackerCapsule, vector<MCapsule>& outvecColCapsule, float& fColMax_z, float& fColMin_z);

public:
	XHitEffectPosCalculator(){}
	vec3 GetPos(XObject* pVictim, CSMeshInfo* cHitterMeshInfo, XObject* pAttacker, CSMeshInfo* cAttackerMeshInfo, int nTalentID);


};
