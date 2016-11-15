#include "stdafx.h"
#include "TReActionAnimationInfo.h"
#include "MXml.h"

TReActionAnimationInfo::TReActionAnimationInfo()
: XReActionAnimationInfo()
{

}

TReActionAnimationInfo::~TReActionAnimationInfo()
{

}

void TReActionAnimationInfo::DelReActionAnimationInfo( string strBaseAni )
{
	map<tstring, ReAction_DATA>::iterator itFind = m_mapReActionAnimationList.find(strBaseAni);
	if(itFind != m_mapReActionAnimationList.end())
	{
		m_mapReActionAnimationList.erase(itFind);
	}
}
