#include "stdafx.h"
#include "XCutSceneObject.h"
#include "XCutSceneObjectController.h"
#include "XCutSceneObjectControllerEnv.h"
#include "XCutSceneObjectControllerLetterBox.h"
#include "XCutSceneObjectControllerCaption.h"
#include "XCutSceneObjectControllerBGM.h"
#include "XCutSceneObjectControllerAnimation.h"
#include "XCutSceneObjectControllerTalent.h"
#include "XCutSceneObjectControllerDOF.h"
#include "XCutSceneObjectControllerMove.h"
#include "XCutSceneObjectControllerFade.h"
#include "XCutSceneObjectControllerKeyEvent.h"

#include "MLocale.h"

#include <string>
#include <map>

//////////////////////////////////////////////////////////////////////////

XCutSceneObjectController::TYPE GetTypeFromString( const TCHAR* _pSzName )
{
	static bool bInit  = false;

	typedef std::map< std::tstring, XCutSceneObjectController::TYPE > MAP_CTRL_TYPE;
	static MAP_CTRL_TYPE mapStringObjectController;

	if (!bInit)
	{
		mapStringObjectController.insert( MAP_CTRL_TYPE::value_type(_T("move"), XCutSceneObjectController::E_MOVE) );
		mapStringObjectController.insert( MAP_CTRL_TYPE::value_type(_T("dof"), XCutSceneObjectController::E_DOF) );
		mapStringObjectController.insert( MAP_CTRL_TYPE::value_type(_T("ani"), XCutSceneObjectController::E_ANIMATION) );
		mapStringObjectController.insert( MAP_CTRL_TYPE::value_type(_T("talent"), XCutSceneObjectController::E_TALENT) );
		bInit = true;
	}

	MAP_CTRL_TYPE::iterator itr = mapStringObjectController.find(_pSzName);
	if (itr == mapStringObjectController.end())
	{
		return XCutSceneObjectController::E_TYPE_NUM;
	}

	return itr->second;
}

XCutSceneObjectController* CreateCommonController(XCutSceneObject* pOwner, XCutSceneObjectController::TYPE _eType, MXmlElement* _pElement)
{
	XCutSceneObjectController* pController = NULL;
	switch (_eType)
	{
	case XCutSceneObjectController::E_MOVE:
		{
			XCutSceneObjectControllerMove* pMoveController = new XCutSceneObjectControllerMove(pOwner);
			if (pOwner->GetType() == XCutSceneObject::E_CAMERA)
			{
				pMoveController->SetUsingUp(true);
			}
			pController = pMoveController;
			pController->ParseFromElement(_pElement);
		}
		break;
	case XCutSceneObjectController::E_ANIMATION:
		{
			pController = new XCutSceneObjectControllerAnimation(pOwner);
			pController->ParseFromElement(_pElement);
		}
		break;
	case XCutSceneObjectController::E_TALENT:
		{
			pController = new XCutSceneObjectControllerTalent(pOwner);
			pController->ParseFromElement(_pElement);
		}
		break;
	}

	return pController;
}

XCutSceneObjectController* CreateCameraController(XCutSceneObject* pOwner, MXmlElement* _pElement)
{
	XCutSceneObjectController::TYPE eType = GetTypeFromString( MLocale::ConvAnsiToTCHAR(_pElement->Value()).c_str());
	XCutSceneObjectController* pController = CreateCommonController(pOwner, eType, _pElement);
	if(NULL == pController)
	{
		if (XCutSceneObjectController::E_DOF == eType)
		{
			pController = new XCutSceneObjectControllerDOF(pOwner);
			pController->ParseFromElement(_pElement);
		}
	}

	return pController;
}

XCutSceneObjectController* CreateActorController(XCutSceneObject* pOwner, MXmlElement* _pElement)
{
	XCutSceneObjectController::TYPE eType = GetTypeFromString( MLocale::ConvAnsiToTCHAR(_pElement->Value()).c_str());
	XCutSceneObjectController* pController = CreateCommonController(pOwner, eType, _pElement);
	if(NULL == pController)
	{
	}

	return pController;
}

