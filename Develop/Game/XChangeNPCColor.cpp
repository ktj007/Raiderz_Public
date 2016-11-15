#include "stdafx.h"
#include "XChangeNPCColor.h"
#include "XCharacter.h"
#include "XNaming.h"

#define TEX_COLOR_CHANGE_TIME 1.0f

XChangeNPCColor::XChangeNPCColor()
: m_bUse(false)
{
	m_fElapsedTime = 0.0f;

	m_colorTexChange			= 0xffffffff;
	m_colorSkinChange			= 0xffffffff;
	m_colorHairChange			= 0xffffffff;
}

XChangeNPCColor::~XChangeNPCColor()
{

}

void XChangeNPCColor::Init( XCharacter* pActor )
{
	m_pActor = pActor;
}

void XChangeNPCColor::Start( CHANGE_NPC_COLOR npcColor )
{
	m_NPCColor = npcColor;

	// 만약에... 현재 작동중인데... 스타트를 하신다면...
	if(m_bUse)
	{
		// 변하는중의 색깔부터 시작한다.
		m_NPCColor.colorTexSource = m_colorTexChange;		
		m_NPCColor.colorSkinSource = m_colorSkinChange;		
		m_NPCColor.colorHairSource = m_colorHairChange;		
	}
	
	m_fElapsedTime = 0.0f;
	m_bUse = true;
}

void XChangeNPCColor::Update( float fDelta )
{
	if(m_bUse == false || m_pActor == NULL || m_pActor->IsLoadingCompleted() == false)
		return;

	m_fElapsedTime += fDelta;

	float t = (m_fElapsedTime / TEX_COLOR_CHANGE_TIME);
	if (t > 1.0f) 
	{
		t = 1.0f;
		m_bUse = false;
	}

	// 텍스 컬러
	m_colorTexChange = GetColorInterpolation(m_NPCColor.colorTexSource, m_NPCColor.colorTexTarget, t);
	m_pActor->SetColor(m_colorTexChange.GetARGB());

	// 스킨 컬러(원본 사용)
	m_colorSkinChange = GetColorInterpolation(m_NPCColor.colorSkinSource, m_NPCColor.colorSkinTarget, t);
	SetSkin(m_pActor, m_colorSkinChange.GetARGB());

	// 헤어 컬러(원본 사용)
	m_colorHairChange = GetColorInterpolation(m_NPCColor.colorHairSource, m_NPCColor.colorHairTarget, t);
	SetHair(m_pActor, m_colorHairChange.GetARGB());
}

MCOLOR XChangeNPCColor::GetColorInterpolation(const MCOLOR& colorSource, const MCOLOR& colorTarget, float t)
{
	MCOLOR colorChnage;

	colorChnage.a = MMath::LinearInterpolation(colorSource.a, colorTarget.a, t);
	colorChnage.r = MMath::LinearInterpolation(colorSource.r, colorTarget.r, t);
	colorChnage.g = MMath::LinearInterpolation(colorSource.g, colorTarget.g, t);
	colorChnage.b = MMath::LinearInterpolation(colorSource.b, colorTarget.b, t);

	return colorChnage;
}

void XChangeNPCColor::SetSkin( XCharacter* pActor, unsigned int nColor )
{
	RActorNode* pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(XNaming::GetPartsNodeName(ITEMSLOT_HEAD)).c_str());
	if (pActorNode) pActorNode->SetPartsColor(nColor);
	pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(XNaming::GetPartsNodeName(ITEMSLOT_FACE)).c_str());
	if (pActorNode) pActorNode->SetPartsColor(nColor);
	pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(XNaming::GetPartsNodeName(ITEMSLOT_BODY)).c_str());
	if (pActorNode) pActorNode->SetPartsColor(nColor);
	pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(XNaming::GetPartsNodeName(ITEMSLOT_HANDS)).c_str());
	if (pActorNode) pActorNode->SetPartsColor(nColor);
	pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(XNaming::GetPartsNodeName(ITEMSLOT_LEG)).c_str());
	if (pActorNode) pActorNode->SetPartsColor(nColor);
	pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(XNaming::GetPartsNodeName(ITEMSLOT_FEET)).c_str());
	if (pActorNode) pActorNode->SetPartsColor(nColor);

}

void XChangeNPCColor::SetHair( XCharacter* pActor, unsigned int nColor )
{
	RActorNode* pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(XNaming::HairNodeName()).c_str());
	if (pActorNode) pActorNode->SetPartsColor(nColor);
}

void XChangeNPCColor::SetEye( XCharacter* pActor, unsigned int nColor )
{
	RActorNode* pActorNode = pActor->GetActorNode(MLocale::ConvUTF16ToAnsi(XNaming::EyeNodeName()).c_str());
	if (pActorNode) pActorNode->SetPartsColor(nColor);
}
