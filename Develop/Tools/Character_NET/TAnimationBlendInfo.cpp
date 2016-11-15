#include "stdafx.h"
#include "TAnimationBlendInfo.h"
#include "MXml.h"

void TAnimationBlendInfo::GetAnimationBlendList( string& strSource, ANIMATION_BLEND_LIST& pOutBlendList )
{
	pOutBlendList.strSource = strSource;

	for(AnimationBlendData::iterator it = m_AnimationBlendData.begin(); it != m_AnimationBlendData.end(); ++it)
	{
		if(it->first.first == strSource)
		{
			ANIMATION_BLEND_DATA blendData;
			blendData.strTarget = it->first.second;
			blendData.fDurationTime = it->second.fDurationTime;

			pOutBlendList.vecTargetList.push_back(blendData);
		}
	}
}
