#include "stdafx.h"
#include "RClothParameter.h"

bool RClothParameter::save( FILE* pFile )
{
	_ASSERT(NULL != pFile);

	int countToSave = 0;
	countToSave = fwrite(this,sizeof(RClothParameter),1,pFile);
	if( 1 != countToSave ) return false;
	return true;
}

bool RClothParameter::load( FILE* pFile )
{
	_ASSERT(NULL != pFile);

	int countToSave = 0;
	countToSave = fread(this,sizeof(RClothParameter),1,pFile);
	if( 1 != countToSave ) return false;
	return true;
}

RClothParameter::RClothParameter( void )
{
	//-[ simulation ]
	m_dampingCoefficientX		= 0.85f;	//새속도 = 계수 * 현재속도
	m_dampingCoefficientY		= 0.85f;
	m_dampingCoefficientZ		= 0.90f;
	m_dampingJigglingVelocity	= 0.2f;	//새속도 = 계수 * 현재속도
	m_timeToSimulate			= 0.032f;
	m_worldSimulation			= true;

	m_gravityX	=   0.0f;
	m_gravityY	=   0.0f;
	m_gravityZ	= -240.0f;

	memset(m_szResolvingLogic,0,sizeof(m_szResolvingLogic));
	strcpy_s(m_szResolvingLogic,"dsdsdscdsdsdsvc");

	//-[ constraint ]
	m_distanceRange					= 1.0f;		
	m_distanceStiffness				= 0.9f;	//이동해야하는 거리 = 계수 * 현재이동해야하는 거리

	m_stretchHorizontalRange		= 1.0f;
	m_stretchHorizontalStiffness	= 0.8f;

	m_stretchVerticalRange			= 1.0f;
	m_stretchVerticalStiffness		= 0.8f;

	//-[virtual translation ]
	m_virtualTranslationThresholdX	= 20.0f;	// 현프레임의 옷의 x이동거리가 이것보다 크면 작동
	m_virtualTranslationThresholdY	= 20.0f;	
	m_virtualTranslationThresholdZ	= 3.0f;

	m_virtualTranslationSizeX = 0.0f;	//옷을 이동시킬 거리
	m_virtualTranslationSizeY = 0.0f;
	m_virtualTranslationSizeZ = 1.3f;

	//-[ warp ]
	m_warpThreshold = 50.0f;

	//-[ option ]
	m_bDrawCollision = false;
}