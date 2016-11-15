#ifndef _GFIELD_MGR_H
#define _GFIELD_MGR_H

#include <hash_map>

enum GAME_TIME_TYPE;
enum DYNAMIC_FIELD_TYPE;
struct FIELD_GROUP_INFO;
class GFieldInfo;
class GField;
class GSharedField;
class GSharedFieldMaster;
class GDynamicField;
class GDynamicFieldMaster;
class GTrialField;
class GInnRoomField;
class GInnRoomFieldGroup;
class GWeatherMgr;
class GEntityPlayer;
class GFieldFactory;
class GFieldEnteringMgr;

/// 필드 관리자
class GFieldMgr : public MTestMemPool<GFieldMgr>
{
	friend class GFieldFactory;
public:	
	typedef stdext::hash_map<uint64, GField*>	FIELD_BY_UID_MAP;
	typedef map<int, GSharedFieldMaster*>		SHARED_FIELD_MASTER_MAP;
	typedef map<MUID, GDynamicFieldMaster*>		DYNAMIC_FIELD_GROUP_MAP;

public:
	GFieldMgr();
	virtual ~GFieldMgr();

	// 필드매니저 정보 초기화
	void				Clear();
	// 필드매니저 업데이트
	void				Update(float fDelta);

	// 필드 갯수 반환
	int					GetFieldQty()								{ return (int)m_mapFields.size(); }
	// 공유필드마스터 갯수 반환
	int					GetSharedFieldQty()							{ return (int)m_mapSharedFieldMasters.size(); }
	// 동적필드마스터 갯수 반환
	int					GetDynamicFieldGroupQty()					{ return (int)m_mapDynamicFieldMasters.size(); }

	// >> 일반필드 관련
	// ================================================================
	/// 필드 추가
	void				AddField(GField* pField);
	/// 필드를 목록에서 제거
	void				EraseField(GField* pField);
	/// 필드 반환
	GField*				GetField(const MUID& uidField);
	/// 필드 제거
	void				DeleteField(const MUID& uidField);
	/// 생성된 모든 필드 반환
	vector<GField*>		GetFieldList(int nFieldID);
	
	// 필드 컨테이너 반환
	FIELD_BY_UID_MAP&	GetFieldMap()								{ return m_mapFields; }


	// >> 공유필드 관련 
	// ================================================================
	// 공유필드마스터 컨테이너 반환
	SHARED_FIELD_MASTER_MAP&	GetSharedFieldMap()					{ return m_mapSharedFieldMasters; }
	// 공유필드마스터 반환
	GSharedFieldMaster*			GetSharedFieldMaster(int nFieldID);
	// 이미 생성되어있는 채널을 반환
	GSharedField*				GetSharedField(int nFieldID, CHANNELID nChannelID=PRIME_CHANNELID);
	// 들어갈 채널이 없으면 NULL반환
	GSharedField*				GetEnterableSharedField(int nFieldID);
	// 필드생성 추가
	void						AddSharedField(MUID uidField, int nFieldID, int nChannelID);

	// >> 동적필드 관련
	// ================================================================
	GDynamicFieldMaster*	CreateDynamicFieldMaster(int nFieldGroupInfoID, DYNAMIC_FIELD_TYPE nType, const MUID& uidCreator);
	GDynamicFieldMaster*	CreateDynamicFieldMaster(MUID uidNew, int nFieldGroupInfoID, DYNAMIC_FIELD_TYPE nType, const MUID& uidCreator, vector<pair<int, MUID>>& vecDynamicFieldUID=vector<pair<int, MUID>>());

	DYNAMIC_FIELD_GROUP_MAP&GetDynamicFieldGroupMap() { return m_mapDynamicFieldMasters; }
	GDynamicFieldMaster*	GetDynamicFieldMaster(const MUID& uidDynamicFieldGroup);
	GDynamicFieldMaster*	GetDynamicFieldMaster(GEntityPlayer* pPlayer);
	GField*					GetDynamicField(const MUID& uidDynamicFieldGroup, int nFieldId);

	void					OnTimeChanged( GAME_TIME_TYPE nOldTime, GAME_TIME_TYPE nNewTime );
	void					OnTimeChangedDetail( int nTime );

	GFieldFactory*			GetFactory()			{ return m_pFieldFactory; }		///< 필드 팩토리. 필드 생성은 이것을 통해서 생성해야 한다.
	GFieldEnteringMgr*	GetFieldEnteringQueue()	{ return m_pFieldEnteringQueue; }

protected:
	virtual MUID			NewUID();

	// 공유필드마스터 추가
	GSharedFieldMaster*		CreateSharedFieldMaster(GFieldInfo* pFieldInfo);
	// 공유필드 추가, 반환값은 할당된 공유필드 (공유필드마스터가 없다면 맞춰서 생성)
	GSharedField*			CreateSharedField(MUID uidNewField, int nFieldID, CHANNELID nNewChannelID); 

	void					CreateTrialField( GDynamicFieldMaster* pNewDynamicFieldGroup, FIELD_GROUP_INFO* pFieldGroupInfo, const MUID uidDynamicFieldGroup, vector<pair<int, MUID>>& vecDynamicFieldUID );
	void					CreateInnRoom( GDynamicFieldMaster* pNewDynamicFieldGroup, FIELD_GROUP_INFO* pFieldGroupInfo, const MUID uidDynamicFieldGroup, vector<pair<int, MUID>>& vecDynamicFieldUID );
	void					CreateBattleArena( GDynamicFieldMaster* pNewDynamicFieldGroup, FIELD_GROUP_INFO* pFieldGroupInfo, const MUID uidDynamicFieldGroup, vector<pair<int, MUID>>& vecDynamicFieldUID );
	MUID					_PickDynamicFieldUID( const GFieldInfo* pFieldInfo, vector<pair<int, MUID>>& vecDynamicFieldUID );

	void					OnDeleteSharedField(GSharedField* pSharedField);
	void					OnDeleteDynamicFieldMaster(GDynamicFieldMaster* pDynamicFieldGroup);

	// 세분화된 업데이트
	void					Update_SharedField(float fDelta);
	void					Update_DynamicField(float fDelta);
	void					Update_CommonField(float fDelta);

protected:
	GFieldEnteringMgr*		m_pFieldEnteringQueue;

private:
	FIELD_BY_UID_MAP		m_mapFields;
	SHARED_FIELD_MASTER_MAP	m_mapSharedFieldMasters;
	DYNAMIC_FIELD_GROUP_MAP	m_mapDynamicFieldMasters;
	GFieldFactory*			m_pFieldFactory;
};



#endif