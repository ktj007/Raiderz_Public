#pragma  once
#include "XMovableProcessor.h"

class XMovableProcTerrain : public XMovableProcessor
{
private:
	XModuleMovable *				m_pMovableProcOwner;
	XModuleCollision *				m_pModuleCollision;

	float							m_fFallingStartReadyTime;

private:

	bool							CheckTerrainWater(vec3& vPos);
	bool							CheckAllowedHeight(const vec3& vEntityPosition, const vec3 &vec);
	bool							CheckFallingStart(float fDelta);

	void							FallingSetting(XMovableFactor& _rMovableFactor, const vec3& _rVecCurrentPos);

public:
	XMovableProcTerrain() : m_fFallingStartReadyTime(0.0) {}
	virtual ~XMovableProcTerrain(){}

	virtual void Init(XModuleMovable* pProcOwner);
	virtual void Fini(){}

	virtual bool IsProcessable(const XMovableFactor& _rMovableFactor);
	virtual void Run( XMovableFactor& _rMovableFactor, const vec3& _rVecCurrentPos, const vec3& _rVecCurrentDir, vec3& _rVecTo, vec3& _rVecDir, vec3& _rUpVec, float fDelta );

	virtual const wchar_t* GetDebugName() { return L"Terrain"; }

protected:
	bool CheckThrowColision( XMovableFactor& _rMovableFactor, const vec3& _rVecCurrentPos, vec3& _rVecTo, vec3& _rVecDir, float fDelta);
};
