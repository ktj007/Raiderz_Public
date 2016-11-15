#include "stdafx.h"
#include "XSoundField.h"
#include "XFieldInfo.h"
#include "XMyPlayer.h"
#include "XGameState.h"


XSoundField::XSoundField()
{
	m_nMuteAmbient = 0;
	m_dwLastTime = 0;
}


void XSoundField::Clear()
{
	m_nMuteAmbient = 0;

	if ( global.sound)
		global.sound->ClearLocalReverb();
}


bool XSoundField::IsMutedAmbient() const
{
	return ( m_nMuteAmbient > 0  ?  true : false);
}


XSoundField* XSoundField::GetInstance()
{
	static XSoundField instance;
	return &instance;
}


void XSoundField::Update( XFieldInfo* pInfo)
{
	if ( pInfo == NULL  ||  gvar.Game.pMyPlayer == NULL)	return;


	// 1초에 한번만 실행하도록.
	DWORD dwCurTime = timeGetTime();
	if ( ( m_dwLastTime - dwCurTime) < 1000)				return;
	m_dwLastTime = dwCurTime;


	int nPrevMuteAmbient = m_nMuteAmbient;
	bool bUpdatedAmbient = false;

	vector< FIELDSOUND_INFO>& vecSound = pInfo->m_vecSound;
	size_t _size = vecSound.size();
	for ( size_t i = 0;  i < _size;  i++)
	{
		FIELDSOUND_INFO& info = vecSound[i];

		if (global.sound  &&  global.sound->IsBGMPalying())
		{
			char szID[ 256];
			sprintf_s( szID, "fieldsound_%u", i);
			global.sound->Stop( szID);

			// contact 초기화
			info.bContact = false;
			continue;
		}

		// 사운드 위치가 적정 거리 안에 있는지 확인
		vec3 vSrc = gvar.Game.pMyPlayer->GetPosition();
		vec3 vTar = info.vPos;
		float fRadius = info.fMaxDist;
		bool inner = false;

		// 계산 속도를 위해서 먼저 x,y,z 축각 거리를 비교
		{
			float dx = abs( vTar.x - vSrc.x);
			if ( dx < fRadius)
			{
				float dy = abs( vTar.y - vSrc.y);
				if ( dy < fRadius)
				{
					float dz = abs( vTar.z - vSrc.z);
					if ( dz < fRadius)
					{
						// 축각 거리 안에 존재하면 직선 거리를 비교
						if ( (dx * dx + dy * dy + dz * dz) < (fRadius * fRadius + (info.bContact ? 250000.0f : 0.0f)))
							inner = true;
					}
				}
			}
		}


		// 안으로 진입했는지 확인
		if ( info.bContact == false  &&  inner == true)
		{
			char szID[ 256];
			sprintf_s( szID, "fieldsound_%u", i);
			RSound* pSound = NULL;
			if ( global.sound)
				pSound = global.sound->PlayStatic3D( MLocale::ConvUTF16ToAnsi(info.strName.c_str()).c_str(), szID, 0, info.vPos);
			if ( pSound != NULL)			pSound->SetDistance( info.fMinDist, info.fMaxDist);

			info.bContact = true;

			if ( info.bMuteAmbient == true)
			{
				m_nMuteAmbient++;
				bUpdatedAmbient = true;
			}
		}

		// 밖으로 나갔는지 확인
		else if ( info.bContact == true  &&  inner == false)
		{
			char szID[ 256];
			sprintf_s( szID, "fieldsound_%u", i);

			if ( global.sound)
				global.sound->Stop( szID);

			info.bContact = false;
			bUpdatedAmbient = true;

			if ( info.bMuteAmbient == true)
			{
				m_nMuteAmbient--;
				bUpdatedAmbient = true;
			}
		}
	}


	// Indoor 사운드 개수 변화를 체크해서 ambient 사운드를 업데이트 한다
	if ( bUpdatedAmbient == true)
	{
		if ( nPrevMuteAmbient == 0  &&  m_nMuteAmbient > 0)
		{
			if ( global.sound)
				global.sound->Stop( "ambient");
		}
		else if ( nPrevMuteAmbient > 0  &&  m_nMuteAmbient == 0)
		{
			if ( gg.currentgamestate && gg.currentgamestate->GetWorld())
				gg.currentgamestate->GetWorld()->PlayFieldSound();
		}
	}
}
