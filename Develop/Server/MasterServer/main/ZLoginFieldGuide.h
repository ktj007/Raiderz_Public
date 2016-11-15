#pragma once

class ZGameServerObject;
class ZField;

class ZLoginFieldGuide
{
public:
	struct PLAYER_FIELD_INFO
	{
		int		nSharedFieldID;
		MUID	uidDynamicFieldGroup;
		int		nDynamicFieldID;

		PLAYER_FIELD_INFO()
			: nSharedFieldID(0)
			, uidDynamicFieldGroup(MUID::ZERO)
			, nDynamicFieldID(0)
		{}
		PLAYER_FIELD_INFO(PLAYER_FIELD_INFO& refPlayerFieldInfo)
			: nSharedFieldID(refPlayerFieldInfo.nSharedFieldID)
			, uidDynamicFieldGroup(refPlayerFieldInfo.uidDynamicFieldGroup)
			, nDynamicFieldID(refPlayerFieldInfo.nDynamicFieldID)
		{}
	};
public:
	ZLoginFieldGuide(CID nCID, PLAYER_FIELD_INFO& refPlayerFieldInfo);
	~ZLoginFieldGuide();

	bool				Search(ZGameServerObject*& poutDestServer, ZField*& poutDestField);

private:
	CID					m_nCID;
	PLAYER_FIELD_INFO	m_PlayerFieldInfo;

	bool				CheckExistGameServer();

	bool				SearchFirstLoginField(ZGameServerObject*& poutDestServer, ZField*& poutDestField);
	bool				SearchDynamicField(ZGameServerObject*& poutDestServer, ZField*& poutDestField);
	bool				SearchSharedField(ZGameServerObject*& poutDestServer, ZField*& poutDestField);

	bool				SearchExceptionField(ZGameServerObject*& poutDestServer, ZField*& poutDestField);
};
