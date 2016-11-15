#include "stdafx.h"
#include ".\mcvconfig.h"
#include "REngine.h"
#include "MFileSystem.h"

const float INIT_FARZ = 2000.f;


McvConfig::McvConfig(void) : m_fFarZ(INIT_FARZ)
{	
}

McvConfig::~McvConfig(void)
{
}

// 패스를 지정 해 주는 부분이 이렇게 따로 있다 - COMMENT OZ
bool McvConfig::LoadConfig(const string& strFileName)
{
	MXml xml;
	if (xml.LoadFile(strFileName.c_str()) == false) return false;


	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pRootElement = docHandle.FirstChild("XML").Element();

	MXmlElement* pFarZ = pRootElement->FirstChildElement("FARZ");
	if(pFarZ)
		m_fFarZ = atoi(pFarZ->GetText());

	MXmlElement* pLightAni = pRootElement->FirstChildElement("LIGHTANI");
	if(pLightAni)
		m_strLightAni = pLightAni->GetText();

	m_strRegistryRoot = "McvRS3";
	MXmlElement* pName = pRootElement->FirstChildElement("NAME");
	if(pName)
	{
		m_strRegistryRoot += "_";
		m_strRegistryRoot += pName->GetText();
	}

	return true;
}

McvConfig& McvConfig::GetInstance()
{
	static McvConfig instance;
	return instance;
}
