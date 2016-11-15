#include "StdAfx.h"
#include "TMeshColInfoBoxEditor.h"
#include "RRenderHelper.h"
#include "toolPick.h"
#include "RSUtil.h"

#define MESH_COL_BOX_SELECT				0x20FF0000
#define MESH_COL_BOX_NONE_SELECT		0x20C0C0C0

TMeshColInfoBoxEditor::TMeshColInfoBoxEditor() 
: m_bSelectBox(false)
, m_nSelectedFaceIndex(-1)
, m_nSelectedBox(-1)
{
	InitSelectBox();
}

void TMeshColInfoBoxEditor::CalAnimationRenderBox( CSColBox * pColBox, CSColBox & outBox )
{
	RMatrix matActor			= g_pMainApp->GetActor()->GetWorldTransform();

	outBox.m_matWorld = pColBox->m_matWorld * matActor;
}

void TMeshColInfoBoxEditor::RenderColBox( int nSelectColID )
{
	CSMeshInfo * pMeshInfo = TMeshInfoMgr::GetInstance().GetMeshInfo(m_strModelName);

	if(m_strModelName.empty() ||
		pMeshInfo == NULL)
	{
		InitSelectBox();
		return;
	}

	m_bSelectBox = false;
	CSColBox renderBox;
	int nCIndex = COLLISION_BOX_START_INDEX;

	for(vector<CSColBox>::iterator itBox = pMeshInfo->m_vecColBox.begin();
		itBox != pMeshInfo->m_vecColBox.end(); itBox++)
	{
		renderBox = *itBox;

		// 애니메이션이 가능하게 본이 설정되어 있는가?
		CalAnimationRenderBox(&(*itBox), renderBox);

		// 캡슐 선택이라면... 선택 캡슐 이외는 회색으로
		if(nCIndex == nSelectColID)
		{
			m_bSelectBox = true;
			m_selectBox = *itBox;
			m_nSelectedBox = nSelectColID;
		}
		else
		{
			RenderBox(renderBox, MESH_COL_BOX_NONE_SELECT);
		}

		nCIndex++;
	}

	if(m_bSelectBox)
	{
		renderBox = m_selectBox;
		CalAnimationRenderBox(&m_selectBox, renderBox);
		RenderEditionBox(renderBox, MESH_COL_BOX_SELECT);
	}
	else 
		InitSelectBox();

}

void TMeshColInfoBoxEditor::RenderBox( CSColBox & boxInfo, DWORD dwColor /*= 0x20ff00ff*/ )
{
	g_pMainApp->SetCapsuleRenderState();

	RRenderHelper::RenderBox(boxInfo.m_matWorld, boxInfo, dwColor);

	g_pMainApp->EndCapsuleRenderState();

}

void TMeshColInfoBoxEditor::RenderEditionBox( CSColBox & boxInfo, DWORD dwColor )
{
	// 부분 판정 정보를 가져온다.
	CSColBox box = boxInfo;

	//------------------------------------------------------------------------
	// 캡슐 렌더링
	RenderBox(box, dwColor);

	g_pMainApp->SetCapsuleRenderState();
	RRenderHelper::RenderAxis(boxInfo.m_matWorld, 30);
	//------------------------------------------------------------------------
	// 박스
	vec3 vOrgCenter = box.GetCenter();

	// +x축
	m_Face[0].RenderFace(box.maxx - vOrgCenter.x, vOrgCenter, box.m_matWorld);
	// -x축
	m_Face[1].RenderFace(vOrgCenter.x - box.minx, vOrgCenter, box.m_matWorld);

	// +y축
	m_Face[2].RenderFace(box.maxy - vOrgCenter.y, vOrgCenter, box.m_matWorld);
	// -y축
	m_Face[3].RenderFace(vOrgCenter.y - box.miny, vOrgCenter, box.m_matWorld);

	// +z축
	m_Face[4].RenderFace(box.maxz - vOrgCenter.z, vOrgCenter, box.m_matWorld);
	// -z축
	m_Face[5].RenderFace(vOrgCenter.z - box.minz, vOrgCenter, box.m_matWorld);

	g_pMainApp->EndCapsuleRenderState();
}

