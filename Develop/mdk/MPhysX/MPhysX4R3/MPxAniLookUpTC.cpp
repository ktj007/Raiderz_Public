#include "stdafx.h"
#include "MPxAniLookUpTC.h"

#include "MPxRagDollTC.h"
#include "MPxDynamicCollisionTC.h"

namespace physx
{

using namespace rs3;

MPxAniLookUpTC::MPxAniLookUpTC(MPxTcEntity* pTcEntity)
: MPxTcImpl(pTcEntity)
{
	m_bCollisionTcHooking = false;
}

bool MPxAniLookUpTC::HookOnAnimationSetting( string& strAniName )
{
	// 설정될 애니메이션이 랙돌이라면 후킹한다.
	MPxRagDollTC* pRagDollTransform = GetTransformController<MPxRagDollTC>();
	if (pRagDollTransform)
	{
		const char* pSzRagDollPrefix = strstr(strAniName.c_str(), "ragdoll_");
		if ( NULL != pSzRagDollPrefix )
		{
			// 랙돌 상태를 요청받았을때.
			const char* pSzRagDollContext = pSzRagDollPrefix + 8; //문자열 ragdoll_ 의 크기가 8이다
			if (pRagDollTransform->IsRagDollAvailable())
			{
				// 이미 랙돌상태일때는 Context로부터 랙돌을 변경시킨다.
				pRagDollTransform->ChangeRagDollFromContext(pSzRagDollContext);
			}
			else
			{
				// 랙돌 상태 트리깅
				pRagDollTransform->StartRagDoll(pSzRagDollContext);
			}
			return true;
		}
		else
		{
			// 랙돌 상태아님 트리깅
			pRagDollTransform->EndRagDoll();

			// 다른 애니메이션 이름 후킹 : col과 uncol
			if (m_bCollisionTcHooking)
			{
				const char* pSzCollisionPrefix = strstr(strAniName.c_str(), "uncol_");
				if(pSzCollisionPrefix)
				{
					strAniName = pSzCollisionPrefix + 6; //문자열 uncol_ 은 6글자
					MPxDynamicCollisionTC* pCollisionTc = GetTransformController<MPxDynamicCollisionTC>();
					if (pCollisionTc)
					{
						pCollisionTc->DeActivateCollision();
					}
				}
				else if ( (pSzCollisionPrefix = strstr(strAniName.c_str(), "col_")) )
				{
					strAniName = pSzCollisionPrefix + 4; //문자열 col_ 은 4글자
					MPxDynamicCollisionTC* pCollisionTc = GetTransformController<MPxDynamicCollisionTC>();
					if (pCollisionTc)
					{
						pCollisionTc->ActivateCollision();
					}
				}
			}
		}
	}

	return false;
}

}
