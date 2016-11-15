#include "stdafx.h"
#include "THitAreaRenderer.h"
//#include "TSkillHitArea.h"
#include "RActor.h"
#include "RDevice.h"
#include "MainApp.h"
#include "RVType.h"
#include "RRenderHelper.h"
#include "RFont.h"

THitAreaRenderer& THitAreaRenderer::GetInstance()
{
	static THitAreaRenderer instance;
	return instance;
}


void THitAreaRenderer::Render(CSTalentInfo* pSkillInfo, int nSegID, int nCapsuleID, bool bDrawSegment, bool opa)
{
	if (m_pDevice == NULL) return;
	if (pSkillInfo == NULL) return;

	int nSegCount = pSkillInfo->m_HitInfo.m_vSegments.size();
	if( nSegCount == 0) return;
	if( nSegID < 0 || nSegCount < (nSegID+1) ) return;

	int i = nSegID;
	if( pSkillInfo->m_HitInfo.m_vSegments[i].m_vCapsules.size() == 0) return;

	RRenderHelper::SetRenderState();


	for (size_t k=0; k < pSkillInfo->m_HitInfo.m_vSegments[i].m_vCapsules.size(); k++)
	{
		MCapsule cap = pSkillInfo->m_HitInfo.m_vSegments[i].m_vCapsules[k];

		if(pSkillInfo->m_bHitCapsulePosSync && g_pMainApp->GetActor())
		{
			MMatrix matWorldModel;
			matWorldModel.SetLocalMatrix(g_pMainApp->GetActor()->GetPosition(), g_pMainApp->GetActor()->GetDirection(), g_pMainApp->GetActor()->GetUp());
			cap = cap.Multiply(matWorldModel);
		}

		RenderHitCapsule(cap, bDrawSegment, opa, (i == nSegID && k == nCapsuleID));


		RenderPlayerMoveHitArea(pSkillInfo->m_HitInfo.m_vSegments[i].m_vCapsules[k], ( i == nSegID && k == nCapsuleID));

	}

	RRenderHelper::EndRenderState();
}

void THitAreaRenderer::RenderPlayerMoveHitArea( MCapsule cap, bool bSelected )
{
	if(g_pMainApp->GetActor() == NULL)
		return;

	string n = g_pMainApp->GetActor()->GetMeshName();
	string nn = n.substr(0, n.rfind("."));

	// 탤런트 애니메이션이어야 한다.
	// 플레이어야 한다.
	if(g_pMainApp->m_bTalentAnimation == false ||
		(nn != "hf" && nn != "hm"))
		return;

	int nCurFrame = g_pMainApp->GetActor()->GetCurFrame();
	int nBackFrame = g_pMainApp->GetActor()->GetSecToFrame(0.2f);
	int nCalFrame = nCurFrame - nBackFrame;
	if(nCalFrame < 0)
		nCalFrame = 0;

	MMatrix matResult;
	RActorNode* pActorNode = g_pMainApp->GetActor()->GetAnimatedActorNodeTransform2(nCalFrame, string("dummy_loc"), matResult, E_TS_LOCAL_RESULT);
	if(pActorNode == NULL)
		matResult = g_pMainApp->GetActor()->GetWorldTransform();

	MMatrix matDummy = g_pMainApp->GetMotion()->GetAnimationDummyLocLastMatrix(g_pMainApp->GetMotion()->GetCurAniSequence(), *(g_pMainApp->m_TalentAnimation.GetDummyLocLast()));

	cap = cap.Multiply(matDummy * matResult);

	if(bSelected)
		RRenderHelper::RenderCapsule(cap, 0x200000FF);
	else
		RRenderHelper::RenderCapsule(cap, 0x20C4C4FF);
}

