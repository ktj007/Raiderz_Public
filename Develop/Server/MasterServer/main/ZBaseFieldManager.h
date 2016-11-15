#pragma once

class ZField;
class ZSharedField;
class ZSharedFieldMaster;
class ZDynamicField;
class ZDynamicFieldMaster;

class ZBaseFieldManager
{
public:
	typedef map<MUID, ZField*>				ZFieldMap;
	typedef map<MUID, ZSharedField*>		ZSharedFieldMap;
	typedef map<int, ZSharedFieldMaster*>	ZSharedFieldMasterMap;
	typedef map<MUID, ZDynamicFieldMaster*>	ZDynamicFieldMasterMap;

private:
	virtual void			AddSharedFieldMaster(ZSharedFieldMaster* pSharedFieldMaster);

protected:
	ZFieldMap				m_FieldMap;
	ZSharedFieldMap			m_SharedFieldMap;
	ZSharedFieldMasterMap	m_SharedFieldMasterMap;
	ZDynamicFieldMasterMap	m_DynamicFieldMasterMap;

	void					AddField(ZField* pField);
	void					DelField(MUID uidField);

	virtual void			AddSharedField(ZSharedField* pSharedField);
	virtual void			DelSharedField(MUID uidSharedField);

	virtual void			AddDynamicFieldMaster(ZDynamicFieldMaster* pDynamicFieldMaster);
	virtual void			DelDynamicFieldMaster(MUID uidDynamicFieldMaster);

public:
	ZBaseFieldManager() {}
	virtual					~ZBaseFieldManager();

	virtual void			Clear();

	ZField*					FindField(MUID uidField);
	ZSharedField*			FindSharedField(MUID uidSharedField);
	ZSharedField*			FindSharedField(int nFieldID, int nChannelID);
	ZSharedFieldMaster*		FindSharedFieldMaster(int nFieldID);
	ZDynamicFieldMaster*	FindDynamicFieldMaster(MUID uidDynamicFieldMaster);

	ZSharedFieldMap&		GetSharedFieldMap()				{ return m_SharedFieldMap; }
	size_t					GetSharedFieldCount()			{ return m_SharedFieldMap.size(); }
	ZSharedFieldMasterMap&	GetSharedFieldMasterMap()		{ return m_SharedFieldMasterMap; }
	size_t					GetSharedFieldMasterCount()		{ return m_SharedFieldMasterMap.size(); }
	ZDynamicFieldMasterMap&	GetDynamicFieldMasterMap()		{ return m_DynamicFieldMasterMap; }
	size_t					GetDynamicFieldMasterCount()	{ return m_DynamicFieldMasterMap.size(); }
};