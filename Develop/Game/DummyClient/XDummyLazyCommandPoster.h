#pragma once

#include "XBirdClient.h"

class XBirdClient;

#define XLAZYCMDPOST0(_CLIENT, _ID)					{ MCommand* pCommand; XBIRDNEWCMD0(pCommand, _CLIENT, _ID); XDummyLazyCommandPoster::GetInstance().InsertCommand(_CLIENT, pCommand);}
#define XLAZYCMDPOST1(_CLIENT, _ID, _P1)			{ MCommand* pCommand; XBIRDNEWCMD1(pCommand, _CLIENT, _ID, _P1); XDummyLazyCommandPoster::GetInstance().InsertCommand(_CLIENT, pCommand);}
#define XLAZYCMDPOST2(_CLIENT, _ID, _P1, _P2)		{ MCommand* pCommand; XBIRDNEWCMD2(pCommand, _CLIENT, _ID, _P1, _P2); XDummyLazyCommandPoster::GetInstance().InsertCommand(_CLIENT, pCommand);}
#define XLAZYCMDPOST3(_CLIENT, _ID, _P1, _P2, _P3)	{ MCommand* pCommand; XBIRDNEWCMD3(pCommand, _CLIENT, _ID, _P1, _P2, _P3); XDummyLazyCommandPoster::GetInstance().InsertCommand(_CLIENT, pCommand);}

typedef vector<pair<XBirdClient*,MCommand*>> VEC_LAZYCOMMAND;
typedef map<XBirdClient*, unsigned int>	MAP_LASTSENDTIME;

class XDummyLazyCommandPoster : public MSingleton<XDummyLazyCommandPoster>
{
public:
	XDummyLazyCommandPoster();
	~XDummyLazyCommandPoster();

	void InsertCommand(XBirdClient* pClient, MCommand* pCommand);
	void Update(float fDelta);

private:
	bool IsPostable(XBirdClient* pClient);
	void RecordSendTime(XBirdClient* pClient);

private:
	VEC_LAZYCOMMAND		m_vecLazyCommand;
	MAP_LASTSENDTIME	m_mapLastSendTime;	
};