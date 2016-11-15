#include "stdafx.h"
#include "XGameEffectInfo.h"
#include "XStrings.h"

XGameEffectInfoMgr::XGameEffectInfoMgr()
: XEffectInfoMgr()
{

}

void XGameEffectInfoMgr::GetCommaStringSplitter( const tstring& strValue, vector<tstring>& vecOut )
{
	XStrings::GetCommaStringSplitter(strValue, vecOut);
}
