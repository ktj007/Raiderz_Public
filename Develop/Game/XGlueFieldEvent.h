#ifndef _XGLUE_FIELD_EVENT_H
#define _XGLUE_FIELD_EVENT_H

#include "XUISystem.h"

class XFieldInfo;
class XObject;

class XGlueFieldEvent
{
private:
	MWLua *					m_pMWLua;
	bool PreCheck();

public:
	XGlueFieldEvent();
	void Init(MWLua* pLua);

	void SetNULL( XFieldInfo* pFieldInfo );

	void Load(XFieldInfo* pFieldInfo);
	bool HasSensorScript(int nFieldID, int nSensorID);

	bool OnSensorEnter(XFieldInfo* pField, int nSensorID, XObject* pActor);
	bool OnSensorLeave(XFieldInfo* pField, int nSensorID, XObject* pActor);
	//bool OnSensorInteract(XFieldInfo* pField, int nSensorID, XObject* pPlayer);
	//bool OnSensorTalent(XFieldInfo* pField, int nSensorID, XObject* pActor, int nTalentID);

	bool OnAreaEnter( XFieldInfo* pField, int nAreaID);
	bool OnAreaLeave( XFieldInfo* pField, int nAreaID);
};


#endif 