#include "stdafx.h"
#include "WS_CutSceneLogicObjectEffect.h"
#include "WS_CutSceneLogicObjectBase.h"
#include "../../Game/XCutSceneObjectControllerMove.h"

void CCutSceneLogicObjectEffect::OnRender()
{
	using namespace rs3;
	size_t count = m_vMoveKey.size();
	RVector3 vPos, vPosOld, vDir;
	RMatrix mat;
	for(size_t i = 0; i < count; ++i)
	{
		CLogicObjectKey* pMoveKeyObj = m_vMoveKey[i];
		vPosOld = vPos;
		pMoveKeyObj->GetPosition(vPos);
		pMoveKeyObj->GetDirection(vDir);
		mat.SetLocalMatrix(vPos, vDir, RVector3::AXISZ);
		RRenderHelper::RenderAxis(mat, 20, 1.1f, 2.5f,3.0f);

		DWORD dwBoxColor = 0xffffffff;

		if (pMoveKeyObj->m_bSelected)
		{
			rs3::RMatrix matT;
			matT.SetTranslationMatrix(vPos);
			matT._43 = matT._43 + m_fHeight;
			//DWORD dwColor = (pEntitySpawn->GetValid() ? pEntitySpawn->GetColor() : INVAILD_COLOR);
			RRenderHelper::RenderCorn( MAT_PI_RotForCorn * matT, CEntityObject::SPAWN_HEIGHT, CEntityObject::SPAWN_HEIGHT, -1);
			//, dwColor );

			dwBoxColor = 0xffff6868;
		}

		RRenderHelper::RenderBox(mat, m_box, dwBoxColor);

		if (m_pXCutSceneController && pMoveKeyObj->m_bLocationChanged)
		{
			//float fTime;
			//bool bWarp;
			//RVector3 vKeyPrevPos, vKeyPrevDir;
			//m_pXCutSceneController->GetKeyInfo(i, fTime, vKeyPrevPos, vKeyPrevDir, bWarp);
			//m_pXCutSceneController->SetKeyInfo(i, fTime, vPos, vDir, bWarp);

			//pMoveKeyObj->m_bLocationChanged = false;
		}
	}
}

void CCutSceneLogicObjectEffect::SetXCutSceneController( XCutSceneObjectControllerEffect* pXCutSceneController )
{
	m_pXCutSceneController = pXCutSceneController;
}
