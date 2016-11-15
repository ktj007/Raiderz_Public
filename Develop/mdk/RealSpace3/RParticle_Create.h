#pragma once

#include "RTypes.h"

namespace rs3 {

//////////////////////////////////////////////////////////////////////////
// 현재 파티클을 어떻게 생성할 것인지를 결정하기 위한 인터페이스
class RParticleCreator
{
public:
	virtual ~RParticleCreator() {}
	virtual RVector3	GetDirection(float fAngle) = 0;
	virtual RVector3	GetPosition(const RVector& vRange) = 0;

protected:
	virtual float		GetRandomFloat( float fMin, float fMax );
	virtual RVector3	GetRandomDirection(float fAngle);
};

//////////////////////////////////////////////////////////////////////////
// Box 일정한 방향으로 발생.
// - vRange는 Emitter의 영역을 결정한다.
class RBoxEmit : public RParticleCreator
{
public:
	virtual RVector3	GetDirection(float fAngle) override;
	virtual RVector3	GetPosition(const RVector& vRange) override;
};

//////////////////////////////////////////////////////////////////////////
// 방사형으로 퍼져나가는 것.
// vRange는 발생 위치(X,Y,Z)의 타원체로서 결정된다.
class RSphereEmit : public RParticleCreator
{
public:
	virtual RVector3	GetDirection(float fAngle) override;
	virtual RVector3	GetPosition(const RVector& vRange) override;
};

//////////////////////////////////////////////////////////////////////////
// Ring emitter
class RRingEmit : public RParticleCreator
{
	// torus의 반지름을 R 그리고 두께를 r이라고 할때 z축을 중심으로 회전한 torus의 equation은
	// ( R - sqrt(x^2+y^2) )^2 + z^2 = r^2
public:
	virtual RVector3	GetDirection(float fAngle) override;
	virtual RVector3	GetPosition(const RVector& vRange) override;
};

}