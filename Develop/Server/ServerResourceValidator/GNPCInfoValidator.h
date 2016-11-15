#ifndef _G_NPC_INFO_VALIDATOR_H_
#define _G_NPC_INFO_VALIDATOR_H_

#include "GValidatorLib.h"
#include "CSBaseValidator.h"
#include "CSModifier.h"

class GNPCInfo;
class GNPCInteractionInfo;
class GLootInfo;

enum NPC_TYPE;

class GVALIDATOR_API GNPCInfoValidator : public CSBaseValidator
{
private:
	void CheckGrade(int nGrade);
	void CheckMeshName(const wstring& strMeshName);
	void CheckLevel(int nMinLevel, int nMaxLevel);
	void CheckMaxHP(int nMaxHP);
	void CheckAP(int nAP);
	void CheckSpeed(float fSpeed, float fRunSpeed, float fRotateSpeed);
	void CheckSightRange(int nSightRange);
	void CheckScale(float fScale);
	void CheckMFModifier(PerMulVar<short> MotionFactors[MF_PRIORITY_SIZE]);
	void CheckInteraction(const GNPCInteractionInfo& vecInteractionInfo, GNPCInfo* pNPCInfo);
	void CheckThinkable(bool bThinkable, NPC_TYPE nType);
	void CheckMode(GNPCInfo* pNPCInfo);
	void CheckGather(GNPCInfo* pNPCInfo);

public:
	virtual bool Check();
};

#endif //_G_NPC_INFO_VALIDATOR_H_
