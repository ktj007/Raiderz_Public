#include "stdafx.h"
#include "XRealSound.h"
#include "XModuleEntity.h"
#include "XCharacter.h"
#include "XGlobal.h"
#include "XMyPlayer.h"
#include "XBaseApplication.h"



// XSample
XSample::XSample()
{
	m_pData = NULL;
	m_nDataSize = 0;
}


XSample::XSample( const RSampleInfo& _info)
: RSample( _info)
{
	m_pData = NULL;
	m_nDataSize = 0;
}


XSample::~XSample()
{
	if ( m_pData != NULL)
	{
		delete [] m_pData;
		m_pData = NULL;
		m_nDataSize = 0;
	}
}


// GetDataPtr
const char* XSample::GetDataPtr()
{
	return m_pData;
}


// GetDataSize
const size_t XSample::GetDataSize()
{
	return m_nDataSize;
}


// OnPreCreateSound
void XSample::OnPreCreateSound( int* pMode)
{
	RSample::OnPreCreateSound( pMode);
	
	if ( global.filesys->GetAccessMode() == MFILEACCESS_ENCRYPTED)
		*pMode |= FMOD_OPENMEMORY;
}


// OnLoadData
void XSample::OnLoadData()
{
	if ( m_pData != NULL)		return;

	MFile mf;
	if ( mf.Open( m_strFileName.c_str()) == false)
		return;

	m_nDataSize = mf.GetLength();
	m_pData = new char[ m_nDataSize];
	mf.ReadAll( m_pData, m_nDataSize);
	mf.Close();
}


// OnReleaseData
void XSample::OnReleaseData()
{
	if ( m_pData == NULL)		return;

	delete [] m_pData;
	m_pData = NULL;
	m_nDataSize = 0;
}








// XSoundDynamic3D
XSoundDynamic3D::XSoundDynamic3D( const MUID& uidObject, const char* szBone)
{
	m_uidObject = uidObject;
	if ( szBone != NULL)		m_strBone = szBone;
}


XSoundDynamic3D::~XSoundDynamic3D()
{
	m_uidObject.SetZero();
	m_strBone.clear();
}


// GetUID
MUID XSoundDynamic3D::GetUID() const
{
	return m_uidObject;
}


// GetBone
const char* XSoundDynamic3D::GetBone() const
{
	return m_strBone.c_str();
}


// StopDynamic3D
void XSoundDynamic3D::StopDynamic3D()
{
	m_uidObject.SetZero();
	m_strBone.clear();
}


// GetPosition
bool XSoundDynamic3D::GetPosition( FMOD_VECTOR* pPos, FMOD_VECTOR* pVel)
{
	// UID가 없을 때
	if ( m_uidObject == MUID::ZERO  ||  gg.omgr == NULL)
	{
		Stop();
		return false;
	}


	XObject* pObject = gg.omgr->Find( m_uidObject);
	if ( pObject == NULL)
	{
		Stop();
		return false;
	}


	if ( m_strBone.empty() == true)
	{
		vec3 _pos = pObject->GetPosition();
		pPos->x = _pos.x;
		pPos->y = _pos.y;
		pPos->z = _pos.z;

		pVel->x = 0.0;
		pVel->y = 0.0;
		pVel->z = 0.0;

		return true;
	}
	else
	{
		XModuleEntity* pMdouleEntity = pObject->GetModuleEntity();
		if ( pMdouleEntity != NULL)
		{
			XCharacter* pActor = pMdouleEntity->GetActor();
			if ( pActor != NULL)
			{
				MMatrix matActorNode;
				matActorNode.MakeIdentity();
				RActorNode* pActorNode = pActor->GetActorNodeTransform( MLocale::ConvAnsiToUTF16(m_strBone.c_str()).c_str(), matActorNode, E_TS_WORLD, true);
				vec3 _pos = ( pActorNode != NULL)  ?  matActorNode.GetTranslation()  :  pActor->GetWorldPosition();
				pPos->x = _pos.x;
				pPos->y = _pos.y;
				pPos->z = _pos.z;

				pVel->x = 0.0;
				pVel->y = 0.0;
				pVel->z = 0.0;

				return true;
			}
			else
			{
				vec3 _pos = pObject->GetPosition();
				pPos->x = _pos.x;
				pPos->y = _pos.y;
				pPos->z = _pos.z;

				pVel->x = 0.0;
				pVel->y = 0.0;
				pVel->z = 0.0;

				return true;
			}
		}
		else
		{
			vec3 _pos = pObject->GetPosition();
			pPos->x = _pos.x;
			pPos->y = _pos.y;
			pPos->z = _pos.z;

			pVel->x = 0.0;
			pVel->y = 0.0;
			pVel->z = 0.0;

			return true;
		}
	}

	return false;
}


