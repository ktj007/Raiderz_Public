#include "stdafx.h"
#include "XGameFrameUILoader.h"

#define GAME_FRAME_XML_TAG_UI			"FRAME_INCLUDE"
#define GAME_FRAME_XML_TAG_INCLUDE		"Include"
#define GAME_FRAME_XML_TAG_ID			"id"

void XGameFrameUILoader::AddFrameList( wstring strFrameName, wstring strFileName )
{
	m_FrameDescriptorList.insert(map<wstring, wstring>::value_type(strFrameName, strFileName));
}

bool XGameFrameUILoader::LoadFrameDescriptor()
{
	MXml xml;

	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(FILENAME_UI_MAINXML).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(GAME_FRAME_XML_TAG_UI).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), GAME_FRAME_XML_TAG_INCLUDE))
		{
			string strID;
			string strFileName;

			_Attribute(strID, pElement, GAME_FRAME_XML_TAG_ID, "");
			transform(strID.begin(), strID.end(), strID.begin(), tolower);	// 소문자로 변환

			_Contents(strFileName, pElement, "");
			AddFrameList(MLocale::ConvAnsiToUTF16(strID.c_str()), MLocale::ConvAnsiToUTF16(strFileName.c_str()));
		}
	}

	return true;
}

bool XGameFrameUILoader::Load( const wchar_t* szGameFrameName/*=NULL*/, bool bReload )
{
	if ( bReload == false  &&  global.ui->IsExistSheet( szGameFrameName) == true)
		return true;

	if (LoadFrameDescriptor() == false) return false;

	wstring strGameFrameName = szGameFrameName;
	transform(strGameFrameName.begin(), strGameFrameName.end(), strGameFrameName.begin(), tolower);	// 소문자로 변환

	map<wstring, wstring>::iterator itor = m_FrameDescriptorList.find(strGameFrameName);
	if(itor != m_FrameDescriptorList.end())
	{
		wstring strFilePath = PATH_UI + itor->second;

		if (bReload)
		{
			global.ui->Reload((wchar_t*)strFilePath.c_str());
		}
		else
		{
			global.ui->Load((wchar_t*)strFilePath.c_str());
		}


		MCursorSystem::Set( "arrow");

		return true;
	}

	return false;
}