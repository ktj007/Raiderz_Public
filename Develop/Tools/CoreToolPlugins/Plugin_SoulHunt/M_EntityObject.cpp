#include "StdAfx.h"
//#include "MainFrm.h"
#include "M_ToolSceneManager.h"
#include "M_ToolLogicObjectManager.h"
#include "M_ToolActor.h"

#include "C_XMLEditInfoFile.h"
#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperRender.h"

// for effect load
//#include "MainFrm.h"
#include "REffectInstance.h"
#include "REffectSceneNode.h"
#include "VR_EntityObjectList.h"
//#include "VR_CutSceneObjectList.h"

#include "WS_Zone.h"
#include "WS_Field.h"

#include "MDebug.h"

#include <algorithm>

#include "MFileSystem.h"

// 스케일
#define SPAWN_SCALE 2.2f
#define MARKER_SCALE 2.5f
#define ARROW_SCALE 3.0f

// 모델 디파인
#define SPAWN_MODEL_FILE_NAME "n_cone.scene.xml"
#define MARKER_MODEL_FILE_NAME "n_marker.scene.xml"
#define ARROW_MODEL_FILE_NAME "n_arrow.scene.xml"

using namespace rs3;

CControlMediator* CEntityObject::m_pControlMediator = NULL;

CEntityObject::CEntityObject(void)
{
	m_bEntityLogic = true;
	m_bVaildPosition = false;
	m_bZoneLoad = false;
}

CEntityObject::~CEntityObject(void)
{
}

CEntityObject* CEntityObject::CreateEntityObject(TYPE _type)
{
	CEntityObject* pNewEntityObject(NULL);
	if(_type == ENTITY_SPAWN)
	{
		pNewEntityObject = new CEntitySpawn;
		pNewEntityObject->m_eEntityObjectType_ = _type;
	}
	else if(_type == ENTITY_MARKER)
	{
		pNewEntityObject = new CEntityMarker;
		pNewEntityObject->m_eEntityObjectType_ = _type;
	}
	else if(_type == ENTITY_SENSOR)
	{
		pNewEntityObject = new CEntitySensor;
		pNewEntityObject->m_eEntityObjectType_ = _type;
	}
	else if(_type == ENTITY_SOUND)
	{
		pNewEntityObject = new CEntitySound;
		pNewEntityObject->m_eEntityObjectType_ = _type;
	}
	else if (_type == ENTITY_AREA)
	{
		pNewEntityObject = new CEntityArea;
		pNewEntityObject->m_eEntityObjectType_ = _type;
	}
	else if (_type == ENTITY_ROOM)
	{
		pNewEntityObject = new CEntityRoom;
		pNewEntityObject->m_eEntityObjectType_ = _type;
	}
	else
	{
		_ASSERT(!"기본 타입이 아닙니다.");
	}

	return pNewEntityObject;
}

CLogicObject* CEntityObject::Clone()
{
	return CloneAtWB( RVector(0,0,0) );
}

ICommandBuffer*	CEntityObject::CreateCloneNewCmdBuffer()
{
	return new CEntityNewCmdBuffer(this);
}

CEntityObject* CEntityObject::CloneAtWB(const rs3::RVector& _refPos)
{
	CEntityObject* pEntityObject = CreateEntityObject(m_eEntityObjectType_);
	if(m_pLogicObjectElement_ == NULL)
	{
		AfxMessageBox("치명적인 오류\n 팔레트가 잘못되어 있어서 새로운 Entity Object 를 심을수 없습니다.");
		pEntityObject->DestroyLogicObject();
		delete pEntityObject;
		return NULL;
	}
	else
	{
		SElementAtXML* pElement = new SElementAtXML;
		pElement->m_strNodeName = m_pLogicObjectElement_->m_strNodeName;
		pElement->m_strSubInfo = m_pLogicObjectElement_->m_strSubInfo;
		pElement->m_pParentElement = NULL;

		pEntityObject->m_pLogicObjectElement_ = pElement;

		///////////////////////////////////////
		//
		// 복사에 필요한 임시변수 지정
		// Copy Element에서 포인터의 방향을 바꿔준다.
		//
		//pos
		double dX = (double)(_refPos.x);
		double dY = (double)(_refPos.y);
		double dZ = (double)(_refPos.z);

		pEntityObject->m_pdX_ = &dX;
		pEntityObject->m_pdY_ = &dY;
		pEntityObject->m_pdZ_ = &dZ;

		//dir
		pEntityObject->m_pdDirX_ = m_pdDirX_;
		pEntityObject->m_pdDirY_ = m_pdDirY_;
		pEntityObject->m_pdDirZ_ = m_pdDirZ_;

		CopyElement(pEntityObject, m_pLogicObjectElement_, pEntityObject->m_pLogicObjectElement_, true);
		pEntityObject->OnLoadCompleted_();
	}

	CEntityObject::TYPE type = pEntityObject->m_eEntityObjectType_;
	int num = CXMLEditInfoFile::GetReference().GetNumAfterEntityAdded<CEntityObject>(type);
	if (num != -1 && pEntityObject->m_pdID_)
		*(pEntityObject->m_pdID_) = num;

	//set name
	char szBuffer[256];
	szBuffer[0] = '\0';
	switch(type)
	{
		#pragma warning(disable:4996)
		case ENTITY_SPAWN:
		{
			sprintf(szBuffer, "spawn_%05d", num);
		}
		break;
		case ENTITY_MARKER:
		{
			sprintf(szBuffer, "MK%05d", num);
		}
		break;
		case ENTITY_SENSOR:
		{
			sprintf(szBuffer, "SR%05d", num);
		}
		break;
		case ENTITY_SOUND:
		{
			sprintf(szBuffer, "SND%05d", num);
		}
		break;
		case ENTITY_AREA:
		{
			sprintf(szBuffer, "area_%05d", num);
		}
		break;
		case ENTITY_ROOM:
		{
			sprintf(szBuffer, "room_%05d", num);
		}
		break;
		#pragma warning (default : 4996 )
	}

	ASSERT(pEntityObject->m_pStrLogicObjectName_);
	pEntityObject->SetObjectName(szBuffer);
	pEntityObject->OnUpdateAABB_();

	if (!m_strSubInfo_.empty())
		pEntityObject->SetSubInfo( m_strSubInfo_.c_str() );

	return pEntityObject;
}

void CEntityObject::SaveToXML(MXmlElement& _refElement, bool bCommon)
{
	char *pcElementName = NULL;
	switch (m_eEntityObjectType_)
	{
		case ENTITY_SPAWN:
			pcElementName = "SPAWN";
		break;
		case ENTITY_MARKER:
			pcElementName = "MARKER";
		break;
		case ENTITY_SENSOR:
			pcElementName = "SENSOR";
		break;
		case ENTITY_SOUND:
			pcElementName = "SOUND";
		break;
		case ENTITY_AREA:
			pcElementName = "AREA";
		break;
		case ENTITY_ROOM:
			pcElementName = "ROOM";
		break;
	}

	ASSERT(m_pLogicObjectElement_);
	if (m_pLogicObjectElement_==NULL)
	{
		mlog("%s node Entity is NULL\n", pcElementName);
		return;
	}

	// common entity object 만 저장하려할때
	if (bCommon)
	{
		// object element 가 common이 아니라면 저장하지 않는다.
		if (m_pLogicObjectElement_->m_strSubInfo != "common")
			return;
	}
	else
	{
		if (m_pLogicObjectElement_->m_strSubInfo == "common")
			return;
	}

	CControlMediator::EDITMODE nEditMode = CControlMediator::EDITMODE_NONE;
	SElementAtXML* pRootProperty = GetRootElement();
	ASSERT(pRootProperty);
	for (SElementAtXML::AttributeList::iterator itr = pRootProperty->m_vecAttribute.begin();
		itr != pRootProperty->m_vecAttribute.end(); ++itr)
	{
		if ((*itr)->m_strAttrName == "edit_mode") 
		{
			if ((*itr)->m_strGenericString == "field")			nEditMode = CControlMediator::EDITMODE_FIELD;
			else if ((*itr)->m_strGenericString == "zone")		nEditMode = CControlMediator::EDITMODE_ZONE;
			else
				ASSERT(false && "unknown edit mode");
		}
	}
	if (nEditMode != CControlMediator::EDITMODE_NONE &&
		nEditMode != m_pControlMediator->GetEditMode())
		return; // 다른 모드

	if (pcElementName!=NULL)
	{
		MXmlElement* pElement = new MXmlElement(pcElementName);
		_refElement.LinkEndChild(pElement);
			SaveElement_(pElement, m_pLogicObjectElement_);
	}
}

CEntityObject::TYPE CEntityObject::IdentifyTypeFromChar(const char* _pSzType)
{
	if ( _stricmp(_pSzType,"SPAWN")==0 ) return ENTITY_SPAWN;
	else if ( _stricmp(_pSzType,"MARKER")==0 ) return ENTITY_MARKER;
	else if ( _stricmp(_pSzType,"SENSOR")==0 ) return ENTITY_SENSOR;
	else if ( _stricmp(_pSzType,"SOUND")==0) return ENTITY_SOUND;
	else if ( _stricmp(_pSzType,"AREA")==0) return ENTITY_AREA;
	else if ( _stricmp(_pSzType,"ROOM")==0) return ENTITY_ROOM;
	else return ENTITY_OBJ_NUM;
}

