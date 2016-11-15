#include "stdafx.h"
#include "XReActionAnimationInfo.h"

XReActionAnimationInfo::XReActionAnimationInfo()
{

}

XReActionAnimationInfo::~XReActionAnimationInfo()
{

}

void XReActionAnimationInfo::Set( tstring& source, ReAction_DATA& reAction_Data )
{
	m_mapReActionAnimationList.insert(map<tstring, ReAction_DATA>::value_type(source, reAction_Data));
}

ReAction_DATA * XReActionAnimationInfo::Get( tstring strBaseAni )
{
	map<tstring, ReAction_DATA>::iterator itFind = m_mapReActionAnimationList.find(strBaseAni);
	if(itFind != m_mapReActionAnimationList.end())
		return &(itFind->second);

	return NULL;
}
