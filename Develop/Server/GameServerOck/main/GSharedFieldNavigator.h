#pragma once


class GSharedFieldMaster;
class GSharedField;

class GSharedFieldNavigator : public MTestMemPool<GSharedFieldNavigator>
{
public:
	GSharedFieldNavigator(GSharedFieldMaster* pMaster);
	~GSharedFieldNavigator(void);

	CHANNELID GetNewID();
	CHANNELID GetEnterableChannelID();
	GSharedField* GetEnterableChannel();
private:
	GSharedFieldMaster*	m_pMaster;
	CHANNELID			m_nNextChannelID;
};
