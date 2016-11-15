#include "StdAfx.h"
#include "M_LogicObject.h"
#include "M_LogicObjectCommandBuffer.h"

void CLogicObject::OnSetObjectName(const char* szName)
{
	if(m_pStrLogicObjectName_ != NULL) 
		*m_pStrLogicObjectName_ = szName;
}

void CLogicObject::OnSetLayerName(const char* szName)
{
	m_strLayer = szName;
}

void CLogicObject::OnSetPos_(const rs3::RVector& _refUpdatePos)
{
	if(m_pdX_ != NULL && m_pdY_ != NULL && m_pdZ_ != NULL)
	{
		*m_pdX_ = _refUpdatePos.x;
		*m_pdY_ = _refUpdatePos.y;
		*m_pdZ_ = _refUpdatePos.z;

		OnUpdateAABB_();
	}
}

void CLogicObject::OnSetDirUp_(const rs3::RVector &_refUpdateDir, const rs3::RVector &_refUpdateUp)
{
	bool bNeedUpdate = false;
	if(m_pdDirX_ != NULL && m_pdDirY_ != NULL && m_pdDirZ_ != NULL)
	{
		*m_pdDirX_ = _refUpdateDir.x;
		*m_pdDirY_ = _refUpdateDir.y;
		*m_pdDirZ_ = _refUpdateDir.z;

		bNeedUpdate = true;
	}

	if(m_pdUpX_ != NULL && m_pdUpY_ != NULL && m_pdUpZ_ != NULL)
	{
		*m_pdUpX_ = _refUpdateUp.x;
		*m_pdUpY_ = _refUpdateUp.y;
		*m_pdUpZ_ = _refUpdateUp.z;

		bNeedUpdate = true;
	}

	if(bNeedUpdate)
		OnUpdateAABB_();
}

void CLogicObject::OnSetScale_(const float fUpdateRadius)
{
	_ASSERT(m_pdRadius_);
	if(m_pdRadius_ != NULL)
	{
		*m_pdRadius_ = fUpdateRadius;
		OnUpdateAABB_();
	}
}

bool CLogicObject::OnGetPos_(rs3::RVector& _refVecPos) const
{
	if(m_pdX_ != NULL && m_pdY_ != NULL && m_pdZ_ != NULL)
	{
		_refVecPos.x = (float)*m_pdX_;
		_refVecPos.y = (float)*m_pdY_;
		_refVecPos.z = (float)*m_pdZ_;
		return true;
	}

	return false;
}

bool CLogicObject::OnGetDir_(rs3::RVector &_refVecDir) const
{
	if(m_pdDirX_ != NULL && m_pdDirY_ != NULL && m_pdDirZ_ != NULL)
	{
		_refVecDir.x = (float)*m_pdDirX_;
		_refVecDir.y = (float)*m_pdDirY_;
		_refVecDir.z = (float)*m_pdDirZ_;
		return true;
	}

	return false;
}

bool CLogicObject::OnGetUp_(rs3::RVector &_refVecUp) const
{
	if(m_pdUpX_ != NULL && m_pdUpY_ != NULL && m_pdUpZ_ != NULL)
	{
		_refVecUp.x = (float)*m_pdUpX_;
		_refVecUp.y = (float)*m_pdUpY_;
		_refVecUp.z = (float)*m_pdUpZ_;
		return true;
	}

	return false;
}

bool CLogicObject::OnGetScale_(float& _refSize) const
{
	if(m_pdRadius_ != NULL)
	{
		_refSize = (float)*m_pdRadius_;
		return true;
	}
	else
		return false;
}

CLogicObject::CLogicObject(void)
{
	m_pdX_ = NULL;
	m_pdY_ = NULL;
	m_pdZ_ = NULL;
	m_pdDirX_ = NULL;
	m_pdDirY_ = NULL;
	m_pdDirZ_ = NULL;
	m_pdUpX_ = NULL;
	m_pdUpY_ = NULL;
	m_pdUpZ_ = NULL;

	m_pPosAttribute_ = NULL;
	m_pDirAttribute_ = NULL;
	m_pUpAttribute_ = NULL;
	m_pRadiusAttribute_ = NULL;

	m_pdRadius_ = NULL;
	m_pdID_ = NULL;

	m_pLogicObjectElement_ = NULL;
	m_bNeedUpdate = false;
	m_bVisibility_ = true;

	m_pStrLogicObjectName_ = &m_strName;

	m_bEditable = false;
	m_bEntityLogic = false;
}

CLogicObject::~CLogicObject(void)
{
}

const char*	CLogicObject::GetTypeName()
{
	return "LogicObject";
}

void CLogicObject::DestroyLogicObject()
{
	if(m_pLogicObjectElement_ != NULL)
		m_pLogicObjectElement_->DestroyElement();
}

