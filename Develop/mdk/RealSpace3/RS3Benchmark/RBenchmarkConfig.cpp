#include "stdafx.h"
#include "RBenchmarkConfig.h"
#include "MCommandLine.h"
#include "MXml.h"
#include "MStringUtil.h"
#include "RShadowStrategy.h"

using namespace cml2;

namespace rs3
{

const char* RBenchmarkConfig::KEYWORD_CONFIG_FILENAME = "CONFIG_FILENAME";

RBenchmarkConfig::RBenchmarkConfig() :
 m_bNavigationMode(false)
,m_FilePathString("./")
,m_ScreenWidth(1024)
,m_ScreenHeight(768)
,m_FullScreen(false)
,m_CameraPosition(RVector::ZERO)
,m_CameraDirection(RVector::AXISY)
,m_CameraFovAngle(90.f)
,m_CameraNear(10.f)
,m_CameraFar(50000.f)
,m_CameraFov(DegreeToRadian(90.f))
,m_CameraAspectRatio((float)1024/(float)768)
,m_ShadowEnable(false)
,m_ShadowTextureSize(512)
,m_ShadowSplitCount(4)
,m_HDREnable(false)
,m_DOFEnable(false)
,m_DOFFocusDist(100.f)
,m_DOFFocusRange(2500.0f)
,m_DOFCoCSize(5.0)
,m_DistortionEnable(false)
,m_SoftParticleEnable(false)
,m_SSAOEnable(false)
,m_SSAOQuality(1)
{
	m_ConfigFileName = "bench.config.xml";

	m_ShadowSplitDistance[0] = 500.f;
	m_ShadowSplitDistance[1] = 1500.f;
	m_ShadowSplitDistance[2] = 10000.f;
	m_ShadowSplitDistance[3] = 20000.f;
 }

RBenchmarkConfig::~RBenchmarkConfig()
{

}

void RBenchmarkConfig::SaveCameraTransform( RVector vPos, RVector vDir )
{
	m_CameraPosition = vPos;
	m_CameraDirection = vDir;
}

RSHADOWMETHOD RBenchmarkConfig::GetShadowType() const
{
	string shadowTypeString = cml2::ToLower( m_ShadowType );
	if( shadowTypeString == "pssm" )
		return SHADOW_PSSM;
	else
	if( shadowTypeString == "projection" )
		return SHADOW_PROJECTION;

	return SHADOW_NONE;
}

bool RBenchmarkConfig::LoadConfig()
{
	MCommandLine commandLine;
	commandLine.ParseFromCommandLine();

	vector<string> args = commandLine.GetLooseValues();
	if( false == args.empty() )
	{
		string configFileName = args[0];
		if( false == configFileName.empty() )
			m_ConfigFileName = configFileName;
	}
	return ReadConfigFile( m_ConfigFileName );
}

bool RBenchmarkConfig::ReloadConfig()
{
	return ReadConfigFile( m_ConfigFileName );
}

bool RBenchmarkConfig::SaveConfig()
{
	MXml xml;
	
	MXmlElement* pElement = new MXmlElement( "CONFIG" );
	xml.Doc()->LinkEndChild( pElement );

	{
		MXmlElement* pChildElement = new MXmlElement( "SCENE" );
		pElement->LinkEndChild( pChildElement );
		_SetAttribute( pChildElement, "filename", m_SceneFileName );
		_SetAttribute( pChildElement, "env_filename", m_SceneEnvFileName );
	}
	{
		MXmlElement* pChildElement = new MXmlElement( "RESOURCE" );
		pElement->LinkEndChild( pChildElement );
		_SetAttribute( pChildElement, "path", m_FilePathString );
	}
	{
		MXmlElement* pChildElement = new MXmlElement( "SCREEN" );
		pElement->LinkEndChild( pChildElement );
		_SetAttribute( pChildElement, "width", m_ScreenWidth );
		_SetAttribute( pChildElement, "height", m_ScreenHeight );
		_SetAttribute( pChildElement, "fullscreen", m_FullScreen );
	}
	{
		MXmlElement* pChildElement = new MXmlElement( "CAMERA" );
		pElement->LinkEndChild( pChildElement );
		_SetAttribute( pChildElement, "pos_x", m_CameraPosition.x );
		_SetAttribute( pChildElement, "pos_y", m_CameraPosition.y );
		_SetAttribute( pChildElement, "pos_z", m_CameraPosition.z );

		_SetAttribute( pChildElement, "dir_x", m_CameraDirection.x );
		_SetAttribute( pChildElement, "dir_y", m_CameraDirection.y );
		_SetAttribute( pChildElement, "dir_z", m_CameraDirection.z );

		_SetAttribute( pChildElement, "fov", m_CameraFovAngle );

		_SetAttribute( pChildElement, "near", m_CameraNear );
		_SetAttribute( pChildElement, "far", m_CameraFar );
	}
	{
		MXmlElement* pChildElement = new MXmlElement( "SHADOW" );
		pElement->LinkEndChild( pChildElement );
		_SetAttribute( pChildElement, "enable", m_ShadowEnable );
		_SetAttribute( pChildElement, "type", m_ShadowType );
		_SetAttribute( pChildElement, "size", m_ShadowTextureSize );
		_SetAttribute( pChildElement, "count", m_ShadowSplitCount );
		_SetAttribute( pChildElement, "dist0", m_ShadowSplitDistance[0] );
		_SetAttribute( pChildElement, "dist1", m_ShadowSplitDistance[1] );
		_SetAttribute( pChildElement, "dist2", m_ShadowSplitDistance[2] );
		_SetAttribute( pChildElement, "dist3", m_ShadowSplitDistance[3] );
	}
	{
		MXmlElement* pChildElement = new MXmlElement( "HDR" );
		pElement->LinkEndChild( pChildElement );
		_SetAttribute( pChildElement, "enable", m_HDREnable );
	}
	{
		MXmlElement* pChildElement = new MXmlElement( "DOF" );
		pElement->LinkEndChild( pChildElement );
		_SetAttribute( pChildElement, "enable", m_DOFEnable );
		_SetAttribute( pChildElement, "focus_dist", m_DOFFocusDist );
		_SetAttribute( pChildElement, "focus_range", m_DOFFocusRange );
		_SetAttribute( pChildElement, "coc_size", m_DOFCoCSize );
	}
	{
		MXmlElement* pChildElement = new MXmlElement( "Distortion" );
		pElement->LinkEndChild( pChildElement );
		_SetAttribute( pChildElement, "enable", m_DistortionEnable );
	}
	{
		MXmlElement* pChildElement = new MXmlElement( "SoftParticle" );
		pElement->LinkEndChild( pChildElement );
		_SetAttribute( pChildElement, "enable", m_SoftParticleEnable );
	}
	{
		MXmlElement* pChildElement = new MXmlElement( "SSAO" );
		pElement->LinkEndChild( pChildElement );
		_SetAttribute( pChildElement, "enable", m_SSAOEnable );
		_SetAttribute( pChildElement, "quality", m_SSAOQuality );
	}

	return xml.SaveFile( m_ConfigFileName.c_str() );
}

bool RBenchmarkConfig::ReadConfigFile( const string& fileName )
{
	MXml xml;
	bool bReadSuccess = xml.Doc()->LoadFile( fileName.c_str() );
	if( false == bReadSuccess )
		return false;

	MXmlElement* pElement = xml.Doc()->FirstChildElement( "CONFIG" );
	if( NULL == pElement )
		return false;

	if( MXmlElement* pChildElement = pElement->FirstChildElement( "SCENE" ) )
	{
		_Attribute( m_SceneFileName, pChildElement, "filename" );
		_Attribute( m_SceneEnvFileName, pChildElement, "env_filename" );
	}

	if( MXmlElement* pChildElement = pElement->FirstChildElement( "RESOURCE" ) )
	{
		_Attribute( m_FilePathString, pChildElement, "path" );
	}

	if( MXmlElement* pChildElement = pElement->FirstChildElement( "SCREEN" ) )
	{
		_Attribute( m_ScreenWidth, pChildElement, "width" );
		_Attribute( m_ScreenHeight, pChildElement, "height" );
		_Attribute( m_FullScreen, pChildElement, "fullscreen" );

		m_CameraAspectRatio = (float)m_ScreenWidth / (float)m_ScreenHeight;
	}

	if( MXmlElement* pChildElement = pElement->FirstChildElement( "CAMERA" ) )
	{
		_Attribute( m_CameraPosition.x, pChildElement, "pos_x" );
		_Attribute( m_CameraPosition.y, pChildElement, "pos_y" );
		_Attribute( m_CameraPosition.z, pChildElement, "pos_z" );

		_Attribute( m_CameraDirection.x, pChildElement, "dir_x" );
		_Attribute( m_CameraDirection.y, pChildElement, "dir_y" );
		_Attribute( m_CameraDirection.z, pChildElement, "dir_z" );

		_Attribute( m_CameraFovAngle, pChildElement, "fov" );

		_Attribute( m_CameraNear, pChildElement, "near" );
		_Attribute( m_CameraFar, pChildElement, "far" );

		m_CameraFov = GetCameraFov( m_ScreenWidth, m_ScreenHeight, m_CameraFovAngle );
	}

	if( MXmlElement* pChildElement = pElement->FirstChildElement( "SHADOW" ) )
	{
		_Attribute( m_ShadowEnable, pChildElement, "enable" );
		_Attribute( m_ShadowType, pChildElement, "type" );
		_Attribute( m_ShadowTextureSize, pChildElement, "size" );
		_Attribute( m_ShadowSplitCount, pChildElement, "count" );
		_Attribute( m_ShadowSplitDistance[0], pChildElement, "dist0" );
		_Attribute( m_ShadowSplitDistance[1], pChildElement, "dist1" );
		_Attribute( m_ShadowSplitDistance[2], pChildElement, "dist2" );
		_Attribute( m_ShadowSplitDistance[3], pChildElement, "dist3" );
	}

	if( MXmlElement* pChildElement = pElement->FirstChildElement( "HDR" ) )
	{
		_Attribute( m_HDREnable, pChildElement, "enable" );
	}

	if( MXmlElement* pChildElement = pElement->FirstChildElement( "DOF" ) )
	{
		_Attribute( m_DOFEnable, pChildElement, "enable" );
		_Attribute( m_DOFFocusDist, pChildElement, "focus_dist" );
		_Attribute( m_DOFFocusRange, pChildElement, "focus_range" );
		_Attribute( m_DOFCoCSize, pChildElement, "coc_size" );
	}

	if( MXmlElement* pChildElement = pElement->FirstChildElement( "Distortion" ) )
	{
		_Attribute( m_DistortionEnable, pChildElement, "enable" );
	}

	if( MXmlElement* pChildElement = pElement->FirstChildElement( "SoftParticle" ) )
	{
		_Attribute( m_SoftParticleEnable, pChildElement, "enable" );
	}

	if( MXmlElement* pChildElement = pElement->FirstChildElement( "SSAO" ) )
	{
		_Attribute( m_SSAOEnable, pChildElement, "enable" );
		_Attribute( m_SSAOQuality, pChildElement, "quality" );
	}

	return true;
}

float RBenchmarkConfig::GetCameraFov( int width, int height, float angle )
{
	float fFovX = DegreeToRadian( angle );
	float aspectRatio34 = 3.f / 4.f;
	float fDefaultFovY = atanf(tanf(fFovX/2.0f)*aspectRatio34)*2.0f; // 4:3 화면 기준 YFOV

	float fAspectRatio = float(width) / float(height);

	// 해당 FOVY 에 맞는 새 FOV 를 구한다
	float fNewFovX = atanf(tanf(fDefaultFovY/2.0f)*fAspectRatio)*2.0f;
	return fNewFovX;
}

}