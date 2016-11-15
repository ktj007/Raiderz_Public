#pragma once

#include "RealSound3.h"


class XObject;



/// class : TSoundDynamic3D
class TSoundDynamic3D : public RSound
{
protected:
	string			m_strBone;


public:
	TSoundDynamic3D();
	TSoundDynamic3D( const char* szBone);

	const char* GetBone() const;
	virtual bool GetPosition( FMOD_VECTOR* pPos, FMOD_VECTOR* pVel) override;
};




/// class : TRealSound
class TRealSound : public RealSound3
{
protected:
	string		m_strBone;


public:
	TRealSound();


	// 동적 3D 사운드
	RSound* PlayDynamic3D( const char* szSampleName, const char* szBone, const char* szID =NULL);
	RSound* PlayDynamic3D( const char* szSampleName, const char* szBone, MUID uid = MUID::ZERO);
	void StopDynamic3D( MUID uid);

	// 사운드 리스트 가져오기
	void GetSoundList( vector<string>& vecEffectList);

	bool CheckSoundInfoModify();

protected:
	virtual RSound* overrideNewSound( const RSample* pSample, DWORD dwData) override;
};