void CEntityObject::SettingEditMode()
{
	string strEditMode = "";
	SElementAtXML* pRootProperty = GetRootElement();
	ASSERT(pRootProperty);

	for (SElementAtXML::AttributeList::iterator itr = pRootProperty->m_vecAttribute.begin();
		itr != pRootProperty->m_vecAttribute.end(); ++itr)
	{
		if ((*itr)->m_strAttrName == "edit_mode") 
		{
			switch (m_pControlMediator->GetEditMode())
			{
			case CControlMediator::EDITMODE_FIELD:
				(*itr)->m_strGenericString = "field";
				break;
			case CControlMediator::EDITMODE_ZONE:
				(*itr)->m_strGenericString = "zone";
				break;
			default:
				{
					if ((NULL != m_pControlMediator->GetCurrentWorkSpaceAs<CWorkSpaceField>()) ||
						(NULL != m_pControlMediator->GetCurrentWorkSpaceAs<CWorkSpaceZone>()) )
					{
						ASSERT(false && "unknown edit mode");
					}
				}
			}
		}
	}
}

CLogicObject* CEntityObject::OnPickForSelection_( float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to )
{
	if( false == GetVisible() )
		return NULL;

	if( false == GetEditable() )
		return NULL;

	return CLogicObject_AABBPicking::OnPickForSelection_( _rPrevPickedSqLength, origin, to );
}

bool CEntityObject::IntersectTriangle( const vec3& orig, const vec3& dir, vec3& v0, vec3& v1, vec3& v2, float *t, float *u, float *v )
{
	// Find vectors for two edges sharing vert0
	vec3 edge1 = v1 - v0;
	vec3 edge2 = v2 - v0;

	// Begin calculating determinant - also used to calculate U parameter
	vec3 pvec;
	pvec = dir.CrossProduct(edge2);

	// If determinant is near zero, ray lies in plane of triangle
	float det = edge1.DotProduct(pvec);

	vec3 tvec;
	if( det > 0 )
	{
		tvec = orig - v0;
	}

	if( det < 0.0001f )
		return false;

	// Calculate U parameter and test bounds
	*u = tvec.DotProduct(pvec);
	if( *u < 0.0f || *u > det )
		return false;

	// Prepare to test V parameter
	vec3 qvec = tvec.CrossProduct(edge1);

	// Calculate V parameter and test bounds
	*v = dir.DotProduct(qvec);
	if( *v < 0.0f || *u + *v > det )
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	*t = edge2.DotProduct(qvec);
	float fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;
}

void CEntityObject::UpdatePickingForSelection( float& fSize, float& fOffSetHeight )
{
	// 선택시 카메라 거리에 따라 선택 폴리곤 사이즈 업데이트
	// 거리가 멀면 폴리곤이 커집니다.
	m_ObjectSelect.Update(this, m_pControlMediator);

	fSize = m_ObjectSelect.m_fSize;
	fOffSetHeight = m_ObjectSelect.m_fOffset;
}

void CEntityObject::OnUpdateAABB_()
{
	UpdateEntityAABB_();

	m_ObjectSelect.UpdateSmartOffsetHeightForIconSelection(this, m_pControlMediator);
}


//////////////////////////////////////////////////////////////////////////
RaiderzLogicObject_Renderer::RaiderzLogicObject_Renderer( CLogicObject_AABBPicking* pLogicObject, CControlMediator* pControlMediator_, string& strFileName )
: CLogicObjectRenderer_SceneNodeInstance( pLogicObject, NULL )
, m_pControlMediator( pControlMediator_ )
, m_strSceneFileName( strFileName )
{
	CreateView();
}

void RaiderzLogicObject_Renderer::CreateView()
{
	LoadSceneNodeInstance( m_pControlMediator, m_strSceneFileName.c_str() );
}

void RaiderzLogicObject_Renderer::SetColor( DWORD dwColor )
{
	RSceneNodeList nodeList = m_pViewNode->GetChildren();
	for(RSceneNodeList::iterator it = nodeList.begin(); it != nodeList.end(); ++it)
	{
		RActor* pActor = MDynamicCast( RActor, *it);
		if(pActor)
		{
			size_t nSize = pActor->GetActorNodes().size();
			for (size_t i = 0; i < nSize; i++)
			{
				pActor->GetActorNodes()[i]->SetPartsColor(dwColor);
			}
		}
	}
}

void RaiderzLogicObject_Renderer::RendererChanged( vec3 vScale, vec3 vPos, vec3 vDir, vec3 vUp )
{
	ReloadSceneNodeInstance( m_pControlMediator, m_strSceneFileName.c_str() );

	m_pViewNode->SetScale(vScale);
	m_pViewNode->SetPosition(vPos);
	m_pViewNode->SetDirection(vDir, vUp);

	rs3::RSceneNodeUpdateVisitor updater;
	m_pViewNode->Traverse( &updater );
}

//////////////////////////////////////////////////////////////////////////
RaiderzLogicObject_ArrowRenderer::RaiderzLogicObject_ArrowRenderer( CEntityObject* pLogicObject, CControlMediator* pControlMediator_ )
: RaiderzLogicObject_Renderer( pLogicObject, pControlMediator_, string(ARROW_MODEL_FILE_NAME))
, m_pDocument(pLogicObject)
{
}

void RaiderzLogicObject_ArrowRenderer::OnChanged( vec3 vScale, vec3 vPos, vec3 vDir, vec3 vUp, DWORD dwColor )
{
	// 모델이 반대이다.
	RendererChanged(vScale, vPos, -vDir, vUp);
	SetColor(dwColor);
}

//////////////////////////////////////////////////////////////////////////

CEntitySpawn::CEntitySpawn()
{
	m_pdBoolRandomDir_ = NULL;
	m_pStrColorName_ = NULL;
	m_pdRoamRadius_ = NULL;
	m_pdRoamDepth_ = NULL;

	m_pdCombatRadius_ = NULL;

	m_dwColor_ = 0xffff0000;
	m_bMapObject_ = false;
}

CEntitySpawn::~CEntitySpawn()
{
	//mlog("스폰 소멸자 불렸음\n");
	for(std::map<int, rs3::RSceneNode*>::iterator itr = m_mapNpcIdNode_.begin();
		itr != m_mapNpcIdNode_.end(); ++itr)
	{
		rs3::RSceneNode* pNode = itr->second;
		pNode->RemoveFromParent();
		delete pNode;
	}
}

void CEntitySpawn::UpdateSpecialBOOL_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase)
{
	//randon_dir(bool) 예외처리
	ASSERT(_pNewEntityAttribute);
	if(_bBaseCase && _pNewEntityAttribute->m_strAttrName == "random_dir")
		m_pdBoolRandomDir_ = &_pNewEntityAttribute->m_vGenericNum[0];
}

void CEntitySpawn::UpdateSpecialSTRING_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase)
{
	//color (string) 에 대한 예외처리
	ASSERT(_pNewEntityAttribute);
	if(_bBaseCase && _pNewEntityAttribute->m_strAttrName == "color" )
		m_pStrColorName_ = & _pNewEntityAttribute->m_strGenericString;
}

void CEntitySpawn::UpdateSpecialUINT_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase)
{
	//color (string) 에 대한 예외처리
	ASSERT(_pNewEntityAttribute);
	if(_bBaseCase)
	{
		if( _pNewEntityAttribute->m_strAttrName == "roam_radius" )
			m_pdRoamRadius_ = &_pNewEntityAttribute->m_vGenericNum[0];
		if( _pNewEntityAttribute->m_strAttrName == "roam_depth" )
			m_pdRoamDepth_ = &_pNewEntityAttribute->m_vGenericNum[0];
		if( _pNewEntityAttribute->m_strAttrName == "combat_radius" )
			m_pdCombatRadius_ = &_pNewEntityAttribute->m_vGenericNum[0];
	}
	else
	{
		SElementAtXML* pClientElement = _pNewEntityAttribute->m_pClientElement;
		//재귀의 BaseCase가 아닌 경우에는 clientElement를 가지고 있어야 한다.
		if( pClientElement == NULL ) return;
		if( _pNewEntityAttribute->m_strAttrName == "id" && pClientElement->m_strNodeName == "NPC")
		{
			rs3::RToolSceneManager* pSceneManager = m_pControlMediator->GetToolSceneManager();
			CToolLogicObjectManager* pLogicObjectManager = m_pControlMediator->GetToolLogicObjectMgr();
			int nNpcId = (int)_pNewEntityAttribute->m_vGenericNum[0];

			AddNpc( pSceneManager, pLogicObjectManager, nNpcId, false);
		}
	}
}

void CEntitySpawn::UpdateNpcDir_()
{
	rs3::RSceneNode* pSceneNode = GetNpcToRender();
	if ( pSceneNode != NULL )
	{
		rs3::RVector vDir, vPos;
		if ( GetDirection(vDir) )
		{
			pSceneNode->SetDirection( -vDir );
			pSceneNode->UpdateTransform();
		}
	}
}

void CEntitySpawn::OnLoadCompleted_()
{
	UpdateColor();

	//arrange npc pos
	rs3::RVector vPos;
	if( GetPosition(vPos) )
		SetNpcPos_( m_mapNpcIdNode_, vPos );

	UpdateNpcDir_();
	UpdateEntityAABB_();
}

