#pragma once

#include "MTypes.h"
#include "MUID.h"
#include "STypes.h"
#include "STransData_M2G.h"

class MockLink;
class MockLoginServerObject;

class ZTestHelper
{
protected:
	static int			m_IDGenerator;
	static MUID			m_uidGenerator;
	static AID			m_AIDGenerator;
	static CID			m_CIDGenerator;
	static int			m_NameGenerator;
	static int			m_StringGenerator;
	static int			m_Number;

public:
	// net ----------------
	static MockLink* NewLink(MUID uidNew=MUID::Invalid(), bool bIgnoreAllSendCommandID = false);
	static void ClearLinks();
	static int NewID();
	static MUID NewUID();
	static AID NewAID();
	static CID NewCID();
	static wstring NewName();
	static int NewNumber();
	static wstring NewString();

	// Object
	static MockLoginServerObject* ZTestHelper::NewLoginServerObject(int nServerID, MUID uidNew=MUID::ZERO);

	// field --------------
	static TD_GATE_FIELD_GROUP_INFO MakeGateFieldGroupInfo(int nFieldGroupID, int nFieldID, DYNAMIC_FIELD_TYPE eType);
	static TD_GATE_INFO MakeGateInfo_SharedField(int nFieldID, int nChannelID);
};