void TMeshColInfoBoxEditor::RenderEditionBoxFace( vec3 vAxis, float fAxisLen, vec3& vCenter, MMatrix& matWorld, DWORD dwColor )
{
	vec3 vX = vAxis;
	matWorld.TransformVect(vAxis, vX);
	vX.Normalize();
	vec3 vStart = vCenter + (vX * fAxisLen);
	vec3 vEnd = vStart + (vX * 50.0f);

	RRenderHelper::RenderLine(RMatrix::IDENTITY, vStart, vEnd, 0xFFFF00FF);
	RRenderHelper::RenderCorn(vEnd, vX, 5.0f, 10.0f, 0xFFFF00FF);


	RBoundingBox box = RBoundingBox(RVector(-5,0,-5),RVector(5,10,5));
	RMatrix matLocal;
	matLocal.SetLocalMatrix(vCenter + (vX * (50.0f + fAxisLen)), vX, vec3(0,0,1));

	RRenderHelper::RenderBox(matLocal, box, 0xffffffff);
}

bool TMeshColInfoBoxEditor::CheckMousePick( int stat, RVector2& pos, RVector2 prePos )
{
	if(m_bSelectBox == false || 
		(stat == 1 && m_nSelectedFaceIndex == -1))		// 마우스를 눌렀는데... 현재 선택된 상태가 아니면... 리턴... 선택된 이후에 들어와라...
		return false;

	MRect rect;
	rect.left = 0;
	rect.right = REngine::GetConfig().m_nWidth;
	rect.top = 0;
	rect.bottom = REngine::GetConfig().m_nHeight;

	float fMinDist = FLT_MAX;
	int nSelectIndex = -1;
	for(int i = 0; i < BOX_EDITOR_FACE_COUNT; ++i)
	{
		m_Face[i].NonSelect();
		if(m_Face[i].PickTest(pos, rect, fMinDist))
			nSelectIndex = i;
	}

	if(m_nSelectedFaceIndex == -1 && nSelectIndex != -1)
	{
		//if(stat == 1)
		//{
		//	m_Face[nSelectIndex].Selected();
		//}
		//else
		{
			m_Face[nSelectIndex].CheckSelect();
			m_nSelectedFaceIndex = -1;
		}
	}

	if(stat == 1 && m_nSelectedFaceIndex != -1)
	{
		m_Face[m_nSelectedFaceIndex].Selected();

		// 크기 조절부분이다.
		vec3 vMoveValue = m_Face[m_nSelectedFaceIndex].GetMoveValue(pos - prePos);
		CSColBox* pColBox = GetColBox(m_nSelectedBox);
		if(pColBox)
		{
			if(m_Face[m_nSelectedFaceIndex].CheckVauleType())
			{
				// max값
				pColBox->vmax += CalEditionBoxFaceMoveData(pColBox->vmin, pColBox->vmax, vMoveValue, false);
			}
			else
			{
				pColBox->vmin += CalEditionBoxFaceMoveData(pColBox->vmin, pColBox->vmax, vMoveValue, true);
			}
		}

		return true;
	}
	else
	{
		m_nSelectedFaceIndex = -1;
	}

	return false;
}

void TMeshColInfoBoxEditor::InitSelectBox()
{
	m_selectBox.m_matWorld.MakeIdentity();
	m_selectBox.vmin = vec3::ZERO;
	m_selectBox.vmax = vec3::ZERO;

	m_bSelectBox = false;
	m_nSelectedBox = -1;

	m_Face[0].Init(vec3(1,0,0));
	m_Face[1].Init(vec3(-1,0,0));
	m_Face[2].Init(vec3(0,1,0));
	m_Face[3].Init(vec3(0,-1,0));
	m_Face[4].Init(vec3(0,0,1));
	m_Face[5].Init(vec3(0,0,-1));
}

bool TMeshColInfoBoxEditor::CheckMouseDown( int stat, RVector2& pos, RVector2 prePos )
{
	if(m_bSelectBox == false)
		return false;

	MRect rect;
	rect.left = 0;
	rect.right = REngine::GetConfig().m_nWidth;
	rect.top = 0;
	rect.bottom = REngine::GetConfig().m_nHeight;

	float fMinDist = FLT_MAX;
	m_nSelectedFaceIndex = -1;
	int nSelectIndex = -1;
	for(int i = 0; i < BOX_EDITOR_FACE_COUNT; ++i)
	{
		if(m_Face[i].PickTest(pos, rect, fMinDist))
			nSelectIndex = i;
	}

	// 선택하는가?
	if(nSelectIndex != -1 && stat == 1)
	{
		m_Face[nSelectIndex].Selected();
		m_nSelectedFaceIndex = nSelectIndex;

		return true;
	}

	return false;
}

