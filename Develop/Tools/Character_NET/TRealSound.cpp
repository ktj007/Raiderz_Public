#include "stdafx.h"
#include "TRealSound.h"
#include "TCharacter.h"
#include "TRealSoundLoader.h"


// TSoundDynamic3D
TSoundDynamic3D::TSoundDynamic3D()
{
}

TSoundDynamic3D::TSoundDynamic3D( const char* szBone)
{
	if ( szBone != NULL)		m_strBone = szBone;
}


// GetBone
const char* TSoundDynamic3D::GetBone() const
{
	return m_strBone.c_str();
}


// GetPosition
bool TSoundDynamic3D::GetPosition( FMOD_VECTOR* pPos, FMOD_VECTOR* pVel)
{
	TCharacter* pCharacter = g_pMainApp->GetActor();
	if ( pCharacter == NULL)
	{
		Stop();
		return false;
	}


	if ( m_strBone.empty() == true)
	{
		vec3 _pos = pCharacter->GetWorldPosition();
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
		RActorNode* pActorNode = pCharacter->GetActorNode( m_strBone.c_str());
		vec3 _pos = ( pActorNode != NULL)  ?  pActorNode->GetWorldPosition()  :  pCharacter->GetWorldPosition();
		pPos->x = _pos.x;
		pPos->y = _pos.y;
		pPos->z = _pos.z;

		pVel->x = 0.0;
		pVel->y = 0.0;
		pVel->z = 0.0;

		return true;
	}

	return false;
}












// TRealSound
TRealSound::TRealSound()
{
}


// PlayDynamic3D
RSound* TRealSound::PlayDynamic3D( const char* szSampleName, const char* szBone, const char* szID /*=NULL*/)
{
	SetCurrentDirectory("../../");


	m_strBone.clear();
	if ( szBone != NULL)		m_strBone = szBone;

	TSoundDynamic3D* pSound = (TSoundDynamic3D*)RealSound3::Play( szSampleName, szID, m_strBone.empty() ? 0 : 1);

	m_strBone.clear();

	g_pMainApp->SetCurrentDir();
	return pSound;
}

RSound* TRealSound::PlayDynamic3D( const char* szSampleName, const char* szBone, MUID uid /*= MUID::ZERO*/ )
{
	char* pchID = NULL;
	char chID[256] = {0,};
	if(uid != MUID::ZERO)
	{
		sprintf_s(chID, "%d", uid.High);
		pchID = chID;
	}

	return PlayDynamic3D(szSampleName, szBone, pchID);
}


// GetSoundList
void TRealSound::GetSoundList( vector<string>& vecEffectList )
{
	for ( map< string, RSample*>::iterator it = m_vSamples.begin();  it != m_vSamples.end();  ++it)
	{
		vecEffectList.push_back( it->first);
	}
}


// overrideNewSound
RSound* TRealSound::overrideNewSound( const RSample* pSample, DWORD dwData)
{
	if ( (dwData & PLAY_USER_LAST) == 1)		return new TSoundDynamic3D( m_strBone.c_str());
	return new RSound();
}

void TRealSound::StopDynamic3D( MUID uid )
{
	if(uid == MUID::ZERO)
		return;

	char chID[256] = {0,};
	sprintf_s(chID, "%d", uid.High);

	Stop(chID);
}

bool TRealSound::CheckSoundInfoModify()
{
	// 모두 삭제
	RemoveAllSamples();

	TRealSoundLoader soundLoader;
	return soundLoader.LoadSamples( FILENAME_SOUNDLIST_TOOL );
}