void CLogicObject::UpdateProperty_(SElementAtXML* _pObjectElement, SElementAtXML* _pObjectProperty, MXmlElement* _pElement, bool _bBaseCase)
{
	// copy sub information
	_pObjectElement->m_strSubInfo = _pObjectProperty->m_strSubInfo;

	//Attribute
	for(SElementAtXML::AttributeList::iterator itr = _pObjectProperty->m_vecAttribute.begin();
		itr != _pObjectProperty->m_vecAttribute.end(); ++ itr)
	{
		SAttributeAtXML* pAttribute = *itr;
		if(pAttribute->m_strAttrType == "position")
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_POS;
			pNewEntityAttribute->m_strAttrName = "Position";
			float x = 0, y = 0, z = 0;
			if(_pElement != NULL)
			{
				_Attribute(&x, _pElement, "x");
				_Attribute(&y, _pElement, "y");
				_Attribute(&z, _pElement, "z");
			}
			pNewEntityAttribute->m_vGenericNum.push_back(x);
			pNewEntityAttribute->m_vGenericNum.push_back(y);
			pNewEntityAttribute->m_vGenericNum.push_back(z);

			if(_bBaseCase)
			{
				m_pdX_ = &pNewEntityAttribute->m_vGenericNum[0];
				m_pdY_ = &pNewEntityAttribute->m_vGenericNum[1];
				m_pdZ_ = &pNewEntityAttribute->m_vGenericNum[2];
				m_pPosAttribute_ = pNewEntityAttribute;
			}

			pNewEntityAttribute->m_pClientElement = _pObjectElement;
			_pObjectElement->m_vecAttribute.push_back(pNewEntityAttribute);

			//pos 에 대한 예외처리
			UpdateSpecialPos_(pNewEntityAttribute, _bBaseCase);
		}
		else if(pAttribute->m_strAttrType == "direction")
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_DIR;
			pNewEntityAttribute->m_strAttrName = "Direction";

			//기본 direction
			float x = 1.f;
			float y = 0.f;
			float z = 0.f;
			if(_pElement != NULL)
			{
				_Attribute(&x, _pElement, "dir_x");
				_Attribute(&y, _pElement, "dir_y");
				_Attribute(&z, _pElement, "dir_z");
			}
			pNewEntityAttribute->m_vGenericNum.push_back(x);
			pNewEntityAttribute->m_vGenericNum.push_back(y);
			pNewEntityAttribute->m_vGenericNum.push_back(z);

			if(_bBaseCase)
			{
				m_pdDirX_ = &pNewEntityAttribute->m_vGenericNum[0];
				m_pdDirY_ = &pNewEntityAttribute->m_vGenericNum[1];
				m_pdDirZ_ = &pNewEntityAttribute->m_vGenericNum[2];
				m_pDirAttribute_ = pNewEntityAttribute;
			}

			pNewEntityAttribute->m_pClientElement = _pObjectElement;
			_pObjectElement->m_vecAttribute.push_back(pNewEntityAttribute);

			//dir 에 대한 예외처리
			UpdateSpecialDir_(pNewEntityAttribute, _bBaseCase);
		}
		else if(pAttribute->m_strAttrType == "up")
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_UP;
			pNewEntityAttribute->m_strAttrName = "Up";

			//기본 Up
			float x = 0.f;
			float y = 0.f;
			float z = 1.f;
			if(_pElement != NULL)
			{
				_Attribute(&x, _pElement, "up_x");
				_Attribute(&y, _pElement, "up_y");
				_Attribute(&z, _pElement, "up_z");
			}
			pNewEntityAttribute->m_vGenericNum.push_back(x);
			pNewEntityAttribute->m_vGenericNum.push_back(y);
			pNewEntityAttribute->m_vGenericNum.push_back(z);

			if(_bBaseCase)
			{
				m_pdUpX_ = &pNewEntityAttribute->m_vGenericNum[0];
				m_pdUpY_ = &pNewEntityAttribute->m_vGenericNum[1];
				m_pdUpZ_ = &pNewEntityAttribute->m_vGenericNum[2];
				m_pUpAttribute_ = pNewEntityAttribute;
			}

			pNewEntityAttribute->m_pClientElement = _pObjectElement;
			_pObjectElement->m_vecAttribute.push_back(pNewEntityAttribute);

			//up 에 대한 예외처리
			UpdateSpecialUp_(pNewEntityAttribute, _bBaseCase);
		}
		else if(pAttribute->m_strAttrType == "radius")
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_RADIUS;
			pNewEntityAttribute->m_strAttrName = "radius";

			//default 확인
			float radius;
			if(pAttribute->m_strGenericString.empty())
				radius = 100.0f;
			else
				radius = (float)atof(pAttribute->m_strGenericString.c_str());

			if(_pElement != NULL)
			{
				_Attribute(&radius, _pElement, "radius");
			}
			pNewEntityAttribute->m_vGenericNum.push_back(radius);

			if(_bBaseCase)
			{
				m_pdRadius_ = &pNewEntityAttribute->m_vGenericNum[0];
				m_pRadiusAttribute_ = pNewEntityAttribute;
			}

			pNewEntityAttribute->m_pClientElement = _pObjectElement;
			_pObjectElement->m_vecAttribute.push_back(pNewEntityAttribute);
		}
		else if(pAttribute->m_strAttrType == "bool")
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_BOOL;
			pNewEntityAttribute->m_strAttrName = pAttribute->m_strAttrName;

			std::string strBool;
			//default 확인
			if(!pAttribute->m_strGenericString.empty())
				strBool = pAttribute->m_strGenericString;

			//read
			if(_pElement != NULL)
			{
				_Attribute(strBool, _pElement, pNewEntityAttribute->m_strAttrName.c_str());
			}

			//set value
			pNewEntityAttribute->m_vGenericNum.push_back(SAttributeAtXML::SetAttributeBool(strBool));

			//add attribute
			pNewEntityAttribute->m_pClientElement = _pObjectElement;
			_pObjectElement->m_vecAttribute.push_back(pNewEntityAttribute);

			//bool 에 대한 예외처리
			UpdateSpecialBOOL_(pNewEntityAttribute, _bBaseCase);
		}
		else if(pAttribute->m_strAttrType == "uint")
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_UINT;
			pNewEntityAttribute->m_strAttrName = pAttribute->m_strAttrName;

			//default 확인
			int a;
			if(pAttribute->m_strGenericString.empty())
				a = SAttributeAtXML::SYM_EMPTY;
			else
			{
				if(pAttribute->m_strGenericString == "none")
				{
					a = SAttributeAtXML::SYM_NONE;
				}
				else
				{
					a = atoi(pAttribute->m_strGenericString.c_str());
				}
			}

			if(_pElement != NULL)
			{
				_Attribute(&a, _pElement, pNewEntityAttribute->m_strAttrName.c_str());
			}
			pNewEntityAttribute->m_vGenericNum.push_back(a);

			pNewEntityAttribute->m_pClientElement = _pObjectElement;
			_pObjectElement->m_vecAttribute.push_back(pNewEntityAttribute);

			//id 예외처리
			if(pNewEntityAttribute->m_strAttrName == "id")
			{
				if(_bBaseCase) m_pdID_ = &pNewEntityAttribute->m_vGenericNum[0];
				_pObjectElement->m_pdElementID = &pNewEntityAttribute->m_vGenericNum[0];
			}

			//uint에 대한 예외처리
			UpdateSpecialUINT_(pNewEntityAttribute, _bBaseCase);
		}
		else if(pAttribute->m_strAttrType == "float")
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_FLOAT;
			pNewEntityAttribute->m_strAttrName = pAttribute->m_strAttrName;

			//default 확인
			float a;
			if(pAttribute->m_strGenericString.empty())
				a = -1.f;
			else
				a = atoi(pAttribute->m_strGenericString.c_str());

			if(_pElement != NULL)
			{
				_Attribute(&a, _pElement, pNewEntityAttribute->m_strAttrName.c_str());
			}

			pNewEntityAttribute->m_vGenericNum.push_back(a);
			pNewEntityAttribute->m_pClientElement = _pObjectElement;
			_pObjectElement->m_vecAttribute.push_back(pNewEntityAttribute);

			//float에 대한 예외처리
			UpdateSpecialFLOAT_(pNewEntityAttribute, _bBaseCase);
		}
		else if(pAttribute->m_strAttrType == "string")
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_STRING;
			pNewEntityAttribute->m_strAttrName = pAttribute->m_strAttrName;

			//type_extra
			pNewEntityAttribute->m_strAttrTypeExtra = pAttribute->m_strAttrTypeExtra;

			//default 확인
			if(!pAttribute->m_strGenericString.empty())
				pNewEntityAttribute->m_strGenericString = pAttribute->m_strGenericString;

			//read
			if(_pElement != NULL)
			{
				_Attribute(pNewEntityAttribute->m_strGenericString, _pElement, pNewEntityAttribute->m_strAttrName.c_str());
			}

			//add attribute
			pNewEntityAttribute->m_pClientElement = _pObjectElement;
			_pObjectElement->m_vecAttribute.push_back(pNewEntityAttribute);

			//name 예외처리
			if(pNewEntityAttribute->m_strAttrName == "name" && _bBaseCase)
			{
				m_pStrLogicObjectName_ = &pNewEntityAttribute->m_strGenericString;
			}

			//string 에 대한 예외처리
			UpdateSpecialSTRING_(pNewEntityAttribute, _bBaseCase);
		}
	}

	for(SElementAtXML::ElementList::iterator itr = _pObjectProperty->m_vecChildElement.begin();
		itr != _pObjectProperty->m_vecChildElement.end(); ++itr)
	{
		std::string strNodeName( (*itr)->m_strNodeName );

		MXmlElement* pElement = NULL;
		if( _pElement != NULL)
			pElement = _pElement->FirstChildElement( strNodeName.c_str() );

		if(pElement == NULL)
		{
			SElementAtXML* pEntityElement = new SElementAtXML;
			pEntityElement->m_strNodeName = strNodeName;

			_pObjectElement->m_vecChildElement.push_back(pEntityElement);
			pEntityElement->m_pParentElement = _pObjectElement;

			UpdateProperty_(pEntityElement, *itr, NULL, false);
		}
		else
		{
			while(pElement)
			{
				SElementAtXML* pEntityElement = new SElementAtXML;
				pEntityElement->m_strNodeName = strNodeName;

				_pObjectElement->m_vecChildElement.push_back(pEntityElement);
				pEntityElement->m_pParentElement = _pObjectElement;

				UpdateProperty_(pEntityElement, *itr, pElement, false);
				pElement = pElement->NextSiblingElement( strNodeName.c_str() );
			}
		}
	}
}

