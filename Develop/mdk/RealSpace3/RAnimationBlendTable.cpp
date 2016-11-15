#include "StdAfx.h"
#include "RAnimationBlendTable.h"
#include "RMeshAnimationMgr.h"
#include "RSkeleton.h"

namespace rs3 {

const RANIMATIONTRANSITIONTYPE	BLENDDATA::DEFAULT_BLEND_TYPE		= RAT_IMMEDIATE_BLEND;
const unsigned int				BLENDDATA::DEFAULT_BLEND_DURATION	= 100;

const BLENDDATA	RAnimationBlendTable::DEFAULT_ANIMATION_TRANSITION_DATA( BLENDDATA::DEFAULT_BLEND_TYPE, BLENDDATA::DEFAULT_BLEND_DURATION );

RAnimationBlendTable::RAnimationBlendTable() :
	m_AnimationMgr(NULL)
{
}

RAnimationBlendTable::~RAnimationBlendTable()
{
}

void RAnimationBlendTable::Clear()
{
	m_AnimationBlendData.clear();
	m_AnimationMgr = NULL;
}

void RAnimationBlendTable::Reset( RMeshAnimationMgr* const animationMgr )
{
	assert( animationMgr );

	/// 이미 셋팅 된 같은 애니메이션 매니저가 있다면 기존 데이터를 보존한다.
	if( m_AnimationMgr != animationMgr )
	{
		Clear();
		m_AnimationMgr = animationMgr;
	}
}

bool RAnimationBlendTable::SetAnimationTransData( const char* source, const char* target, RANIMATIONTRANSITIONTYPE aniTransType, unsigned int nDuration )
{
	if( !IsValid() )
		return false;

	RAnimation* sourceAni = m_AnimationMgr->GetAnimation( source );
	if( !sourceAni )
		return false;

	RAnimation* targetAni = m_AnimationMgr->GetAnimation( target );
	if( !targetAni )
		return false;

	return SetAnimationTransData( sourceAni->GetID(), targetAni->GetID(), aniTransType, nDuration );
}

bool RAnimationBlendTable::SetAnimationTransData( int source, int target, RANIMATIONTRANSITIONTYPE aniTransType, unsigned int nDuration )
{
	if( !IsValid() )
		return false;

	if( source == target )
		return false;

	AnimationBlendPair pair = AnimationBlendPair( source, target );
	m_AnimationBlendData[pair] = BLENDDATA(aniTransType, nDuration);
	return true;
}

BLENDDATA RAnimationBlendTable::GetAnimationTransData( const char* source, const char* target )
{
	if( !IsValid() )
		return DEFAULT_ANIMATION_TRANSITION_DATA;

	RAnimation* sourceAni = m_AnimationMgr->GetAnimation( source );
	if( !sourceAni )
		return DEFAULT_ANIMATION_TRANSITION_DATA;

	RAnimation* targetAni = m_AnimationMgr->GetAnimation( target );
	if( !targetAni )
		return DEFAULT_ANIMATION_TRANSITION_DATA;

	return GetAnimationTransData( sourceAni->GetID(), targetAni->GetID() );
}

BLENDDATA RAnimationBlendTable::GetAnimationTransData( int source, int target )
{
	AnimationBlendData::const_iterator i = m_AnimationBlendData.find( AnimationBlendPair( source, target ) );
	if( i != m_AnimationBlendData.end() )
	{
		return (i->second);
	}

	return DEFAULT_ANIMATION_TRANSITION_DATA;
}

bool RAnimationBlendTable::DeleteAnimationTransType( const char* source, const char* target )
{
	if( !IsValid() )
		return false;
	
	RAnimation* sourceAni = m_AnimationMgr->GetAnimation( source );
	if( !sourceAni )
		return false;

	RAnimation* targetAni = m_AnimationMgr->GetAnimation( target );
	if( !targetAni )
		return false;

	return DeleteAnimationTransType( sourceAni->GetID(), targetAni->GetID() );
}

bool RAnimationBlendTable::DeleteAnimationTransType( int source, int target )
{
	return ( m_AnimationBlendData.erase( AnimationBlendPair( source, target ) ) > 0 );
}

void RAnimationBlendTable::SaveXml( MXmlElement* parentXmlElement )
{
	if( !IsValid() )
		return;

	MXmlElement* pAniTransElements = new MXmlElement( "AnimationTransitions" );
	parentXmlElement->LinkEndChild( pAniTransElements );

	for( AnimationBlendData::const_iterator i = m_AnimationBlendData.begin(); i != m_AnimationBlendData.end(); ++i )
	{
		if( IsNotDefaultValue( i->second ) )
		{
			MXmlElement* pAniTransElement = new MXmlElement( "AnimationTransition" );
			pAniTransElements->LinkEndChild( pAniTransElement );

			RAnimation* sourceAni = m_AnimationMgr->m_NodeHashList[i->first.first];
			RAnimation* targetAni = m_AnimationMgr->m_NodeHashList[i->first.second];

			_SetAttribute( pAniTransElement, "source", sourceAni->GetAliasName() );
			_SetAttribute( pAniTransElement, "target", targetAni->GetAliasName() );
			_SetAttribute( pAniTransElement, "duration", (int)i->second.nDuration );
			_SetAttribute( pAniTransElement, "blend_type", GetTransTypeString( i->second.eType ) );
		}
	}
}

void RAnimationBlendTable::LoadXml( RMeshAnimationMgr* const animationMgr, MXmlElement* parentXmlElement, const char* szFilter)
{
	// 데이터 로드 시에는 모든 데이터를 무조건 초기화 함.
//	Clear();
//	Reset( animationMgr );

	if ( m_AnimationMgr != animationMgr)
		m_AnimationMgr = animationMgr;


	char sourceBuff[MAX_PATH];
	char targetBuff[MAX_PATH];
	char blendTypeBuff[MAX_PATH];
	char szDuration[MAX_PATH];

	size_t nFilterLen = strlen( szFilter);

//	MXmlElement* pAniTransElements = parentXmlElement->FirstChildElement( "AnimationTransitions" );
//	if( pAniTransElements )
//	{
		MXmlElement* pAniTransElement = parentXmlElement->FirstChildElement( "AnimationTransition" );
		for( pAniTransElement; pAniTransElement != NULL; pAniTransElement = pAniTransElement->NextSiblingElement() )
		{
			_Attribute( sourceBuff,		MAX_PATH, pAniTransElement, "source", "" );
			_Attribute( targetBuff,		MAX_PATH, pAniTransElement, "target", "" );

			if ( nFilterLen > 2)
			{
				if ( strncmp( sourceBuff, szFilter, nFilterLen) != 0  ||  strncmp( targetBuff, szFilter, nFilterLen) != 0)
					continue;
			}

			strcpy_s( sourceBuff, sourceBuff + nFilterLen);
			strcpy_s( targetBuff, targetBuff + nFilterLen);

			_Attribute( blendTypeBuff,	MAX_PATH, pAniTransElement, "blend_type", "" );
			
			unsigned int nDuration = BLENDDATA::DEFAULT_BLEND_DURATION;
			if ( _Attribute( szDuration,		MAX_PATH, pAniTransElement, "duration", "" ) )
				nDuration = atoi(szDuration);

			RANIMATIONTRANSITIONTYPE eType = BLENDDATA::DEFAULT_BLEND_TYPE;
			if( strlen( sourceBuff ) && strlen( targetBuff ) && strlen( blendTypeBuff ) )
				eType = static_cast<RANIMATIONTRANSITIONTYPE>( GetTransTypeId( blendTypeBuff ) );

			BLENDDATA blendData( eType, nDuration );
			if( IsNotDefaultValue( blendData ) )
			{
				SetAnimationTransData( sourceBuff, targetBuff, eType, nDuration );
			}

		}
//	}
}

bool RAnimationBlendTable::IsValid()
{
	if( m_AnimationMgr )
		return true;

	return false;
}

bool RAnimationBlendTable::IsAnimationNameMatch( const char* source, const char* target )
{
	if( strncmp( source, target, strlen( source ) ) == 0 )
	{
		return true;
	}

	return false;
}

bool RAnimationBlendTable::IsNotDefaultValue( BLENDDATA blendData )
{
	if( blendData.eType != BLENDDATA::DEFAULT_BLEND_TYPE )
		return true;

	if( blendData.nDuration != BLENDDATA::DEFAULT_BLEND_DURATION )
		return true;

	return false;
}

const char* RAnimationBlendTable::GetTransTypeString( int type )
{
	static const char* transTypeString[] =
	{
		"None",
		"Immediate",
		"Delayed",
		"Crossfade",
	};

	return transTypeString[type];
}

int RAnimationBlendTable::GetTransTypeId( const char* typestr )
{
	for( int i = 0; i <= RAT_CROSS_FADE; ++i )
	{
		if( strcmp( GetTransTypeString( i ), typestr ) == 0 )
			return i;
	}

	return 0;
}

}