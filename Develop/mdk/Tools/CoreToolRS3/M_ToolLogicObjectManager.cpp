#include "StdAfx.h"

#include "M_ToolLogicObjectManager.h"

#include "C_XMLResourceList.h"
#include "C_XMLConfig.h"
#include "C_XMLUserConfig.h"
#include "C_XMLEditInfoFile.h"

#include "C_ControlMediatorHelperAction.h"

#include "WS_WorkSpaceManager.h"

using namespace rs3;

CToolLogicObjectManager::CToolLogicObjectManager(CControlMediator* _pControlMediator)
{
	m_pControlMediator_ = _pControlMediator;
}

void CToolLogicObjectManager::LoadTablesSafely()
{
	if (m_mapPropertyTable_.empty())
	{
		_ASSERT(m_mapNpcIdInfo_.empty());
		LoadPropertyAndNpcTable_();
		LoadSoundEffectTable_();
	}
}

CToolLogicObjectManager::~CToolLogicObjectManager(void)
{
	//map 순회하면서 SElementAtXML 지우기
	for(MAP_PROPERTY::iterator itr = m_mapPropertyTable_.begin();
		itr != m_mapPropertyTable_.end(); ++itr)
	{
		itr->second->DestroyElement();
	}

	m_mapPropertyTable_.clear();
}

void CToolLogicObjectManager::ResetLogicObjectManager()
{
	//Entity Object 지우기
	int nEntityRepoSize = (int)m_listEntityObject_.size();
	for(int i = 0; i < nEntityRepoSize; ++i)
	{
		for(ENTITY_OBJECTLIST::iterator itr = m_listEntityObject_[i].begin();
			itr != m_listEntityObject_[i].end(); ++itr)
		{
			CLogicObject* pEntityObject = (CLogicObject*)(*itr);
			pEntityObject->DestroyLogicObject();
			delete pEntityObject;
		}

		m_listEntityObject_[i].clear();
	}
	m_listEntityObject_.clear();

	// 보통과 삭제될때와 같은 경로(RemoveFromManager)로 제거되게 수정
	
	while( !m_listNoneEntityObject_.empty() )
	{
		CLogicObject* pLogicObject = m_listNoneEntityObject_.front();
		RemoveFromManager( pLogicObject );
 		pLogicObject->DestroyLogicObject();
 		delete pLogicObject;
	}
// 	for (LOGIC_OBJECTLIST::iterator itr = m_listNoneEntityObject_.begin(); itr != m_listNoneEntityObject_.end(); ++itr)
// 	{
// 		CLogicObject* pLogicObject = *itr;
// 		pLogicObject->DestroyLogicObject();
// 		delete pLogicObject;
// 	}
// 	m_listNoneEntityObject_.clear();
}

void CToolLogicObjectManager::AddToManager(CLogicObject* _pLogicObject)
{
	ASSERT(_pLogicObject);

	if (_pLogicObject->IsEntityLogic())
	{
		CEntityObject* pEntityObject = (CEntityObject*)_pLogicObject;
		CControlMediatorHelperEntityAction* pActionHelper = m_pControlMediator_->GetWorkSpaceMgr()->GetHelperEntityAction();
		_ASSERT(NULL != pActionHelper);

		int nCnt(0), nIdx(0);
		pActionHelper->OnAddToLogicObjectManager(pEntityObject, nCnt, nIdx);

		_ASSERT((int)m_listEntityObject_.size() == nCnt);
		_ASSERT(nCnt > nIdx);

		ENTITY_OBJECTLIST* plistEntityObject = &m_listEntityObject_[nIdx];
		if(plistEntityObject->empty())
		{//리스트에 원소가 존재하지 않을경우
			plistEntityObject->push_back(pEntityObject);
		}
		else
		{
			ENTITY_OBJECTLIST::iterator itr = plistEntityObject->begin();
			for(; itr != plistEntityObject->end(); ++itr)
			{//새로 추가하는 원소의 ID값을 비교하여 오름차순으로 정렬 
				CLogicObject* pLogicitr= (CLogicObject*)(*itr);
				if (_pLogicObject->GetObjectID() <= pLogicitr->GetObjectID())
				{
					if(_pLogicObject->GetObjectID() != -1 && _pLogicObject->GetObjectID() == pLogicitr->GetObjectID())
					{//중복된 ID가 존재함
						m_pControlMediator_->m_mouseInfo.mouseState.bLButton = false;
						RepeatIDMessage(pLogicitr->GetObjectID());
					}
					break;
				}
			}
		
			plistEntityObject->insert(itr, pEntityObject);
		}
	}
	else
	{
		m_listNoneEntityObject_.push_back(_pLogicObject);
	}
	m_pControlMediator_->BroadCastMessage( CoreMessage( VIEW_MESSAGE::LOGIC_NEW, _pLogicObject ) );
}

