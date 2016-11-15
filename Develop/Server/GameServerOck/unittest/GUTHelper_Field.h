#pragma once

class MockField;
class GFieldInfo;
class GSharedField;
class GDynamicFieldMaster;

class GUTHelper_Field
{
public:
	// 재활용되는 먹필드를 반환한다.  사용후 꼭 Destroy() 함수를 호출해야한다.
	static MockField*		DefaultMockField();

	// 필드정보 생성
	static GFieldInfo*		NewFieldInfo(int nID=INVALID_ID);
	// 필드그룹 정보 생성
	static FIELD_GROUP_INFO*		NewFieldGroupInfo(GFieldInfo* pFieldInfo=NULL, int nGroupID=0);	
	// 마커 정보 생성
	static MARKER_INFO*			InsertNewMarkerInfo(const GFieldInfo* pFieldInfo, uint32 nMarkerID=0);
	// 센서 정보 생성
	static SENSOR_INFO*			InsertNewSensorInfo(const GFieldInfo* pFieldInfo, uint32 nSensorID=0);
	// 스폰 정보 생성
	static SPAWN_INFO*			InsertNewSpawnInfo(const GFieldInfo* pFieldInfo, SPAWN_ID nSpawnID=0, int nNPCID=INVALID_ID);

	// 공유 필드 생성
	static GSharedField*	NewSharedField(GFieldInfo* pFieldInfo=NULL, int nChannelID=1);
	// 트라이얼 필드 마스터 생성
	static GDynamicFieldMaster*	NewTrialFieldMaster(FIELD_GROUP_INFO* pFieldGroupInfo=NULL);
	// 전장 필드 마스터 생성
	static GDynamicFieldMaster*	NewBattleArenaFieldMaster(FIELD_GROUP_INFO* pFieldGroupInfo=NULL);
	// 먹필드 생성
	static MockField*				NewMockField(GFieldInfo* pFieldInfo=NULL);

	// 먹필드와 플레이어 생성
	static void NewMockFieldAndPlayer(MockField*& pNewField, GEntityPlayer*& pNewEntityPlayer, vec3 vPlayerPos = vec3::ZERO, MUID uidNewPlayer=MUID(0,0));
};
