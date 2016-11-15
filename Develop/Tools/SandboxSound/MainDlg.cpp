#include "stdafx.h"
#include "Main.h"
#include "MainDlg.h"
#include <mmsystem.h>


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SEARCH1, OnBnClickedSearch1)
	ON_BN_CLICKED(IDC_SEARCH2, OnBnClickedSearch2)
	ON_BN_CLICKED(IDC_SEARCH3, OnBnClickedSearch3)
	ON_BN_CLICKED(IDC_SEARCH4, OnBnClickedSearch4)
	ON_CBN_SELCHANGE(IDC_TYPE1, OnCbnSelchangeType1)
	ON_CBN_SELCHANGE(IDC_TYPE2, OnCbnSelchangeType2)
	ON_CBN_SELCHANGE(IDC_TYPE3, OnCbnSelchangeType3)
	ON_CBN_SELCHANGE(IDC_TYPE4, OnCbnSelchangeType4)
	ON_BN_CLICKED(IDC_PLAY1, OnBnClickedPlay1)
	ON_BN_CLICKED(IDC_PLAY2, OnBnClickedPlay2)
	ON_BN_CLICKED(IDC_PLAY3, OnBnClickedPlay3)
	ON_BN_CLICKED(IDC_PLAY4, OnBnClickedPlay4)
	ON_BN_CLICKED(IDC_PLAYALL, OnBnClickedPlayAll)
	ON_BN_CLICKED(IDC_STOPALL, OnBnClickedStopAll)
	ON_BN_CLICKED(IDC_RELEASEALL, OnBnClickedReleaseAll)
	ON_BN_CLICKED(IDC_MUTE1, OnBnClickedMute1)
	ON_BN_CLICKED(IDC_MUTE2, OnBnClickedMute2)
	ON_BN_CLICKED(IDC_MUTE3, OnBnClickedMute3)
	ON_BN_CLICKED(IDC_MUTE4, OnBnClickedMute4)
	ON_BN_CLICKED(IDC_ENCOMPRESSOR, OnBnClickedEncompressor)
	ON_CBN_SELCHANGE(IDC_REVERB, OnCbnSelchangeReverb)
	ON_BN_CLICKED(IDC_HWACCEL, &CMainDlg::OnBnClickedHwaccel)
	ON_BN_CLICKED(IDC_DOPPLER, &CMainDlg::OnBnClickedDoppler)
	ON_BN_CLICKED(IDC_SILENCE, &CMainDlg::OnBnClickedSilence)
	ON_CBN_SELCHANGE(IDC_SPEAKERMODE, &CMainDlg::OnCbnSelchangeSpeakermode)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_RANGE, m_SoundRange);
}





CMainDlg::CMainDlg(CWnd* pParent)
: CDialog(CMainDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for ( int i = 0;  i < 4;  i++)
		m_nPlayedSound[ i] = RSD_CHANNELID_NONE;
}


BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);


	// Init sound
	m_Sound.Create();


	// Init UI
	GetDlgItem( IDC_FILE1)->SetWindowText( AfxGetApp()->GetProfileString( "Sound unit 1", "name", ""));
	GetDlgItem( IDC_FILE2)->SetWindowText( AfxGetApp()->GetProfileString( "Sound unit 2", "name", ""));
	GetDlgItem( IDC_FILE3)->SetWindowText( AfxGetApp()->GetProfileString( "Sound unit 3", "name", ""));
	GetDlgItem( IDC_FILE4)->SetWindowText( AfxGetApp()->GetProfileString( "Sound unit 4", "name", ""));

	((CComboBox*)GetDlgItem( IDC_TYPE1))->SetCurSel( AfxGetApp()->GetProfileInt( "Sound unit 1", "type", 1));
	((CComboBox*)GetDlgItem( IDC_TYPE2))->SetCurSel( AfxGetApp()->GetProfileInt( "Sound unit 2", "type", 1));
	((CComboBox*)GetDlgItem( IDC_TYPE3))->SetCurSel( AfxGetApp()->GetProfileInt( "Sound unit 3", "type", 1));
	((CComboBox*)GetDlgItem( IDC_TYPE4))->SetCurSel( AfxGetApp()->GetProfileInt( "Sound unit 4", "type", 1));

	((CButton*)GetDlgItem( IDC_LOOP1))->SetCheck( AfxGetApp()->GetProfileInt( "Sound unit 1", "loop", 0) ? true : false);
	((CButton*)GetDlgItem( IDC_LOOP2))->SetCheck( AfxGetApp()->GetProfileInt( "Sound unit 2", "loop", 0) ? true : false);
	((CButton*)GetDlgItem( IDC_LOOP3))->SetCheck( AfxGetApp()->GetProfileInt( "Sound unit 3", "loop", 0) ? true : false);
	((CButton*)GetDlgItem( IDC_LOOP4))->SetCheck( AfxGetApp()->GetProfileInt( "Sound unit 4", "loop", 0) ? true : false);



	m_Sound.SetReverb( AfxGetApp()->GetProfileInt( "Sound reverb", "type", 0));
	((CComboBox*)GetDlgItem( IDC_REVERB))->SetCurSel( m_Sound.GetReverb());


	m_Sound.SetChannelGroupVolume( 0, (float)AfxGetApp()->GetProfileInt( "Sound volume", "master", 10) * 0.1f);
	m_Sound.SetChannelGroupVolume( 1, (float)AfxGetApp()->GetProfileInt( "Sound volume", "2D", 10) * 0.1f);
	m_Sound.SetChannelGroupVolume( 2, (float)AfxGetApp()->GetProfileInt( "Sound volume", "3D", 10) * 0.1f);
	m_Sound.SetChannelGroupVolume( 3, (float)AfxGetApp()->GetProfileInt( "Sound volume", "BGM", 10) * 0.1f);

	CSliderCtrl* pSlider;
	pSlider = (CSliderCtrl*)GetDlgItem( IDC_VOLUME1);
	pSlider->SetRange( 0, 10);
	pSlider->SetPos( (int)( m_Sound.GetChannelGroupVolume( 0) * 10.0f));
	pSlider = (CSliderCtrl*)GetDlgItem( IDC_VOLUME2);
	pSlider->SetRange( 0, 10);
	pSlider->SetPos( (int)( m_Sound.GetChannelGroupVolume( 1) * 10.0f));
	pSlider = (CSliderCtrl*)GetDlgItem( IDC_VOLUME3);
	pSlider->SetRange( 0, 10);
	pSlider->SetPos( (int)( m_Sound.GetChannelGroupVolume( 2) * 10.0f));
	pSlider = (CSliderCtrl*)GetDlgItem( IDC_VOLUME4);
	pSlider->SetRange( 0, 10);
	pSlider->SetPos( (int)( m_Sound.GetChannelGroupVolume( 3) * 10.0f));

	m_Sound.SetHardwareAccelerate( AfxGetApp()->GetProfileInt( "Sound hardwareacc", "enable", 1) ? true : false);
	((CButton*)GetDlgItem( IDC_HWACCEL))->SetCheck( m_Sound.GetHardwareAccelerate() ? TRUE : FALSE);

	m_Sound.SetDoppler( AfxGetApp()->GetProfileInt( "Sound doppler", "enable", 0) ? true : false);
	((CButton*)GetDlgItem( IDC_DOPPLER))->SetCheck( m_Sound.GetDoppler() ? TRUE : FALSE);

	((CComboBox*)GetDlgItem( IDC_SPEAKERMODE))->SetCurSel( m_Sound.GetSpeakerMode());


	m_Sound.SetCompressor( AfxGetApp()->GetProfileInt( "Sound compressor", "enable", 1) ? true : false);

	((CButton*)GetDlgItem( IDC_ENCOMPRESSOR))->SetCheck( m_Sound.GetCompressor() ? TRUE : FALSE);
	m_Sound.SetCompressorProp( (float)((int)AfxGetApp()->GetProfileInt( "Sound compressor", "threshold", -4)),
		(float)AfxGetApp()->GetProfileInt( "Sound compressor", "attack", 10),
		(float)AfxGetApp()->GetProfileInt( "Sound compressor", "release", 300),
		(float)AfxGetApp()->GetProfileInt( "Sound compressor", "gainmakeup", 0));

	float fThreshold, fAttack, fRelease, fGainMakeup;
	m_Sound.GetCompressorProp( &fThreshold, &fAttack, &fRelease, &fGainMakeup);
	pSlider = (CSliderCtrl*)GetDlgItem( IDC_THRESHOLD);
	pSlider->SetRange( 0, 60);
	pSlider->SetPos( (int)fThreshold + 60);
	pSlider = (CSliderCtrl*)GetDlgItem( IDC_ATTACK);
	pSlider->SetRange( 10, 200);
	pSlider->SetPos( (int)fAttack);
	pSlider = (CSliderCtrl*)GetDlgItem( IDC_RELEASE);
	pSlider->SetRange( 20, 1000);
	pSlider->SetPos( (int)fRelease);
	pSlider = (CSliderCtrl*)GetDlgItem( IDC_GAINMAKEUP);
	pSlider->SetRange( 0, 30);
	pSlider->SetPos( (int)fGainMakeup);

	CString str;
	str.Format( "%d db", (int)fThreshold);
	GetDlgItem( IDC_COMPVAL1)->SetWindowText( str);
	str.Format( "%d ms", (int)fAttack);
	GetDlgItem( IDC_COMPVAL2)->SetWindowText( str);
	str.Format( "%d ms", (int)fRelease);
	GetDlgItem( IDC_COMPVAL3)->SetWindowText( str);
	str.Format( "%d db", (int)fGainMakeup);
	GetDlgItem( IDC_COMPVAL4)->SetWindowText( str);


	m_SoundRange.SetPosition( 0, AfxGetApp()->GetProfileInt( "Sound unit 1", "pos_x", -500), AfxGetApp()->GetProfileInt( "Sound unit 1", "pos_y", 500));
	m_SoundRange.SetPosition( 1, AfxGetApp()->GetProfileInt( "Sound unit 2", "pos_x", 500),  AfxGetApp()->GetProfileInt( "Sound unit 2", "pos_y", 500));
	m_SoundRange.SetPosition( 2, AfxGetApp()->GetProfileInt( "Sound unit 3", "pos_x", -500), AfxGetApp()->GetProfileInt( "Sound unit 3", "pos_y", -500));
	m_SoundRange.SetPosition( 3, AfxGetApp()->GetProfileInt( "Sound unit 4", "pos_x", 500),  AfxGetApp()->GetProfileInt( "Sound unit 4", "pos_y", -500));


	// Create sound range dialog
	m_SoundRange.Create();


	// Run timer
	SetTimer( 0, 1, NULL);
	SetTimer( 1, 200, NULL);
	SetTimer( 2, 5000, NULL);

	return TRUE;
}


void CMainDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
		CDialog::OnPaint();
}


void CMainDlg::OnTimer(UINT nIDEvent)
{
	switch ( nIDEvent)
	{
	case 0 :
		{
			float pos[] = { 0.0f, 0.0f, 0.0f };
			float dir[] = { 0.0f, 1.0f, 0.0f };
			float up[] = { 0.0f, 0.0f, 1.0f };
			m_Sound.SetListener( pos, dir, up);
			m_Sound.Update();
			break;
		}

	case 1 :
		{
			int nPlayID[] = { IDC_PLAY1, IDC_PLAY2, IDC_PLAY3, IDC_PLAY4 };
			int nTimeID[] = { IDC_TIMELINE1, IDC_TIMELINE2, IDC_TIMELINE3, IDC_TIMELINE4 };

			for ( int i = 0;  i < 4;  i++)
			{
				if ( m_nPlayedSound[ i] != RSD_CHANNELID_NONE)
				{
					((CSliderCtrl*)GetDlgItem( nTimeID[ i]))->SetRange( 0, max( 1, m_Sound.GetChannelPlayingTime( m_nPlayedSound[ i])));
					((CSliderCtrl*)GetDlgItem( nTimeID[ i]))->SetPos( m_Sound.GetChannelPosition( m_nPlayedSound[ i]));
				}
				else
				{
					((CSliderCtrl*)GetDlgItem( nTimeID[ i]))->SetRange( 0, 1);
					((CSliderCtrl*)GetDlgItem( nTimeID[ i]))->SetPos( 0);
				}
			}

			break;
		}

	case 2 :
		{
			POINT _p = m_SoundRange.GetPosition( 0);
			AfxGetApp()->WriteProfileInt( "Sound unit 1", "pos_x", _p.x);
			AfxGetApp()->WriteProfileInt( "Sound unit 1", "pos_y", _p.y);

			_p = m_SoundRange.GetPosition( 1);
			AfxGetApp()->WriteProfileInt( "Sound unit 2", "pos_x", _p.x);
			AfxGetApp()->WriteProfileInt( "Sound unit 2", "pos_y", _p.y);

			_p = m_SoundRange.GetPosition( 2);
			AfxGetApp()->WriteProfileInt( "Sound unit 3", "pos_x", _p.x);
			AfxGetApp()->WriteProfileInt( "Sound unit 3", "pos_y", _p.y);

			_p = m_SoundRange.GetPosition( 3);
			AfxGetApp()->WriteProfileInt( "Sound unit 4", "pos_x", _p.x);
			AfxGetApp()->WriteProfileInt( "Sound unit 4", "pos_y", _p.y);
			break;
		}
	}

	CDialog::OnTimer(nIDEvent);
}