XCutSceneObjectController* CreateEventController(XCutSceneObject* pOwner, MXmlElement* _pElement)
{
	std::tstring strElementValue = MLocale::ConvAnsiToTCHAR(_pElement->Value());
	if (strElementValue == _T("bgm"))
	{
		int nUid = -1;
		XCutSceneObjectControllerBGM* pBgmController = new XCutSceneObjectControllerBGM(pOwner);
		_Attribute(nUid, _pElement, "uid");
		pBgmController->SetUID(nUid);

		pBgmController->ParseFromElement(_pElement);
		return pBgmController;
	}
	else if(strElementValue == _T("caption"))
	{
		XCutSceneObjectControllerCaption* pCaptionController = new XCutSceneObjectControllerCaption(pOwner);

		string strFontName;
		_Attribute(strFontName, _pElement, "font");
		
#ifdef _UNICODE
		pCaptionController->SetFontName(MLocale::ConvUTF8ToUTF16(strFontName.c_str()));
#else
		pCaptionController->SetFontName(MLocale::ConvUTF8ToAnsi(strFontName.c_str()));
#endif		

		int nSize = 15;
		bool bBold = false;
		bool bItalic = false;
		_Attribute(nSize, _pElement, "size", 12);
		_Attribute(bBold, _pElement, "bold", false);
		_Attribute(bItalic, _pElement, "italic", false);

		pCaptionController->SetSize(nSize);
		pCaptionController->SetBold(bBold);
		pCaptionController->SetItalic(bItalic);

		pCaptionController->ParseFromElement(_pElement);
		return pCaptionController;
	}
	else if(strElementValue == _T("letterbox"))
	{
		XCutSceneObjectControllerLetterBox* pLetterBoxController = new XCutSceneObjectControllerLetterBox(pOwner);
	
		pLetterBoxController->ParseFromElement(_pElement);
		return pLetterBoxController;
	}
	else if (strElementValue == _T("env"))
	{
		XCutSceneObjectControllerEnv* pEnvController = new XCutSceneObjectControllerEnv(pOwner);

		pEnvController->ParseFromElement(_pElement);
		return pEnvController;
	}
	else if(strElementValue == _T("effect"))
	{
		XCutSceneObjectControllerEffect* pEffectController = new XCutSceneObjectControllerEffect(pOwner);

		pEffectController->ParseFromElement(_pElement);
		return pEffectController;
	}
	else if(strElementValue == _T("fade"))
	{
		XCutSceneObjectControllerFade* pFadeController = new XCutSceneObjectControllerFade(pOwner);

		pFadeController->ParseFromElement(_pElement);
		return pFadeController;
	}
	else if(strElementValue == _T("keyevent"))
	{
		XCutSceneObjectControllerKeyEvent* pKeyEventController = new XCutSceneObjectControllerKeyEvent(pOwner);
	
		//pLetterBoxController->ParseFromElement(_pElement);
		return pKeyEventController;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

XCutSceneObject::~XCutSceneObject()
{
	delete m_pCutSceneObjectEntity;
	for (VEC_CUTSCENE_CONTROLLERS::iterator itr = m_vecCutSceneControllers.begin(); itr != m_vecCutSceneControllers.end(); ++itr)
	{
		delete *itr;
	}
}

void XCutSceneObject::AddController( XCutSceneObjectController* _pNewController )
{
	if (!_pNewController)
		return;

	m_vecCutSceneControllers.push_back(_pNewController);
}

XCutSceneObject::XCutSceneObject( XCutSceneObjectEntity* pEntity, XCutSceneClapper* pClapper, TYPE _eType )
: m_pCutSceneObjectEntity(pEntity), m_pCutSceneClapper(pClapper), m_eType(_eType)
{

}

void XCutSceneObject::RemoveController( int nControllerId )
{
	XCutSceneObjectController* pController = GetController(nControllerId);
	delete pController;
	GetControllers().erase( GetControllers().begin() + nControllerId );
}

void XCutSceneObject::Update( float fElapsed )
{
	for (VEC_CUTSCENE_CONTROLLERS::iterator itr = m_vecCutSceneControllers.begin(); itr != m_vecCutSceneControllers.end(); ++itr)
	{
		(*itr)->Update(fElapsed);
	}

	return;
}

void XCutSceneObject::ResetUpdate()
{
	for (VEC_CUTSCENE_CONTROLLERS::iterator itr = m_vecCutSceneControllers.begin(); itr != m_vecCutSceneControllers.end(); ++itr)
	{
		(*itr)->ResetUpdate();
	}

	if (m_pCutSceneObjectEntity)
		m_pCutSceneObjectEntity->ResetUpdate();
}


bool XCutSceneObject::OnEvent(mint3::MEvent& e)
{
	for (VEC_CUTSCENE_CONTROLLERS::iterator itr = m_vecCutSceneControllers.begin(); itr != m_vecCutSceneControllers.end(); ++itr)
	{
		if((*itr)->OnEvent(e))
			return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////

XCutSceneObjectCamera::XCutSceneObjectCamera( XCutSceneObjectEntity* pEntity, XCutSceneClapper* pClapper )
: XCutSceneObject(pEntity, pClapper, E_CAMERA)
{
	m_strName = _T("camera");
}

bool XCutSceneObjectCamera::ParseFromElement( MXmlElement* pObjectElement )
{
	MXmlElement* pChildElement = pObjectElement->FirstChildElement();
	while (pChildElement)
	{
		AddController(CreateCameraController(this, pChildElement));
		pChildElement = pChildElement->NextSiblingElement();
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

XCutSceneObjectActor::XCutSceneObjectActor( XCutSceneObjectEntity* pEntity, XCutSceneClapper* pClapper )
: XCutSceneObject(pEntity, pClapper, E_ACTOR)
{

}

bool XCutSceneObjectActor::ParseFromElement( MXmlElement* pObjectElement )
{
	MXmlElement* pChildElement = pObjectElement->FirstChildElement();
	while (pChildElement)
	{
		AddController(CreateActorController(this, pChildElement));
		pChildElement = pChildElement->NextSiblingElement();
	}
	return true;
}

void XCutSceneObjectActor::AddAttributeOnSave( MXmlElement* pObjectElement )
{
#ifdef _UNICODE
	std::string strName = MLocale::ConvUTF16ToUTF8(m_strName.c_str());	
#else
	std::string strName = MLocale::ConvAnsiToUTF8(m_strName.c_str());	
#endif
	
	
	_SetAttribute(pObjectElement, "name", strName);
	_SetAttribute(pObjectElement, "type", MLocale::ConvTCHARToAnsi(m_strType.c_str()).c_str());
}

int XCutSceneObjectActor::GetActorIdFromEntityInfo( const std::tstring& _rFromEntityCreationId )
{
	if (_rFromEntityCreationId.find(_T("npc_")) != std::tstring::npos)
	{
		// npc_ : 4
		return _ttoi( _rFromEntityCreationId.substr(4).c_str() );
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////////

XCutSceneObjectEvent::XCutSceneObjectEvent( XCutSceneObjectEntity* pEntity, XCutSceneClapper* pClapper )
: XCutSceneObject(pEntity, pClapper, E_EVENT)
{
	m_strName = _T("event");
}

bool XCutSceneObjectEvent::ParseFromElement( MXmlElement* pObjectElement )
{
	MXmlElement* pChildElement = pObjectElement->FirstChildElement();
	while (pChildElement)
	{
		AddController(CreateEventController(this, pChildElement));
		pChildElement = pChildElement->NextSiblingElement();
	}
	return true;
}

void XCutSceneObjectEvent::AddCutSceneSkipObjectControllerKeyEvent()
{
	XCutSceneObjectControllerKeyEvent* pKeyEventController = new XCutSceneObjectControllerKeyEvent(this);
	AddController(pKeyEventController);
}

