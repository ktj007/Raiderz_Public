#pragma once

#include "CStringResMgr.h"

class XSoundResMgr : public CStringResMgr
{
public:
	typedef map<tstring, CStringRes<int>*>	result_map;

protected:
	result_map	m_mapResultStrings;

	virtual bool OnInit(const CLangNode& node) override;

public:
	XSoundResMgr(void);
	virtual ~XSoundResMgr(void);
	static XSoundResMgr* GetInstance();

	const TCHAR* GetCmdResultSound(int nKey);

	void PlayTalentResultSound( int nResult );
};

inline void XCmdResultSound(int nKey)
{
	if ( XGETCFG_AUDIO_SYSTEMMESSAGE == 2) return;

	wstring strMsg = XSoundResMgr::GetInstance()->GetCmdResultSound(nKey);
	if(strMsg == _T("")) return;

	if ( XGETCFG_AUDIO_SYSTEMMESSAGE == 1)
	{
		size_t pos = strMsg.find(_T("male"));
		strMsg.replace(pos, 4, _T("female"));
	}

	if ( global.sound)
		global.sound->Play( MLocale::ConvUTF16ToAnsi(strMsg).c_str());
}