void CMainDlg::OnBnClickedSearch1()		{ OpenFileDialog( 1);		}
void CMainDlg::OnBnClickedSearch2()		{ OpenFileDialog( 2);		}
void CMainDlg::OnBnClickedSearch3()		{ OpenFileDialog( 3);		}
void CMainDlg::OnBnClickedSearch4()		{ OpenFileDialog( 4);		}

void CMainDlg::OpenFileDialog( int nIndex)
{
	CFileDialog dlg( TRUE, NULL, NULL, OFN_EXPLORER | OFN_FILEMUSTEXIST, "All Files (*.*)|*.*|Wave Files (*.wav)|*.wav|MP3 Files (*.mp3)|*.mp3|Ogg Files (*.ogg)|*.ogg||", this);
	if ( dlg.DoModal() != IDOK)		return;

	CString strSample;
	strSample.Format( "_sample%d", nIndex);

	if ( m_Sound.IsExistSample( strSample.GetBuffer()) == true)
		m_Sound.RemoveSample( strSample.GetBuffer());

	int nID[] = { IDC_FILE1, IDC_FILE2, IDC_FILE3, IDC_FILE4 };
	GetDlgItem( nID[ nIndex - 1])->SetWindowText( dlg.GetPathName().GetBuffer());

	((CEdit*)GetDlgItem( nID[ nIndex - 1]))->SetSel( dlg.GetPathName().GetLength(), dlg.GetPathName().GetLength());


	CString str;
	str.Format( "Sound unit %d", nIndex);
	AfxGetApp()->WriteProfileString( str, "name", dlg.GetPathName());
}


void CMainDlg::OnCbnSelchangeType1()	{ ChangeType( 1);			}
void CMainDlg::OnCbnSelchangeType2()	{ ChangeType( 2);			}
void CMainDlg::OnCbnSelchangeType3()	{ ChangeType( 3);			}
void CMainDlg::OnCbnSelchangeType4()	{ ChangeType( 4);			}

void CMainDlg::ChangeType( int nIndex)
{
	CString strSample;
	strSample.Format( "_sample%d", nIndex);

	if ( m_Sound.IsExistSample( strSample.GetBuffer()) == true)
		m_Sound.RemoveSample( strSample.GetBuffer());
}


void CMainDlg::OnBnClickedPlay1()			{ Play( 1);					}
void CMainDlg::OnBnClickedPlay2()			{ Play( 2);					}
void CMainDlg::OnBnClickedPlay3()			{ Play( 3);					}
void CMainDlg::OnBnClickedPlay4()			{ Play( 4);					}

void CMainDlg::Play( int nIndex)
{
	CString strSample;
	strSample.Format( "_sample%d", nIndex);

	if ( m_Sound.IsExistSample( strSample.GetBuffer()) == false)
	{
		int nFileID[] = { IDC_FILE1, IDC_FILE2, IDC_FILE3, IDC_FILE4 };
		int nTypeID[] = { IDC_TYPE1, IDC_TYPE2, IDC_TYPE3, IDC_TYPE4 };
		int nLoop[] = { IDC_LOOP1, IDC_LOOP2, IDC_LOOP3, IDC_LOOP4 };

		CString strFileName;
		GetDlgItem( nFileID[ nIndex - 1])->GetWindowText( strFileName);
		if ( strFileName.IsEmpty() == TRUE)		return;

		int nType = ((CComboBox*)GetDlgItem( nTypeID[ nIndex - 1]))->GetCurSel() + 1;
		bool bLoop = ((CButton*)GetDlgItem( nLoop[ nIndex - 1]))->GetCheck() == TRUE ? true : false;

		m_Sound.AddSample( strSample.GetBuffer(), strFileName.GetBuffer(), nType, 1.0f, bLoop, false, 128, 100, 1000, nIndex == 1 ? true : false);


		CString str;
		str.Format( "Sound unit %d", nIndex);
		AfxGetApp()->WriteProfileInt( str, "type", nType - 1);
		AfxGetApp()->WriteProfileInt( str, "loop", ((CButton*)GetDlgItem( nLoop[ nIndex - 1]))->GetCheck());
	}

	int nID = m_Sound.Play( strSample.GetBuffer(), strSample.GetBuffer(), new XSoundAdapter( nIndex));
	if ( nID == RSD_CHANNELID_NONE)
		return;

	for ( int i = 0;  i < 4;  i++)
	{
		if ( m_nPlayedSound[ i] == nID)
			m_nPlayedSound[ i] = RSD_CHANNELID_NONE;
	}

 	m_nPlayedSound[ nIndex - 1] = nID;
}


