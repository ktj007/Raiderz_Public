#pragma once

#include "RealSound3.h"


class XObject;



/// class : XSample
class XSample : public RSample
{
protected:
	char*		m_pData;
	size_t		m_nDataSize;


public:
	XSample();
	XSample( const RSampleInfo& _info);
	virtual ~XSample();

	virtual const char* GetDataPtr();
	virtual const size_t GetDataSize();


protected:
	virtual void OnPreCreateSound( int* pMode) override;
	virtual void OnLoadData() override;
	virtual void OnReleaseData() override;
};





/// class : XSound
class XSound : public RSound
{
public:
	XSound()											{}
	virtual ~XSound()									{}

	virtual bool IsDynamic3D() const					{ return false;		}
};




/// class : XSoundDynamic3D - 동적 위치 3D 사운드
class XSoundDynamic3D : public XSound
{
protected:
	MUID			m_uidObject;
	string			m_strBone;


public:
	XSoundDynamic3D( const MUID& uidObject, const char* szBone);
	virtual ~XSoundDynamic3D();

	MUID GetUID() const;
	const char* GetBone() const;

	virtual bool IsDynamic3D() const override			{ return true;		}

	void StopDynamic3D();

	virtual bool GetPosition( FMOD_VECTOR* pPos, FMOD_VECTOR* pVel) override;


protected:
	virtual void OnStop() override;
	virtual void OnDestroy() override;
};





/// class : XSoundStatic3D - 정적 위치 3D 사운드
class XSoundStatic3D : public XSound
{
protected:
	vec3			m_Position;


public:
	XSoundStatic3D( const vec3& vPosition);
	virtual ~XSoundStatic3D();

	virtual bool GetPosition( FMOD_VECTOR* pPos, FMOD_VECTOR* pVel) override;
};





/// class : XRealSound
class XRealSound : public RealSound3
{
protected:
	struct
	{
		int			nType;
		MUID		uidObject;
		string		strBone;
		vec3		vPosition;

		void set( int _type, const MUID& _object, const char* szBone)
		{
			nType = _type;
			uidObject = _object;
			strBone.clear();
			if ( szBone != NULL)		strBone = szBone;
			vPosition.Set( 0.0f, 0.0f, 0.0f);
		}

		void set( int _type, const vec3& _pos)
		{
			nType = _type;
			uidObject.SetZero();
			strBone.clear();
			vPosition.Set( _pos);
		}

		void clear()
		{
			nType = 0;
			uidObject.SetZero();
			strBone.clear();
			vPosition.Set( 0.0f, 0.0f, 0.0f);
		}
	}				m_ReservedSoundInfo;

	bool			m_bSilenceWhenInactivate;
	bool			m_bWindowActivate;
	string			m_strDefaultBGM;


public:
	XRealSound();


	// 동적 위치 3D 사운드
	RSound* PlayDynamic3D( const char* szSampleName, const char* szID, DWORD dwData, const MUID& uidObject, const char* szBone =NULL);
	RSound* PlayDynamic3D( const char* szSampleName, MUID uid, DWORD dwData, const MUID& uidObject, const char* szBone =NULL);
	bool StopDynamic3D( const MUID& uidObject, bool bMute =false);
	bool StopSound( const MUID& uid);


	// 정적 위치 3D 사운드
	RSound* PlayStatic3D( const char* szSampleName, const char* szID, DWORD dwData, const vec3& vPosition);
	RSound* PlayStatic3D( const char* szSampleName, MUID uid, DWORD dwData, const vec3& vPosition);


	// 비활성화때 소리 안나게 설정
	void SilenceWhenInactivate( bool bSet);

	// BGM 재생여부
	bool IsBGMPalying();

	// 기본 BGM 설정
	void SetDefalutBGM( const char* szSampleName =NULL);


protected:
	virtual RSound* overrideNewSound( const RSample* pSample, DWORD dwData) override;


protected:
	virtual void OnPreUpdate() override;
	virtual void OnUpdate() override;
};
