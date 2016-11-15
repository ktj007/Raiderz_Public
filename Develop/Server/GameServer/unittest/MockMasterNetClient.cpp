#include "stdafx.h"
#include "MockMasterNetClient.h"
#include "GCmdHandlerGroup.h"
#include "SUnitTestUtil.h"

MockMasterNetClient::MockMasterNetClient( const MNetClientDesc& desc ) : MMockNetClient(desc)
{
	m_pCmdHandlerGroup = new GCmdHandlerGroup_MasterNetClient(this);

	m_This = SUnitTestUtil::NewUID();
	m_Server = SUnitTestUtil::NewUID();
}

MockMasterNetClient::~MockMasterNetClient()
{
	SAFE_DELETE(m_pCmdHandlerGroup);
}
