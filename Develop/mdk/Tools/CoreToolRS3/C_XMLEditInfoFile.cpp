#include "StdAfx.h"
#include "C_XMLEditInfoFile.h"
#include "VR_TerrainGrass.h"
#include "RSceneNodeID.h"

using namespace std;
using namespace rs3;

CXMLEditInfoFile::CXMLEditInfoFile(void)
{
	m_pLoadedTree_ = NULL;
	m_bObjectAdded_ = false;
	m_bGrassChanged_ = false;
}

CXMLEditInfoFile::~CXMLEditInfoFile(void)
{
}

bool CXMLEditInfoFile::LoadNumbering(MXmlElement *pElement)
{
	for ( MXmlElement *pChildElement = pElement->FirstChildElement();
		pChildElement; pChildElement = pChildElement->NextSiblingElement())
	{
		std::string str = pChildElement->Value();
		int nNum;
		if ( _Contents(&nNum, pChildElement))
			m_kMapNumbering[str] = nNum;
	}

	return true;
}

bool CXMLEditInfoFile::SaveNumbering(MXmlElement *pElement)
{
	for ( NUMBERINGMAP::iterator it = m_kMapNumbering.begin(); it != m_kMapNumbering.end(); ++it )
	{
		MXmlElement *pChildElement = new MXmlElement(it->first.c_str());
		_SetContents(pChildElement,it->second);
        pElement->LinkEndChild(pChildElement);
	}

	return true;
}

bool CXMLEditInfoFile::LoadZoneInformationFile(const char *filename)
{
	//자료 초기화
	//오브젝트 자료
	m_bObjectAdded_ = false;
	m_kMapNumbering.clear();

	//Grass 자료
	m_strXmlGrass_.clear();
	m_grassNumbering_ = 0;
	m_pLoadedTree_ = NULL;
	m_bGrassChanged_ = false;

	if(filename == NULL) return false;

	//obi 파일, 꼭 있어야 한다.
	//파일이름으로부터 확장자 바꾸기
	CString obiName(filename);
	obiName.Replace(".zone.xml", ".obi");

	MXml obiXml;
	if ( !obiXml.LoadFile(obiName))
		return false;

	MXmlElement *pObiElement = obiXml.Doc()->FirstChildElement("OBJECTNUMBERING");
	if ( pObiElement == NULL ) return false;
	if (!LoadNumbering(pObiElement)) return false;

	//gri 파일, 없어도 상관 없으나 grass edit 정보가 날라간다.
	CString griName(filename);
	griName.Replace(".zone.xml", ".gri");

	MXml griXml;
	if ( !griXml.LoadFile(griName)) return true; //없어도 상관없으므로.

	MXmlElement* pGriElement = griXml.Doc()->FirstChildElement("GRASS");
	if ( pGriElement == NULL ) return false;

	//string 파일로 만들어 두고 로딩의 책임은 view에게 미룬다.
	MXmlPrinter griPrinter;
	pGriElement->Accept(&griPrinter);
	m_strXmlGrass_ = griPrinter.CStr();

	return true;
}

