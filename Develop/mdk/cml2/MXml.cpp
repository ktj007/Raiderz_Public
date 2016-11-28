#include "stdafx.h"
#include "MXml.h"
#include "MFileSystem.h"
#include "MLocale.h"
//ÀÏ´Ü
//#include "MZFileSystem.h"

MXml::MXml() : m_bUTF8(true), m_LangID( LANG_KOREAN)
{
}

MXml::~MXml()
{
	Clear();
}

MXmlDeclaration* MXml::CreateDeclaration(const std::string& strVersion, 
										const std::string& strEncoding,
										const std::string& strStandalone)
{

	MXmlDeclaration* pNew = new MXmlDeclaration(strVersion, strEncoding, strStandalone);
	return pNew;
}

std::string	MXml::ToAnsi(const char* pszString)
{
	if (pszString == NULL) return std::string("");

	if (m_bUTF8)
		return MLocale::ConvUTF8ToAnsi( pszString, m_LangID);

	return std::string(pszString);
}

std::wstring MXml::ToUTF16(const char* pszString)
{
	if (pszString == NULL) return std::wstring();

	if (m_bUTF8)
	{
		return MLocale::ConvUTF8ToUTF16(pszString);
	}

	return MLocale::ConvAnsiToUCS2(pszString);	
}

void MXml::Clear()
{
	m_Document.Clear();
	m_bUTF8 = true;
}


void MXml::AfterCreateFileRead(const char* szFileName)
{
	MFile mf;
	string szOnlyFileName;
	mf.FileFindAndChangeState( szFileName, szOnlyFileName  );
}


bool MXml::LoadFile(const char* szFileName)
{
	if (szFileName == NULL) return false;

//	if (m_Document.LoadFile(szFileName) == false) return false;

	//if (pZFS == NULL)
	//{
	//	if (m_Document.LoadFile(szFileName) == false) return false;
	//}
	//else
	{
		MFile mf;
		if( !mf.Open(szFileName) ) return false;

		if (mf.GetLength() > 0)
		{
			char* pBuffer = new char[mf.GetLength()+1];
			if( 0 == pBuffer ) return false;

			if( !mf.Read(pBuffer,mf.GetLength()) ) 
			{
				delete [] pBuffer;
				return false;
			}
			
			m_Document.SetValue(szFileName);

			pBuffer[mf.GetLength()]=0;

			m_Document.Parse(pBuffer);

			if ( m_Document.Error() )
			{
				int nRow = m_Document.ErrorRow();
				int nCol = m_Document.ErrorCol();
				int nErrorCide = m_Document.ErrorId();
				char * pDesc = (char*)m_Document.ErrorDesc();
				delete [] pBuffer;
				return false;
			}

			delete [] pBuffer;
		}

		m_strFileName = mf.GetFileName();
	}

	m_bUTF8 = CheckUTF8();

	return true;
}

bool MXml::LoadStream(const char* pStream)
{
	if (pStream == NULL) return false;

	m_Document.Parse(pStream);

	if ( m_Document.Error() )
	{
		return false;
	}

	m_bUTF8 = CheckUTF8();

	return true;
}

bool MXml::CheckUTF8()
{
	MXmlNode *pNode = m_Document.FirstChild();
	while (pNode != NULL)
	{
		MXmlDeclaration *pDec = pNode->ToDeclaration(); 

		if (pDec != NULL)
		{
			if ((strlen(pDec->Encoding()) == 0) || (!_stricmp(pDec->Encoding(), "UTF-8")))
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		pNode = pNode->NextSibling();
	}

	return true;
}

bool MXml::SaveFile(const char* szFileName)
{
	m_strFileName = szFileName;
	return m_Document.SaveFile(szFileName);
}

bool MXml::SaveFile()
{
	return m_Document.SaveFile(m_strFileName);
}

MXmlNode* MXml::LinkEndChild( MXmlNode* addThis )
{
	return m_Document.LinkEndChild(addThis);
}

std::string MXml::GetBuffer()
{
	TiXmlPrinter printer;
	Doc()->Accept(&printer);
	return printer.CStr();
}

////////////////////////////////////////////////

bool _Attribute(char* pOutValue, MXmlElement* pElement, const char* name, MXml* pXml)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		if (pXml)
			strcpy(pOutValue, pXml->ToAnsi(szAttr).c_str());
		else
			strcpy(pOutValue, szAttr);
	}
	else
	{
		return false;
	}

	return true;
}

