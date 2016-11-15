#include "stdafx.h"
#include "XTalentEffectHitCapsule.h"
#include "XEffectInvoker.h"
#include "XCharacter.h"


XTalentEffectHitCapsule::XTalentEffectHitCapsule()
{

}

XTalentEffectHitCapsule::~XTalentEffectHitCapsule()
{

}

void XTalentEffectHitCapsule::AddTalentHitCapsuleImpact( const MUID& uid, XTalentInfo* pTalentInfo, XTalentEffectInfo* pTalentEffectInfo, TALENT_EFFECT_TYPE eType, vec3 vTalentDir )
{
	XObject* pObject = gg.omgr->Find(uid);
	if(pObject == NULL)
		return;

	XBaseTalentEffectHitCapsule::AddTalentHitCapsuleImpact(pObject->GetActor(), pTalentInfo, pTalentEffectInfo, eType, vTalentDir, uid);
}

bool XTalentEffectHitCapsule::CheckExistActor( MUID uidAttacker )
{
	XObject* pObject = gg.omgr->Find(uidAttacker);
	if(pObject == NULL)
		return false;

	return true;
}

void XTalentEffectHitCapsule::CreateEffect( wstring& strEffectName, vec3& vPos, vec3& vDir )
{
	XEffectSinglyInvoker effectInvoker;
	vec3 vUp = vec3::AXISZ;
	effectInvoker.Invoke(strEffectName, vPos, vDir, vUp);
}