// OnStop
void XSoundDynamic3D::OnStop()
{
	m_uidObject.SetZero();
	m_strBone.clear();
}


// OnDestroy
void XSoundDynamic3D::OnDestroy()
{
	m_uidObject.SetZero();
	m_strBone.clear();
}














// XSoundStatic3D
XSoundStatic3D::XSoundStatic3D( const vec3& vPosition)
{
	m_Position = vPosition;
}


XSoundStatic3D::~XSoundStatic3D()
{
}


// GetPosition
bool XSoundStatic3D::GetPosition( FMOD_VECTOR* pPos, FMOD_VECTOR* pVel)
{
	pPos->x = m_Position.x;
	pPos->y = m_Position.y;
	pPos->z = m_Position.z;

	pVel->x = 0.0;
	pVel->y = 0.0;
	pVel->z = 0.0;

	return true;
}












// 3D Sound type
enum
{
	SOUNDTYPE_NONE		= 0,
	SOUNDTYPE_DYNAMIC	= 0x0001,		// 동적 위치 사운드
	SOUNDTYPE_STATIC	= 0x0002		// 정적 위치 사운드
};


// XRealSound
XRealSound::XRealSound()
{
	m_ReservedSoundInfo.clear();

	m_bSilenceWhenInactivate = false;
	m_bWindowActivate = true;

	m_strDefaultBGM.clear();
}


// PlayDynamic3D
RSound* XRealSound::PlayDynamic3D( const char* szSampleName, const char* szID, DWORD dwData, const MUID& uidObject, const char* szBone)
{
	if ( uidObject == MUID::ZERO)		return NULL;


	// Reserve 3D sound
	m_ReservedSoundInfo.set( SOUNDTYPE_DYNAMIC, uidObject, szBone);


	// Play sound
	dwData |= SOUNDTYPE_DYNAMIC;
	XSoundDynamic3D* pSound = (XSoundDynamic3D*)RealSound3::Play( szSampleName, szID, dwData);

	m_ReservedSoundInfo.clear();
	return pSound;
}

RSound* XRealSound::PlayDynamic3D( const char* szSampleName, MUID uid, DWORD dwData, const MUID& uidObject, const char* szBone /*=NULL*/ )
{
	char chID[256] = {0,};
	sprintf_s(chID, "%d", uid.High);

	return PlayDynamic3D(szSampleName, chID, dwData, uidObject, szBone);
}


// StopDynamic3D
bool XRealSound::StopDynamic3D( const MUID& uidObject, bool bMute)
{
	// Enter critical section
	Lock();


	if ( bMute == false)
	{
		for ( list< pair< string, RSound*> >::iterator itr = m_vSounds.begin();  itr != m_vSounds.end();  itr++)
		{
			XSound* pSound = (XSound*)( (*itr).second);
			if ( pSound != NULL  &&  pSound->IsDynamic3D() == true)
			{
				XSoundDynamic3D* pDynamicSound = (XSoundDynamic3D*)pSound;
				if ( uidObject == pDynamicSound->GetUID())
				{
					pDynamicSound->StopDynamic3D();
					pDynamicSound->Stop();
				}
			}
		}
	}
	else
	{
		for ( list< pair< string, RSound*> >::iterator itr = m_vSounds.begin();  itr != m_vSounds.end();  itr++)
		{
			XSound* pSound = (XSound*)( (*itr).second);
			if ( pSound != NULL  &&  pSound->IsDynamic3D() == true)
			{
				XSoundDynamic3D* pDynamicSound = (XSoundDynamic3D*)pSound;
				if ( uidObject == pDynamicSound->GetUID())
				{
					pDynamicSound->StopDynamic3D();

					if ( pDynamicSound->IsDelete() == false)
						pDynamicSound->_destroy();

					if ( pDynamicSound == m_pCurrentBGM)
						m_pCurrentBGM = NULL;
				}
			}
		}
	}


	// Leave critical section
	Unlock();

	return true;
}