void CLogicObject::LoadPropertyFromXML(SElementAtXML* _pEntityProperty, MXmlElement* _pElement)
{
	if(_pEntityProperty == NULL) return;
	ASSERT(m_pLogicObjectElement_ == NULL);

	// set sub information from property
	m_strSubInfo_ = _pEntityProperty->m_strSubInfo;

	m_pLogicObjectElement_ =  new SElementAtXML;
	m_pLogicObjectElement_->m_strNodeName = _pEntityProperty->m_strNodeName;
	m_pLogicObjectElement_->m_pParentElement = NULL;

	UpdateProperty_(m_pLogicObjectElement_, _pEntityProperty, _pElement, true);
	OnLoadCompleted_();
	OnUpdateAABB_();
}

void CLogicObject::UpdateElementWithTreeCtrl_(HTREEITEM hItem, SElementAtXML* _pElement, CTreeCtrl& _refTreeCtrl)
{
	for(SElementAtXML::AttributeList::iterator itr = _pElement->m_vecAttribute.begin();
		itr != _pElement->m_vecAttribute.end(); ++itr)
		(*itr)->UpdateMFCTreeItem(_refTreeCtrl, hItem, false);

	for(SElementAtXML::ElementList::iterator itr = _pElement->m_vecChildElement.begin();
		itr != _pElement->m_vecChildElement.end(); ++itr)
	{

		HTREEITEM hChild = _refTreeCtrl.InsertItem((*itr)->m_strNodeName.c_str(), hItem, TVI_LAST);
		UpdateElementWithTreeCtrl_(hChild, *itr, _refTreeCtrl);
	}
}

