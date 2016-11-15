#include "stdafx.h"

#include "MDebug.h"
#include "MLocale.h"

#include "XCutSceneContent.h"
#include "XCutSceneObject.h"

//////////////////////////////////////////////////////////////////////////

bool XCutSceneClapper::ParseFromElement( MXmlElement* pClapperElement )
{
	string strCutSceneDesc;
	if (!_Attribute(strCutSceneDesc, pClapperElement, "desc"))
		return false;

#ifdef _UNICODE
	m_strCutSceneDesc = MLocale::ConvUTF8ToUTF16(strCutSceneDesc.c_str());
#else
	m_strCutSceneDesc = MLocale::ConvUTF8ToAnsi(strCutSceneDesc.c_str());
#endif


	if (!_Attribute(m_nField, pClapperElement, "field"))
		return false;

	if (!_Attribute(m_fDuration, pClapperElement, "duration"))
		return false;

	
	return true;
}

std::tstring XCutSceneClapper::CreateXMLString( const TCHAR* _pSzName, int _nField, float _fDuration )
{
	TCHAR buf[8196];
	_stprintf_s( buf, _T("<clapper desc=\"%s\" field=\"%d\" duration=\"%.3f\" in=\"none\" out=\"none\" />"), _pSzName, _nField, _fDuration );
	return std::tstring(buf);
}

//////////////////////////////////////////////////////////////////////////

XCutSceneContent::~XCutSceneContent()
{
	// reset
	ResetRunning();

	// delete object
	delete m_pClapper;
	for (VEC_CUTSCNEOBJ::iterator itr = m_vCutSceneObjects.begin(); itr != m_vCutSceneObjects.end(); ++itr)
	{
		delete *itr;
	}
}

XCutSceneObject* XCutSceneContent::FindCutSceneObject( const std::tstring& _strName )
{
	XCutSceneObject* pObject = NULL;
	for (VEC_CUTSCNEOBJ::iterator itr = m_vCutSceneObjects.begin(); itr != m_vCutSceneObjects.end(); ++itr)
	{
		XCutSceneObject* pTemp = *itr;
		if (pTemp->GetName() == _strName)
		{
			pObject = pTemp;
			break;
		}
	}

	return pObject;
}

void XCutSceneContent::Run( float fElapsed )
{
	// 전체 사운드 싱크등의 문제로 초기 0.f 이벤트는 반드시 맞춘다.
	if (m_fCurrentRunningTime == 0.f)
	{
		for (VEC_CUTSCNEOBJ::iterator itr = m_vCutSceneObjects.begin(); itr != m_vCutSceneObjects.end(); ++itr)
		{
			(*itr)->Update(0.f);
		}
	}

	for (VEC_CUTSCNEOBJ::iterator itr = m_vCutSceneObjects.begin(); itr != m_vCutSceneObjects.end(); ++itr)
	{
		(*itr)->Update(fElapsed);
	}

	m_fCurrentRunningTime += fElapsed;
	return;
}

void XCutSceneContent::ResetRunning()
{
	for (VEC_CUTSCNEOBJ::iterator itr = m_vCutSceneObjects.begin(); itr != m_vCutSceneObjects.end(); ++itr)
	{
		(*itr)->ResetUpdate();
	}

	m_fCurrentRunningTime = 0.f;
}

