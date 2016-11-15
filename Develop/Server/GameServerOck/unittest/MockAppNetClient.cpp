#include "stdafx.h"
#include "MockAppNetClient.h"
#include "GCmdHandlerGroup.h"
#include "SUnitTestUtil.h"

MockAppNetClient::MockAppNetClient( const MNetClientDesc& desc ) : MMockNetClient(desc)
{
	m_pCmdHandlerGroup = new GCmdHandlerGroup_AppNetClient(this);

	m_This = SUnitTestUtil::NewUID();
	m_Server = SUnitTestUtil::NewUID();
}

MockAppNetClient::~MockAppNetClient()
{
	SAFE_DELETE(m_pCmdHandlerGroup);
}
