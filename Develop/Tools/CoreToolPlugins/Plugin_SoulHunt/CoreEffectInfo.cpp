#include "stdafx.h"
#include "CoreEffectInfo.h"

CoreEffectInfoMgr::CoreEffectInfoMgr()
: XEffectInfoMgr()
{

}

void CoreEffectInfoMgr::GetCommaStringSplitter( const tstring& strValue, vector<tstring>& vecOut )
{
	tstring t = strValue;
	tstring motion, motionret = _T("");
	while(1)
	{
		int f = t.find(_T(","));
		if (f != wstring::npos)
		{
			tstring motion = t.substr(0, f);
			vecOut.push_back(motion);

			t = t.substr(f+1);
		}
		else
		{
			vecOut.push_back(t);
			return;
		}
	}
}