bool _Attribute(char* pOutValue, MXmlElement* pElement, const char* name, const char* szDefaultValue, MXml* pXml)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		if (pXml)
			strcpy(pOutValue, pXml->ToAnsi(szAttr).c_str());
		else
			strcpy(pOutValue, szAttr);
	}
	else
	{
		if (pXml)
			strcpy(pOutValue, pXml->ToAnsi(szDefaultValue).c_str());
		else
			strcpy(pOutValue, szDefaultValue);
		return false;
	}

	return true;
}

bool _Attribute(char* pOutValue, size_t len, MXmlElement* pElement, const char* name, const char* szDefaultValue, MXml* pXml)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		if (pXml)
			strcpy_s(pOutValue, len, pXml->ToAnsi(szAttr).c_str());
		else
			strcpy_s(pOutValue, len, szAttr);
	}
	else
	{
		if (pXml)
			strcpy_s(pOutValue, len, pXml->ToAnsi(szDefaultValue).c_str());
		else
			strcpy_s(pOutValue, len, szDefaultValue);
		return false;
	}

	return true;
}

bool _Attribute(int* pOutValue, MXmlElement* pElement, const char* name)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr && pOutValue)
	{
		*pOutValue = atoi(szAttr);
	}
	else
	{
		return false;
	}

	return true;
}

bool _Attribute(int* pOutValue, MXmlElement* pElement, const char* name, int nDefaultValue)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		*pOutValue = atoi(szAttr);
	}
	else
	{
		*pOutValue = nDefaultValue;
		return false;
	}

	return true;

}

bool _Attribute(short* pOutValue, MXmlElement* pElement, const char* name)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr && pOutValue)
	{
		*pOutValue = atoi(szAttr);
	}
	else
	{
		return false;
	}

	return true;
}


bool _Attribute(short* pOutValue, MXmlElement* pElement, const char* name, short nDefaultValue)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		*pOutValue = atoi(szAttr);
	}
	else
	{
		*pOutValue = nDefaultValue;
		return false;
	}

	return true;

}

bool _Attribute(unsigned int* pOutValue, MXmlElement* pElement, const char* name)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr && pOutValue)
	{
		*pOutValue = atoi(szAttr);
	}
	else
	{
		return false;
	}

	return true;
}

bool _Attribute(unsigned char* pOutValue, MXmlElement* pElement, const char* name)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr && pOutValue)
	{
		*pOutValue = atoi(szAttr);
	}
	else
	{
		return false;
	}

	return true;
}

bool _Attribute(unsigned short* pOutValue, MXmlElement* pElement, const char* name)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr && pOutValue)
	{
		*pOutValue = atoi(szAttr);
	}
	else
	{
		return false;
	}

	return true;
}

bool _Attribute(unsigned int* pOutValue, MXmlElement* pElement, const char* name, unsigned int nDefaultValue)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		*pOutValue = atoi(szAttr);
	}
	else
	{
		*pOutValue = nDefaultValue;
		return false;
	}

	return true;
}

bool _Attribute(unsigned long* pOutValue, MXmlElement* pElement, const char* name)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr && pOutValue)
	{
		*pOutValue = atoi(szAttr);
	}
	else
	{
		return false;
	}

	return true;
}

bool _Attribute(unsigned long* pOutValue, MXmlElement* pElement, const char* name, unsigned int nDefaultValue)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		*pOutValue = atoi(szAttr);
	}
	else
	{
		*pOutValue = nDefaultValue;
		return false;
	}

	return true;
}

bool _Attribute(unsigned __int64* pOutValue, MXmlElement* pElement, const char* name, unsigned __int64 nDefaultValue)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		*pOutValue = atoi(szAttr);
	}
	else
	{
		*pOutValue = nDefaultValue;
		return false;
	}

	return true;
}

bool _Attribute(int& nOutValue, MXmlElement* pElement, const char* name)
{
	return (_Attribute(&nOutValue, pElement, name));
}

bool _Attribute(int& nOutValue, MXmlElement* pElement, const char* name, int nDefaultValue)
{
	return (_Attribute(&nOutValue, pElement, name, nDefaultValue));
}

bool _Attribute(unsigned int& nOutValue, MXmlElement* pElement, const char* name)
{
	return _Attribute(&nOutValue, pElement, name);
}

bool _Attribute(unsigned char& nOutValue, MXmlElement* pElement, const char* name )
{
	return _Attribute(&nOutValue, pElement, name);
}

