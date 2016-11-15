#pragma once

#include "GSharedField.h"
#include "GWeatherMgr.h"
#include "MMemPool.h"

class GSharedFieldNavigator;
class GFieldMgr;	// 삭제 되어야함

typedef map<CHANNELID, GSharedField*> SHAREDFIELD_MAP;



//////////////////////////////////////////////////////////////////////////
// 
// GSharedFieldMaster
//
//////////////////////////////////////////////////////////////////////////

class GSharedFieldMaster
{
public:
						GSharedFieldMaster(GFieldInfo* pFieldInfo);
	virtual				~GSharedFieldMaster();

	SHAREDFIELD_MAP&	GetChannels();
	void				GetChannels(vector<int>& vecChannelID);
	GSharedField*		GetChannel(int nChannelID);

	int					GetFieldID()		{ return m_nFieldID; }
	int					NewChannelID();

	GSharedField* CreateSharedField(MUID uidNewField);
	bool AddChannel(GSharedField* pField, CHANNELID nNewChannelID);
	GSharedField*		DeleteChannel(int nChannelID); // 삭제되는 채널이 반환
	CHANNELID			GetEnterableChannelID();

private:
	void				Update(float fDelta);
	
	GSharedField*		GetEnterableChannel();
	int					GetDefaultChannelCount()		{ return m_nDefaultChannelCount; }

	void				Clear();	
	
private:
	friend GFieldMgr;	// 삭제 되어야함
		
	int					m_nFieldID;
	int					m_nDefaultChannelCount;
	SHAREDFIELD_MAP		m_ChannelMap;		///< 채널 필드 맵 (first: 채널 아이디)
	GWeatherMgr			m_WeatherMgr;		///< 날씨 관리
	GSharedFieldNavigator*	m_pNavigator;	///< 채널 진입관리
};

class GSharedFieldMasterMLeaf : public GSharedFieldMaster, public MMemPool<GSharedFieldMasterMLeaf>
{
public:
	GSharedFieldMasterMLeaf(GFieldInfo* pFieldInfo): GSharedFieldMaster(pFieldInfo) {}
	virtual ~GSharedFieldMasterMLeaf() {}
};