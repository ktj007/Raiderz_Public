#pragma once

class ZSharedFieldMaster;
class ZSharedField;

class ZChannelBeholder
{
public:
	ZChannelBeholder();
	~ZChannelBeholder();

	void Update(float fDelta);

	bool IsCrowd(ZSharedFieldMaster* pSharedFieldMaster);
	bool IsCrowd(ZSharedField* pSharedField);

	bool IsSilent(ZSharedField* pSharedField);
	int	GetSilentChannelCount() { return (int)m_mapSilentChannelExpire.size(); }

private:
	MRegulator*	m_pBeholderRglt;

	class ZSilentChannelExpireMap : public map<MUID, uint32>
	{
	public:
		void Add(MUID uidChannel);
		void Del(MUID uidChannel);
		bool IsExist(MUID uidChannel);
		bool CheckExpired(MUID uidChannel);
	};
	ZSilentChannelExpireMap	m_mapSilentChannelExpire;

	void CollectExpiredSilentChannel(ZSharedFieldMaster* pSharedFieldMaster, vector<MUID>& rvecChannelToDispose);
	void DestroySilentChannels(vector<MUID>& rvecChannelToDispose);
};