bool _Attribute(unsigned short& nOutValue, MXmlElement* pElement, const char* name )
{
	return _Attribute(&nOutValue, pElement, name);
}

bool _Attribute(unsigned int& nOutValue, MXmlElement* pElement, const char* name, unsigned int nDefaultValue)
{
	return _Attribute(&nOutValue, pElement, name, nDefaultValue);
}

bool _Attribute(unsigned long& nOutValue, MXmlElement* pElement, const char* name)
{
	return _Attribute(&nOutValue, pElement, name);
}

bool _Attribute(unsigned long& nOutValue, MXmlElement* pElement, const char* name, unsigned int nDefaultValue)
{
	return _Attribute(&nOutValue, pElement, name, nDefaultValue);
}

bool _Attribute(unsigned __int64& nOutValue, MXmlElement* pElement, const char* name, unsigned __int64 nDefaultValue)
{
	return _Attribute(&nOutValue, pElement, name, nDefaultValue);
}

bool _Attribute(short& nOutValue, MXmlElement* pElement, const char* name)
{
	return _Attribute(&nOutValue, pElement, name);
}

bool _Attribute(short& nOutValue, MXmlElement* pElement, const char* name, short nDefaultValue)
{
	return _Attribute(&nOutValue, pElement, name, nDefaultValue);
}

bool _Attribute(float* pOutValue, MXmlElement* pElement, const char* name)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr && pOutValue)
	{
		*pOutValue = (float)atof(szAttr);
	}
	else
	{
		return false;
	}

	return true;

}

bool _Attribute(float* pOutValue, MXmlElement* pElement, const char* name, float fDefaultValue)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		*pOutValue = (float)atof(szAttr);
	}
	else
	{
		*pOutValue = fDefaultValue;
		return false;
	}

	return true;

}

bool _Attribute(float& pOutValue, MXmlElement* pElement, const char* name)
{
	return _Attribute(&pOutValue, pElement, name);
}

bool _Attribute(float& pOutValue, MXmlElement* pElement, const char* name, float fDefaultValue)
{
	return _Attribute(&pOutValue, pElement, name, fDefaultValue);
}

bool _Attribute(bool* pOutValue, MXmlElement* pElement, const char* name)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		if ((!_stricmp(szAttr, "true")) || (!_stricmp(szAttr, "1")))
		{
			*pOutValue = true;
		}
		else if (!_stricmp(szAttr, "false"))
		{
			*pOutValue = false;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;

}

bool _Attribute(bool* pOutValue, MXmlElement* pElement, const char* name, bool bDefaultValue)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		if ((!_stricmp(szAttr, "true")) || (!_stricmp(szAttr, "1")))
		{
			*pOutValue = true;
		}
		else if (!_stricmp(szAttr, "false"))
		{
			*pOutValue = false;
		}
		else
		{
			*pOutValue = bDefaultValue;
			return false;
		}
	}
	else
	{
		*pOutValue = bDefaultValue;
		return false;
	}

	return true;

}

bool _Attribute(bool& pOutValue, MXmlElement* pElement, const char* name)
{
	return _Attribute(&pOutValue, pElement, name);
}

bool _Attribute(bool& pOutValue, MXmlElement* pElement, const char* name, bool bDefaultValue)
{
	return _Attribute(&pOutValue, pElement, name, bDefaultValue);
}

bool _Attribute(std::string& strOutValue, MXmlElement* pElement, const char* name, MXml* pXml)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		if (pXml)
			strOutValue = pXml->ToAnsi(szAttr);
		else
			strOutValue = string(szAttr);
	}
	else
	{
		return false;
	}

	return true;
}

