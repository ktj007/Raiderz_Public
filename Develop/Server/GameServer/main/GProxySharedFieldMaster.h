#pragma once


class GProxyField;

class GProxySharedFieldMaster : public MTestMemPool<GProxySharedFieldMaster>
{
public:
	typedef map<int, GProxyField*>	GProxySharedFieldMap;
public:
	GProxySharedFieldMaster(int nFieldID) : m_nFieldID(nFieldID)	{}
	~GProxySharedFieldMaster()	{}

	bool AddSharedField(GProxyField* pSharedField);
	void DelSharedField(MUID uidField);

	GProxyField*			GetSharedField(int nChannelID);
	void					GetChannelIDList(vector<int>& vecChannelID);

	GProxySharedFieldMap&	GetContainer()	{ return m_mapSharedField; }

	int GetFieldCount()	{ return (int)m_mapSharedField.size(); }

private:
	int						m_nFieldID;
	GProxySharedFieldMap	m_mapSharedField;

};