bool CXMLEditInfoFile::SaveZoneInformationFile(const char *filename, bool _bForceObjSave, bool _bForceGriSave)
{
	//obi 파일
	if(m_bObjectAdded_ || _bForceObjSave)
	{
		CString obiName(filename);
		if(obiName.Find(".zone.xml") != -1)
			obiName.Replace(".zone.xml", ".obi");
		else
			return false;

		MXml obiXml;
		MXmlDeclaration* pObiDecl = obiXml.CreateDeclaration("1.0", "UTF-8", "");
		obiXml.Doc()->LinkEndChild(pObiDecl);

		//Object Numbering
		MXmlElement *pNumberingElement = new MXmlElement("OBJECTNUMBERING");
		_SetContents(pNumberingElement,"");
		obiXml.Doc()->LinkEndChild(pNumberingElement);

		if ( !SaveNumbering(pNumberingElement)) return false;
		obiXml.SaveFile(obiName);
	}

	//grass edit information
	if( m_pLoadedTree_ != NULL && (m_bGrassChanged_ || _bForceGriSave ) )
	{
		//gri 파일
		CString griName(filename);
		griName.Replace(".zone.xml", ".gri");

		MXml griXml;
		MXmlDeclaration* pGriDecl = griXml.CreateDeclaration("1.0", "UTF-8", "");
		griXml.Doc()->LinkEndChild(pGriDecl);

		MXmlElement* pElementGrassEditInfo = new MXmlElement( "GRASS" );
		griXml.Doc()->LinkEndChild(pElementGrassEditInfo);
		_SetAttribute(pElementGrassEditInfo, "numbering", m_grassNumbering_);
		//HTREEITEM hParent = m_pLoadedTree_->GetNextItem(NULL, TVGN_FIRSTVISIBLE);
		HTREEITEM hParent = m_pLoadedTree_->GetRootItem();
		while (hParent != NULL)
		{
			MXmlElement* pElementGrassBrush = new MXmlElement( "BRUSH" );
			pElementGrassEditInfo->LinkEndChild(pElementGrassBrush);
			_SetAttribute(pElementGrassBrush, "name", m_pLoadedTree_->GetItemText(hParent));

			HTREEITEM hChild = m_pLoadedTree_->GetNextItem(hParent, TVGN_CHILD);
			while (hChild != NULL)
			{
				MXmlElement* pElementGrassPiece = new MXmlElement( "PIECE" );
				pElementGrassBrush->LinkEndChild(pElementGrassPiece);

				SPieceGrassInfo* pPieceGrass = (SPieceGrassInfo*)m_pLoadedTree_->GetItemData(hChild);
				_SetAttribute(pElementGrassPiece, "filename", pPieceGrass->m_strPieceGrassName);
				_SetAttribute(pElementGrassPiece, "size", pPieceGrass->m_nSize);
				_SetAttribute(pElementGrassPiece, "num",  pPieceGrass->m_nNumber);
				hChild = m_pLoadedTree_->GetNextSiblingItem(hChild);
			}
			hParent = m_pLoadedTree_->GetNextSiblingItem(hParent);
		}
		griXml.SaveFile(griName);
	}

	return true;
}

bool CXMLEditInfoFile::LoadFieldInformationFile(const char *filename)
{
	//자료 초기화
	m_entityObjectNum_.m_nSpawn = 0;
	m_entityObjectNum_.m_nMarker = 0;
	m_entityObjectNum_.m_nSensor = 0;
	m_entityObjectNum_.m_nSound = 0;
	m_entityObjectNum_.m_nArea = 0;

	m_entityObjectNum_.m_bAdded = false;

	if(filename == NULL) return false;

	std::string fullPath(filename);
	int find_token = fullPath.rfind("\\") + 1;

	std::string strPath(fullPath.substr(0, find_token));
	std::string strName(fullPath.substr(find_token, fullPath.length() - find_token - 4)); //확장자 제외

	MXml eoiXml;
	if ( !eoiXml.LoadFile( (strPath + "editor_info\\" + strName + ".eoi").c_str() ))
		return false;

	MXmlElement *pRootElement = eoiXml.Doc()->FirstChildElement("ENTITY_NUM");
	if ( pRootElement == NULL ) return false;

	if (MXmlElement *pSpawn = pRootElement->FirstChildElement("SPAWN"))
		_Attribute(m_entityObjectNum_.m_nSpawn, pSpawn, "num");
	if (MXmlElement *pMarker = pRootElement->FirstChildElement("MARKER"))
		_Attribute(m_entityObjectNum_.m_nMarker, pMarker, "num");
	if (MXmlElement *pSensor = pRootElement->FirstChildElement("SENSOR"))
		_Attribute(m_entityObjectNum_.m_nSensor, pSensor, "num");
	if (MXmlElement *pSound = pRootElement->FirstChildElement("SOUND"))
		_Attribute(m_entityObjectNum_.m_nSound, pSound, "num");
	if (MXmlElement *pArea = pRootElement->FirstChildElement("AREA"))
		_Attribute(m_entityObjectNum_.m_nArea, pArea, "num");
	if (MXmlElement *pRoom = pRootElement->FirstChildElement("ROOM"))
		_Attribute(m_entityObjectNum_.m_nRoom, pRoom, "num");

	return true;
}

