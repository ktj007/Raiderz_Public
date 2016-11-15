#include "StdAfx.h"
#include "XDebugSoundFrm.h"

XDebugSoundFrm::XDebugSoundFrm( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: mint3::MTextFrame(szName, pParent, pListener)
{
	SetRect(10, 120, 380, 520);
//	SetOpacity(0.9f);
	SetText("Sound Infomation");
	UpdateScrollBar();
}

void XDebugSoundFrm::OnDraw( MDrawContext* pDC )
{
	MTextFrame::OnDraw(pDC);

	ResetContent();

	global.sound->Lock();

	const list< pair< string, RSound*>>* pSounds = global.sound->GetSoundList();

	char szBuff[ 1024];

	sprintf_s( szBuff, "Sounds playing = %d ,  Reverb = %s ,  Compressor = %s , Hardware Acc. = %s",
		pSounds->size(), global.sound->IsEnabledReverb() == true ? "On" : "Off", global.sound->IsEnabledCompressor() == true ? "On" : "Off", global.sound->IsHardwareAccelerate() == true ? "On" : "Off");

	AddString( szBuff);

	if ( pSounds->empty() == true)
	{
		const map< string, RSample*>* pSamples = global.sound->GetSampleList();
		if ( pSamples->empty() == false)
		{
			AddString( ".");
			AddString( "[ SAMPLES INFO ]");

			TIMEVAL _cur_time = GetCurTime();

			for ( map< string, RSample*>::const_iterator itr = pSamples->begin();  itr != pSamples->end();  itr++)
			{
				string strName = (*itr).first;
				RSample* pSample = (*itr).second;


				TIMEVAL _acc_time = pSample->GetLastAccessTime();
				TIMEVAL _remain = max( 0.0, 5.0 - (_cur_time - _acc_time) / 60000.0);

				int nReference = pSample->GetReference();
				if ( pSample->GetSound() == NULL  &&  nReference == 0)	continue;

				sprintf_s( szBuff, "%s : release = %.2f min remained , reference = %d", strName.c_str(), _remain, nReference);

				AddString( szBuff);
			}
		}
	}

	else
	{
		AddString( ".");
		AddString( "[ SOUNDS INFO ]");

		for ( list< pair< string, RSound*>>::const_iterator itr = pSounds->begin();  itr != pSounds->end();  itr++)
		{
			RSound* pSound = (*itr).second;

			unsigned int _cur_time = 0;
			unsigned int _play_time = 0;
			if ( pSound->GetChannel() != NULL)
			{
				_cur_time = pSound->GetPlayingPosition();
				_play_time = pSound->GetPlayingTime();
			}

			int nCurSec = _cur_time / 1000;
			int nCurMin = nCurSec / 60;
			nCurSec -= nCurMin * 60;
			int nPlaySec = _play_time / 1000;
			int nPlayMin = nPlaySec / 60;
			nPlaySec -= nPlayMin * 60;

			string strID = (*itr).first;

			string strType;
			switch ( pSound->GetType())
			{
			case 0 :	strType = "[N]";		break;
			case 1 :	strType = "[G]";		break;
			case 2 :	strType = "[B]";		break;
			case 3 :	strType = "[E]";		break;
			}

			float fVol = ( pSound->GetChannel() != NULL)  ?  pSound->GetVolume() : 0.0f;
			bool bVirtual = ( pSound->GetChannel() != NULL)  ?  pSound->IsVirtual() : false;
			int nReference = ( pSound->GetSample() != NULL)  ?  pSound->GetSample()->GetReference() : 0;

			sprintf_s( szBuff, "[%02d:%02d]/%02d:%02d] , %s , %s , vol=%.1f , ref=%03d  ||  %s (%s)",
				nCurMin, nCurSec, nPlayMin, nPlaySec, strType.c_str(), bVirtual ? "[V]" : "[R]",
				fVol, nReference, pSound->GetFileName(), strID.c_str());

			AddString( szBuff);
		}
	}

	global.sound->Unlock();
}