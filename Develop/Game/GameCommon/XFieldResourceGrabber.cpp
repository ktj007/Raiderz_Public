#include "stdafx.h"
#include "XFieldResourceGrabber.h"
#include "MXml.h"
#include "REngine.h"
#include "RDevice.h"
#include "RTextureManager.h"
#include "XResourceRefMgr.h"
#include "RResourceID.h"
#include "RSceneManager.h"
#include "RMesh.h"
#include "RTreeResource.h"
#include "RMeshMgr.h"
#include "RTreeResourceMgr.h"
#include "MLocale.h"



void XFieldResourceGrabber::Grab(tstring& strZoneResFile)
{
	vector<tstring> vecTextureResourceNames;
	vector<tstring> vecMeshResourceNames;
	vector<tstring> vecTreeResourceNames;

	MXml xml;

	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(strZoneResFile.c_str()).c_str())) 
	{
		return;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild("ROOT").FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), "RESOURCE"))
		{
			string strValue;
			_Attribute(strValue, pElement, "type");

			if (strValue == "texture")
			{
				ParseResource(pElement, vecTextureResourceNames);
			}
			else if (strValue == "tree")
			{
				ParseResource(pElement, vecTreeResourceNames);
			}
			else if (strValue == "mesh")
			{
				ParseResource(pElement, vecMeshResourceNames);
			}

		}
	}
	
	// 다음 존에서 사용하는 리소스는 레퍼런스를 하나 올린다.
	GrabResources(vecTextureResourceNames, RRESOURCETYPE::RR_TEXTURE);
	GrabResources(vecMeshResourceNames, RRESOURCETYPE::RR_MESH);
	GrabResources(vecTreeResourceNames, RRESOURCETYPE::RR_TREE);
}

void XFieldResourceGrabber::ParseResource( MXmlElement* pElement, vector<tstring> &outvecTextureResourceNames )
{
	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), "li"))
		{
			tstring strName;
			_Attribute(strName, pChild, "name");

			if (!strName.empty())
			{
				outvecTextureResourceNames.push_back(strName);
			}
		}
	}
}

void XFieldResourceGrabber::Drop()
{
	m_pResourceRefMgr->DropAll(&REngine::GetDevice());
}

XFieldResourceGrabber::XFieldResourceGrabber()
{
	m_pResourceRefMgr = new XResourceRefMgr();
}

XFieldResourceGrabber::~XFieldResourceGrabber()
{
	SAFE_DELETE(m_pResourceRefMgr);
}

void XFieldResourceGrabber::GrabResources( vector<tstring> &vecResourceNames, int nResID )
{
	for (size_t i = 0; i < vecResourceNames.size(); ++i)
	{
		RResource* pRes = NULL;
		switch (nResID)
		{
		case RRESOURCETYPE::RR_TEXTURE:
			{
				pRes = REngine::GetDevice().GetTexture(MLocale::ConvTCHARToAnsi(vecResourceNames[i].c_str()).c_str());
			}
			break;
		case RRESOURCETYPE::RR_MESH:
			{
				pRes = REngine::GetSceneManager().GetResource< RMesh >(MLocale::ConvTCHARToAnsi(vecResourceNames[i].c_str()).c_str());
			}
			break;
		case RRESOURCETYPE::RR_TREE:
			{
				pRes = REngine::GetSceneManager().GetResource< RTreeResource >(MLocale::ConvTCHARToAnsi(vecResourceNames[i].c_str()).c_str());
			}
			break;
		}
		if (pRes)
		{
			m_pResourceRefMgr->Grab(pRes);
		}
	}
}
