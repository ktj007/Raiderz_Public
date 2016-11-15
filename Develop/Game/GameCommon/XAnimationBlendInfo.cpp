#include "stdafx.h"
#include "XAnimationBlendInfo.h"
#include "MLocale.h"

const float				BLEND_DATA::DEFAULT_BLEND_DURATION	= 0.0f;

const BLEND_DATA	XAnimationBlendInfo::DEFAULT_ANIMATION_TRANSITION_DATA( BLEND_DATA::DEFAULT_BLEND_DURATION );

XAnimationBlendInfo::XAnimationBlendInfo()
{

}

XAnimationBlendInfo::~XAnimationBlendInfo()
{

}

void XAnimationBlendInfo::Clear()
{
	m_AnimationBlendData.clear();
}

bool XAnimationBlendInfo::SetAnimationTransData( std::tstring& source, std::tstring& target, float fDurationTime )
{
	if( source == target )
		return false;

	AnimationBlendPair pair = AnimationBlendPair( source, target );
	m_AnimationBlendData[pair] = BLEND_DATA(fDurationTime);

	return true;
}

BLEND_DATA XAnimationBlendInfo::GetAnimationTransData( std::tstring& source, std::tstring& target, RMeshAnimationMgr* AnimationMgr )
{
	if(AnimationMgr &&
		CheckExistAnimation(AnimationMgr, source, target) == false)
	{
		return DEFAULT_ANIMATION_TRANSITION_DATA;
	}

	AnimationBlendData::const_iterator i = m_AnimationBlendData.find( AnimationBlendPair( source, target ) );
	if( i != m_AnimationBlendData.end() )
	{
		return (i->second);
	}

	return DEFAULT_ANIMATION_TRANSITION_DATA;
}

bool XAnimationBlendInfo::DeleteAnimationTransType( std::tstring& source, std::tstring& target )
{
	return ( m_AnimationBlendData.erase( AnimationBlendPair( source, target ) ) > 0 );
}

bool XAnimationBlendInfo::IsAnimationNameMatch( const TCHAR* source, const TCHAR* target )
{
	if( _tcsncmp( source, target, _tcslen( source ) ) == 0 )
	{
		return true;
	}

	return false;
}

bool XAnimationBlendInfo::IsNotDefaultValue( BLEND_DATA blendData )
{
	if( blendData.fDurationTime != BLEND_DATA::DEFAULT_BLEND_DURATION )
		return true;

	return false;
}

bool XAnimationBlendInfo::CheckExistAnimation( RMeshAnimationMgr* AnimationMgr, std::tstring& source, std::tstring& target )
{
	RAnimation* sourceAni = AnimationMgr->GetAnimation( MLocale::ConvTCHARToAnsi(source.c_str()).c_str() );
	if( !sourceAni )	return false;

	RAnimation* targetAni = AnimationMgr->GetAnimation( MLocale::ConvTCHARToAnsi(target.c_str()).c_str() );
	if( !targetAni )	return false;

	return true;
}