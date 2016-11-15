#pragma once

class XCharacterCaption;
class XMiniHpBar;

#define	CAPTION_CAP				0.5f 
#define CAPTION_MARGIN			5.0f
#define MINIBAR_MARGIN			(-2.0f)
#define NPC_ICON_MARGIN			(-5.0f)

#define PLAYER_ID_OFFSET_Z		180.0f

/// 플레이어, NPC의 머리위의 아이콘, 타이틀, 인디케이터 등의 위치를 계산해주는 클래스
class XHeadIconPlacer
{
protected:
	XCharacterCaption*		m_pNameCaption;
	XCharacterCaption*		m_pGuildCaption;
	XMiniHpBar*				m_pMiniHPBar;

	RActorNode*				m_pTitleBoneActorNode;

	MUID					m_uidNPCIConEffect;
	bool					m_bNameCaptionLastVisible;
	bool					m_bGuildCaptionLastVisible;

	virtual	float GetFontHeight();
	bool IsCaptionVisible( XCharacterCaption* pCaption );
	void NextOffset(float& foutOffset, float fFontHeight)
	{
		foutOffset += (fFontHeight + CAPTION_MARGIN);
	}
	void Refresh(XObject* pOwner);
public:
	XHeadIconPlacer(XCharacterCaption* pNameCaption, XCharacterCaption* pGuildCaption, XMiniHpBar* pMiniHPBar);
	virtual ~XHeadIconPlacer() {}

	void SetCaptionOffset(float fOffset, XCharacterCaption* pNameCaption, XCharacterCaption* pGuildCaption, XMiniHpBar* pMiniHPBar);

	RActorNode* FindTitleBone(XObject* pOwner);
	void MakeNPCIconLocalMatrix(RMatrix& matOut, float fTitleHeight, vec3& vDirection);
	bool MakeIndicatorLocalMatrix( RMatrix& matOut, XObject* pOwner);

	void UpdatePlayerNameCaption();
	void UpdateNPCNameCaption(XCharacter* pActor, wstring strTitleBoneName, float fTitleHeight, float fScale);
	void CheckRefresh(XObject* pOwner);

	bool IsHeadIcon()	{ return (m_uidNPCIConEffect != MUID::ZERO) ? true : false;}
	void SetNPCIConEffectUID(MUID uidEffect);
};

