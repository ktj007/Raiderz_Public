#pragma once

#include "XTalentInfo.h"
#include "XTalentEffectInfo.h"

//----------------------------------------------------------------------------//
// Åø Àü¿ë TalentEventMgr Editor                                                                     
//----------------------------------------------------------------------------//
enum TALENT_EVENT_STARTTYPE
{
	TES_NORMAL		= 0,
	TES_ACT,
	TES_MAX
};

class TTalentEventMgr
{
private:
	void								SetTalentXmlElement( CSTalentInfo * pTalentInfo, MXmlElement * pMaietElement );

	bool								Save(const char* szFileName);
	void								SaveEvent(MXmlElement * pParentElement, CSTalentEvents & pEvent);
	void								SaveEventData(MXmlElement * pParentElement, CSTalentEventInfo* pEventInfo);
	void								SaveProjectile(MXmlElement * pParentElement, CSTalentEventInfo* pEventInfo);
	void								SaveTalentActAnimationTime(CSTalentInfo * pTalentInfo, MXmlElement * pMaietElement);

public:
	TTalentEventMgr();
	virtual ~TTalentEventMgr();

	int									m_nMaxNormalIndex;
	int									m_nMaxActIndex;

	bool								Save();
	bool								AddTalentEventInfo(CSTalentInfo* pTalentInfo, CSTalentEventInfo& talentEvnetInfo, TALENT_EVENT_STARTTYPE eStartType);
	bool								CreateTalentEventInfo(CSTalentInfo* pTalentInfo, float fStartTime, TALENT_EVENT_STARTTYPE nStartType);
	bool								DeleteTalentEventInfo(CSTalentInfo* pTalentInfo, int nIndex, TALENT_EVENT_STARTTYPE nStartType);

	CSTalentEvents*						GetTalentEvent(CSTalentInfo* pTalentInfo);
	CSTalentEventInfo *					GetTalentEventInfo(CSTalentInfo* pTalentInfo, int nIndex, TALENT_EVENT_STARTTYPE nStartType);
	int									GetCount(CSTalentInfo* pTalentInfo, TALENT_EVENT_STARTTYPE nStartType);

};
//////////////////////////////////////////////////////////////////////////