CSColBox* TMeshColInfoBoxEditor::GetColBox( int nIndex )
{
	CSMeshInfo * pMeshInfo = TMeshInfoMgr::GetInstance().GetMeshInfo(m_strModelName);

	if(m_strModelName.empty() ||
		pMeshInfo == NULL)
	{
		return NULL;
	}
	int nCIndex = COLLISION_BOX_START_INDEX;

	for(vector<CSColBox>::iterator itBox = pMeshInfo->m_vecColBox.begin();
		itBox != pMeshInfo->m_vecColBox.end(); itBox++)
	{
		// 캡슐 선택이라면... 선택 캡슐 이외는 회색으로
		if(nCIndex == nIndex)
		{
			return &(*itBox);
		}

		++nCIndex;
	}

	return NULL;
}

bool TMeshColInfoBoxEditor::CheckEditionBoxFaceDataMax( vec3& v1, vec3& v2 )
{
	if(v1.x <= v2.x ||
		v2.y <= v2.y ||
		v2.z <= v2.z)
	{
		return false;
	}

	return true;
}

vec3 TMeshColInfoBoxEditor::CalEditionBoxFaceMoveData( vec3 vmin, vec3 vmax, vec3 vMove, bool bMinCal )
{
	vec3 vCalMin = vmin;
	vec3 vCalMax = vmax;
	
	float fCorrectMin = 0.0f;
	float fCorrectMax = 0.0f;
	if(bMinCal)
	{
		vCalMin += vMove;
		fCorrectMin = -1.0f;
	}
	else
	{
		vCalMax += vMove;
		fCorrectMax = -1.0f;
	}

	if(vCalMin.x + fCorrectMin > vCalMax.x + fCorrectMax ||
		vCalMin.y + fCorrectMin > vCalMax.y + fCorrectMax ||
		vCalMin.z + fCorrectMin > vCalMax.z + fCorrectMax)
	{
		vec3 vTemp = vMove;
		vTemp.Normalize();
		if(vTemp.x < 0.0f || vTemp.y < 0.0f || vTemp.z < 0.0f)
			vTemp *= -1.0f;

		vec3 vTempMove = -vMove;
		vTempMove.Normalize();

		vCalMin = vmin * vTemp;
		vCalMax = vmax * vTemp;

		vMove = ((vCalMin + vTempMove) - vCalMax);
	}

	return vMove;
}

//////////////////////////////////////////////////////////////////////////

TMeshColInfoBoxEditor::TBoxEditorFace::TBoxEditorFace()
: m_vAxis(vec3::ZERO)
, m_bCheckSelect(false)
, m_bSelected(false)
{

}

void TMeshColInfoBoxEditor::TBoxEditorFace::Init( vec3 vAxis )
{
	m_vAxis = vAxis;
	m_bCheckSelect = false;
	m_bSelected = false;
	m_boxPick.vmin = vec3::ZERO;
	m_boxPick.vmax = vec3::ZERO;
}

