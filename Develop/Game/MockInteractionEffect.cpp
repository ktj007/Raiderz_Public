#include "stdafx.h"
#include "MockInteractionEffect.h"
#include "RSceneManager.h"
#include "XUnitTestUtil.h"
#include "XDecalEffect.h"

MockInteractionEffect::MockInteractionEffect( XObject* pObject )
:XInteractionEffect(pObject)
{

}

MockInteractionEffect::~MockInteractionEffect()
{

}

void MockInteractionEffect::Init( wstring strTextureName, vec3 vPos, vec3 vDir, float fSize )
{

}

void MockInteractionEffect::CreateInteractionEffect(INTERACTION_EFFECT_TYPE eType )
{
	SetInteractionEffectType(eType);

	RBoundingBox objAABB = GetObjectAABB();
	vec3 vCenter = GetInteractionCenter(objAABB);
	float fSize = GetInteractionSize(objAABB);

	MUID uidDecal = XUnitTestUtil::NewMUID();

	XDecalEffect declaEffect;
	bool bCreate = declaEffect.Create(uidDecal, L"Target.tga", DET_PROJECTION, vCenter, fSize, fSize, fSize);
	CHECK(bCreate);

	SetInteractionColor(&declaEffect);

	global.smgr->AddSceneNode(this);
}