void CToolLogicObjectManager::RemoveFromManager(CLogicObject* _pLogicObject)
{
	ASSERT(_pLogicObject);

	if (_pLogicObject->IsEntityLogic())
	{
		CEntityObject* pEntityObject = (CEntityObject*)_pLogicObject;
		CControlMediatorHelperEntityAction* pActionHelper = m_pControlMediator_->GetWorkSpaceMgr()->GetHelperEntityAction();
		_ASSERT(pActionHelper);

		int nCnt(0), nIdx(0);
		pActionHelper->OnRemoveFromLogicObjectManager(pEntityObject, nCnt, nIdx);

		_ASSERT((int)m_listEntityObject_.size() == nCnt);
		_ASSERT(nCnt > nIdx);

		m_listEntityObject_[nIdx].remove(pEntityObject);
	}
	else
	{
		m_listNoneEntityObject_.remove(_pLogicObject);
	}

	// copy list 나 selected list 에서도 삭제
	m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject()->remove( _pLogicObject );
	m_pControlMediator_->GetHelperAction()->GetSelectedLogicObjectToCopy()->remove( _pLogicObject );

	m_pControlMediator_->BroadCastMessage( CoreMessage( VIEW_MESSAGE::LOGIC_DELETE, _pLogicObject ) );
}

void CToolLogicObjectManager::RepeatIDMessage(int nID)
{
	CString strOutput;
	strOutput.Format("중복된 ID가 있습니다.(ID: %d)", nID);
	::AfxMessageBox(strOutput);
}

void CToolLogicObjectManager::LoadSoundEffectTable_()
{
	std::string strSoundPath(CXMLConfig::GetInstance()->pcSoundPath);
	std::string strEffectXMLFilename = strSoundPath + "sound.xml";

	m_mapBGMList.clear();
	m_mapBGMList[""] = "";

	MXml kXml;
	if ( !kXml.LoadFile(strEffectXMLFilename.c_str()) )
		return;
	
	if ( MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("maiet"))
	{
		char pcBuffer[MAX_PATH];
		pcBuffer[0] = 0;

		for ( MXmlElement *pSoundElement = pRootElement->FirstChildElement("SOUND");
			pSoundElement != NULL; pSoundElement = pSoundElement->NextSiblingElement("SOUND"))
		{
			const char *pcName = pSoundElement->Attribute("name");
			const char *pcFilename = pSoundElement->Attribute("filename");
			
			_ASSERT(pcName);
			if ( pcFilename == NULL )
			{
				sprintf(pcBuffer,"%s.wav",pcName);
				pcFilename = pcBuffer;
			}

			bool bBG = false;
			_Attribute(&bBG, pSoundElement, "BG", false);

			if ( bBG == true /** stream loop 만 BGM의 대상으로 한다. */ )
				m_mapBGMList[pcName] = pcFilename;

			pcFilename = NULL;
		}
	}
}

void CToolLogicObjectManager::ReLoadPropertyAndNpcTable_()
{
	m_mapNpcIdInfo_.clear();
	LoadPropertyAndNpcTable_();
}