// PlayStatic3D
RSound* XRealSound::PlayStatic3D( const char* szSampleName, const char* szID, DWORD dwData, const vec3& vPosition)
{
	// Reserve 3D sound
	m_ReservedSoundInfo.set( SOUNDTYPE_STATIC, vPosition);


	// Play sound
	dwData |= SOUNDTYPE_STATIC;
	XSoundStatic3D* pSound = (XSoundStatic3D*)RealSound3::Play( szSampleName, szID, dwData);

	m_ReservedSoundInfo.clear();
	return pSound;
}

RSound* XRealSound::PlayStatic3D( const char* szSampleName, MUID uid, DWORD dwData, const vec3& vPosition )
{
	char chID[256] = {0,};
	sprintf_s(chID, "%d", uid.High);

	return PlayStatic3D(szSampleName, chID, dwData, vPosition);
}


// SilenceWhenInactivate
void XRealSound::SilenceWhenInactivate( bool bSet)
{
	if ( m_bSilenceWhenInactivate == bSet)		return;
	m_bSilenceWhenInactivate = bSet;
}


// IsBGMPalying
bool XRealSound::IsBGMPalying()
{
	Lock();

	bool bPlaying = ( m_pCurrentBGM != NULL)  ?  true : false;

	Unlock();

	return bPlaying;
}


// SetDefalutBGM
void XRealSound::SetDefalutBGM( const char* szSampleName)
{
	if ( szSampleName == NULL  ||  szSampleName[ 0] == 0)
	{
		m_strDefaultBGM.clear();

		Stop( "default_bgm");
	}
	else if ( m_strDefaultBGM != string( szSampleName))
	{
		m_strDefaultBGM.clear();

		RSound* pSound = Play( szSampleName, "default_bgm");
		if ( pSound != NULL  &&  pSound->GetLoop() == true)
			m_strDefaultBGM = szSampleName;
	}
}


// overrideNewSound
RSound* XRealSound::overrideNewSound( const RSample* pSample, DWORD dwData)
{
	if ( dwData & SOUNDTYPE_DYNAMIC)
		return new XSoundDynamic3D( m_ReservedSoundInfo.uidObject, m_ReservedSoundInfo.strBone.c_str());

	else if ( dwData & SOUNDTYPE_STATIC)
		return new XSoundStatic3D( m_ReservedSoundInfo.vPosition);

	return new XSound();
}


// OnPreUpdate
void XRealSound::OnPreUpdate()
{
	RealSound3::OnPreUpdate();


	// Play default BGM
	if ( m_strDefaultBGM.empty() == false  &&  IsBGMPalying() == false)
	{
		Play( m_strDefaultBGM.c_str(), "default_bgm");
	}
}


// OnUpdate
void XRealSound::OnUpdate()
{
	RealSound3::OnUpdate();


	// Mute when inactivate
	bool bMute = m_bSilence  ||  m_bMute[ RSD_CHANNELGROUP_MASTER];
	if ( m_bSilenceWhenInactivate == true  &&  global.app)
	{
		if ( global.app->IsActive() == false)
			bMute = true;
	}

	if ( m_pChannelGroups[ RSD_CHANNELGROUP_MASTER] != NULL)
		m_pChannelGroups[ RSD_CHANNELGROUP_MASTER]->setMute( bMute);
}

bool XRealSound::StopSound( const MUID& uid )
{
	char chID[256] = {0,};
	sprintf_s(chID, "%d", uid.High);

	return RealSound3::Stop(chID);
}