bool CXMLEditInfoFile::SaveFieldInformationFile(const char *filename, bool _bForceSave)
{
	//eoi 파일
	if(m_entityObjectNum_.m_bAdded || _bForceSave)
	{
		std::string fullPath(filename);
		int find_token = fullPath.rfind("\\") + 1;

		std::string strPath(fullPath.substr(0, find_token));
		std::string strName(fullPath.substr(find_token, fullPath.length() - find_token - 4)); //확장자 제외

		MXml eoiXml;
		MXmlDeclaration* pEoiDecl = eoiXml.CreateDeclaration("1.0", "UTF-8", "");
		eoiXml.Doc()->LinkEndChild(pEoiDecl);

		//Entity Object Numbering
		MXmlElement* pRootElement = new MXmlElement("ENTITY_NUM");
		eoiXml.Doc()->LinkEndChild(pRootElement);

		MXmlElement* pSpawn = new MXmlElement("SPAWN");
		pRootElement->LinkEndChild(pSpawn);
		_SetAttribute(pSpawn, "num", m_entityObjectNum_.m_nSpawn);

		MXmlElement* pMarker = new MXmlElement("MARKER");
		pRootElement->LinkEndChild(pMarker);
		_SetAttribute(pMarker, "num", m_entityObjectNum_.m_nMarker);

		MXmlElement* pSensor = new MXmlElement("SENSOR");
		pRootElement->LinkEndChild(pSensor);
		_SetAttribute(pSensor, "num", m_entityObjectNum_.m_nSensor);

		MXmlElement* pSound = new MXmlElement("SOUND");
		pRootElement->LinkEndChild(pSound);
		_SetAttribute(pSound, "num", m_entityObjectNum_.m_nSound);

		MXmlElement* pArea = new MXmlElement("AREA");
		pRootElement->LinkEndChild(pArea);
		_SetAttribute(pArea, "num", m_entityObjectNum_.m_nArea);

		MXmlElement* pRoom = new MXmlElement("ROOM");
		pRootElement->LinkEndChild(pRoom);
		_SetAttribute(pRoom, "num", m_entityObjectNum_.m_nRoom);

		eoiXml.SaveFile( (strPath + "editor_info\\" + strName + ".eoi").c_str() );
	}
	return true;
}

int CXMLEditInfoFile::GetNumbering(const std::string* pStrName)
{
	NUMBERINGMAP::iterator it = m_kMapNumbering.find(*pStrName);
	if ( it == m_kMapNumbering.end())
	{
		m_kMapNumbering[*pStrName] = 2;
		return 1;
	}

	int nNum = it->second;
	++it->second;
	m_bObjectAdded_ = true;

	return nNum;
}

void CXMLEditInfoFile::LoadGrassInfo(CTreeCtrl* _pTreeCtrl)
{
	m_grassNumbering_ = 0;
	m_pLoadedTree_ = _pTreeCtrl;

	MXmlDocument doc;
	doc.Parse(m_strXmlGrass_.c_str());
	MXmlElement* pRootElement = doc.FirstChildElement();
	if(pRootElement == NULL)
		return;

	pRootElement->Attribute("numbering", &m_grassNumbering_);
	HTREEITEM hParent, hChild;
	for ( MXmlElement *pChildElement = pRootElement->FirstChildElement("BRUSH");
		pChildElement; pChildElement = pChildElement->NextSiblingElement())
	{
		const char* name = pChildElement->Attribute("name");
		hParent = m_pLoadedTree_->InsertItem(name, TVI_ROOT);
		for(MXmlElement* pPiece = pChildElement->FirstChildElement("PIECE");
			pPiece; pPiece = pPiece->NextSiblingElement())
		{
			int size, num;
			std::string filename(pPiece->Attribute("filename"));
			pPiece->Attribute("size", &size);
			pPiece->Attribute("num", &num);

			hChild = m_pLoadedTree_->InsertItem(filename.c_str(),hParent,TVI_LAST);
			SPieceGrassInfo* pGrassInfo = new SPieceGrassInfo;
			pGrassInfo->m_nNumber = num;
			pGrassInfo->m_strPieceGrassName = filename;
			pGrassInfo->m_nSize = size;
			m_pLoadedTree_->SetItemData(hChild, (DWORD_PTR)pGrassInfo);
		}
	}
}

void CXMLEditInfoFile::AddGrassBrush()
{
	if(!ChangeGrassInfo())
		return;

	++m_grassNumbering_;
	CString brushName;
	brushName.Format("Grass%02d", m_grassNumbering_);
	//HTREEITEM hItem = m_pLoadedTree_->InsertItem(brushName, TVI_ROOT);
	HTREEITEM hCurrentItem = m_pLoadedTree_->InsertItem(brushName, TVI_ROOT);
	m_pLoadedTree_->SelectItem(hCurrentItem);
}

bool CXMLEditInfoFile::ChangeGrassInfo()
{
	if(m_pLoadedTree_ == NULL )
		return false;

	m_bGrassChanged_ = true;
	return true;
}
