#pragma once

class ZSharedField;

class ZSharedFieldMaster
{
public:
	typedef vector<ZSharedField* >	ZChannelVector;

public:
	ZSharedFieldMaster(int nFieldID);
	~ZSharedFieldMaster();

	void			Add(ZSharedField* pSharedField);
	void			Remove(int nChannelID);
	bool			IsExistChannel(int nChannelID);
	bool			IsDefaultChannel(int nChannelID);

	int				GetFieldID()		{ return m_nFieldID; }

	ZChannelVector&	GetChannelVec()		{ return m_vecSharedField; }
	int				GetChannelCount()	{ return (int)m_vecSharedField.size(); }
	ZSharedField*	FindChannel(int nChannelID);
	ZSharedField*	GetEnterableChannel();

	int				GetAllocableChannelID();

private:
	int				m_nFieldID;
	ZChannelVector	m_vecSharedField;
	uint32			m_nEnterableChannelCursor;
	set<int>		m_setDefaultChannel;
};
