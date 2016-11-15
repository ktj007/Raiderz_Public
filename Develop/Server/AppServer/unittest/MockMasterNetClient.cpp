#include "StdAfx.h"
#include "MockMasterNetClient.h"
#include "PCmdHandler_Master_Global.h"
#include "SUnitTestUtil.h"


MockMasterNetClient::MockMasterNetClient( const MNetClientDesc& desc ) : MMockNetClient(desc)
{
	m_pGlobalCmdHandler = new PCmdHandler_Master_Global(this);

	m_This = SUnitTestUtil::NewUID();
	m_Server = SUnitTestUtil::NewUID();
}

MockMasterNetClient::~MockMasterNetClient()
{
	SAFE_DELETE(m_pGlobalCmdHandler);
}
