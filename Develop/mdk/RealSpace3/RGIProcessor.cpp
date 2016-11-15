#include "StdAfx.h"
#include "RGIProcessor.h"
#include "REngine.h"


namespace rs3 {

RGIProcessor::RGIProcessor( int nQualityMode)
: m_QualityMode( nQualityMode)
, m_nWidth( 0)
, m_nHeight( 0)
, m_Radius(50.0f)
, m_AngleBias(30)
, m_Contrast(3.0f)
, m_Attenuation(0.98f)
, m_BrightBias(0.85f)
, m_pTexShadow(NULL)
, m_bNotShadowOnly(false)
, m_bSubtract(false)
, m_bAllLight(false)
{
	//LoadConfig( "Data/system/Default.gcf");
	LoadConfig( "Default.gcf");
	if( m_QualityMode < 1)
		m_QualityMode = 1;
}

RGIProcessor::~RGIProcessor(void)
{
}


void RGIProcessor::Init(  int nW, int nH)
{
	m_nWidth = nW;
	m_nHeight = nH;

	CreateBuffer();
}

bool RGIProcessor::CreateBuffer()
{
	RFORMAT bufferFormat = RFMT_RGB16;
	if ( false == REngine::GetDevice().QueryFeature(RQF_RGB16_RTF))
		bufferFormat = RFMT_A8R8G8B8;

	if( NULL == m_pTexShadow )
		m_pTexShadow = REngine::GetDevice().CreateRenderTargetTexture( m_nWidth, m_nHeight, bufferFormat);

	if( m_pTexShadow != NULL )
	{
		_ASSERT( m_pTexShadow->GetInfo().nTextureWidth == m_nWidth );
		_ASSERT( m_pTexShadow->GetInfo().nTextureHeight == m_nHeight );
		return true;
	}

	return false;
}

void RGIProcessor::DestroyBuffer()
{
	if( m_pTexShadow)
		REngine::GetDevice().DeleteTexture( m_pTexShadow);
	m_pTexShadow = NULL;
}

void RGIProcessor::Resize( int nW, int nH )
{
	m_nWidth = nW;
	m_nHeight = nH;

	DestroyBuffer();
	CreateBuffer();
}

void RGIProcessor::Invalidate()
{
	DestroyBuffer();
}

void RGIProcessor::Restore()
{
	CreateBuffer();
}

void RGIProcessor::Destroy()
{
	DestroyBuffer();
}

void RGIProcessor::LoadConfig( string strPath)
{
	m_strConfigPath = strPath;

	MXml kXml;
	if ( !kXml.LoadFile( m_strConfigPath.c_str() ) )
		return ;

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("GI_CONFIG");
	if(pRootElement == NULL)
		return ;

	LoadToXML(*pRootElement);
}

void RGIProcessor::SaveConfig( string strPath)
{
	m_strConfigPath = strPath;

	MXml xml;

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);
	MXmlElement *pRootElement = new MXmlElement("GI_CONFIG");
	xml.LinkEndChild(pRootElement);

	SaveToXML(*pRootElement);
	
	xml.SaveFile( strPath.c_str());

}

#define SAVE_ELEMENT( str, attr)	{ pElement = new MXmlElement( str); _SetContents(pElement, attr); element.LinkEndChild(pElement);}

void RGIProcessor::SaveToXML(MXmlElement &element)
{
	MXmlElement *pElement = NULL;

	SAVE_ELEMENT( "RADIUS", m_Radius);
	SAVE_ELEMENT( "ANGLEBIAS", m_AngleBias);
	SAVE_ELEMENT( "ATTENUATION", m_Attenuation);
	SAVE_ELEMENT( "CONTRAST", m_Contrast);
	SAVE_ELEMENT( "BRIGHTBIAS", m_BrightBias);
	SAVE_ELEMENT( "NOTSHADOWONLY", m_bNotShadowOnly);
	SAVE_ELEMENT( "SUBTRACT", m_bSubtract);
	SAVE_ELEMENT( "ALLLIGHT", m_bAllLight);
}

#define LOAD_ELEMENT( str, attr)	if ( pElement = element.FirstChildElement( str) ) { _Contents( &attr, pElement); }

void RGIProcessor::LoadToXML(MXmlElement &element)
{
	char szBuffer[256];
	szBuffer[0] = '\0';

	MXmlElement *pElement = NULL;

	LOAD_ELEMENT( "RADIUS", m_Radius);
	LOAD_ELEMENT( "ANGLEBIAS", m_AngleBias);
	LOAD_ELEMENT( "ATTENUATION", m_Attenuation);
	LOAD_ELEMENT( "CONTRAST", m_Contrast);
	LOAD_ELEMENT( "BRIGHTBIAS", m_BrightBias);
	LOAD_ELEMENT( "NOTSHADOWONLY", m_bNotShadowOnly);
	LOAD_ELEMENT( "SUBTRACT", m_bSubtract);
	LOAD_ELEMENT( "ALLLIGHT", m_bAllLight);
}

}