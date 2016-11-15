#pragma once

#include "MWidget.h"
#include <vector>


using namespace std;
using namespace mint3;



/// class : XTalentSlotLook
class XTalentSlotLook : public MLookBase
{
/// Member variables
protected:
	MFont*		m_pSmallFont;
	MBitmap*	m_pGlowImage;


/// Member functions
public:
	XTalentSlotLook();

	void SetSmallFont( const char* szFont);
	MFont* GetSmallFont()							{ return ( m_pSmallFont ? m_pSmallFont : GetFont());	}

	void SetGlowImage( const char* szImage);
	MBitmap* GetGlowImage()							{ return m_pGlowImage;									}


	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;
};





/// class : XUITalentSlot
class XUITalentSlot : public MWidget
{
protected:
	vector< int>		m_TalentList;
	XTalentInfo*		m_pTalentInfo;
	bool				m_bLearned;
	bool				m_bLearnable;
	bool				m_bFocusTalent;
	int					m_nCurrRank;
	MBitmap*			m_pIconImage;
	MTimer				m_tLearning;



public:
	XUITalentSlot( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	virtual const char* GetState() override;
	virtual int GetStateNum() override;


	void SetTalent( int nRank, int nID);
	XTalentInfo* GetTalent()								{ return m_pTalentInfo;							}

	const wchar_t* GetTalentName();
	bool IsLearned() const									{ return m_bLearned;							}
	bool IsLearnable() const								{ return m_bLearnable;							}
	bool IsFocusTalent() const								{ return m_bFocusTalent;						}
	int GetCurrentRank() const								{ return m_nCurrRank;							}
	int GetMaxRank() const									{ return (int)m_TalentList.size();				}
	MBitmap* GetIconImage() const							{ return m_pIconImage;							}
	MTimer& GetLearningTimer()								{ return m_tLearning;							}

	bool UpdateInfo();

	bool DoLearnTalent();

	virtual bool IsEnableDragAndDrop() const override		{ return true;									}



protected:
	virtual bool OnDragBegin( MDragData* pDragData, MPOINT& pt) override;



public:
	int glueGetTalentID()									{ return (m_pTalentInfo) ? m_pTalentInfo->m_nID : 0; }
	const wchar_t* glueGetTalentName()							{ return GetTalentName();						}
	void glueUpdateInfo()									{ UpdateInfo();									}
	void glueDoLearnTalent()								{ DoLearnTalent();								}
	bool glueIsLearnable()									{ return IsLearnable();							}


#define MINT_TALENTSLOT		"TalentSlot"
	virtual const char* GetClassName() override				{ return MINT_TALENTSLOT;						}
};
