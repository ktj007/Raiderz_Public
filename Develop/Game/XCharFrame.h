
#pragma once

#include "XGameFrame.h"
#include "XGameFrameID.h"


using namespace rs3;


struct XCHARACTER_CREATEINFO
{
	TALENT_STYLE	nStyle;
	SEX				nSex;
	int				nFace;
	int				nHair;
	int				nSkinColor;
	int				nHairColor;
	int				nEyeColor;
	int				nVoice;
	int				nMakeUp;
	int				nTattoo;
	float			fTattooScale;
	vec2			vTattooPos;
	int				nEquipments;
	int				nEquipmentsCol;
	bool			bBattleStance;
};



/// class : XCharFrame
class XCharFrame : public XBaseFrame
{
/// Member variables
protected:
	bool			m_bLoadCampaign;
	bool			m_bUpdateOnce;
	bool			m_bLoaded;

	XNetPlayer*		m_pPlayers[ MAX_ACCOUNT_CHAR_COUNT];
	XNetPlayer*		m_pMockPlayers[ SEX_MAX];
	XNetPlayer*		m_pCurrPlayer;

	XObjectManager	m_obj;

	int				m_nButtonDown;
	MPOINT			m_ptButtonDown;

	DWORD			m_dwMotionTimer;


/// Member functions
	bool UpdateOnce( float fDelta );

	void CharBuffEnchantAllLost(XNetPlayer* pCurrPlayer);
public:
	XCharFrame();
	virtual ~XCharFrame();

	virtual bool Open( int nPrevFrameID, const wchar_t* szArgs) override;

	void Load();
	virtual void Close( int nNextFrameID) override;

	void CreatePlayers();
	void DeletePlayers();

	virtual int GetID() override				{ return CHAR_FRAME_ID;					}

	bool DispatchEvent( MEvent& e);

	void SelectChar( int nCharIndex);
	void DeleteCurrChar();
	TD_UPDATE_CACHE_PLAYER GetDefaultCharData( const XCHARACTER_CREATEINFO& ci);
	void ChangeMockChar( const XCHARACTER_CREATEINFO& ci);
	void ChangeMockCharStanceToBattle( bool bStance);
	bool IsMockCharSanceBattle();
	void RotateMockChar( float fRotate);


/// Event handler
public:
	virtual bool OnEvent( MEvent& e) override;
	virtual void Update( float fDelta) override;
	virtual void OnRender() override;
};
