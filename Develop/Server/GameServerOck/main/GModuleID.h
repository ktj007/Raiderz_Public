#ifndef _GMODULE_ID_H
#define _GMODULE_ID_H


/// 모듈 ID
enum GModuleID
{
	GMID_NA			= 0,
	GMID_TASK,
	GMID_AI,
	GMID_BUFF,
	GMID_COMBAT,			///< 이 모듈을 가지고 있는 엔티티는 전투가 가능하다.
	GMID_MOVABLE,

	MAX_MODULE_ID,
};




#endif	