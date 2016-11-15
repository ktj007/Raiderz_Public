#pragma  once
#include "XMovableProcessor.h"

class XMovableProcTerrainNPC : public XMovableProcessor
{
private:
	XModuleMovable *			m_pMovableProcOwner;
	XModuleCollision *			m_pModuleCollision;
	bool CheckAbnormalObstacle(vec3& _out_nextPos, const vec3& _nextTo, const vec3& _currentPos);
	void UpdateMoveCheckType(XMovableFactor& _rMovableFactor, const vec3& _rVecCurrentPos);

public:
	XMovableProcTerrainNPC(){}
	virtual ~XMovableProcTerrainNPC(){}

	virtual void Init(XModuleMovable* pProcOwner);
	virtual void Fini(){}

	virtual bool IsProcessable(const XMovableFactor& _rMovableFactor);
	virtual void Run( XMovableFactor& _rMovableFactor, const vec3& _rVecCurrentPos, const vec3& _rVecCurrentDir, vec3& _rVecTo, vec3& _rVecDir, vec3& _rUpVec, float fDelta );

	virtual const wchar_t* GetDebugName() { return L"TerrainNPC"; }
};
