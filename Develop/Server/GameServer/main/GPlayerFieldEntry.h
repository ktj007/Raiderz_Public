#pragma once

#include "CSWorldInfo.h"
#include "CSTriggerInfo.h"
#include "STransData_M2G.h"
#include "GPlayerFieldEntry.h"

class GEntityActor;
class GEntityPlayer;
class GField;
class GPlayerFieldListener;
class GDynamicField;
class GSharedField;
class GDynamicFieldMaster;
struct FIELD_GROUP_INFO;
enum DYNAMIC_FIELD_TYPE;


/// 플레이어의 필드 진입 처리
class GPlayerFieldEntry
{
	friend class GPlayerField;
private:
	struct WarpReservedInfo
	{
		int nField;
		MUID uidField;
		vec3 vPos;
		vec3 vDir;
		WarpReservedInfo() {Reset();}
		void Reset()
		{
			nField = 0;
			uidField = 0;
			vPos = vec3::ZERO;
			vDir = vec3::AXISY;
		}
	};
private:
	GPlayerFieldListener*		m_pLisener;					///< 리스너
	WarpReservedInfo			m_WarpReservedInfo;			///< 들어갈 필드 UID, 위치, 방향

	void SetLisener(GPlayerFieldListener* pLisener)		{ m_pLisener = pLisener; }
public:
	GPlayerFieldEntry() {}
	~GPlayerFieldEntry() {}
	void ReserveWarp(MUID uidField, vec3 vPos, vec3 vDir);
	void ReserveWarp(int nField, vec3 vPos);
	void ClearReserveWarp();

	// 다른 필드로 이동완료
	bool OnEnterFieldComplete();

	// 필드이동 정보 반환
	MUID	GetReservedWarpField()		{ return m_WarpReservedInfo.uidField; }
	int		GetReservedWarpFieldID()	{ return m_WarpReservedInfo.nField; }
	vec3	GetReservedWarpPos()		{ return m_WarpReservedInfo.vPos; }

	bool	IsReserved();
};