void CLogicObject::PutToTreeCtrl(CTreeCtrl& _refTreeCtrl)
{
	_refTreeCtrl.DeleteAllItems();
	if(m_pLogicObjectElement_ == NULL) return;

	HTREEITEM hParent = _refTreeCtrl.InsertItem( m_pLogicObjectElement_->m_strNodeName.c_str(), TVI_ROOT);
	UpdateElementWithTreeCtrl_(hParent, m_pLogicObjectElement_,_refTreeCtrl);

	_refTreeCtrl.Expand(_refTreeCtrl.GetRootItem(), TVE_EXPAND);
}

void CLogicObject::CopyElement(CLogicObject* _pClientObject, SElementAtXML* _pFromElement, SElementAtXML* _pToElement, bool _bBaseCase)
{
	for(SElementAtXML::AttributeList::iterator itr = _pFromElement->m_vecAttribute.begin();
		itr != _pFromElement->m_vecAttribute.end(); ++itr)
	{
		SAttributeAtXML* pAttribute = *itr;
		if(pAttribute->m_eAttrType == SAttributeAtXML::E_POS)
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_POS;
			pNewEntityAttribute->m_strAttrName = "Position";

			if (_pClientObject->m_pdX_)
			{
				pNewEntityAttribute->m_vGenericNum.push_back(*_pClientObject->m_pdX_);
				pNewEntityAttribute->m_vGenericNum.push_back(*_pClientObject->m_pdY_);
				pNewEntityAttribute->m_vGenericNum.push_back(*_pClientObject->m_pdZ_);
			}
			else
			{
				pNewEntityAttribute->m_vGenericNum.push_back(0);
				pNewEntityAttribute->m_vGenericNum.push_back(0);
				pNewEntityAttribute->m_vGenericNum.push_back(0);
			}

			if(_bBaseCase)
			{
				_pClientObject->m_pdX_ = &pNewEntityAttribute->m_vGenericNum[0];
				_pClientObject->m_pdY_ = &pNewEntityAttribute->m_vGenericNum[1];
				_pClientObject->m_pdZ_ = &pNewEntityAttribute->m_vGenericNum[2];
				_pClientObject->m_pPosAttribute_ = pNewEntityAttribute;
			}

			pNewEntityAttribute->m_pClientElement = _pToElement;
			_pToElement->m_vecAttribute.push_back(pNewEntityAttribute);

			//pos 예외처리
			_pClientObject->UpdateSpecialPos_(pNewEntityAttribute, _bBaseCase);
		}
		else if(pAttribute->m_eAttrType == SAttributeAtXML::E_DIR)
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_DIR;
			pNewEntityAttribute->m_strAttrName = "Direction";

			if (_pClientObject->m_pdDirX_)
			{
				pNewEntityAttribute->m_vGenericNum.push_back(*_pClientObject->m_pdDirX_);
				pNewEntityAttribute->m_vGenericNum.push_back(*_pClientObject->m_pdDirY_);
				pNewEntityAttribute->m_vGenericNum.push_back(*_pClientObject->m_pdDirZ_);
			}
			else
			{
				pNewEntityAttribute->m_vGenericNum.push_back(1);
				pNewEntityAttribute->m_vGenericNum.push_back(0);
				pNewEntityAttribute->m_vGenericNum.push_back(0);
			}

			if(_bBaseCase)
			{
				_pClientObject->m_pdDirX_ = &pNewEntityAttribute->m_vGenericNum[0];
				_pClientObject->m_pdDirY_ = &pNewEntityAttribute->m_vGenericNum[1];
				_pClientObject->m_pdDirZ_ = &pNewEntityAttribute->m_vGenericNum[2];
				_pClientObject->m_pDirAttribute_ = pNewEntityAttribute;
			}

			pNewEntityAttribute->m_pClientElement = _pToElement;
			_pToElement->m_vecAttribute.push_back(pNewEntityAttribute);

			//dir 예외처리
			_pClientObject->UpdateSpecialDir_(pNewEntityAttribute, _bBaseCase);
		}
		else if(pAttribute->m_eAttrType == SAttributeAtXML::E_UP)
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_UP;
			pNewEntityAttribute->m_strAttrName = "Up";

			if (_pClientObject->m_pdUpX_)
			{
				pNewEntityAttribute->m_vGenericNum.push_back(*_pClientObject->m_pdUpX_);
				pNewEntityAttribute->m_vGenericNum.push_back(*_pClientObject->m_pdUpY_);
				pNewEntityAttribute->m_vGenericNum.push_back(*_pClientObject->m_pdUpZ_);
			}
			else
			{
				pNewEntityAttribute->m_vGenericNum.push_back(0);
				pNewEntityAttribute->m_vGenericNum.push_back(0);
				pNewEntityAttribute->m_vGenericNum.push_back(1);
			}

			if(_bBaseCase)
			{
				_pClientObject->m_pdUpX_ = &pNewEntityAttribute->m_vGenericNum[0];
				_pClientObject->m_pdUpY_ = &pNewEntityAttribute->m_vGenericNum[1];
				_pClientObject->m_pdUpZ_ = &pNewEntityAttribute->m_vGenericNum[2];
				_pClientObject->m_pUpAttribute_ = pNewEntityAttribute;
			}

			pNewEntityAttribute->m_pClientElement = _pToElement;
			_pToElement->m_vecAttribute.push_back(pNewEntityAttribute);

			//up 예외처리
			_pClientObject->UpdateSpecialUp_(pNewEntityAttribute, _bBaseCase);
		}
		else if(pAttribute->m_eAttrType == SAttributeAtXML::E_RADIUS)
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_RADIUS;
			pNewEntityAttribute->m_strAttrName = "radius";
			pNewEntityAttribute->m_vGenericNum.push_back(pAttribute->m_vGenericNum[0]);

			if(_bBaseCase)
			{
				_pClientObject->m_pdRadius_ = &pNewEntityAttribute->m_vGenericNum[0];
				_pClientObject->m_pRadiusAttribute_ = pNewEntityAttribute;
			}

			pNewEntityAttribute->m_pClientElement = _pToElement;
			_pToElement->m_vecAttribute.push_back(pNewEntityAttribute);
		}
		else if(pAttribute->m_eAttrType == SAttributeAtXML::E_BOOL)
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_BOOL;
			pNewEntityAttribute->m_strAttrName = pAttribute->m_strAttrName;
			pNewEntityAttribute->m_vGenericNum.push_back( pAttribute->m_vGenericNum[0] );

			pNewEntityAttribute->m_pClientElement = _pToElement;
			_pToElement->m_vecAttribute.push_back(pNewEntityAttribute);

			//bool 예외 복사
			_pClientObject->UpdateSpecialBOOL_(pNewEntityAttribute, _bBaseCase);
		}
		else if(pAttribute->m_eAttrType == SAttributeAtXML::E_UINT)
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_UINT;
			pNewEntityAttribute->m_strAttrName = pAttribute->m_strAttrName;
			pNewEntityAttribute->m_vGenericNum.push_back(pAttribute->m_vGenericNum[0]);

			//object id
			if(pNewEntityAttribute->m_strAttrName == "id")
			{
				if(_bBaseCase)
					_pClientObject->m_pdID_ = &pNewEntityAttribute->m_vGenericNum[0];

				_pToElement->m_pdElementID = &pNewEntityAttribute->m_vGenericNum[0];
			}

			pNewEntityAttribute->m_pClientElement = _pToElement;
			_pToElement->m_vecAttribute.push_back(pNewEntityAttribute);

			//uint 에 대한 예외복사
			_pClientObject->UpdateSpecialUINT_(pNewEntityAttribute, _bBaseCase);
		}
		else if(pAttribute->m_eAttrType == SAttributeAtXML::E_FLOAT)
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_FLOAT;
			pNewEntityAttribute->m_strAttrName = pAttribute->m_strAttrName;
			pNewEntityAttribute->m_vGenericNum.push_back(pAttribute->m_vGenericNum[0]);

			pNewEntityAttribute->m_pClientElement = _pToElement;
			_pToElement->m_vecAttribute.push_back(pNewEntityAttribute);

			// float에 대한 예외복사
			_pClientObject->UpdateSpecialFLOAT_(pNewEntityAttribute, _bBaseCase);
		}
		else if(pAttribute->m_eAttrType == SAttributeAtXML::E_STRING)
		{
			SAttributeAtXML* pNewEntityAttribute = new SAttributeAtXML;
			pNewEntityAttribute->m_eAttrType = SAttributeAtXML::E_STRING;
			pNewEntityAttribute->m_strAttrName = pAttribute->m_strAttrName;
			pNewEntityAttribute->m_strAttrTypeExtra = pAttribute->m_strAttrTypeExtra;
			pNewEntityAttribute->m_strGenericString = pAttribute->m_strGenericString;

			//object name
			if( _bBaseCase && pNewEntityAttribute->m_strAttrName == "name" )
				_pClientObject->m_pStrLogicObjectName_ = &pNewEntityAttribute->m_strGenericString;

			pNewEntityAttribute->m_pClientElement = _pToElement;
			_pToElement->m_vecAttribute.push_back(pNewEntityAttribute);

			//string 예외 복사
			_pClientObject->UpdateSpecialSTRING_(pNewEntityAttribute, _bBaseCase);
		}
	}

	for(SElementAtXML::ElementList::iterator itr = _pFromElement->m_vecChildElement.begin();
		itr != _pFromElement->m_vecChildElement.end(); ++itr)
	{
		SElementAtXML* pEntityElement = new SElementAtXML;
		pEntityElement->m_strNodeName = (*itr)->m_strNodeName;

		_pToElement->m_vecChildElement.push_back(pEntityElement);
		pEntityElement->m_pParentElement = _pToElement;

		CopyElement(_pClientObject, *itr, pEntityElement, false);
	}
}

