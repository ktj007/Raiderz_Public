#include "stdafx.h"
#include "WS_CutSceneLogicObjectMove.h"
#include "WS_CutSceneLogicObjectBase.h"
#include "../../Game/XCutSceneObjectControllerMove.h"

void CCutSceneLogicObjectMove::OnRender()
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

		RVector3 vUp(RVector3::AXISZ);
		pMoveKeyObj->GetUp(vUp);
		mat.SetLocalMatrix(vPos, vDir, vUp);
		//RRenderHelper::RenderSphere(mat, 100);
		RRenderHelper::RenderAxis(mat, 20, 1.1f, 2.5f,3.0f);

		DWORD dwBoxColor = 0xffffffff;
		if(m_bSelected)
			dwBoxColor = 0xffff6868;

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
			float fTime;
			bool bWarp;
			RVector3 vKeyPrevPos, vKeyPrevDir, vKeyPreUp;
			m_pXCutSceneController->GetKeyInfo(i, fTime, vKeyPrevPos, vKeyPrevDir, vKeyPreUp, bWarp);
			m_pXCutSceneController->SetKeyInfo(i, fTime, vPos, vDir, vUp, bWarp);

			pMoveKeyObj->m_bLocationChanged = false;
		}

		if (i != 0)
		{
			RRenderHelper::RenderLine( RMatrix::IDENTITY, vPosOld, vPos, dwBoxColor );
		}
	}
}

void CCutSceneLogicObjectMove::OnSelected_( bool bSelected )
{
	m_bSelected = bSelected;
	if (bSelected)
		return;

	for (std::vector<CLogicObjectKey*>::iterator itr = m_vMoveKey.begin(); itr != m_vMoveKey.end(); ++itr)
	{
		(*itr)->SetSelection(false);
	}
}

void CCutSceneLogicObjectMove::SetXCutSceneController( XCutSceneObjectControllerMove* pXCutSceneController, int nCtrlIdx )
{
	m_pXCutSceneController = pXCutSceneController;
	m_nControllerIdx = nCtrlIdx;
}