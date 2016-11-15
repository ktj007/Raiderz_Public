#ifndef _MOCK_FIELD_H_
#define _MOCK_FIELD_H_

#include "GField.h"
#include "GSpawnManager.h"
#include "GWeatherMgr.h"
#include "GGlueField.h"

class GNPCInfo;
class MockField;

class DefaultMockField
{
private:
	static MockField* m_pDefaultMockField;
public:
	static MockField* Get();
	static void Free();
};
class MockField : public GField
{
private:
	static GFieldInfo s_FieldInfo;
	bool m_bInitSector;
	bool m_bExceptToPickingZIsZero;
private:
	virtual MUID NewUID();
	bool CreateNPCAndEndterField(GEntityNPC* pNPC, GNPCInfo* pNPCInfo, const vec3& pos, const vec3& dir);
	void InitNPCEtc(GEntityNPC* pNPC);

public:
	MockField() : m_bExceptToPickingZIsZero(false) {}
	MockField(MUID& uid);;
	virtual ~MockField();;

	virtual bool Create();
	virtual void Destroy();

	void CreateSectors();

	void SetFieldInfo(GFieldInfo* pFieldInfo)			{ m_pInfo = pFieldInfo; }
	void SetWeatherMgr(GWeatherMgr* pWeatherMgr)		{ m_pWeatherMgr = pWeatherMgr; }

	virtual void RemoveEntity(GEntity* pEntity)			{ __super::RemoveEntity(pEntity); }

	static float FieldMinX()		{ return s_FieldInfo.MinX(); }
	static float FieldMinY()		{ return s_FieldInfo.MinY(); }
	static float FieldMaxX()		{ return s_FieldInfo.MaxX(); }
	static float FieldMaxY()		{ return s_FieldInfo.MaxY(); }

	void SendSectorInfoForMe(GEntityPlayer* pPlayer) {	}
	virtual bool PickingZ(vec3& outPos, vec3 vOriginPos, float fPickAddedOriginZ = PICKING_ADDED_VALUE_Z);

	GEntityNPC* SpawnTestNPC(GNPCInfo* pNPCInfo, const vec3& pos=vec3(1, 1, 1), const vec3& dir=vec3(1, 1, 1), SPAWN_INFO* pSpawnInfo=NULL);

	template<typename T>
	T* NewMockNPC(GNPCInfo* pNPCInfo, const vec3& pos=vec3(1, 1, 1), const vec3& dir=vec3(1, 1, 1))
	{
		T* pNPC = new T();

		if (!CreateNPCAndEndterField(pNPC, pNPCInfo, pos, dir))
		{
			return NULL;
		}		

		InitNPCEtc(pNPC);

		return pNPC;
	}


	void SetInfo(GFieldInfo* pInfo)		{ m_pInfo = pInfo; }

	void ExceptToPickingZIsZero(bool b) { m_bExceptToPickingZIsZero = b; }
};


#endif //_MOCK_FIELD_H_