#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <assert.h>
#include "RTypes.h"
#include "RVType.h"

namespace rs3 {

template<typename T>
T GetCosineInterpolate(T &A, T &B, float x)
{
	float ft = x * MMath::PI;
	float f = ( 1 - cos(ft)) * 0.5f;
	return ( A*(1.0f-f) + B*f);
}

struct REmitterVariable
{
	float	fEventTime;
	UINT	uAmount;
	RVector vRange;	//이미터 크기
	//비지블
	RVector	vPos;	//위치값
	//로테이션
	float	fAngle;
	float	fMinSpeed;
	float	fMaxSpeed;
	
	REmitterVariable()
	{
		fEventTime	= 0.0f;
		uAmount		= 0;
		vRange		= RVector(0.0f, 0.0f, 0.0f);
		vPos		= RVector(0.0f, 0.0f, 0.0f);
		fAngle		= 0.0f;
		fMinSpeed = 0.0f;
		fMaxSpeed = 0.0f;
	}
};

struct RParticleVariable
{
	float fEventTime;
	RVector4 vColor;

	float fAddScale;
	float fMulScale;
	float fInitScaleX;
	float fInitScaleY;

	/**
	 	외부에 나타내는 이름은 Force 이며, 실제로 Force 로서 적용된다.
	 */
	float fMaxSpeed;
	float fMinSpeed;
	RVector vDirection;

	float fGRate;
	float fScaleX;
	float fScaleY;
	float fNoise;

	RParticleVariable ()
	{
		fEventTime = 0.0f;
		vColor.Set( 1.f, 1.f, 1.f, 1.f );
		
		fAddScale = 0.0f;
		fMulScale = 1.0f;
		fInitScaleX = 0.0f;
		fInitScaleY = 0.0f;

		fMaxSpeed = fMinSpeed = 0.0f;
		vDirection	= RVector(0.0f, 0.0f, 0.0f);

		fGRate	= 1.0f;
		fScaleX = 1.0f;
		fScaleY = 1.0f;
		fNoise	= 0.0f;
	}
};

}