void CLogicObject::SaveElement_(MXmlElement* _pXmlElement, SElementAtXML* _pLogicObjectElement)
{
	for(SElementAtXML::AttributeList::iterator itr = _pLogicObjectElement->m_vecAttribute.begin();
		itr != _pLogicObjectElement->m_vecAttribute.end(); ++itr)
	{
		SAttributeAtXML* pAttribute = *itr;
		if(pAttribute->m_eAttrType == SAttributeAtXML::E_POS)
		{
			_SetAttribute(_pXmlElement, "x", (float)pAttribute->m_vGenericNum[0]);
			_SetAttribute(_pXmlElement, "y", (float)pAttribute->m_vGenericNum[1]);
			_SetAttribute(_pXmlElement, "z", (float)pAttribute->m_vGenericNum[2]);
		}
		else if(pAttribute->m_eAttrType == SAttributeAtXML::E_DIR)
		{
			_SetAttribute(_pXmlElement, "dir_x", (float)pAttribute->m_vGenericNum[0]);
			_SetAttribute(_pXmlElement, "dir_y", (float)pAttribute->m_vGenericNum[1]);
			_SetAttribute(_pXmlElement, "dir_z", (float)pAttribute->m_vGenericNum[2]);
		}
		else if(pAttribute->m_eAttrType == SAttributeAtXML::E_UP)
		{
			_SetAttribute(_pXmlElement, "up_x", (float)pAttribute->m_vGenericNum[0]);
			_SetAttribute(_pXmlElement, "up_y", (float)pAttribute->m_vGenericNum[1]);
			_SetAttribute(_pXmlElement, "up_z", (float)pAttribute->m_vGenericNum[2]);
		}
		else if(pAttribute->m_eAttrType == SAttributeAtXML::E_RADIUS)
		{
			float radius = pAttribute->m_vGenericNum[0];
			if(radius >= 0)
				_SetAttribute(_pXmlElement, "radius", (float)pAttribute->m_vGenericNum[0]);
		}
		else if(pAttribute->m_eAttrType == SAttributeAtXML::E_STRING)
		{
			if(!pAttribute->m_strGenericString.empty())
			{
				// Type Extra가 설정되어 있고 none 이면 기록하지 않는다.
				if (!pAttribute->m_strAttrTypeExtra.empty())
				{
					if (pAttribute->m_strGenericString!="none")
					{
						_SetAttribute(_pXmlElement, pAttribute->m_strAttrName.c_str(), pAttribute->m_strGenericString);
					}
				}
				else
				{
					_SetAttribute(_pXmlElement, pAttribute->m_strAttrName.c_str(), pAttribute->m_strGenericString);
				}
			}
		}
		else if(pAttribute->m_eAttrType == SAttributeAtXML::E_BOOL)
		{
			float fBoolValue = pAttribute->m_vGenericNum[0];
			if(fBoolValue != -1.f)
				_SetAttribute(_pXmlElement, pAttribute->m_strAttrName.c_str(), SAttributeAtXML::GetAttributeBool(fBoolValue) );
		}
		else if(pAttribute->m_eAttrType == SAttributeAtXML::E_UINT)
		{
			int u_int = (int)pAttribute->m_vGenericNum[0];

			// -1, -2등 0보다 작은값은 특별한 심볼
			if(u_int >= 0)
				_SetAttribute(_pXmlElement, pAttribute->m_strAttrName.c_str(), u_int);
		}
		else if(pAttribute->m_eAttrType == SAttributeAtXML::E_FLOAT)
		{
			_SetAttribute(_pXmlElement, pAttribute->m_strAttrName.c_str(), (float)pAttribute->m_vGenericNum[0]);
		}
	}

	for(SElementAtXML::ElementList::iterator itr = _pLogicObjectElement->m_vecChildElement.begin();
		itr != _pLogicObjectElement->m_vecChildElement.end(); ++itr)
	{
		SElementAtXML* pLogicObjectElement = *itr;
		if (!pLogicObjectElement->m_vecAttribute.empty())
		{
			SAttributeAtXML* pLogicObjectAttribute = pLogicObjectElement->m_vecAttribute[0];
			if (pLogicObjectAttribute->m_eAttrType == SAttributeAtXML::E_UINT &&
				pLogicObjectAttribute->m_vGenericNum[0] == SAttributeAtXML::SYM_NONE )
				continue;

			if (pLogicObjectAttribute->m_eAttrType == SAttributeAtXML::E_STRING &&
				pLogicObjectAttribute->m_strGenericString == "none" )
				continue;
		}

		MXmlElement* pXmlElement = new MXmlElement( pLogicObjectElement->m_strNodeName );
		_pXmlElement->LinkEndChild(pXmlElement);
		SaveElement_(pXmlElement, pLogicObjectElement);
	}
}

