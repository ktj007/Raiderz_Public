#pragma once

#include "CSTalentInfo.h"
#include "CSCommonLib.h"

class CSTalentInfoMgr;

class CSCOMMON_API CSTalentInfoExtParser
{
protected:
	void _ParseTalentEvents( MXmlElement* pElement, MXml* pXml, CSTalentInfo* pTalentInfo );
	void _ParseTalentEvent( MXmlElement* pElement, MXml* pXml, vector<CSTalentEventInfo>& vecEvents );
	void _ParseProjectile( MXmlElement* pElement, MXml* pXml, CSTalentEventInfo* pTalentEventInfo );
	void _ParseTalentActAnimationTime(MXmlElement* pElement, MXml* pXml, CSTalentInfo* pTalentInfo);

	void _ParseExt_Server( MXmlElement* pElement, MXml* pXml, CSTalentInfo* pTalentInfo );	// 서버만 사용함

	virtual void _Parse(MXmlElement* pElement, MXml* pXml, CSTalentInfo* pTalentInfo);
public:
	CSTalentInfoExtParser() {}
	virtual ~CSTalentInfoExtParser() {}

	void Parse(CSTalentInfoMgr* pTalentInfoMgr, MXml* pXml, MXmlElement* pElement, CSTalentInfo* pTalentInfo=NULL);

	void CopyTalentEventInfoByReference(CSTalentInfoMgr* pTalentInfoMgr);
	void CopyTalentEventData(CSTalentInfoMgr* pTalentInfoMgr, CSTalentInfo* pTalentInfo);
};