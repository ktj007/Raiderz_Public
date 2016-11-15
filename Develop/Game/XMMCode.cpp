#include "stdafx.h"
#include "XMMCode.h"
#include "XMMCodeProc.h"
#include "XStrings.h"

XMMCode::XMMCode()
{
	Init();
}

void XMMCode::Init()
{
	InitControlCode();
	InitReplaceChars();
}

void XMMCode::InitControlCode()
{
	// [ani]
	m_TagManager.AddControlCode("ani", new mmcode::MControlCode("ani", new XAniControlCodeProc()));

	// [motion]
	m_TagManager.AddControlCode("motion", new mmcode::MControlCode("motion", new XMotionControlCodeProc()));

	// [talent]
	m_TagManager.AddControlCode("talent", new mmcode::MControlCode("talent", new XTalentControlCodeProc()));

	// [sound]
	m_TagManager.AddControlCode("sound", new mmcode::MControlCode("sound", new XSoundControlCodeProc()));
}

void XMMCode::InitReplaceChars()
{
	// \n
	m_TagManager.AddReplaceChars("[br]", new mmcode::MReplaceChars("\n"));

	// %t
	m_TagManager.AddReplaceChars("%t", new mmcode::MReplaceChars("", new XNameReplaceCharsProc()));

	// %s
	m_TagManager.AddReplaceChars("%s", new mmcode::MReplaceChars("", new XSexReplaceCharsProc()));

	// %r
	m_TagManager.AddReplaceChars("%r", new mmcode::MReplaceChars("", new XRaceReplaceCharsProc()));

}

//////////////////////////////////////////////////////////////

XMMCodeMgr& XMMCodeMgr::GetInstance()
{
	static XMMCodeMgr	st_mgr;
	return st_mgr;
}


void XMMCodeMgr::ExecControlCode( const wchar_t* szKey, MUID uidOwner )
{
	wstring strBuf;
	XStringResMgr::GetInstance()->TransKeyStr(strBuf, szKey);

	mmcode::MControlCodeTransformer transformer(MLocale::ConvUTF16ToAnsi(strBuf.c_str()), m_MMCode.GetTagManager());
	mmcode::MControlCodeTransResult result = transformer.transform();
	result.Proc(&uidOwner);
}

mmcode::MControlCodeTransResult XMMCodeMgr::TransControlCodeAndReplaceChars( const wchar_t* szKey, MUID uidOwner )
{
	wstring strBuf;
	XStringResMgr::GetInstance()->TransKeyStr(strBuf, szKey);

	mmcode::MReplaceCharsTransformer replace_transformer(MLocale::ConvUTF16ToAnsi(strBuf.c_str()), m_MMCode.GetTagManager());
	string replace_out = replace_transformer.transform(&uidOwner).c_str();

	mmcode::MControlCodeTransformer transformer(replace_out, m_MMCode.GetTagManager());

	return transformer.transform();
}

void XMMCodeMgr::TransControlSpliterChars(list<string>& strlist, const char* szKey, MUID uidOwner )
{
	mmcode::MSpliterCharsTransformer transformer(szKey, m_MMCode.GetTagManager());

	transformer.Splite(strlist);
}