void CMainDlg::OnBnClickedPlayAll()
{
	Play( 1);
	Play( 2);
	Play( 3);
	Play( 4);
}


void CMainDlg::OnBnClickedStopAll()
{
	m_Sound.StopAll();
}


void CMainDlg::OnBnClickedReleaseAll()
{
//	m_Sound.ReleaseAllSound();
}


void CMainDlg::OnBnClickedHwaccel()
{
	bool bEnable = ((CButton*)GetDlgItem( IDC_HWACCEL))->GetCheck()  ?  true : false;
	m_Sound.SetHardwareAccelerate( bEnable);

	AfxGetApp()->WriteProfileInt( "Sound hardwareacc", "enable", m_Sound.GetHardwareAccelerate()  ? 1 : 0);
}


void CMainDlg::OnBnClickedDoppler()
{
	bool bEnable = ((CButton*)GetDlgItem( IDC_DOPPLER))->GetCheck()  ?  true : false;
	m_Sound.SetDoppler( bEnable);

	AfxGetApp()->WriteProfileInt( "Sound doppler", "enable", m_Sound.GetDoppler()  ? 1 : 0);
}


void CMainDlg::OnBnClickedSilence()
{
	bool bSilence = ((CButton*)GetDlgItem( IDC_SILENCE))->GetCheck()  ?  true : false;
	m_Sound.SetSilence( bSilence);
}


void CMainDlg::OnCbnSelchangeSpeakermode()
{
	int nMode = ((CComboBox*)GetDlgItem( IDC_SPEAKERMODE))->GetCurSel();
	m_Sound.SetSpeakerMode( nMode);
}


void CMainDlg::OnBnClickedMute1()		{ Mute( 1);			}
void CMainDlg::OnBnClickedMute2()		{ Mute( 2);			}
void CMainDlg::OnBnClickedMute3()		{ Mute( 3);			}
void CMainDlg::OnBnClickedMute4()		{ Mute( 4);			}

void CMainDlg::Mute( int nIndex)
{
	int nID[] = { IDC_MUTE1, IDC_MUTE2, IDC_MUTE3, IDC_MUTE4 };

	CButton* pMute = (CButton*)GetDlgItem( nID[ nIndex - 1]);
	if ( pMute != NULL)
	{
		int nChannel = nIndex - 1;
		bool bMute = pMute->GetCheck()  ?  true : false;

		m_Sound.SetChannelGroupMute( nChannel, bMute);
	}
}


void CMainDlg::OnBnClickedEncompressor()
{
	bool bEnable = ((CButton*)GetDlgItem( IDC_ENCOMPRESSOR))->GetCheck()  ?  true : false;
	m_Sound.SetCompressor( bEnable);

	AfxGetApp()->WriteProfileInt( "Sound compressor", "enable", m_Sound.GetCompressor() ? 1 : 0);
}


void CMainDlg::OnCbnSelchangeReverb()
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem( IDC_REVERB);
	if ( pCombo != NULL)
	{
		int nReverb = pCombo->GetCurSel();
		m_Sound.SetReverb( nReverb);

		AfxGetApp()->WriteProfileInt( "Sound reverb", "type", nReverb);
	}
}


void CMainDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch ( pScrollBar->GetDlgCtrlID())
	{
	case IDC_TIMELINE1 :
	case IDC_TIMELINE2 :
	case IDC_TIMELINE3 :
	case IDC_TIMELINE4 :
		{
			int nIndex = -1;
			switch ( pScrollBar->GetDlgCtrlID())
			{
			case IDC_TIMELINE1 :	nIndex = 0;		break;
			case IDC_TIMELINE2 :	nIndex = 1;		break;
			case IDC_TIMELINE3 :	nIndex = 2;		break;
			case IDC_TIMELINE4 :	nIndex = 3;		break;
			}

 			if ( m_nPlayedSound[ nIndex] != NULL)
 				m_Sound.SetChannelPosition( m_nPlayedSound[ nIndex], ((CSliderCtrl*)pScrollBar)->GetPos());

			break;
		}

	case IDC_VOLUME1 :
		m_Sound.SetChannelGroupVolume( 0, (float)((CSliderCtrl*)pScrollBar)->GetPos() * 0.1f);
		AfxGetApp()->WriteProfileInt( "Sound volume", "master", (int)( m_Sound.GetChannelGroupVolume( 0) * 10.0f));
		break;

	case IDC_VOLUME2 :
		m_Sound.SetChannelGroupVolume( 1, (float)((CSliderCtrl*)pScrollBar)->GetPos() * 0.1f);
		AfxGetApp()->WriteProfileInt( "Sound volume", "2D", (int)( m_Sound.GetChannelGroupVolume( 1) * 10.0f));
		break;

	case IDC_VOLUME3 :
		m_Sound.SetChannelGroupVolume( 2, (float)((CSliderCtrl*)pScrollBar)->GetPos() * 0.1f);
		AfxGetApp()->WriteProfileInt( "Sound volume", "3D", (int)( m_Sound.GetChannelGroupVolume( 2) * 10.0f));
		break;

	case IDC_VOLUME4 :
		m_Sound.SetChannelGroupVolume( 3, (float)((CSliderCtrl*)pScrollBar)->GetPos() * 0.1f);
		AfxGetApp()->WriteProfileInt( "Sound volume", "BGM", (int)( m_Sound.GetChannelGroupVolume( 3) * 10.0f));
		break;

	case IDC_THRESHOLD :
	case IDC_ATTACK :
	case IDC_RELEASE :
	case IDC_GAINMAKEUP :
		{
			float fThreshold, fAttack, fRelease, fGainMakeup;
			m_Sound.GetCompressorProp( &fThreshold, &fAttack, &fRelease, &fGainMakeup);

			switch ( pScrollBar->GetDlgCtrlID())
			{
			case IDC_THRESHOLD :
				fThreshold = (float)((CSliderCtrl*)GetDlgItem( IDC_THRESHOLD))->GetPos() - 60;
				AfxGetApp()->WriteProfileInt( "Sound compressor", "threshold", (int)fThreshold);
				break;

			case IDC_ATTACK :
				fAttack = (float)((CSliderCtrl*)GetDlgItem( IDC_ATTACK))->GetPos();
				AfxGetApp()->WriteProfileInt( "Sound compressor", "attack", (int)fAttack);
				break;

			case IDC_RELEASE :
				fRelease = (float)((CSliderCtrl*)GetDlgItem( IDC_RELEASE))->GetPos();
				AfxGetApp()->WriteProfileInt( "Sound compressor", "release", (int)fRelease);
				break;

			case IDC_GAINMAKEUP :
				fGainMakeup = (float)((CSliderCtrl*)GetDlgItem( IDC_GAINMAKEUP))->GetPos();
				AfxGetApp()->WriteProfileInt( "Sound compressor", "gainmakeup", (int)fGainMakeup);
				break;
			}

			m_Sound.SetCompressorProp( fThreshold, fAttack, fRelease, fGainMakeup);


			CString str;
			str.Format( "%d db", (int)fThreshold);
			GetDlgItem( IDC_COMPVAL1)->SetWindowText( str);
			str.Format( "%d ms", (int)fAttack);
			GetDlgItem( IDC_COMPVAL2)->SetWindowText( str);
			str.Format( "%d ms", (int)fRelease);
			GetDlgItem( IDC_COMPVAL3)->SetWindowText( str);
			str.Format( "%d db", (int)fGainMakeup);
			GetDlgItem( IDC_COMPVAL4)->SetWindowText( str);

			break;
		}
	}
}
