
#pragma  once

#include "XMovableProcessor.h"


class XMovableProcThrow : public XMovableProcessor
{
private:
	XModuleMovable*			m_pMovableProcOwner;
	XModuleCollision*		m_pModuleCollision;

	MUID					m_uidAttacker;			// 공격자 UID
	bool					m_bGrip;				// 잡힌적이 있는지 여부
	bool					m_bGripStart;


public:
	XMovableProcThrow();
	virtual ~XMovableProcThrow();

	virtual void Init( XModuleMovable* pProcOwner);
	virtual void Fini();

	virtual bool IsProcessable(const XMovableFactor& _rMovableFactor);
	virtual void Run( XMovableFactor& rMovFactor, const vec3& vPosCurr, const vec3& vDirCurr, vec3& vPosTo, vec3& vDirTo, vec3& vUpVec, float fDelta);

	virtual const wchar_t* GetDebugName() { return L"ProcessThrow"; }


public:


protected:
	// 잡기 시작시... 보간 업데이트
	void _UpdateGrapplStart(XMovableFactor& rMovFactor, const vec3& vPosCurr, vec3& vPosTo, vec3& vDirTo, vec3& vUpVec, float fDelta);

	// 잡힌 상태 업데이트
	void _UpdateGrappled( XMovableFactor& rMovFactor, const vec3& vPosCurr, vec3& vPosTo, vec3& vDirTo, vec3& vUpVec, float fDelta);

	// 잡혀서 날라가는 상태 업데이트
	void _UpdateThrow( XMovableFactor& rMovFactor, const vec3& vPosCurr, vec3& vPosTo, vec3& vDirTo, float fDelta);


	// 더미 노드의 위치 및 방향을 구함
	bool _GetGripPosDir( const MUID& uidAttacker, vec3& vPos, vec3& vDir, vec3& vUpVec);
};