void TMeshColInfoBoxEditor::TBoxEditorFace::RenderFace(float fAxisLen, vec3& vOrgCenter, MMatrix& matWorld )
{
	// 축이므로 회전값만 적용한다.
	MMatrix matRoatation = matWorld;
	matRoatation.SetTranslation(vec3::ZERO);
	vec3 vX = m_vAxis;
	matRoatation.TransformVect(m_vAxis, vX);

	// 센터위치 계산
	vX.Normalize();
	vec3 vCenter = vec3::ZERO;
	matWorld.TransformVect(vOrgCenter, vCenter);

	m_vViewStart = vCenter + (vX * fAxisLen);
	m_vViewEnd = m_vViewStart + (vX * 50.0f);

	DWORD dwColor = 0xFFFFFFFF;
	if(m_bSelected)
		dwColor = 0xFFFF00FF;
	else if(m_bCheckSelect)
		dwColor = 0xFF1485CF;

	RRenderHelper::RenderLine(RMatrix::IDENTITY, m_vViewStart, m_vViewEnd, dwColor);
	RRenderHelper::RenderCorn(m_vViewEnd, vX, 5.0f, 10.0f, dwColor);

	// Pick용으로 쓰일 박스를 셋팅합니다. 
	RMatrix matLocal;
	matLocal.SetLocalMatrix(vOrgCenter + (m_vAxis * fAxisLen), m_vAxis, vec3(0,0,1));

	matLocal.TransformVect(RVector(-5,0,-5), m_boxPick.vmin);
	matLocal.TransformVect(RVector(5,60,5), m_boxPick.vmax);

	m_boxPick.m_matWorld = matWorld;

	// 확인용
	//RRenderHelper::RenderBox(matWorld, m_boxPick, 0xffffffff);
}

bool TMeshColInfoBoxEditor::TBoxEditorFace::PickTest( RVector2& pos, MRect& rect, float& fMinDist )
{
	float fDist = 0.0f;
	if(CToolPick::AABBPicking(m_boxPick, MPoint(pos.x, pos.y), rect, m_boxPick.m_matWorld, g_pMainApp->GetDevice()->GetTransform(RST_VIEW), g_pMainApp->GetDevice()->GetTransform(RST_PROJECTION), fDist))
	{
		if(fMinDist > fDist)
		{
			fMinDist = fDist;
			return true;
		}
	}

	return false;
}

void TMeshColInfoBoxEditor::TBoxEditorFace::CheckSelect()
{
	m_bCheckSelect = true;
	m_bSelected = false;
}

void TMeshColInfoBoxEditor::TBoxEditorFace::Selected()
{
	m_bCheckSelect = false;
	m_bSelected = true;
}

void TMeshColInfoBoxEditor::TBoxEditorFace::NonSelect()
{
	m_bCheckSelect = false;
	m_bSelected = false;
}

vec3 TMeshColInfoBoxEditor::TBoxEditorFace::GetMoveValue( vec2 vMove )
{
	float fMoveLen = vMove.Length();

	vec3 vStart = m_vViewStart;
	vec3 vEnd = m_vViewEnd;
	//MMatrix matRotation = m_boxPick.m_matWorld;
	//matRotation.SetTranslation(vec3::ZERO);
	//matRotation.TransformVect(m_vViewStart, vStart);
	//matRotation.TransformVect(m_vViewEnd, vEnd);

	vEnd.x *= -1.0f;
	vEnd.z *= -1.0f;

	vec2 vStartAxis2D;
	vec2 vEndAxis2D;
	GetScreenFromWorld(&vStartAxis2D, &vStart, &REngine::GetDevice());
	GetScreenFromWorld(&vEndAxis2D, &vEnd, &REngine::GetDevice());
	vec2 vAxis2D = vEndAxis2D - vStartAxis2D;
	vAxis2D.Normalize();
	vMove.Normalize();

	if(vAxis2D.DotProduct(vMove) <  0)
	{
		fMoveLen *= -1.0f;
	}

	return m_vAxis * fMoveLen;


	//RECT rt;
	//rt.bottom = rect.bottom;
	//rt.left = rect.left;
	//rt.right = rect.right;
	//rt.top = rect.top;

	//vec3 vMove = CToolPick::CalObejectMovefrom2D(vPrePos, vPos, m_vAxis, m_boxPick.m_matWorld.GetTranslation(), rt, g_pMainApp->GetDevice(), g_pMainApp->GetCamera()->GetPosition(), g_pMainApp->GetCamera()->GetDirection());
	//fMoveLen = vMove.Length();

	//vMove.Normalize();
	//if(m_vAxis.DotProduct(vMove) <= 0)
	//	fMoveLen *= -1.0f;

	//return m_vAxis * fMoveLen;
}

bool TMeshColInfoBoxEditor::TBoxEditorFace::CheckVauleType()
{
	// min, max 값인지 체크
	// max면 true

	if(m_vAxis.x > 0 || m_vAxis.y > 0 || m_vAxis.z > 0)
		return true;

	return false;
}