bool XCutSceneContent::OnEvent(mint3::MEvent& e)
{
	for (VEC_CUTSCNEOBJ::iterator itr = m_vCutSceneObjects.begin(); itr != m_vCutSceneObjects.end(); ++itr)
	{
		if((*itr)->OnEvent(e))
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

bool XCutSceneContentBuilder::Build( const TCHAR* _pSzFileName, XCutSceneContent* _pOutCutSceneContents )
{
	MXml aXml;
	if (!aXml.LoadFile(MLocale::ConvTCHARToAnsi(_pSzFileName).c_str()))
		return false;

	MXmlElement* pRootElement = aXml.DocHandle().FirstChildElement("maiet").Element();
	return Build(pRootElement, _pOutCutSceneContents);
}

bool XCutSceneContentBuilder::Build( MXmlElement* pRootElement, XCutSceneContent* _pOutCutSceneContents )
{
	if (!pRootElement)
		return false;

	// clapper
	MXmlElement* pClapperElement = pRootElement->FirstChildElement("clapper");
	if (!pClapperElement)
		return false;

	XCutSceneClapper* pClapper = new XCutSceneClapper;
	if (!pClapper->ParseFromElement(pClapperElement) || !OnClapperCreated(pClapper) )
	{
		delete pClapper;
		return false;
	}
	_pOutCutSceneContents->AddClapper(pClapper);

	// camera
	MXmlElement* pCameraElement = pRootElement->FirstChildElement( MLocale::ConvTCHARToAnsi(XCutSceneObject::GetTypeStr(XCutSceneObject::E_CAMERA)));
	if (!pCameraElement)
		return false;

	XCutSceneObjectCamera* pCameraObject = new XCutSceneObjectCamera(OnCreateCameraEntity(), pClapper);
	if (!pCameraObject->ParseFromElement(pCameraElement))
	{
		delete pCameraObject;
		return false;
	}
	_pOutCutSceneContents->AddCutSceneObject(pCameraObject);

	// event object
	MXmlElement* pEventElement = pRootElement->FirstChildElement( MLocale::ConvTCHARToAnsi(XCutSceneObject::GetTypeStr(XCutSceneObject::E_EVENT)));
	if (pEventElement)
	{
		XCutSceneObjectEvent* pEventObject = new XCutSceneObjectEvent( OnCreateEventEntity(pEventElement), pClapper );

		// CutSceneObjectKeyEvent 클래스 생성(CutScene이 생성되는 시점) - Skip 기능 
		// 게임에서 기본 기능으로 제공을 위해서 xml로 하지 않고 바로 생성을 합니다.
		pEventObject->AddCutSceneSkipObjectControllerKeyEvent();

		if (!pEventObject->ParseFromElement(pEventElement))
		{
			mlog("cutscene xml error : event object is invalid\n");
		}
		_pOutCutSceneContents->AddCutSceneObject(pEventObject);
	}

	// actor
	MXmlElement* pActorElement = pRootElement->FirstChildElement( MLocale::ConvTCHARToAnsi(XCutSceneObject::GetTypeStr(XCutSceneObject::E_ACTOR)));
	while (pActorElement)
	{
		std::tstring strType;
		_Attribute(strType, pActorElement, "type");
		XCutSceneObjectActor* pActorObject = new XCutSceneObjectActor( OnCreateActorEntity(strType), pClapper );
		pActorObject->SetType(strType);

		std::string strName;
		_Attribute(strName, pActorElement, "name");		

#ifdef _UNICODE
		pActorObject->SetName(MLocale::ConvUTF8ToUTF16(strName.c_str()) );
#else
		pActorObject->SetName(MLocale::ConvUTF8ToAnsi(strName.c_str()) );
#endif

		

		if (!pActorObject->ParseFromElement(pActorElement))
		{
			mlog("cutscene xml error : actor %s is invalid\n", MLocale::ConvTCHARToAnsi(pActorObject->GetName().c_str()).c_str());
		}

		_pOutCutSceneContents->AddCutSceneObject(pActorObject);
		pActorElement = pActorElement->NextSiblingElement( MLocale::ConvTCHARToAnsi(XCutSceneObject::GetTypeStr(XCutSceneObject::E_ACTOR)));
	}

	return true;
}

void XCutSceneContent::SetCutSceneSkip(bool bCutSceneSkip)
{
	m_bCutSceneSkip = true;
}

bool XCutSceneContent::IsCutSceneSkip()
{
	return m_bCutSceneSkip;
}
