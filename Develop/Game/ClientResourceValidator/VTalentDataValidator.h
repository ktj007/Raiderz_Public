#pragma once

#include "VDataValidator.h"

class VTalentDataValidator;

typedef void(*CHECKTALENTEVENTHANDLER) (VTalentDataValidator&, CSTalentEventInfo *, VValidatorInfoMgr *, string * );

class VTalentDataValidator : VDataValidator
{
private:
	int									m_nTalentCount;

public:
	VErrorMsg							m_TalentEventValidatorError;
	VErrorMsg							m_TalentHitInfoValidatorError;

private:
	void								CheckEventGo(CSTalentEventInfo * pEvent, VValidatorInfoMgr* vInfoMg, string* pStrErrorPre);

public:
	VTalentDataValidator();
	virtual ~VTalentDataValidator();

	static map<TALENT_EVENT, CHECKTALENTEVENTHANDLER> m_CheckTalentEventHandlers;

	virtual void						Init(VValidatorInfoMgr* vInfoMgr);
	virtual	void						CheckStart(VValidatorInfoMgr* vInfoMgr);

	void								CheckTalentInfoXml();
	void								CheckTalentEvent(XTalentInfoMgr* pTalentInfoMgr, XTalentInfo* pTalentInfo, VValidatorInfoMgr* vInfoMgr);
	void								CheckTalentHitInfo(XTalentInfoMgr* pTalentInfoMgr, XTalentInfo* pTalentInfo, VValidatorInfoMgr* vInfoMgr);

	virtual int							GetCheckCount() { return m_nTalentCount; }

	static void							OnCheckTalentEvent_None(VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre);
	static void							OnCheckTalentEvent_FireEffect(VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre);
	static void							OnCheckTalentEvent_Projectile(VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre);
	static void							OnCheckTalentEvent_Effect(VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre);
	static void							OnCheckTalentEvent_Sound(VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre);
	static void							OnCheckTalentEvent_Camera(VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre);
	static void							OnCheckTalentEvent_Custom(VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre);
	static void							OnCheckTalentEvent_Delayed_Act(VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre);
	static void							OnCheckTalentEvent_ExtraActive_1(VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre);
	static void							OnCheckTalentEvent_ExtraActive_2(VTalentDataValidator& talentEventValidator, CSTalentEventInfo * pEvent, VValidatorInfoMgr * vInfoMgr, string* pStrErrorPre);
};