void THitAreaRenderer::RenderHitCapsule( MCapsule& cap, bool bDrawSegment, bool opa, bool bSelected, bool bAllSelected /*= false*/ )
{
	if (bDrawSegment)
	{
		RBoundingBox box = RBoundingBox(RVector(-5,-5,-5),RVector(5,5,5));
		RMatrix matTM;
		matTM.MakeIdentity();

		RRenderHelper::RenderLine(RMatrix::IDENTITY, cap.bottom, cap.top, 0xFFFF0000);

		RVector t1 = cap.bottom;
		t1.z = -20.f;
		RRenderHelper::RenderLine(RMatrix::IDENTITY, cap.bottom, t1, 0xFFFF00FF);
		matTM.SetTranslation(t1);
		RRenderHelper::RenderBox(matTM, box, 0xffffffff);
		RRenderHelper::RenderAxis(matTM,30,0.5f);
		g_pMainApp->RenderAxisText(matTM, 30);
		//g_pMainApp->m_pTextNode1->SetPosition(t);

		RVector t2 = cap.top;
		t2.z = -20.f;
		RRenderHelper::RenderLine(RMatrix::IDENTITY, cap.top, t2, 0xFFFF00FF);
		matTM.SetTranslation(t2);
		RRenderHelper::RenderBox(matTM, box, 0xffffffff);
		RRenderHelper::RenderAxis(matTM,30,0.5f);
		g_pMainApp->RenderAxisText(matTM, 30);

		if(bSelected)
		{
			// billboard
			rs3::RFont::BeginFont(/*D3DXSPRITE_ALPHABLEND | D3DXSPRITE_OBJECTSPACE*/);

			// DrawTextIn3DSpace를 사용하려면 VIEW를 설정해 주어야 합니다.
			// BeginFont에서 VIEW를 보관하고 End에서 복구해 주기 때문에 BeginFont 이후에 설정 되어야 합니다.
			m_pDevice->SetTransform(RST_VIEW, g_pMainApp->GetCamera()->GetViewMatrix());

			rs3::RMatrix matInverse;
			g_pMainApp->GetCamera()->GetViewMatrix().GetInverse( &matInverse );

			rs3::RMatrix matTextReflectAndScale
				(
				1.0f,    0.f,    0.f,   0.f,
				0.f,     -1.0f,  0.f,   0.f,
				0.f,      0.f,    1.0f, 0.f,
				0.f,      0.f,    0.f,   1.f
				);

			matInverse._41 = t1.x;
			matInverse._42 = t1.y;
			matInverse._43 = t1.z;

			RMatrix matT = matTextReflectAndScale * matInverse;

			if( GlobalObjects::g_pMainForm->Bottom_toolStripButton->Checked ||
				GlobalObjects::g_pMainForm->TopBottom_toolStripButton->Checked)
				g_pMainApp->GetFont()->DrawTextIn3DSpace( matT, L"1", -1, 0xFFFF0000);
			else
				g_pMainApp->GetFont()->DrawTextIn3DSpace( matT, L"1" );

			matInverse._41 = t2.x;
			matInverse._42 = t2.y;
			matInverse._43 = t2.z;

			matT = matTextReflectAndScale * matInverse;

			if( GlobalObjects::g_pMainForm->Top_toolStripButton->Checked ||
				GlobalObjects::g_pMainForm->TopBottom_toolStripButton->Checked)
				g_pMainApp->GetFont()->DrawTextIn3DSpace( matT, L"2", -1, 0xFFFF0000);
			else
				g_pMainApp->GetFont()->DrawTextIn3DSpace( matT, L"2" );

			rs3::RFont::EndFont();

			//g_pMainApp->m_pTextNode2->SetPosition(t);
			//g_pMainApp->GetPaneMgr()->RenderPane();
		}
	}

	RRenderHelper::SetRenderState();
	if(bAllSelected)
	{
		RRenderHelper::RenderCapsule(cap, 0x2000FF00);
	}
	else if(bSelected)
	{
		if(opa)
			RRenderHelper::RenderCapsule(cap, m_dwColor);
		else
			RRenderHelper::RenderCapsule(cap, 0x20FF0000);
	}
	else
		RRenderHelper::RenderCapsule(cap, 0x20C0C0C0);
}

void THitAreaRenderer::AllRender( CSTalentInfo* pSkillInfo, int nSelectedSegID, bool opa )
{
	if (m_pDevice == NULL) return;
	if (pSkillInfo == NULL) return;

	RRenderHelper::SetRenderState();

	for(size_t j = 0; j < pSkillInfo->m_HitInfo.m_vSegments.size(); ++j)
	{
		if(nSelectedSegID == j)
			continue;

		for (size_t k=0; k < pSkillInfo->m_HitInfo.m_vSegments[j].m_vCapsules.size(); ++k)
		{
			MCapsule cap = pSkillInfo->m_HitInfo.m_vSegments[j].m_vCapsules[k];

			if(pSkillInfo->m_bHitCapsulePosSync && g_pMainApp->GetActor())
			{
				MMatrix matWorldModel;
				matWorldModel.SetLocalMatrix(g_pMainApp->GetActor()->GetPosition(), g_pMainApp->GetActor()->GetDirection(), g_pMainApp->GetActor()->GetUp());
				cap = cap.Multiply(matWorldModel);
			}

			RenderHitCapsule(cap, false, opa, false, true);

			RenderPlayerMoveHitArea(pSkillInfo->m_HitInfo.m_vSegments[j].m_vCapsules[k], false);
		}
	}

	RRenderHelper::EndRenderState();
}
