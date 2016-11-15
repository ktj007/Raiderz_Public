#include "StdAfx.h"
#include "RConfiguration.h"
#include "MXml.h"

namespace rs3 {

RConfiguration::RConfiguration()
: m_bFullScreen(false)
, m_nInitX(0)
, m_nInitY(0)
, m_nWidth(640)
, m_nHeight(480)
, m_nTextureReduction(0)
, m_bUsingShader(true)
, pixelFormat(RFMT_XRGB32)
, depthFormat(RFMT_D24S8)
, m_fGlobalTextureMipmapLodBias(0.f)
, m_fSpeedTreeCompositeTextureMipmapLodBias(-0.5f)
, m_fSpeedGrassTextureMipmapLodBias(-0.5f)
, m_bLogResource(false)
, m_bUsingCompiledShader(false)
, m_bUsingCompiledShaderCaching(false)
, m_bBackgroundCreateShaderComposite(true)
, m_TextureFilter(RTF_TRILINEAR)
, m_dwTextureMaxAnisotropy(1)
{
	m_strDataPath = "Data/";
}

string RConfiguration::MakePath(const char *szName)
{
	string str = szName;
	if ( str.length() == 0 )
		return str;

	unsigned char lastChar = str[str.length()-1];
	if(lastChar!='\\' && lastChar!='/')
		str+= "/";

	return str;
}

void RConfiguration::SetDataPath(const char* szPath)
{
	m_strDataPath = MakePath(szPath);
}

string RConfiguration::GetDataPath( const char *DataFilename)
{
	return m_strDataPath + DataFilename;
}

void RConfiguration::SetVideo(MXmlElement *pElement)
{
	_ASSERT(pElement);

	// 외부에서 텍스쳐 밉맵 바이어스 설정을 하지 못하도록 주석처리함 - charmke
	//const char *pGlobalMipmapBias = pElement->Attribute("globalmipmapbias");
	//if ( pGlobalMipmapBias )
	//	m_fGlobalTextureMipmapLodBias = atof(pGlobalMipmapBias);

	const char *pTextureReduce = pElement->Attribute("texturereduction");
	if ( pTextureReduce )
		m_nTextureReduction = atoi(pTextureReduce);

	return;
}

bool RConfiguration::LoadFromXMLFile(const char *filename)
{
	MXml kXml;
	if ( !kXml.LoadFile(filename))
		return false;

	MXmlElement *pRoot = kXml.Doc()->RootElement();
	MXmlElement *pElement = NULL;

	if ( pElement = pRoot->FirstChildElement("Video"))
		SetVideo(pElement);

	return true;
}

}