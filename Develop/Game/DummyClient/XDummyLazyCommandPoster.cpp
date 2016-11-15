#include "StdAfx.h"
#include "XDummyLazyCommandPoster.h"
#include "XBirdClient.h"

XDummyLazyCommandPoster::XDummyLazyCommandPoster()
{

}

XDummyLazyCommandPoster::~XDummyLazyCommandPoster()
{
}

void XDummyLazyCommandPoster::Update(float fDelta)
{
	if (true == m_vecLazyCommand.empty()) return;

	vector<VEC_LAZYCOMMAND::iterator> vecEraseItor;

	for (VEC_LAZYCOMMAND::iterator itor = m_vecLazyCommand.begin(); itor != m_vecLazyCommand.end();)
	{
		XBirdClient* pClient = itor->first;
		MCommand* pCommand = itor->second;

		if (false == IsPostable(pClient))
		{
			itor++;
			continue;
		}

		XBirdPostCommand(pClient, pCommand);
		RecordSendTime(pClient);
		itor = m_vecLazyCommand.erase(itor);
	}		
}

void XDummyLazyCommandPoster::InsertCommand(XBirdClient* pClient, MCommand* pCommand)
{
	m_vecLazyCommand.push_back(VEC_LAZYCOMMAND::value_type(pClient, pCommand));

	MAP_LASTSENDTIME::iterator itor = m_mapLastSendTime.find(pClient);
	if (itor == m_mapLastSendTime.end())
	{
		m_mapLastSendTime.insert(MAP_LASTSENDTIME::value_type(pClient, 0));
	}
}

bool XDummyLazyCommandPoster::IsPostable( XBirdClient* pClient )
{
	MAP_LASTSENDTIME::iterator itor = m_mapLastSendTime.find(pClient);
	if (itor == m_mapLastSendTime.end()) return false;

	unsigned int nLastSendTime = itor->second;
	unsigned int nNowTime = XGetNowTime();
	if (nNowTime - nLastSendTime <= 1000) return false;

	return true;
}

void XDummyLazyCommandPoster::RecordSendTime(XBirdClient* pClient)
{
	MAP_LASTSENDTIME::iterator itor = m_mapLastSendTime.find(pClient);
	if (itor == m_mapLastSendTime.end()) return;

	itor->second = XGetNowTime();
}