bool CLogicObject::UpdateSelectionInfo(CLogicObject* _pLogicObjectToAdd, CTreeCtrl& _refTreeCtrl, EntityAttributeMap& _refEntityAttrMap)
{
	HTREEITEM hRoot = _refTreeCtrl.GetRootItem();
	ASSERT(_pLogicObjectToAdd);

	SElementAtXML* pLogicElement = _pLogicObjectToAdd->m_pLogicObjectElement_;
	if(pLogicElement == NULL) return false;

	if(hRoot == NULL)
		hRoot = _refTreeCtrl.InsertItem( pLogicElement->m_strNodeName.c_str(), TVI_ROOT );
	else
	{
		std::string strRootName( _refTreeCtrl.GetItemText(hRoot).GetString() );
		int find_token = strRootName.find(" :");
		std::string strPureRootName( strRootName.substr(0, find_token) );

		//다른종류라면
		if( pLogicElement->m_strNodeName != strPureRootName )
			return false;

		_refTreeCtrl.SetItemText(hRoot, (strPureRootName + " : 다중선택").c_str() );
	}

	UpdateSelectionInfoRecursion_(pLogicElement, _refTreeCtrl, hRoot, _refEntityAttrMap);
	return true;
}

void CLogicObject::UpdateSelectionInfoRecursion_(SElementAtXML* _pLogicObjectElement, CTreeCtrl& _refTreeCtrl, HTREEITEM hItem , EntityAttributeMap& _refEntityAttrMap)
{
	std::vector<HTREEITEM> vecCheckedItem;
	for(SElementAtXML::AttributeList::iterator itr = _pLogicObjectElement->m_vecAttribute.begin();
		itr != _pLogicObjectElement->m_vecAttribute.end(); ++itr)
	{
		HTREEITEM hParent = _refTreeCtrl.GetNextItem( hItem, TVGN_CHILD );
		//모든 같은 위상의 CTreeCtrl의 노드들을 찾아서
		bool bExistAttribute = false;
		while(hParent != NULL)
		{
			std::vector<HTREEITEM>::iterator itr_checked = std::find( vecCheckedItem.begin(), vecCheckedItem.end(), hParent);
			if(itr_checked != vecCheckedItem.end())
			{
				hParent = _refTreeCtrl.GetNextSiblingItem(hParent);
				continue;
			}
			else
			{
				EntityAttributeMap::iterator itr_map = _refEntityAttrMap.find( hParent );
				//예전의 것이 존재하는지 판단
				if( itr_map != _refEntityAttrMap.end() )
				{
					SAttributeAtXML* pAttribute = *itr;
					SAttributeAtXML* pInEntityAttrMapAttribute = _refEntityAttrMap[hParent][0];

					//여전의 것이 존재한다면
					if(pAttribute->m_strAttrName == pInEntityAttrMapAttribute->m_strAttrName)
					{
						pAttribute->UpdateMFCTreeItem(_refTreeCtrl, hParent, pInEntityAttrMapAttribute);
						_refEntityAttrMap[hParent].push_back( pAttribute );
						bExistAttribute = true;
						break;
					}
				}
				hParent = _refTreeCtrl.GetNextSiblingItem(hParent);
			}
		}

		if(bExistAttribute)
		{
			////test
			//mlog("테스트 시작, %d\n", hParent);
			//for(SElementAtXML::AttributeList::iterator it_test = _refEntityAttrMap[hParent].begin();
			//	it_test != _refEntityAttrMap[hParent].end(); ++it_test)
			//{
			//	mlog("%s\n", (*it_test)->m_strAttrName.c_str());
			//}
			//mlog("테스트 끝, %d\n", hParent);
			vecCheckedItem.push_back(hParent);
		}
		else
		{
			HTREEITEM hAddedItem = (*itr)->UpdateMFCTreeItem(_refTreeCtrl, hItem, false);
			//_refTreeCtrl.InsertItem( (*itr)->m_strAttrName.c_str(), hRoot, TVI_LAST);
			if(hAddedItem != NULL)
			{
				_refEntityAttrMap.insert( EntityAttributeMap::value_type(hAddedItem, SElementAtXML::AttributeList()) );
				_refEntityAttrMap[hAddedItem].push_back( *itr );
				vecCheckedItem.push_back(hAddedItem);
			}
		}
	}

	//expend tree item
	_refTreeCtrl.Expand(hItem, TVE_EXPAND);

	for(SElementAtXML::ElementList::iterator itr = _pLogicObjectElement->m_vecChildElement.begin();
		itr != _pLogicObjectElement->m_vecChildElement.end(); ++itr)
	{
		SElementAtXML* pEntityElement = *itr;
		HTREEITEM element_node_item = _refTreeCtrl.GetChildItem(hItem);

		while(element_node_item != NULL)
		{
			std::string strNodeName( _refTreeCtrl.GetItemText(element_node_item).GetString() );
			int find_token = strNodeName.find(" (");
			std::string strNodeNameToWrite( strNodeName.substr(0, find_token) );

			//같은 이름의 노드가 있을경우
			if( pEntityElement->m_strNodeName == strNodeNameToWrite )
			{
				//NPC일 경우, NPC element의 ID attribute가 같은 경우 합치고 다르면 새로 생성
				if( strNodeNameToWrite == "NPC")
				{
					ASSERT(pEntityElement->m_pdElementID);
					double npc_id_1 = *(pEntityElement->m_pdElementID);
					HTREEITEM id_node_item = _refTreeCtrl.GetChildItem(element_node_item);

					//노드상에 들어있는 npc들은 npc attribute의 id가 같은것들이기 때문에
					//npc id가 같은지 판단하기 위해서는 맨앞의 하나의 같만 비교하면 된다.

					SElementAtXML::AttributeList::iterator itr_attr = _refEntityAttrMap[id_node_item].begin();
					double npc_id_2 = (*itr_attr)->m_vGenericNum[0];

					//같으면 합치기 위해 멈춤
					if(npc_id_1 == npc_id_2)
					{
						//노드에는 이 NPC들을 가지는 부모들의 id를 써준다.
						char szBuffer[256];
						szBuffer[0] = '\0';

						//기존의 npc부모노드(스폰)의 id
						ASSERT( (*itr_attr)->m_pClientElement );
						sprintf(szBuffer," (spawn id : %d", (int)( *(*itr_attr)->m_pClientElement->m_pParentElement->m_pdElementID) );
						strNodeNameToWrite = strNodeNameToWrite + szBuffer;

						SElementAtXML::AttributeList::iterator itrAttrMapEnd = _refEntityAttrMap[id_node_item].end();
						for(itr_attr++; itr_attr != itrAttrMapEnd; ++itr_attr)
						{
							sprintf(szBuffer,", %d", (int)( *(*itr_attr)->m_pClientElement->m_pParentElement->m_pdElementID));
							strNodeNameToWrite = strNodeNameToWrite + szBuffer;
						}

						//새로운 스폰의 id
						sprintf(szBuffer,", %d)", (int)( *pEntityElement->m_pParentElement->m_pdElementID));
						strNodeNameToWrite = strNodeNameToWrite + szBuffer;
						_refTreeCtrl.SetItemText(element_node_item, strNodeNameToWrite.c_str());
						break;
					}
				}
			}
			element_node_item = _refTreeCtrl.GetNextSiblingItem(element_node_item);
		}

		if(element_node_item == NULL)
		{
			std::string strNodeName(pEntityElement->m_strNodeName);
			if(strNodeName == "NPC")
			{
				char szBuffer[256];
				szBuffer[0] = '\0';
				sprintf(szBuffer," (spawn id : %d)", (int)( *pEntityElement->m_pParentElement->m_pdElementID));
				strNodeName = strNodeName + szBuffer;
			}
			element_node_item = _refTreeCtrl.InsertItem( strNodeName.c_str(), hItem, TVI_LAST);
		}

		UpdateSelectionInfoRecursion_( pEntityElement, _refTreeCtrl, element_node_item, _refEntityAttrMap);
	}
}

// 기본 LogicObject 삭제 언두 커맨드
ICommandBuffer*	CLogicObject::CreateDeleteCmdBuffer()
{ 
	return new CLogicDeleteCmdBuffer(this); 
}		
