#pragma once

#define TIXML_USE_STL		// STL 사용

#include <string>
#include "TinyXml/tinyxml.h"

namespace mmonitor {

// mmonitor 를 위한 간이 xml 래핑클래스
class MMXml
{
	TiXmlDocument		m_Document;

public:
	MMXml(void);
	~MMXml(void);

	bool LoadFile(const char* szFileName);
	TiXmlDocument* Doc()				{ return &m_Document; }

};

bool _Attribute(char* pOutValue, TiXmlElement* pElement, const char* name);
bool _Attribute(unsigned int* pOutValue, TiXmlElement* pElement, const char* name);
bool _Attribute(std::string& strOutValue, TiXmlElement* pElement, const char* name);
bool _Contents(std::string& strOutValue, TiXmlElement* pElement);

}