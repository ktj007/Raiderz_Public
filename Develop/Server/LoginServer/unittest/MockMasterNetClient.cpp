#include "StdAfx.h"
#include "MockMasterNetClient.h"
#include "LCmdHandlerGroup.h"
#include "LUnitTestUtil.h"

MockMasterNetClient::MockMasterNetClient(const MNetClientDesc& desc)
:MMockNetClient(desc)
{
	m_pCmdHandlerGroup = new LCmdHandlerGroup_MasterNetClient(this);

	m_This = LUnitTestUtil::NewUID();
	m_Server = LUnitTestUtil::NewUID();
}

MockMasterNetClient::~MockMasterNetClient(void)
{
	SAFE_DELETE(m_pCmdHandlerGroup);
}