void CEntitySpawn::UpdateColor()
{
	ASSERT( m_pStrColorName_ );
	if( m_pStrColorName_ != NULL)
	{
		int nFirstToken = m_pStrColorName_->find(",");
		//, 를 찾지 못한경우
		if( nFirstToken == std::string::npos )
		{
			//auto가 아닌 예전방식의 색깔은 auto로 변경해주기
			if( *m_pStrColorName_ != "auto" ) *m_pStrColorName_ = "auto";
			if( m_pLogicObjectElement_ != NULL)
			{
				for(SElementAtXML::ElementList::iterator itr = m_pLogicObjectElement_->m_vecChildElement.begin();
					itr != m_pLogicObjectElement_->m_vecChildElement.end(); ++itr)
				{
					SElementAtXML* pElement = *itr;
					if( pElement->m_strNodeName == "NPC" && pElement->m_pdElementID != NULL )
					{
						m_dwColor_ = GetAutoColorValue_(  (int)( *pElement->m_pdElementID ) );
						break;
					}
				}
			}
		}
		else
		{
			int nSecondToken = m_pStrColorName_->find(",", nFirstToken + 1);
			int nR = atoi( m_pStrColorName_->substr(0, nFirstToken).c_str() );
			int nG = atoi( m_pStrColorName_->substr(nFirstToken + 1, nSecondToken - nFirstToken - 1).c_str() );
			int nB = atoi( m_pStrColorName_->substr(nSecondToken +1, m_pStrColorName_->length() - nSecondToken).c_str() );
			m_dwColor_ = D3DCOLOR_XRGB( nR, nG, nB );
		}
	}
}

void CEntitySpawn::SetColor(COLORREF _color)
{
	if( m_pStrColorName_ != NULL)
	{
		int nR = GetRValue(_color);
		int nG = GetGValue(_color);
		int nB = GetBValue(_color);
		CString cstrRGB;
		cstrRGB.Format("%d,%d,%d", nR, nG, nB);

		*m_pStrColorName_ = cstrRGB.GetString();
		m_dwColor_ = D3DCOLOR_XRGB( nR, nG, nB );
	}
}

void CEntitySpawn::SetAutoColor()
{
	if( m_pStrColorName_ != NULL )
	{
		*m_pStrColorName_ = "auto";
		UpdateColor();
	}
}

DWORD CEntitySpawn::GetAutoColorValue_( int _nID )
{
	int modID = _nID % 32;
	DWORD dwColor = 0;

	switch ( modID )
	{
		case 0 :
			dwColor = D3DCOLOR_XRGB( 255, 0, 0 );
		break;
		case 1 :
			dwColor = D3DCOLOR_XRGB( 205, 102, 0 );
		break;
		case 2 :
			dwColor = D3DCOLOR_XRGB( 205, 129, 98 );
		break;
		case 3 :
			dwColor = D3DCOLOR_XRGB( 205, 175, 149 );
		break;
		case 4 :
			dwColor = D3DCOLOR_XRGB( 205, 91, 69 );
		break;
		case 5 :
			dwColor = D3DCOLOR_XRGB( 193, 205, 193 );
		break;
		case 6 :
			dwColor = D3DCOLOR_XRGB( 205, 133, 0 );
		break;
		case 7 :
			dwColor = D3DCOLOR_XRGB( 205, 112, 84 );
		break;
		case 8 :
			dwColor = D3DCOLOR_XRGB( 205, 16, 118 );
		break;
		case 9 :
			dwColor = D3DCOLOR_XRGB( 205, 96, 144 );
		break;
		case 10 :
			dwColor = D3DCOLOR_XRGB( 205, 85, 85 );
		break;
		case 11 :
			dwColor = D3DCOLOR_XRGB( 205, 140, 149 );
		break;
		case 12 :
			dwColor = D3DCOLOR_XRGB( 205, 55, 0 );
		break;
		case 13 :
			dwColor = D3DCOLOR_XRGB( 205, 104, 137 );
		break;
		case 14 :
			dwColor = D3DCOLOR_XRGB( 205, 50, 120 );
		break;
		case 15 :
			dwColor = D3DCOLOR_XRGB( 205, 145, 158 );
		break;
		case 16 :
			dwColor = D3DCOLOR_XRGB( 205, 0, 0 );
		break;
		case 17 :
			dwColor = D3DCOLOR_XRGB( 255, 140, 0 );
		break;
		case 18 :
			dwColor = D3DCOLOR_XRGB( 255, 160, 122 );
		break;
		case 19 :
			dwColor = D3DCOLOR_XRGB( 255, 218, 80 );
		break;
		case 20 :
			dwColor = D3DCOLOR_XRGB( 255, 127, 80 );
		break;
		case 21 :
			dwColor = D3DCOLOR_XRGB( 240, 255, 240 );
		break;
		case 22 :
			dwColor = D3DCOLOR_XRGB( 255, 165, 0 );
		break;
		case 23 :
			dwColor = D3DCOLOR_XRGB( 250, 128, 114 );
		break;
		case 24 :
			dwColor = D3DCOLOR_XRGB( 255, 20, 147 );
		break;
		case 25 :
			dwColor = D3DCOLOR_XRGB( 255, 105, 180 );
		break;
		case 26 :
			dwColor = D3DCOLOR_XRGB( 255, 106, 106 );
		break;
		case 27 :
			dwColor = D3DCOLOR_XRGB( 255, 174, 185 );
		break;
		case 28 :
			dwColor = D3DCOLOR_XRGB( 255, 69, 0 );
		break;
		case 29 :
			dwColor = D3DCOLOR_XRGB( 255, 130, 171 );
		break;
		case 30 :
			dwColor = D3DCOLOR_XRGB( 255, 62, 150 );
		break;
		case 31 :
			dwColor = D3DCOLOR_XRGB( 255, 181, 197 );
		break;
	}
	return dwColor;
}

void CEntitySpawn::SetNpcPos_(std::map<int, rs3::RSceneNode*>& _refNpcMap, const rs3::RVector& _refPos)
{
	int nNpcNum = (int)_refNpcMap.size();
	if (nNpcNum == 0)
		return;

	// 1개 이상
	std::map<int, rs3::RSceneNode*>::iterator itr = _refNpcMap.begin();
	itr->second->SetPosition(_refPos);

	RSceneNodeUpdateVisitor visitor;
	itr->second->Traverse(&visitor);

	if (nNpcNum > 1) //2개 이상
	{
		++itr;

		rs3::RMatrix matTransDist, matTransPos, matRotate, matRotateInvers;
		matTransDist.MakeIdentity();
		matTransDist.SetTranslation(rs3::RVector(200, 0, 0));

		matTransPos.MakeIdentity();
		matTransPos.SetTranslation(_refPos);
		float fPi = 2 * MMath::PI / nNpcNum;

		int nCount = 0;
		while (itr != _refNpcMap.end())
		{
			float fRot = fPi * nCount;
			matRotate.SetRotationZ(fRot);
			matRotateInvers.SetRotationZ(-fRot);
			itr->second->SetTransform( matRotateInvers * matTransDist * matRotate * matTransPos );
			itr->second->Traverse(&visitor);

			++itr;
			++nCount;
		}
	}
}

void CEntitySpawn::SetNpcDir_(std::map<int, rs3::RSceneNode*>& _refNpcMap, const rs3::RVector& _refDir)
{

}

void CEntitySpawn::AddNpc(rs3::RToolSceneManager* _pSceneManager, CToolLogicObjectManager* _pLogicObjectManager, int _id, bool _bArrange)
{
	SNpcInfo* pNpcInfo = _pLogicObjectManager->GetNpcInfoFromId( _id );

	if(_pSceneManager == NULL || pNpcInfo == NULL )
		return;

	std::map<int, rs3::RSceneNode*>::iterator itr = m_mapNpcIdNode_.find( _id );
	if( itr != m_mapNpcIdNode_.end() )
		return;

	rs3::RSceneNode *pNode = _pSceneManager->CreateSceneNode(rs3::RSID_ACTOR);
	//bool bRet = _pSceneManager->LoadSceneNode( pNode, pNpcInfo->m_strNpcMeshName.c_str() );
	bool bRet = _pSceneManager->LoadSceneNodeFromFile( pNode, MGetPureFileNameExt(pNpcInfo->m_strNpcMeshName).c_str() );
	if( !bRet )
	{
		delete pNode;
		return;
	}

	rs3::RToolActor* pToolActor = static_cast< rs3::RToolActor* >( pNode );
	pToolActor->Stop();

	pToolActor->SetScale( rs3::RVector(pNpcInfo->m_fScale, pNpcInfo->m_fScale, pNpcInfo->m_fScale) );
	pToolActor->SetTexColor(pNpcInfo->m_dwColor);
	m_mapNpcIdNode_[ _id ] = pNode;

	// 맵 오브젝트
	if (pNpcInfo->m_strNpcType == "object")
		m_bMapObject_ = true;

	if(_bArrange)
	{
		rs3::RVector vPos;
		if( GetPosition(vPos) )
			SetNpcPos_( m_mapNpcIdNode_, vPos );
	}

	UpdateNpcDir_();
	UpdateEntityAABB_();
}

