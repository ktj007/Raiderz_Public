#include "stdafx.h"
#include "SDefaultDsnFactory.h"


mdb::MDatabaseDesc SDefaultDsnFactory::GetAccountDSN()
{
	return m_AccDesc;
}

mdb::MDatabaseDesc SDefaultDsnFactory::GetGameDSN()
{
	return m_GameDesc;
}

mdb::MDatabaseDesc SDefaultDsnFactory::GetLogDSN()
{
	return m_LogDesc;
}