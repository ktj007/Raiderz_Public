#include "stdafx.h"
#include "FBaseNetClient.h"
#include "XNetwork.h"
#include "XGlobal.h"


FBaseNetClient::FBaseNetClient() : m_pNetClient(NULL), m_pNetAgent(NULL)
{
	if (global.net)
	{
		m_pNetClient = global.net->GetClient();
		m_pNetAgent = new minet::MTestNetAgent(m_pNetClient);
	}
}

FBaseNetClient::~FBaseNetClient()
{
	SAFE_DELETE(m_pNetAgent);
}