void CEntitySpawn::RemoveNpc(int _id, bool _bArrange)
{
	std::map<int, rs3::RSceneNode*>::iterator itr = m_mapNpcIdNode_.find( _id );
	if( itr == m_mapNpcIdNode_.end() ) return;

	rs3::RSceneNode* pNode = itr->second;
	delete pNode;
	m_mapNpcIdNode_.erase( itr );

	//위치 정렬
	if(_bArrange)
	{
		rs3::RVector vPos;
		if( GetPosition(vPos) )
			SetNpcPos_( m_mapNpcIdNode_, vPos );
	}
}

rs3::RSceneNode* CEntitySpawn::GetNpcToRender()
{
	if (m_mapNpcIdNode_.empty())
		return NULL;
	return m_mapNpcIdNode_.begin()->second;
}

void CEntitySpawn::RenderNpcForSelection(CControlMediator* pControlMediator_)
{
	if (m_mapNpcIdNode_.empty())
		return;

	for (std::map<int, rs3::RSceneNode*>::iterator itr = m_mapNpcIdNode_.begin(); itr != m_mapNpcIdNode_.end(); ++itr)
	{
		pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->RegisterToRenderSceneNode(itr->second);
	}
}

void CEntitySpawn::OnSetPos_(const rs3::RVector &_refUpdatePos)
{
	CEntityObject::OnSetPos_(_refUpdatePos);

	rs3::RVector vPos;
	if( GetPosition(vPos) )
		SetNpcPos_( m_mapNpcIdNode_, vPos );
}

void CEntitySpawn::UpdateEntityAABB_()
{
	float fHeight = 0;
	//float fHeight = SPAWN_HEIGHT * 3;

	rs3::RSceneNode* pSceneNode = GetNpcToRender();
	if (pSceneNode != NULL )
	{
		pSceneNode->UpdateBoundingVolume();
		rs3::RBoundingBox box = pSceneNode->GetAABB();
		_ASSERT( box.IsValid() );
		fHeight = (box.maxz - box.minz) * pSceneNode->GetScale().x;
	}

	#pragma warning(disable:4244)
	m_aabb_.minx = *m_pdX_ - SPAWN_HEIGHT;
	m_aabb_.maxx = *m_pdX_ + SPAWN_HEIGHT;
	m_aabb_.miny = *m_pdY_ - SPAWN_HEIGHT;
	m_aabb_.maxy = *m_pdY_ + SPAWN_HEIGHT;
	m_aabb_.minz = *m_pdZ_;
	m_aabb_.maxz = *m_pdZ_ + fHeight + SPAWN_HEIGHT * 3;
	//m_aabb_.maxz = *m_pdZ_ + fHeight;
	_ASSERT( m_aabb_.IsValid() );
	#pragma warning(default:4244)

	m_fHeight = m_aabb_.maxz - m_aabb_.minz;
}

void CEntitySpawn::OnSetDirUp_(const rs3::RVector &_refUpdateDir, const rs3::RVector &_refUpdateUp)
{
	CEntityObject::OnSetDirUp_(_refUpdateDir, _refUpdateUp);
	UpdateNpcDir_();
}

void CEntitySpawn::LoadPropertyFromXML( SElementAtXML* _pEntityProperty, MXmlElement* _pElement /*= NULL*/ )
{
	CEntityObject::LoadPropertyFromXML(_pEntityProperty, _pElement);

	// BaseXML 구성 시에는 이하 실행하지 않는다.
	if (_pElement == NULL)	return;

	// 스폰 정보에 group 속성 초기화
	SElementAtXML* pRootProperty = GetRootElement();
	ASSERT(pRootProperty);
	for (SElementAtXML::AttributeList::iterator itr = pRootProperty->m_vecAttribute.begin();
	itr != pRootProperty->m_vecAttribute.end(); ++itr)
	{
		if ((*itr)->m_strAttrName == "group") 
			(*itr)->m_vGenericNum[0] = (double)0;
	}
}