void CToolLogicObjectManager::LoadPropertyAndNpcTable_()
{
	const char* path = CXMLResourceList::GetReference().GetPorpertyListPath();
	MXml kXml;
	if ( !kXml.LoadFile(path)) return;

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("maiet");
	if(pRootElement == NULL) return;

	MXmlElement *pEntity = pRootElement->FirstChildElement("ENTITY");
	if ( pEntity == NULL ) return;

	MXmlElement* pChildElement = pEntity->FirstChildElement("PROPERTY");
	while(pChildElement != NULL)
	{
		SElementAtXML* pEntityElement = new SElementAtXML;
		UpdateElement_(pChildElement, pEntityElement);

		// get sub info attribute
		_Attribute(pEntityElement->m_strSubInfo, pChildElement, "sub_info");

		m_mapPropertyTable_[pEntityElement->m_strNodeName] = pEntityElement;
		pChildElement = pChildElement->NextSiblingElement();
	}

	//NPC table 채우기
	MXml npcXml;
	if ( !npcXml.LoadFile(CXMLResourceList::GetReference().GetNpcListPath()) ) return;

	MXmlElement *pNpcRootElement = npcXml.Doc()->FirstChildElement("maiet");
	if ( pNpcRootElement == NULL ) return;

	int id;
	std::string meshName;
	std::string meshPath;
	std::string strType;
	float scale;
	char colorBuff[512];
	DWORD color;

	for(MXmlElement *pNpcElement = pNpcRootElement->FirstChildElement("NPC");
		pNpcElement; pNpcElement = pNpcElement->NextSiblingElement("NPC"))
	{
		_Attribute(&id, pNpcElement, "id");
		_Attribute(meshName, pNpcElement, "MeshName");
		_Attribute(meshPath, pNpcElement, "MeshPath");
		_Attribute(strType, pNpcElement, "Type");

		int path_stroke = meshPath.find_first_of('/');
		if (path_stroke != std::string::npos)
			meshPath.replace(path_stroke, 1, "\\");

		std::string fullMeshName(
			CXMLResourceList::GetReference().GetNpcModelFolderPath() + meshPath + "\\" + meshName + ".elu");

		// scale
		scale = 1.f;
		_Attribute(&scale, pNpcElement, "Scale");

		// color
		color = 0xffffffff;
		if (_Attribute(colorBuff, pNpcElement, "TexColor", &npcXml))
		{
			int colorTmp = strtol(colorBuff, NULL, 16);		// XML에 16진수로 저장되어있다.
			if (colorTmp >= 0)
				color = 0xFF000000 | colorTmp;
		}

		// 삽입
		m_mapNpcIdInfo_.insert(
			std::map< int, SNpcInfo >::value_type( id, SNpcInfo(fullMeshName, strType, scale, color) )
		);
	}
}

void CToolLogicObjectManager::UpdateElement_(MXmlElement* _pXmlElement, SElementAtXML* _pLogicObjectElement)
{
	_Attribute(_pLogicObjectElement->m_strNodeName, _pXmlElement, "name");

	//Get Attribute Info from Attribute Element
	MXmlElement* pAttribute = _pXmlElement->FirstChildElement("ATTRIBUTE");
	while(pAttribute != NULL)
	{
		SAttributeAtXML* pAttributeInfo = new SAttributeAtXML;

		//Set Type and Type Extra
		_Attribute(pAttributeInfo->m_strAttrType, pAttribute, "type");
		_Attribute(pAttributeInfo->m_strAttrTypeExtra, pAttribute, "type_extra");

		//Set Attribute Name
		_Attribute(pAttributeInfo->m_strAttrName, pAttribute, "name");

		//Set Default Value
		_Attribute(pAttributeInfo->m_strGenericString, pAttribute, "default");

		//Push Attribute
		_pLogicObjectElement->m_vecAttribute.push_back(pAttributeInfo);

		//Next Attribute
		pAttribute = pAttribute->NextSiblingElement("ATTRIBUTE");
	}

	//Get Element
	MXmlElement* pElement = _pXmlElement->FirstChildElement("ELEMENT");
	while(pElement != NULL)
	{
		SElementAtXML* pEntityElement = new SElementAtXML;
		_pLogicObjectElement->m_vecChildElement.push_back(pEntityElement);
		UpdateElement_(pElement, pEntityElement);
		pElement = pElement->NextSiblingElement("ELEMENT");
	}
}

SElementAtXML* CToolLogicObjectManager::GetPropertyTable(const char* _pSzName)
{
	MAP_PROPERTY::iterator itr;
	itr = m_mapPropertyTable_.find(_pSzName);
	if(itr != m_mapPropertyTable_.end())
		return itr->second;
	else
		return NULL;
}

SNpcInfo* CToolLogicObjectManager::GetNpcInfoFromId(int _id)
{
	MAP_NPCINFO::iterator itr;
	itr = m_mapNpcIdInfo_.find(_id);
	if(itr != m_mapNpcIdInfo_.end())
		return &itr->second;
	else
		return NULL;
}

