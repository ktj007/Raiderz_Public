#include "stdafx.h"
#include "XAnimationHitSoundInfo.h"
#include "XCharacter.h"

XAnimationHitSoundInfo::XAnimationHitSoundInfo()
{
	m_mapAnimationHitSoundList.clear();
}

XAnimationHitSoundInfo::~XAnimationHitSoundInfo()
{

}

tstring XAnimationHitSoundInfo::Get( tstring strBaseAni )
{
	map<tstring, tstring>::iterator itFind = m_mapAnimationHitSoundList.find(strBaseAni);
	if(itFind != m_mapAnimationHitSoundList.end())
		return itFind->second;

	return _T("");
}

void XAnimationHitSoundInfo::Set( tstring& source, tstring& soundname )
{
	map<tstring, tstring>::iterator it = m_mapAnimationHitSoundList.find(source);
	if(it != m_mapAnimationHitSoundList.end())
		it->second = soundname;
	else
		m_mapAnimationHitSoundList.insert(map<tstring, tstring>::value_type(source, soundname));
}
