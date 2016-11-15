#include "stdafx.h"
#include "TEffectInfoMgr.h"
#include "TFileNameSampling.h"
#include "XStrings.h"

using namespace System::IO;

///////////////////////////////////////////////////////////////////////////////
TEffectInfoMgr::TEffectInfoMgr() 
: XEffectInfoMgr()
{
	m_bEffectFileLoadError = false;

	if(Load(FILENAME_EFFECT_INFO_TOOL) == false)
		m_bEffectFileLoadError = true;
}
TEffectInfoMgr::~TEffectInfoMgr()
{

}

bool TEffectInfoMgr::ReLoad()
{
	for(vector<XEffectInfo *>::iterator it = m_vecEffectInfo.begin(); it != m_vecEffectInfo.end(); it++)
	{
		SAFE_DELETE(*it);
	}

	m_vecEffectInfo.clear();

	return Load(FILENAME_EFFECT_INFO_TOOL);
}

bool TEffectInfoMgr::CheckEffectInfoModify()
{
	// 수정이 되었다.
	return ReLoad();
}

void TEffectInfoMgr::GetCommaStringSplitter( const tstring& strValue, vector<tstring>& vecOut )
{
	XStrings::GetCommaStringSplitter(strValue, vecOut);
}