void CToolLogicObjectManager::PickLogicObjectFrustumForSelection(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum)
{
	// Entity
	int nEntityRepoSize = (int)m_listEntityObject_.size();
	for(int i = 0; i < nEntityRepoSize; ++i)
	{
		for(ENTITY_OBJECTLIST::iterator itr = m_listEntityObject_[i].begin(); itr != m_listEntityObject_[i].end(); ++itr)
		{
			CLogicObject* pEntityObject = (CLogicObject*)(*itr);
			//if( pEntityObject->GetVisible() &&
			//	pEntityObject->GetEditable() &&
			//	_refFrustum.TestBox(pEntityObject->GetAABB()) )
			//{
			//	_refVecLogicObject.push_back( pEntityObject );
			//}
			pEntityObject->PickForSelection(_refVecLogicObject, _refFrustum);
		}
	}

	// Logic
	for (LOGIC_OBJECTLIST::iterator itr = m_listNoneEntityObject_.begin(); itr != m_listNoneEntityObject_.end(); ++itr)
	{
		(*itr)->PickForSelection(_refVecLogicObject, _refFrustum);
	}
}

CLogicObject* CToolLogicObjectManager::PickLogicObjectForSelection(const rs3::RVector &origin, const rs3::RVector &to)
{
	float fLengthSq = FLT_MAX;
	CLogicObject *pPickLogicObject = NULL;

	// Entity
	int nEntityRepoSize = (int)m_listEntityObject_.size();
	for(int i = 0; i < nEntityRepoSize; ++i)
	{
		for(ENTITY_OBJECTLIST::iterator itr = m_listEntityObject_[i].begin();
			itr != m_listEntityObject_[i].end(); ++itr)
		{
			CLogicObject* pEntityObject = (CLogicObject*)(*itr);
			//RBoundingBox &refAABB = pEntityObject->GetAABB();
			//RVector vCenter = (refAABB.vmin + refAABB.vmax)*.5f;

			//if( pEntityObject->GetVisible() &&
			//	pEntityObject->GetEditable() &&
			//	pEntityObject->GetAABB().ContainsLineSegment(origin, to))
			if(pEntityObject->PickForSelection(fLengthSq, origin, to))
			{
				//float fLSq = (origin - vCenter).LengthSq();
				//if ( fLSq < fLengthSq )
				{
					pPickLogicObject = pEntityObject;
					//fLengthSq = fLSq;
				}
			}
		}	// next itr
	}	// next i

	// Logic
	for (LOGIC_OBJECTLIST::iterator itr = m_listNoneEntityObject_.begin(); itr != m_listNoneEntityObject_.end(); ++itr)
	{
		CLogicObject* pLogicObject = (*itr);
		RBoundingBox &refAABB = pLogicObject->GetAABB();
		RVector vCenter = (refAABB.vmin + refAABB.vmax)*.5f;

		if( pLogicObject->GetVisible() &&
			pLogicObject->GetEditable() )
		{
			CLogicObject* pLogicObjectCandidate = (*itr)->PickForSelection(fLengthSq, origin, to);
			if (pLogicObjectCandidate)
			{
				pPickLogicObject = pLogicObjectCandidate;
			}
		}
	}

	return pPickLogicObject;
}

const char* CToolLogicObjectManager::GetUniqueObjectNameOf( const char* szReferenceName )
{
	static char szNewName[256];

	string strPrefix = szReferenceName;

	// 오브젝트 이름에서 마지막 숫자를 벗겨낸 접두사를 찾아낸다
	string::size_type nLastPosNotNumber;
	nLastPosNotNumber = strPrefix.find_last_not_of( "0123456789" );
	if( nLastPosNotNumber != string::npos )
		strPrefix = strPrefix.substr( 0, nLastPosNotNumber+1 );
	else
		// 숫자로만 이루어진 이름인경우 뒤에 _ 를 붙여주자
		strPrefix += "_";

	int nNumber = 1;

	for (LOGIC_OBJECTLIST::iterator itr = m_listNoneEntityObject_.begin(); itr != m_listNoneEntityObject_.end(); ++itr)
	{
		CLogicObject* pLogicObject = *itr;

		const char* szName = pLogicObject->GetObjectName();
		// 접두어가 맞으면
		if( strncmp( szName, strPrefix.c_str(), strPrefix.length() ) == 0 )
		{
			int nThisNumber = atoi( szName + strPrefix.length() );
			if( nThisNumber >= nNumber )
				nNumber = nThisNumber +1;
		}
	}

	sprintf( szNewName, "%s%d", strPrefix.c_str(), nNumber );
	return szNewName;
}