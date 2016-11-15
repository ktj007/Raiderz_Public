#include "stdafx.h"
#include "ZTestHelper.h"
#include "MockLink.h"
#include "MockNetServer.h"
#include "ZGlobal.h"
#include "ZCommandCenter.h"
#include "ZLoginServerFacade.h"

int ZTestHelper::m_IDGenerator = 0;
MUID ZTestHelper::m_uidGenerator = MUID(2545425, 0);
AID ZTestHelper::m_AIDGenerator = 0;
CID ZTestHelper::m_CIDGenerator = 0;
int ZTestHelper::m_Number = 0;
int ZTestHelper::m_NameGenerator = 0;
int ZTestHelper::m_StringGenerator = 0;

MockLink* ZTestHelper::NewLink( MUID uidNew, bool bIgnoreAllSendCommandID /*= false*/ )
{
	if (uidNew.IsInvalid()) uidNew = NewUID();

	MockNetServer* pMockNetServer = dynamic_cast<MockNetServer*>(gsys.pCommandCenter->GetNetServer());
	if (!pMockNetServer) return NULL;

	MockLink* pNewMockLink = dynamic_cast<MockLink*>(pMockNetServer->AddMockLink(uidNew));
	if (!pNewMockLink) return NULL;

	if (bIgnoreAllSendCommandID)
	{
		pNewMockLink->IgnoreAll();
	}

	return pNewMockLink;

}

void ZTestHelper::ClearLinks()
{
	MockNetServer* pMockNetServer = dynamic_cast<MockNetServer*>(gsys.pCommandCenter->GetNetServer());
	if (!pMockNetServer) return;

	pMockNetServer->ClearMockLinks();
}

int ZTestHelper::NewID()
{
	return (++m_IDGenerator);
}

MUID ZTestHelper::NewUID()
{
	return (++m_uidGenerator);
}

AID ZTestHelper::NewAID()
{
	return (++m_AIDGenerator);
}

CID ZTestHelper::NewCID()
{
	return (++m_CIDGenerator);
}

wstring ZTestHelper::NewName()
{
	m_NameGenerator++;

	CString strName;
	strName.Format(L"%d", m_NameGenerator);

	return strName.GetString();
}

int ZTestHelper::NewNumber()
{
	return (++m_Number);
}

wstring ZTestHelper::NewString()
{
	m_StringGenerator++;

	CString strString;
	strString.Format(L"%d", m_StringGenerator);

	return strString.GetString();
}

TD_GATE_FIELD_GROUP_INFO ZTestHelper::MakeGateFieldGroupInfo(int nFieldGroupID, int nFieldID, DYNAMIC_FIELD_TYPE eType)
{
	vec3 vPosition(100, 200, 300);
	vec3 vDirection(0, 1, 0);

	TD_GATE_FIELD_GROUP_INFO GateFieldGroupInfo;
	GateFieldGroupInfo.nFieldGroupID = nFieldGroupID;
	GateFieldGroupInfo.nType = (int8)eType;
	GateFieldGroupInfo.isSingleType = false;
	GateFieldGroupInfo.GateInfo.nFieldID = nFieldID;
	GateFieldGroupInfo.GateInfo.vPosition = vPosition;
	GateFieldGroupInfo.GateInfo.vDirection = vDirection;
	GateFieldGroupInfo.GateInfo.bDynamicField = true;

	return GateFieldGroupInfo;
}

TD_GATE_INFO ZTestHelper::MakeGateInfo_SharedField(int nFieldID, int nChannelID)
{
	vec3 vPosition(100, 200, 300);
	vec3 vDirection(0, 1, 0);

	TD_GATE_INFO GateInfo;
	GateInfo.nFieldID = nFieldID;
	GateInfo.vPosition = vPosition;
	GateInfo.vDirection = vDirection;
	GateInfo.bDynamicField = false;	
	GateInfo.nChannelID = nChannelID;

	return GateInfo;
}