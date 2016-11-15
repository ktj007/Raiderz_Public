#pragma once

#include "SChecksum.h"
#include "ZBaseFieldManager.h"

class ZField;
class ZSharedField;
class ZSharedFieldMaster;
class ZDynamicField;
class ZDynamicFieldMaster;
enum DYNAMIC_FIELD_TYPE;


class ZFieldManager : public ZBaseFieldManager
{
protected:
	MUIDGenerator	m_UIDGenerator;
	SFieldChecksum	m_ChecksumFields;
	SFieldChecksum	m_ChecksumDynamicFieldMasters;

	void			CalcFieldsChecksum();
	void			CalcDynamicFieldMastersChecksum();

	void			AddSharedField(ZSharedField* pField, bool bRoute);
	void			DelSharedField(ZSharedField* pField, bool bRoute);

	void			AddDynamicFieldMaster(ZDynamicFieldMaster* pDynamicFieldMaster, bool bRoute);
	void			DelDynamicFieldMaster(ZDynamicFieldMaster* pDynamicFieldMaster, bool bRoute);

public:
							ZFieldManager();
	virtual					~ZFieldManager();

	void					Clear() override;
	void					ClearOnGameServer(int nGameServerID);

	ZSharedField*			CreateSharedField(int nFieldID, int nChannelID, int nGameServerID, bool bIsDefaultChannel = true, bool bRoute = false);
	void					DestroySharedField(MUID uidField, bool bRoute = false);

	ZDynamicFieldMaster*	CreateDynamicFieldMaster(int nFieldGroupID, int nGameServerID, DYNAMIC_FIELD_TYPE eDynamicFieldType, MUID uidOwnerPlayer, bool bRoute = false);
	void					DestroyDynamicFieldMaster(MUID uidDynamicFieldMaster, bool bRoute = false);

	void					RouteAllFieldInfo();

	uint32					GetChecksumFields()					{ return m_ChecksumFields.GetValue(); }
	uint32					GetChecksumDynamicFieldMasters()	{ return m_ChecksumDynamicFieldMasters.GetValue(); }

	int						GetGameServerIDHasSharedField(int nFieldID, int nChannelID = INVALID_ID);

};

ZSharedField* ToSharedField(ZField* pField);
ZDynamicField* ToDynamicField(ZField* pField);