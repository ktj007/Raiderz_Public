#pragma  once
#include "XMovableFactor.h"

// Movable Processor 관련 디버깅 정보
//#define _DEBUG_SHOW_MOVABLE_PROC_PASSEED
//#define _DEBUG_SHOW_MOVABLE_VELOCITY
//#define _DEBUG_SHOW_TERRAIN_PROC
#define _DEBUG_SHOW_MOVABLE_MOVE_LEN

class XModuleMovable;
class XMovableProcessor
{
public:
	XMovableProcessor(){}
	virtual ~XMovableProcessor(){}

	virtual void Init(XModuleMovable* pProcOwner)			= 0;
	virtual void Fini()										= 0;

	virtual bool IsProcessable(const XMovableFactor& _rMovableFactor) = 0;
	virtual void Run( XMovableFactor& _rMovableFactor, const vec3& _rVecCurrentPos, const vec3& _rVecCurrentDir, vec3& _rVecTo, vec3& _rVecDir, vec3&_rUpVec, float fDelta ) = 0;
	virtual const wchar_t* GetDebugName() { return L"noname"; }
};
