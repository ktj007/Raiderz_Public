#ifndef _XMODULE_UI_H
#define _XMODULE_UI_H

#include "XInteractionEffect.h"

class XNPCInfo;
class XCharacterCaption;
class XUIBalloonTextBox;
class XDebugScreenDialog;
class XMiniHpBar;
class XHeadIconPlacer;
class XCheckTargetableEffect;

class XModuleUI : public XModule, public MMemPool<XModuleUI>
{
	DECLARE_ID(XMID_UI)

protected:
	XModuleEntity*			m_pModuleEntity;

	XHeadIconPlacer*		m_pHeadIconPlacer;
	//pane
	XCharacterCaption*		m_pNameCaption;	///< 이름 표시용
	XCharacterCaption*		m_pGuildCaption;		///< 길드명 표시용
	XMiniHpBar*				m_pMiniBar;

	MRegulator				m_PaneRegulator;

	XCheckTargetableEffect*	m_pCheckTargetableEffect;	///< 인터랙션 인디케이터
	XUIBalloonTextBox*		m_pBalloonTextBox;		///< 말풍선
	XDebugScreenDialog*		m_pDebugScreenDialog;	///< npc colt 디버깅용 말풍선

	bool					m_bShowPartyMiniHpBar;

protected:
	virtual void			OnActivate();
	virtual void			OnDeactivate();

	virtual void			OnInitialized();
	virtual void			OnUpdate(float fDelta);
	virtual XEventResult	OnEvent(XEvent& msg);
	virtual void			OnSubscribeEvent();

	void			SetCaptionVisible(bool bShow);

	void			UpdateNPCPaneVisible( float fDelta, XNPCInfo* pThisNPCInfo );
	void			UpdatePlayerPaneVisible(float fDelta);
public:
	XModuleUI(XObject* pOwner=NULL);
	virtual ~XModuleUI(void);

	void			Destroy();

	void			UpdateCharacterPane(float fDelta);		// 이름 높이 위치 업데이트

	void			UpdateNPCNameCaption(wstring strTitleBoneName, float fTitleHeight);	// 이름 높이 위치 정보가 있으면...
	void			UpdatePlayerNameCaption();	// 이름 높이 위치 기본
	bool			UpdateNPCMiniHpBar();
	bool			UpdatePlayerMiniHpBar();
	void			UpdatePartyMiniHpBar();

	void			SetNameColor(DWORD dwColor);
	void			SetNPCNamePane();
	void			SetCharacterPane( wstring& strName, wstring &strClan, DWORD dwColor);
	void			SetCharacterPane_NPC(bool bShowName, wstring strName, wstring strClan, const int& nLevel, bool bMonster);


	void			SetShowPartyMiniHpBar(bool bForce) { m_bShowPartyMiniHpBar = bForce; }

	vec3			GetNamePos();

	// NPC ICon
	void			SetNPCIConState(NPC_ICON_STATE val);

	void			OnBallonText(wstring szID, wstring szChat, float fMaintainTime);
	void			OnBallonText_Debug(wstring szID, wstring szChat);
	bool			IsShowBallonText();
	float			GetBallonLifeTime(float fMaintainTime, wstring &szChat);

	bool			IsShowLootbleTargetEffect() const;

	XHeadIconPlacer* GetHeadIconPlacer()	{ return m_pHeadIconPlacer; }		// NULL일 수 있으므로 NULL체크를 해야 합니다.
};
#endif //_XMODULE_UI_H