CLogicObjectRenderer* CEntitySpawn::CreateRenderer( CControlMediator* pControlMediator_ )
{
	// 카메라 distance enable이 actor마다 설정이 가능할때 사용하도록합시다.
	// Draw Call 많이 불리는건 좋지 않잖아요

	//class RaiderzLogicObject_SpawnRenderer : public RaiderzLogicObject_Renderer
	//{
	//private:
	//	RaiderzLogicObject_ArrowRenderer*			m_pArrow;

	//public:
	//	RaiderzLogicObject_SpawnRenderer( CEntitySpawn* pLogicObject, CControlMediator* pControlMediator_)
	//		: RaiderzLogicObject_Renderer( pLogicObject, pControlMediator_, string(SPAWN_MODEL_FILE_NAME) )
	//		, pDocument(pLogicObject)
	//		, m_pArrow(NULL)
	//	{
	//		if(pDocument->IsRandomDir() == false)
	//		{
	//			// 방향값이 있다면... 인스턴스 생성
	//			m_pArrow = new RaiderzLogicObject_ArrowRenderer(pLogicObject, pControlMediator_);
	//		}
	//	}

	//	virtual ~RaiderzLogicObject_SpawnRenderer()
	//	{
	//		SAFE_DELETE(m_pArrow);
	//	}

	//	virtual void	OnChanged() override
	//	{
	//		rs3::RVector vecPos = vec3::ZERO;
	//		pDocument->GetPosition( vecPos );
	//		vecPos.z += pDocument->GetHeightWithNpc();

	//		rs3::RVector vecDir = vec3::AXISY;
	//		rs3::RVector vecUp = vec3::AXISZ;
	//		pDocument->GetDirection( vecDir ); 
	//		pDocument->GetUp( vecUp );

	//		//---------------------------------------------------------------------
	//		RendererChanged(vec3(SPAWN_SCALE, SPAWN_SCALE, SPAWN_SCALE), vecPos, vecDir, vecUp);
	//	
	//		m_pLogicObject->SetAABB( m_pViewNode->GetAABB() );

	//		DWORD dwColor = (pDocument->GetValid() ? pDocument->GetColor() : D3DCOLOR_XRGB(0, 0, 0));
	//		SetColor(dwColor);

	//		//---------------------------------------------------------------------
	//		// Arrow
	//		if(m_pArrow)
	//		{
	//			m_pArrow->OnChanged(vec3(ARROW_SCALE, ARROW_SCALE, ARROW_SCALE), vecPos, vecDir, vecUp, D3DCOLOR_XRGB(0, 0, 0));
	//		}
	//	}

	//	CEntitySpawn*			pDocument;
	//};	

	//return new RaiderzLogicObject_SpawnRenderer( this, pControlMediator_ );
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

CEntityMarker::CEntityMarker()
{
}

CEntityMarker::~CEntityMarker()
{
	//mlog("마커 소멸자 불렸음\n");
}

void CEntityMarker::UpdateEntityAABB_()
{
	m_aabb_.minx = (float)*m_pdX_ - MARKER_HEIGHT;
	m_aabb_.maxx = (float)*m_pdX_ + MARKER_HEIGHT;
	m_aabb_.miny = (float)*m_pdY_ - MARKER_HEIGHT;
	m_aabb_.maxy = (float)*m_pdY_ + MARKER_HEIGHT;
	m_aabb_.minz = (float)*m_pdZ_;
	m_aabb_.maxz = (float)*m_pdZ_ + 2 * MARKER_HEIGHT;
	_ASSERT( m_aabb_.IsValid() );
}

CLogicObjectRenderer* CEntityMarker::CreateRenderer( CControlMediator* pControlMediator_ )
{
	//class RaiderzLogicObject_MarketRenderer : public RaiderzLogicObject_Renderer
	//{
	//private:
	//	RaiderzLogicObject_ArrowRenderer*			m_pArrow;

	//public:
	//	RaiderzLogicObject_MarketRenderer( CEntityMarker* pLogicObject, CControlMediator* pControlMediator_)
	//		: RaiderzLogicObject_Renderer( pLogicObject, pControlMediator_, string(MARKER_MODEL_FILE_NAME) )
	//		, pDocument(pLogicObject)
	//		, m_pArrow(NULL)
	//	{
	//		m_pArrow = new RaiderzLogicObject_ArrowRenderer(pLogicObject, pControlMediator_);
	//	}

	//	virtual ~RaiderzLogicObject_MarketRenderer()
	//	{
	//		SAFE_DELETE(m_pArrow);
	//	}

	//	virtual void	OnChanged() override
	//	{
	//		rs3::RVector vecPos = vec3::ZERO;
	//		pDocument->GetPosition( vecPos );
	//		vecPos.z += MARKER_HEIGHT;

	//		rs3::RVector vecDir = vec3::AXISY;
	//		rs3::RVector vecUp = vec3::AXISZ;
	//		pDocument->GetDirection( vecDir ); 
	//		pDocument->GetUp( vecUp );

	//		//---------------------------------------------------------------------
	//		RendererChanged(vec3(MARKER_SCALE, MARKER_SCALE, MARKER_SCALE), vecPos, vecDir, vecUp);

	//		m_pLogicObject->SetAABB( m_pViewNode->GetAABB() );

	//		DWORD dwColor = (pDocument->GetValid() ? 0xff00ff00 : D3DCOLOR_XRGB(0, 0, 0));
	//		SetColor(dwColor);

	//		//---------------------------------------------------------------------
	//		// Arrow
	//		if(m_pArrow)
	//		{
	//			vecPos.z += MARKER_HEIGHT;
	//			m_pArrow->OnChanged(vec3(ARROW_SCALE, ARROW_SCALE, ARROW_SCALE), vecPos, vecDir, vecUp, D3DCOLOR_XRGB(0, 0, 0));
	//		}
	//	}

	//	CEntityMarker*			pDocument;
	//};	

	//return new RaiderzLogicObject_MarketRenderer( this, pControlMediator_ );
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

CEntitySensor::CEntitySensor()
{
	m_bSensorEffectEditMode_ = false;
	m_pSensorEffect_ = NULL;

	m_pSensorEffectPosAttribute_ = NULL;
	m_pSensorEffectDirAttribute_ = NULL;
	m_pSensorEffectUpAttribute_ = NULL;

	ZeroMemory(m_dSensorEffectPos, sizeof(m_dSensorEffectPos));
	ZeroMemory(m_dSensorEffectDir, sizeof(m_dSensorEffectDir));
	ZeroMemory(m_dSensorEffectUp, sizeof(m_dSensorEffectUp));

	m_vSensorEffectScale_.x = 1.f;
	m_vSensorEffectScale_.y = 1.f;
	m_vSensorEffectScale_.z = 1.f;

	m_ObjectSelect.DoUse();
}

CEntitySensor::~CEntitySensor()
{
	//mlog("센서 소멸자 불렸음\n");

	DestroySensorEffect_();
}

void CEntitySensor::LoadPropertyFromXML(SElementAtXML* _pEntityProperty, MXmlElement* _pElement)
{
	CEntityObject::LoadPropertyFromXML(_pEntityProperty, _pElement);
	
	
	// BaseXML 구성 시에는 이하 실행하지 않는다.
	if (_pElement == NULL)	return;


	// Sensor의 Warp 태그의 경우, SharedField용과 TrialField용에 따라 사용하는 속성이 달라
	// 사용하지 않는 속성을 제거한다.
	SElementAtXML::ElementList& vecChildElement = m_pLogicObjectElement_->m_vecChildElement;
	for(SElementAtXML::ElementList::iterator it = vecChildElement.begin(); it != vecChildElement.end(); it++)
	{
		SElementAtXML* pElement = *it;
		if (pElement->m_strNodeName == "WARP")
		{
			RemoveUnusedWarpAttribute(pElement);
		}
	}

	// SENSOR_WARP가 아닌 SENSOR로 취급하기를 원함.
	m_pLogicObjectElement_->m_strNodeName = "SENSOR";
}

void CEntitySensor::RemoveUnusedWarpAttribute(SElementAtXML* pWarpElement)
{
	SElementAtXML::AttributeList& vecAttribute = pWarpElement->m_vecAttribute;

	// 공유필드/트라이얼필드 구분
	bool isSharedField = false;	
	for(SElementAtXML::AttributeList::iterator it = vecAttribute.begin(); it != vecAttribute.end(); it++)
	{
		SAttributeAtXML* pAttribute = *it;
		
		if (pAttribute->m_strAttrName == "field")
		{
			// -1의 경우를 피하기위해 int로 케스팅한다.
			if (pAttribute->m_eAttrType == SAttributeAtXML::E_UINT && 0 < (int)pAttribute->m_vGenericNum[0])
			{
				isSharedField = true;
				break;
			}
		}
	}	


	// 필요없는 속성제거
	SElementAtXML::AttributeList::iterator it = vecAttribute.begin();
	while(it != vecAttribute.end())
	{
		SAttributeAtXML* pAttribute = *it;

		bool isUnusedAttribute = false;
		if (isSharedField)
		{
			if (pAttribute->m_strAttrName == "field_group" || pAttribute->m_strAttrName == "quest")
			{
				isUnusedAttribute = true;
			}
		}
		else
		{
			if (pAttribute->m_strAttrName == "field" || pAttribute->m_strAttrName == "marker")
			{
				isUnusedAttribute = true;
			}
		}

		if (isUnusedAttribute)
		{
			delete pAttribute;
			it = vecAttribute.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void CEntitySensor::UpdateSpecialSTRING_( SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase )
{
	if (_bBaseCase)
		return;

	if (_pNewEntityAttribute->m_pClientElement->m_strNodeName == "EFFECT" && _pNewEntityAttribute->m_strAttrName == "effect")
		CreateSensorEffect_(_pNewEntityAttribute->m_strGenericString);
}

void CEntitySensor::UpdateSpecialFLOAT_( SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase )
{
	if (_bBaseCase)
		return;

	if (_pNewEntityAttribute->m_strAttrName == "scale_x")
	{
		m_vSensorEffectScale_.x = (float)_pNewEntityAttribute->m_vGenericNum[0];
	}
	else if (_pNewEntityAttribute->m_strAttrName == "scale_y")
	{
		m_vSensorEffectScale_.y = (float)_pNewEntityAttribute->m_vGenericNum[0];
	}
	else if (_pNewEntityAttribute->m_strAttrName == "scale_z")
	{
		m_vSensorEffectScale_.z = (float)_pNewEntityAttribute->m_vGenericNum[0];
	}
}

void CEntitySensor::UpdateSpecialPos_( SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase )
{
	if (_bBaseCase)
		return;

	if (m_pSensorEffect_)
	{
		for (int i = 0; i < 3; ++i)
			m_dSensorEffectPos[i] = &_pNewEntityAttribute->m_vGenericNum[i];

		m_pSensorEffectPosAttribute_ = _pNewEntityAttribute;
	}
	else
	{
		_pNewEntityAttribute->m_vGenericNum[0] = 0;
		_pNewEntityAttribute->m_vGenericNum[1] = 0;
		_pNewEntityAttribute->m_vGenericNum[2] = 0;
	}
}

void CEntitySensor::UpdateSpecialDir_( SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase )
{
	if (_bBaseCase)
		return;

	if (!m_pSensorEffect_)
		return;

	for (int i = 0; i < 3; ++i)
		m_dSensorEffectDir[i] = &_pNewEntityAttribute->m_vGenericNum[i];

	m_pSensorEffectDirAttribute_ = _pNewEntityAttribute;
}

void CEntitySensor::UpdateSpecialUp_( SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase )
{
	if (_bBaseCase)
		return;

	if (!m_pSensorEffect_)
		return;

	for (int i = 0; i < 3; ++i)
		m_dSensorEffectUp[i] = &_pNewEntityAttribute->m_vGenericNum[i];

	m_pSensorEffectUpAttribute_ = _pNewEntityAttribute;
}

void CEntitySensor::UpdateEntityAABB_()
{
	if (m_bSensorEffectEditMode_)
	{
		_ASSERT(m_pSensorEffect_);
		m_aabb_.vmin = m_pSensorEffect_->GetWorldPosition() + RVector(-50.f,-50.f,-50.f);
		m_aabb_.vmax = m_pSensorEffect_->GetWorldPosition() + RVector( 50.f, 50.f, 50.f);
	}
	else
	{
		m_aabb_.minx = (float)*m_pdX_ - (float)*m_pdRadius_;
		m_aabb_.maxx = (float)*m_pdX_ + (float)*m_pdRadius_;
		m_aabb_.miny = (float)*m_pdY_ - (float)*m_pdRadius_;
		m_aabb_.maxy = (float)*m_pdY_ + (float)*m_pdRadius_;
		m_aabb_.minz = (float)*m_pdZ_;
		m_aabb_.maxz = (float)*m_pdZ_ + (float)*m_pdRadius_;
	}
	_ASSERT( m_aabb_.IsValid() );
}

void CEntitySensor::OnSelected_( bool bSelected )
{
	if (!bSelected)
	{
		SetEnableSensorEffect_(false);
	}
}

bool CEntitySensor::OnGetPos_( rs3::RVector& _refVecPos ) const
{
	if (m_bSensorEffectEditMode_)
	{
		_ASSERT(m_pSensorEffect_);
		_refVecPos = m_pSensorEffect_->GetWorldPosition();
		return true;
	}
	else
	{
		return CLogicObject::OnGetPos_(_refVecPos);
	}
}

void CEntitySensor::OnSetPos_( const rs3::RVector &_refUpdatePos )
{
	if (m_bSensorEffectEditMode_)
	{
		_ASSERT(m_dSensorEffectPos[0]);
		_ASSERT(m_pdX_);
		*m_dSensorEffectPos[0] = (double)_refUpdatePos.x - *m_pdX_;
		*m_dSensorEffectPos[1] = (double)_refUpdatePos.y - *m_pdY_;
		*m_dSensorEffectPos[2] = (double)_refUpdatePos.z - *m_pdZ_;

		OnUpdateAABB_();
	}
	else
	{
		CLogicObject::OnSetPos_(_refUpdatePos);
	}
	UpdateSensorEffectTM_();
}

SAttributeAtXML* CEntitySensor::OnGetPosAttribute_()
{
	if (m_bSensorEffectEditMode_)
	{
		return m_pSensorEffectPosAttribute_;
	}
	else
	{
		return m_pPosAttribute_;
	}
}

bool CEntitySensor::OnGetDir_( rs3::RVector& _refVecDir ) const
{
	if (m_bSensorEffectEditMode_)
	{
		_ASSERT(m_pSensorEffect_);
		_refVecDir = m_pSensorEffect_->GetWorldDirection();
		return true;
	}
	else
	{
		return CLogicObject::OnGetDir_(_refVecDir);
	}
}

void CEntitySensor::OnSetDirUp_( const rs3::RVector &_refUpdateDir, const rs3::RVector &_refUpdateUp )
{
	if (m_bSensorEffectEditMode_)
	{
		_ASSERT(m_dSensorEffectDir[0]);
		*m_dSensorEffectDir[0] = (double)_refUpdateDir.x;
		*m_dSensorEffectDir[1] = (double)_refUpdateDir.y;
		*m_dSensorEffectDir[2] = (double)_refUpdateDir.z;

		_ASSERT(m_dSensorEffectUp[0]);
		*m_dSensorEffectUp[0] = (double)_refUpdateUp.x;
		*m_dSensorEffectUp[1] = (double)_refUpdateUp.y;
		*m_dSensorEffectUp[2] = (double)_refUpdateUp.z;

		OnUpdateAABB_();
	}
	else
	{
//		CLogicObject::OnSetPos_(_refUpdateDir);	// 으음 ?
		CLogicObject::OnSetDirUp_(_refUpdateDir, _refUpdateUp);
	}
	UpdateSensorEffectTM_();
}

SAttributeAtXML* CEntitySensor::OnGetDirAttribute_()
{
	if (m_bSensorEffectEditMode_)
	{
		return m_pSensorEffectDirAttribute_;
	}
	else
	{
		return m_pDirAttribute_;
	}
}

bool CEntitySensor::OnGetUp_( rs3::RVector& _refVecUp ) const
{
	if (m_bSensorEffectEditMode_)
	{
		_ASSERT(m_pSensorEffect_);
		_refVecUp = m_pSensorEffect_->GetWorldUp();
		return true;
	}
	else
	{
		return CLogicObject::OnGetUp_(_refVecUp);
	}
}

SAttributeAtXML* CEntitySensor::OnGetUpAttribute_()
{
	if (m_bSensorEffectEditMode_)
	{
		return m_pSensorEffectUpAttribute_;
	}
	else
	{
		return m_pUpAttribute_;
	}
}

void CEntitySensor::OnLoadCompleted_()
{
	CLogicObject::OnLoadCompleted_();
	UpdateSensorEffectTM_();
}

void CEntitySensor::CreateSensorEffect_( std::string& _strSensroName )
{
	if (_strSensroName == "none")
		return;

	XEffectInfo* pEffectInfo = m_pControlMediator->GetViewPtr<CRollupEntityObjectList>()->GetEffectInfoMgr()->GetEffectInfo(_strSensroName);
	if (!pEffectInfo)
		return;

	_ASSERT(m_pSensorEffect_ == NULL);
	std::string strResourceName;
	int nEffectCnt = (int)pEffectInfo->m_vecEffectInfoItem.size();
	for (int i = 0; i < nEffectCnt; ++i)
	{
		if (pEffectInfo->m_vecEffectInfoItem[i]->eEffectType == CET_MODEL)
		{
			strResourceName = pEffectInfo->m_vecEffectInfoItem[i]->strResourceName;
		}
	}
	rs3::REffectInstanceSceneNode effectInstance;
	if( !effectInstance.Create( strResourceName.c_str() ) )
		return;

	effectInstance.GetEffectSceneNode()->RemoveFromParent();
	m_pSensorEffect_ = effectInstance.GetEffectSceneNode();
	m_pControlMediator->GetToolSceneManager()->AttachEffect(m_pSensorEffect_, NULL, NULL);
}

void CEntitySensor::UpdateSensorEffectTM_()
{
	if (!m_pSensorEffect_)
		return;

	_ASSERT(m_dSensorEffectPos[0]);
	_ASSERT(m_pdX_);
	RVector3 vPos( float(*m_pdX_ + *m_dSensorEffectPos[0]), float(*m_pdY_ + *m_dSensorEffectPos[1]), float(*m_pdZ_ + *m_dSensorEffectPos[2]) );

	_ASSERT(m_dSensorEffectDir[0]);
	RVector3 vDir( (float)*m_dSensorEffectDir[0], (float)*m_dSensorEffectDir[1], (float)*m_dSensorEffectDir[2] );

	_ASSERT(m_dSensorEffectUp[0]);
	RVector3 vUp( (float)*m_dSensorEffectUp[0], (float)*m_dSensorEffectUp[1], (float)*m_dSensorEffectUp[2] );

	m_pSensorEffect_->SetTransform(vPos, vDir, vUp);
	m_pSensorEffect_->SetScale( m_vSensorEffectScale_ );

	m_pSensorEffect_->UpdateTransform();
}

void CEntitySensor::DestroySensorEffect_()
{
	if (!m_pSensorEffect_)
		return;

	m_pSensorEffect_->RemoveFromParent();
	delete m_pSensorEffect_;
	m_pSensorEffect_ = NULL;
}

void CEntitySensor::SetEnableSensorEffect_( bool bEnable )
{
	m_bSensorEffectEditMode_ = bEnable;
	if (m_bSensorEffectEditMode_ && !m_pSensorEffect_)
		m_bSensorEffectEditMode_ = false;

	UpdateEntityAABB_();
}

bool CEntitySensor::NotifyPosAttributeDClickedJustOneEntity( SAttributeAtXML* _pDClickedEntityAttribute )
{
	bool bAttributeValid = (_pDClickedEntityAttribute != m_pPosAttribute_) && (_pDClickedEntityAttribute != NULL);
	SetEnableSensorEffect_(( bAttributeValid && m_pSensorEffect_));

	return (m_pSensorEffect_ != NULL);
}

bool CEntitySensor::NotifyDirAttributeDClickedJustOneEntity( SAttributeAtXML* _pDClickedEntityAttribute )
{
	bool bAttributeValid = (_pDClickedEntityAttribute != m_pDirAttribute_) && (_pDClickedEntityAttribute != NULL);
	SetEnableSensorEffect_(( bAttributeValid && m_pSensorEffect_));

	return (m_pSensorEffect_ != NULL);
}

bool CEntitySensor::NotifyUpAttributeDClickedJustOneEntity( SAttributeAtXML* _pDClickedEntityAttribute )
{
	bool bAttributeValid = (_pDClickedEntityAttribute != m_pUpAttribute_) && (_pDClickedEntityAttribute != NULL);
	SetEnableSensorEffect_(( bAttributeValid && m_pSensorEffect_));

	return (m_pSensorEffect_ != NULL);
}

bool CEntitySensor::NotifyAttributeChangedJustOneEntity( SAttributeAtXML* _pChangedEntityAttribute )
{
	if (_pChangedEntityAttribute->m_pClientElement->m_strNodeName == "EFFECT" )
	{
		if (_pChangedEntityAttribute->m_strAttrName == "scale_x")
		{
			m_vSensorEffectScale_.x = (float)_pChangedEntityAttribute->m_vGenericNum[0];
			UpdateSensorEffectTM_();
		}
		else if (_pChangedEntityAttribute->m_strAttrName == "scale_y")
		{
			m_vSensorEffectScale_.y = (float)_pChangedEntityAttribute->m_vGenericNum[0];
			UpdateSensorEffectTM_();
		}
		else if (_pChangedEntityAttribute->m_strAttrName == "scale_z")
		{
			m_vSensorEffectScale_.z = (float)_pChangedEntityAttribute->m_vGenericNum[0];
			UpdateSensorEffectTM_();
		}
		else if (_pChangedEntityAttribute->m_strAttrName == "effect")
		{
			DestroySensorEffect_();
			CreateSensorEffect_(_pChangedEntityAttribute->m_strGenericString);

			// 처음으로 더해질때
			if (!m_pSensorEffectPosAttribute_)
			{
				SElementAtXML* pEffectElement = _pChangedEntityAttribute->m_pClientElement;
				for(SElementAtXML::AttributeList::iterator itr = pEffectElement->m_vecAttribute.begin();
					itr != pEffectElement->m_vecAttribute.end(); ++itr)
				{
					SAttributeAtXML* pAttribute = *itr;
					if (pAttribute->m_eAttrType == SAttributeAtXML::E_POS)
					{
						UpdateSpecialPos_(pAttribute, false);
					}
					else if (pAttribute->m_eAttrType == SAttributeAtXML::E_DIR)
					{
						UpdateSpecialDir_(pAttribute, false);
					}
					else if (pAttribute->m_eAttrType == SAttributeAtXML::E_UP)
					{
						UpdateSpecialUp_(pAttribute, false);
					}
				}
			}

			UpdateSensorEffectTM_();
			SetEnableSensorEffect_((m_pSensorEffect_ != NULL));
		}

		return (m_pSensorEffect_ != NULL);
	}
	return false;
}

void CEntitySensor::GetSensorRenderPos( rs3::RMatrix& _refTrans )
{
	_ASSERT(m_pdX_);
	_refTrans._41 = (float)*m_pdX_;
	_refTrans._42 = (float)*m_pdY_;
	_refTrans._43 = (float)*m_pdZ_;
}

CLogicObject* CEntitySensor::OnPickForSelection_( float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to )
{
	if(GetVisible() &&
		GetEditable() &&
		m_ObjectSelect.PickForSelection(this, m_pControlMediator, _rPrevPickedSqLength, origin, to))
		return this;

	return NULL;
}

void CEntitySensor::OnPickForSelection_( std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum )
{
	if(GetVisible() &&
		GetEditable() &&
		m_ObjectSelect.PickForSelection(this, m_pControlMediator, _refFrustum))
		_refVecLogicObject.push_back(this);
}

//////////////////////////////////////////////////////////////////////////

/**
 	변칙적인 방법으로 Scale을 처리 - radius 를 사용하지 않지만, 액션을 사용할 때에만 factor 로서 쓴다.
 */

CEntitySound::CEntitySound()
: m_pdDistanceMin_(NULL)
, m_pdDistanceMax_(NULL)
, m_pdVolume_(NULL)
{
	m_ObjectSelect.DoUse();
}

CEntitySound::~CEntitySound()
{
}

void CEntitySound::OnSetScale_(const float fUpdateRadius)
{
	// m_pfDistanceMax_ 는 m_pfRadius와 링크되어있어서, 기존 루틴으로 업데이트 된다.
	float fRadius = 0.0f;
	if ( !GetScale(fRadius))
		return;

	float fScaleRate = fUpdateRadius / fRadius;
	if ( m_pdDistanceMin_ )
		*m_pdDistanceMin_ *= fScaleRate;

	CEntityObject::OnSetScale_(fUpdateRadius);
}

void CEntitySound::UpdateSpecialSTRING_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase)
{
	_ASSERT(_pNewEntityAttribute);
	if ( _bBaseCase )
	{
	}
}

void CEntitySound::UpdateSpecialFLOAT_(SAttributeAtXML* _pNewEntityAttribute, bool _bBaseCase)
{
	_ASSERT(_pNewEntityAttribute);
	if ( _bBaseCase )
	{
		if ( _pNewEntityAttribute->m_strAttrName == "distance_min" )
		{
			m_pdDistanceMin_ = &_pNewEntityAttribute->m_vGenericNum[0];
			m_pDistanceMinAttribute_ = _pNewEntityAttribute;
		}
		else if ( _pNewEntityAttribute->m_strAttrName == "distance_max" )
		{
			m_pdDistanceMax_ = &_pNewEntityAttribute->m_vGenericNum[0];

			// radius 와 링크
			m_pdRadius_ = &_pNewEntityAttribute->m_vGenericNum[0];
			m_pRadiusAttribute_ = _pNewEntityAttribute;
		}
		else if ( _pNewEntityAttribute->m_strAttrName == "volume" )
		{
			m_pdVolume_ = &_pNewEntityAttribute->m_vGenericNum[0];
		}
	}
	else
	{
		// child element
	}
}

bool CEntitySound::GetDistanceMin(float &_refDistanceMin)
{
	if ( m_pdDistanceMin_ == NULL )
		return false;

	_refDistanceMin = (float)*m_pdDistanceMin_;
	return true;
}

bool CEntitySound::GetDistanceMax(float &_refDistanceMax)
{
	if ( m_pdDistanceMax_ == NULL )
		return false;

	_refDistanceMax = (float)*m_pdDistanceMax_;
	return true;
}

bool CEntitySound::GetVolume(float &_refVolume)
{
	if ( m_pdVolume_ == NULL )
		return false;

	_refVolume = (float)*m_pdVolume_;
	return true;
}

void CEntitySound::UpdateEntityAABB_()
{
	float fRadius = 10.f;
	if ( m_pdDistanceMax_ )
		fRadius = max(fRadius, (float)*m_pdDistanceMax_);

	rs3::RVector3 vPos;
	if ( GetPosition(vPos))
	{
		m_aabb_.minx = (float)*m_pdX_ - fRadius;
		m_aabb_.maxx = (float)*m_pdX_ + fRadius;
		m_aabb_.miny = (float)*m_pdY_ - fRadius;
		m_aabb_.maxy = (float)*m_pdY_ + fRadius;
		m_aabb_.minz = (float)*m_pdZ_ - fRadius;
		m_aabb_.maxz = (float)*m_pdZ_ + fRadius;
	}

	_ASSERT( m_aabb_.IsValid() );
}

CLogicObject* CEntitySound::OnPickForSelection_( float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to )
{
	if(GetVisible() &&
		GetEditable() &&
		m_ObjectSelect.PickForSelection(this, m_pControlMediator, _rPrevPickedSqLength, origin, to))
		return this;

	return NULL;
}

void CEntitySound::OnPickForSelection_( std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum )
{
	if(GetVisible() &&
		GetEditable() &&
		m_ObjectSelect.PickForSelection(this, m_pControlMediator, _refFrustum))
		_refVecLogicObject.push_back(this);
}


//////////////////////////////////////////////////////////////////////////

bool CEntityArea::NotifyAttributeChangedJustOneEntity( SAttributeAtXML* _pChangedEntityAttribute )
{
	if (_pChangedEntityAttribute->m_strAttrName == "radius")
	{
		_pChangedEntityAttribute->m_vGenericNum[0] = m_dScaleTemp_;

		return true;
	}

	return false;
}

void CEntityArea::UpdateEntityAABB_()
{
	m_aabb_.Initialize();

	std::vector< std::pair<SAttributeAtXML*,MBox> > vBoxes;
	GetCylinderAABBs(vBoxes);

	for (std::vector< std::pair<SAttributeAtXML*,MBox> >::iterator itr = vBoxes.begin(); itr != vBoxes.end(); ++itr)
	{
		m_aabb_.Add( itr->second );
	}

	_ASSERT( m_aabb_.IsValid() );

	CylinderPolygonUpdate();
}

void CEntityArea::GetCylinderAABBs( std::vector<std::pair<SAttributeAtXML*,MBox>>& _vMBox )
{
	float x(0.f), y(0.f), z(0.f), r(0.f), h(0.f);
	SElementAtXML* pRootElement = GetRootElement();
	if (pRootElement)
	{
		for (SElementAtXML::ElementList::iterator itr = pRootElement->m_vecChildElement.begin(); itr != pRootElement->m_vecChildElement.end(); ++itr)
		{
			if ((*itr)->m_strNodeName == "CYLINDER")
			{
				SElementAtXML* pCyilinderElement = *itr;
				SAttributeAtXML* pLastAttribute = NULL;

				for (SElementAtXML::AttributeList::iterator itr_attr = pCyilinderElement->m_vecAttribute.begin();
					itr_attr != pCyilinderElement->m_vecAttribute.end(); ++itr_attr)
				{
					pLastAttribute = *itr_attr;
					if (pLastAttribute->m_strAttrName == "Position")
					{
						x = (float)(*itr_attr)->m_vGenericNum[0];
						y = (float)(*itr_attr)->m_vGenericNum[1];
						z = (float)(*itr_attr)->m_vGenericNum[2];
					}
					else if (pLastAttribute->m_strAttrName == "radius")
					{
						r = (float)(*itr_attr)->m_vGenericNum[0];
					}
					else if (pLastAttribute->m_strAttrName == "height")
					{
						h = (float)(*itr_attr)->m_vGenericNum[0];
					}
					else
					{
						_ASSERT(0);
					}
				}

				_vMBox.push_back( std::pair<SAttributeAtXML*,MBox>(pLastAttribute, MBox(x-r, y-r, z, x+r, y+r, z+h)) );
			}
		}
	}
}

bool CEntityArea::OnGetPos_( rs3::RVector& _refVecPos ) const
{
	if (m_pdX_ == NULL)
	{
		if (m_nSubCylinderSelectedIdx > -1)
		{
			_refVecPos = m_CylinderPos;
			return true;
		}
		else
		{
			CEntityArea* pEntityArea = const_cast<CEntityArea*>(this);
			if ( pEntityArea->GetRootElement()->m_vecChildElement.empty())
				return false;

			SElementAtXML* pCurrentCylinderElement = pEntityArea->GetRootElement()->m_vecChildElement[0];
			double* x = NULL;
			double* y = NULL;
			double* z = NULL;

			for (SElementAtXML::AttributeList::iterator itr = pCurrentCylinderElement->m_vecAttribute.begin(); itr != pCurrentCylinderElement->m_vecAttribute.end(); ++itr)
			{
				if ((*itr)->m_strAttrName == "Position")
				{
					x = &(*itr)->m_vGenericNum[0];
					y = &(*itr)->m_vGenericNum[1];
					z = &(*itr)->m_vGenericNum[2];

					break;
				}
			}

			if (x == NULL)
				return false;

			pEntityArea->SetTempPos(x, y, z);
			bool bRet = pEntityArea->GetPosition(_refVecPos);
			pEntityArea->ClearTempPos();
			return bRet;
		}
	}
	else
	{
		return CLogicObject::OnGetPos_(_refVecPos);
	}
}

CLogicObject* CEntityArea::OnPickForSelection_( float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to )
{
	if(CEntityObject::OnPickForSelection_(_rPrevPickedSqLength, origin, to) != NULL)
	{
		float fCylinderDist = 0.0f;
		for(vector<stCylinder>::iterator it = m_vecCylinderForPicking.begin(); it!= m_vecCylinderForPicking.end(); ++it)
		{
			for(int i = 0; i < AREA_CYLINDER_SEGMENT; ++i)
			{
				vec3 vDir = (to - origin);
				vDir.Normalize();

				float fu = 0.0f;
				float fv = 0.0f;
				float fDist = 0.0f;

				int nIndex0 = i * 2;
				int nIndex1 = (nIndex0 + 1) % (AREA_CYLINDER_SEGMENT * 2);
				int nIndex2 = (nIndex0 + 2) % (AREA_CYLINDER_SEGMENT * 2);
				int nIndex3 = (nIndex0 + 3) % (AREA_CYLINDER_SEGMENT * 2);

				if(IntersectTriangle(origin, vDir, it->m_vCylinderVertices[nIndex0], it->m_vCylinderVertices[nIndex1], it->m_vCylinderVertices[nIndex2], &fDist, &fu, &fv) ||
					IntersectTriangle(origin, vDir, it->m_vCylinderVertices[nIndex3], it->m_vCylinderVertices[nIndex2], it->m_vCylinderVertices[nIndex1], &fDist, &fu, &fv) ||
					IntersectTriangle(origin, vDir, it->m_vCylinderVertices[nIndex3], it->m_vCylinderVertices[nIndex1], it->m_vCylinderVertices[nIndex2], &fDist, &fu, &fv) ||
					IntersectTriangle(origin, vDir, it->m_vCylinderVertices[nIndex0], it->m_vCylinderVertices[nIndex2], it->m_vCylinderVertices[nIndex1], &fDist, &fu, &fv))
				{
					// test
					if(fDist < _rPrevPickedSqLength)
					{
						_rPrevPickedSqLength = fDist;
						return this;
					}
				}
			}
		}
	}

	return NULL;
}

void CEntityArea::OnPickForSelection_( std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum )
{
	if(GetVisible() &&
		GetEditable())
	{
		for(vector<stCylinder>::iterator it = m_vecCylinderForPicking.begin(); it!= m_vecCylinderForPicking.end(); ++it)
		{
			for(int i = 0; i < AREA_CYLINDER_SEGMENT * 2; ++i)
			{
				if(_refFrustum.Test(it->m_vCylinderVertices[i]))
				{
					_refVecLogicObject.push_back(this);
					return;
				}
			}
		}
	}
}

void CEntityArea::CylinderPolygonUpdate()
{
	m_vecCylinderForPicking.clear();

	float x(0.f), y(0.f), z(0.f), r(0.f), h(0.f);
	SElementAtXML* pRootElement = GetRootElement();
	if (pRootElement)
	{
		for (SElementAtXML::ElementList::iterator itr = pRootElement->m_vecChildElement.begin(); itr != pRootElement->m_vecChildElement.end(); ++itr)
		{
			if ((*itr)->m_strNodeName == "CYLINDER")
			{
				SElementAtXML* pCyilinderElement = *itr;
				SAttributeAtXML* pLastAttribute = NULL;

				for (SElementAtXML::AttributeList::iterator itr_attr = pCyilinderElement->m_vecAttribute.begin();
					itr_attr != pCyilinderElement->m_vecAttribute.end(); ++itr_attr)
				{
					pLastAttribute = *itr_attr;
					if (pLastAttribute->m_strAttrName == "Position")
					{
						x = (float)(*itr_attr)->m_vGenericNum[0];
						y = (float)(*itr_attr)->m_vGenericNum[1];
						z = (float)(*itr_attr)->m_vGenericNum[2];
					}
					else if (pLastAttribute->m_strAttrName == "radius")
					{
						r = (float)(*itr_attr)->m_vGenericNum[0];
					}
					else if (pLastAttribute->m_strAttrName == "height")
					{
						h = (float)(*itr_attr)->m_vGenericNum[0];
					}
					else
					{
						_ASSERT(0);
					}
				}

				MMatrix matTranslation_;
				matTranslation_.MakeIdentity();
				matTranslation_.SetTranslation(vec3(x, y, z));

				stCylinder cylinderData;
				CalCylinderPolygon(cylinderData, matTranslation_, r, h);
				m_vecCylinderForPicking.push_back(cylinderData);
			}
		}
	}
}

void CEntityArea::CalCylinderPolygon( stCylinder& ClyinderData, const MMatrix &matWorld, float fRadius, float fHeight )
{
	RMatrix matTM;
	matTM.SetScaleMatrix(fRadius,fRadius,fHeight);
	matTM*=matWorld;

	for( int i = 0; i < AREA_CYLINDER_SEGMENT; i++ )
	{
		float theta = (2*MMath::PI*i) / (AREA_CYLINDER_SEGMENT-1);
		
		vec3 vResult = vec3::ZERO;
		vec3 vTemp = RVector( sinf(theta), cosf(theta), 0);
		matTM.TransformVect(vTemp, vResult);

		ClyinderData.m_vCylinderVertices[2*i+0] = vResult;

		vTemp = RVector( sinf(theta), cosf(theta), 1);
		matTM.TransformVect(vTemp, vResult);

		ClyinderData.m_vCylinderVertices[2*i+1] = vResult;
	}
}


//////////////////////////////////////////////////////////////////////////

CEntityRoom::CEntityRoom()
{
}

CEntityRoom::~CEntityRoom()
{
}

void CEntityRoom::UpdateEntityAABB_()
{
	vec3 vPos;
	if ( !GetPosition(vPos))
		return;

	float fWidth = 0.0f;
	float fDepth = 0.0f;
	float fHeight = 0.0f;
	SElementAtXML* pRootProperty = GetRootElement();
	ASSERT(pRootProperty);

	for (SElementAtXML::AttributeList::iterator itr = pRootProperty->m_vecAttribute.begin();
		itr != pRootProperty->m_vecAttribute.end(); ++itr)
	{
		if ((*itr)->m_strAttrName == "width") 
			fWidth = (float)(*itr)->m_vGenericNum[0];
		else if ((*itr)->m_strAttrName == "depth") 
			fDepth = (float)(*itr)->m_vGenericNum[0];
		else if ((*itr)->m_strAttrName == "height") 
			fHeight = (float)(*itr)->m_vGenericNum[0];
	}

	float fSize = sqrt(pow(fWidth,2) + pow(fDepth,2));
	m_aabb_.minx = vPos.x + -fSize;
	m_aabb_.maxx = vPos.x + fSize;
	m_aabb_.miny = vPos.y + -fSize;
	m_aabb_.maxy = vPos.y + fSize;
	m_aabb_.minz = vPos.z + -fHeight;
	m_aabb_.maxz = vPos.z + fHeight;

	_ASSERT( m_aabb_.IsValid() );
}

void CEntityRoom::SaveToXML( MXmlElement& _refElement, bool bCommon )
{
	// ID 속성 구하기
	SElementAtXML* pRootProperty = GetRootElement();
	ASSERT(pRootProperty);

	uint16 nGroupID = 0;
	for (SElementAtXML::AttributeList::iterator itr = pRootProperty->m_vecAttribute.begin();
		itr != pRootProperty->m_vecAttribute.end(); ++itr)
	{
		if ((*itr)->m_strAttrName == "id") 
			nGroupID = (uint16)(*itr)->m_vGenericNum[0];
	}


	rs3::ROBB obb = GetOBB();

	// 스폰 정보에 group 속성 추가
	ENTITY_OBJECTLIST* pList = m_pControlMediator->GetToolLogicObjectMgr()->GetEntityObjectList<CEntityObject>(ENTITY_SPAWN);
	if (pList)
	{
		for(ENTITY_OBJECTLIST::iterator itr = pList->begin(); itr != pList->end(); ++itr)
		{
			CEntitySpawn* pObj = dynamic_cast<CEntitySpawn*>(*itr);
			ASSERT(pObj);
			
			RVector vPos;
			pObj->GetPosition(vPos);
			if (obb.IsOut(vPos))
				continue; // 범위에서 벗어난 스폰

			SElementAtXML* pRootProperty = pObj->GetRootElement();
			ASSERT(pRootProperty);
			for (SElementAtXML::AttributeList::iterator itr = pRootProperty->m_vecAttribute.begin();
				itr != pRootProperty->m_vecAttribute.end(); ++itr)
			{
				if ((*itr)->m_strAttrName == "group") 
					(*itr)->m_vGenericNum[0] = (double)nGroupID;
			}
		}
	}	
	
	__super::SaveToXML(_refElement, bCommon);
}

rs3::ROBB CEntityRoom::GetOBB()
{
	RVector vPos;
	GetPosition(vPos);

	float fWidth = 0.0f;
	float fDepth = 0.0f;
	float fHeight = 0.0f;
	float fRotate = 0.0f;
	SElementAtXML* pRootProperty = GetRootElement();
	ASSERT(pRootProperty);

	for (SElementAtXML::AttributeList::iterator itr = pRootProperty->m_vecAttribute.begin();
		itr != pRootProperty->m_vecAttribute.end(); ++itr)
	{
		if ((*itr)->m_strAttrName == "width") 
			fWidth = (float)(*itr)->m_vGenericNum[0];
		else if ((*itr)->m_strAttrName == "depth") 
			fDepth = (float)(*itr)->m_vGenericNum[0];
		else if ((*itr)->m_strAttrName == "height") 
			fHeight = (float)(*itr)->m_vGenericNum[0];
		else if ((*itr)->m_strAttrName == "rotate") 
			fRotate = (float)(*itr)->m_vGenericNum[0];
	}

	MMatrix m;
	m.SetRotationZ(MMath::DegreeToRadian(fRotate));
	vPos.x += fWidth/2;
	vPos.y += fDepth/2;
	vPos.z += fHeight/2;
	return ROBB(MVector3(m._11,m._12,m._13), MVector3(m._21,m._22,m._23), MVector3(m._31,m._32,m._33), vec3(fWidth/2,fDepth/2,fHeight/2), vPos);
}