bool _Attribute(std::wstring& strOutValue, MXmlElement* pElement, const char* name, MXml* pXml)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{		
		if (pXml)
		{			
			strOutValue = pXml->ToUTF16(szAttr);
		}
		else
		{
			strOutValue = MLocale::ConvAnsiToUCS2(szAttr);
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool _Attribute(std::string& strOutValue, MXmlElement* pElement, const char* name, const char* szDefaultValue, MXml* pXml)
{
	const char* szAttr = pElement->Attribute(name);
	if (szAttr)
	{
		if (pXml)
			strOutValue = pXml->ToAnsi(szAttr);
		else
			strOutValue = string(szAttr);
	}
	else
	{
		if (pXml)
			strOutValue = pXml->ToAnsi(szDefaultValue);
		else
			strOutValue = string(szDefaultValue);
		return false;
	}

	return true;
}

bool _Attribute(std::wstring& strOutValue, MXmlElement* pElement, const char* name, const char* szDefaultValue, MXml* pXml)
{
	const char* szAttr = pElement->Attribute(name);

	if (szAttr)
	{
		if (pXml)
		{			
			strOutValue = pXml->ToUTF16(szAttr);
		}
		else
		{
			strOutValue = MLocale::ConvAnsiToUCS2(szAttr);
		}
	}
	else
	{
		if (pXml)
		{
			strOutValue = pXml->ToUTF16(szDefaultValue);
		}
		else
		{
			strOutValue = MLocale::ConvAnsiToUCS2(szDefaultValue);
		}

		return false;
	}

	return true;

}

bool _Contents(std::string& strOutValue, MXmlElement* pElement, MXml* pXml)
{
	if(!pElement)
		return false;
	const char* szText = pElement->GetText();
	if (szText)
	{
		if (pXml)
			strOutValue = pXml->ToAnsi(szText);
		else
			strOutValue = string(szText);
	}
	else
	{
		return false;
	}

	return true;
}

bool _Contents(std::string& strOutValue, MXmlElement* pElement, const char* szDefaultValue, MXml* pXml)
{
	if(!pElement)
		return false;
	const char* szText = pElement->GetText();
	if (szText)
	{
		if (pXml)
			strOutValue = pXml->ToAnsi(szText);
		else
			strOutValue = string(szText);
	}
	else
	{
		if (pXml)
			strOutValue = pXml->ToAnsi(szDefaultValue);
		else
			strOutValue = string(szDefaultValue);
		return false;
	}

	return true;
}


bool _Contents(std::wstring& strOutValue, MXmlElement* pElement, MXml* pXml)
{
	if(!pElement)
		return false;
	const char* szText = pElement->GetText();
	if (szText)
	{
		if (pXml)
			strOutValue = pXml->ToUTF16(szText);
		else
			strOutValue = MLocale::ConvAnsiToUCS2(szText);			
	}
	else
	{
		return false;
	}

	return true;
}

bool _Contents(std::wstring& strOutValue, MXmlElement* pElement, const char* szDefaultValue, MXml* pXml)
{
	if(!pElement)
		return false;
	const char* szText = pElement->GetText();
	if (szText)
	{
		if (pXml)
			strOutValue = pXml->ToUTF16(szText);
		else
			strOutValue = MLocale::ConvAnsiToUCS2(szText);			
	}
	else
	{
		if (pXml)
			strOutValue = pXml->ToUTF16(szDefaultValue);
		else
			strOutValue = MLocale::ConvAnsiToUCS2(szDefaultValue);

		return false;
	}

	return true;
}

bool _Contents(char* pOutValue, MXmlElement* pElement, MXml* pXml)
{
	if(!pElement)
		return false;
	const char* szText = pElement->GetText();
	if (szText)
	{
		if (pXml)
			strcpy(pOutValue, pXml->ToAnsi(szText).c_str());
		else
			strcpy(pOutValue, szText);
	}
	else
	{
		return false;
	}

	return true;
}

bool _Contents(char* pOutValue, MXmlElement* pElement, const char* szDefaultValue, MXml* pXml)
{
	if(!pElement)
		return false;
	const char* szText = pElement->GetText();
	if (szText)
	{
		if (pXml)
			strcpy(pOutValue, pXml->ToAnsi(szText).c_str());
		else
			strcpy(pOutValue, szText);
	}
	else
	{
		if (pXml)
			strcpy(pOutValue, pXml->ToAnsi(szDefaultValue).c_str());
		else
			strcpy(pOutValue, szDefaultValue);
		return false;
	}

	return true;
}

bool _Contents(int* pOutValue, MXmlElement* pElement)
{
	if(!pElement)
		return false;
	const char* szText = pElement->GetText();
	if (szText && pOutValue)
	{
		*pOutValue = atoi(szText);
	}
	else
	{
		return false;
	}

	return true;

}

bool _Contents(int* pOutValue, MXmlElement* pElement, int nDefaultValue)
{
	if(!pElement)
		return false;
	const char* szText = pElement->GetText();
	if (szText)
	{
		*pOutValue = atoi(szText);
	}
	else
	{
		*pOutValue = nDefaultValue;
		return false;
	}

	return true;

}

bool _Contents(float* pOutValue, MXmlElement* pElement)
{
	if(!pElement)
		return false;
	const char* szText = pElement->GetText();
	if (szText && pOutValue)
	{
		*pOutValue = (float)atof(szText);
	}
	else
	{
		return false;
	}

	return true;
}

bool _Contents(float* pOutValue, MXmlElement* pElement, float fDefaultValue)
{
	if(!pElement)
		return false;
	const char* szText = pElement->GetText();
	if (szText)
	{
		*pOutValue = (float)atof(szText);
	}
	else
	{
		*pOutValue = fDefaultValue;
		return false;
	}

	return true;

}

bool _Contents(bool* pOutValue, MXmlElement* pElement)
{
	if(!pElement)
		return false;
	const char* szText = pElement->GetText();
	if (szText)
	{
		if ((!_stricmp(szText, "TRUE")) || (!_stricmp(szText, "1")))
		{
			*pOutValue = true;
		}
		else if (!_stricmp(szText, "FALSE"))
		{
			*pOutValue = false;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool _Contents(bool* pOutValue, MXmlElement* pElement, bool bDefaultValue)
{
	if(!pElement)
		return false;
	const char* szText = pElement->GetText();
	if (szText)
	{
		if ((!_stricmp(szText, "TRUE")) || (!_stricmp(szText, "1")))
		{
			*pOutValue = true;
		}
		else if (!_stricmp(szText, "FALSE"))
		{
			*pOutValue = false;
		}
		else
		{
			*pOutValue = bDefaultValue;
			return false;
		}
	}
	else
	{
		*pOutValue = bDefaultValue;
		return false;
	}

	return true;
}

void _SetContents(MXmlElement* pElement, const std::string& szText)
{
	pElement->LinkEndChild(new MXmlText(szText));
}

void _SetContents(MXmlElement* pElement, const char* szText)
{
	pElement->LinkEndChild(new MXmlText(szText));
}

void _SetContents(MXmlElement* pElement, int nValue)
{
	char temp[256];
	sprintf(temp, "%d", nValue);
	pElement->LinkEndChild(new MXmlText(temp));
}

void _SetContents(MXmlElement* pElement, float fValue)
{
	char temp[256];
	sprintf(temp, "%.3f", fValue);
	pElement->LinkEndChild(new MXmlText(temp));
}

void _SetContents(MXmlElement* pElement, bool bValue)
{
	if (bValue) pElement->LinkEndChild(new MXmlText("true"));
	else pElement->LinkEndChild(new MXmlText("false"));
}

void _SetAttribute(MXmlElement* pElement, const char* szName, const char* szValue)
{
	pElement->SetAttribute(szName, szValue);
}
void _SetAttribute(MXmlElement* pElement, const char* szName, const std::string& szValue)
{
	pElement->SetAttribute(szName, szValue);
}

void _SetAttribute(MXmlElement* pElement, const char* szName, int nValue)
{
	pElement->SetAttribute(szName, nValue);
}

void _SetAttribute(MXmlElement* pElement, const char* szName, float fValue)
{
	char temp[256];
	sprintf(temp, "%f", fValue);
	pElement->SetAttribute(szName, temp);
}

void _SetAttribute(MXmlElement* pElement, const char* szName, bool bValue)
{
	if (bValue) pElement->SetAttribute(szName, "true");
	else pElement->SetAttribute(szName, "false");
}

/*
bool _LoadXml(MXml* pXml, MZFileSystem* pFileSystem, char* szFileName)
{
	char *buffer;
	MZFile mzf;

	if(pFileSystem) 
	{
		if(!mzf.Open(szFileName, pFileSystem)) 
		{
			if(!mzf.Open(szFileName)) 
				return false;
		}
	} 
	else 
	{
		if(!mzf.Open(szFileName))
			return false;
	}

	buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer,mzf.GetLength());

	if (!pXml->LoadStream(buffer))
	{
		delete[] buffer;
		return false;
	}
	delete[] buffer;
	mzf.Close();
	return true;
}

bool _LoadXml(MXml* pXml, char* szFileName)
{
	if (pXml->LoadFile(szFileName) == false) return false;
	return true;
}

MXmlElement* _GetFirstChildElementFromXml(MXml* pXml)
{
	MXmlHandle docHandle = pXml->DocHandle();
	return docHandle.FirstChild("XML").FirstChild().Element();	//FirstChild("XML").FirstChild(szTagName).Element();
}
*/