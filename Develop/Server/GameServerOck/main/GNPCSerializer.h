#pragma once

class GEntityNPC;
struct SPAWN_INFO;

//////////////////////////////////////////////////////////////////////////
//
// GNPCSerializer
//		- 서버 Reload시 NPC를 새롭게 스폰해 재현하기 위한 용도로 구현
//		- Actor 단위로 시리얼라이징이 필요할 때는 상속구조로 변경하자
//		- Field Instance가 사라지지 않는다는 가정으로 설계되었음
//
//////////////////////////////////////////////////////////////////////////

class GNPCSerializer
{
	struct Info
	{
		int nNPCID;
		MUID uidField;
		vec3 vPos;
		vec3 vDir;
		vec3 vFacingDir;
		const SPAWN_INFO* pSpawnInfo;
	};
public:
	GNPCSerializer(void);
	~GNPCSerializer(void);

	void SaveFromInstance( GEntityNPC* pNPC, bool bDelete=true );
	void SaveFromManager(int nNPCID = INVALID_ID, bool bDelete=true );
	void RestoreAll();
private:
	vector<Info> m_vecNPCs;
};

