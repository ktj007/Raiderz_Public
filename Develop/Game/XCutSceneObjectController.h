#pragma once

#include <string>
#include <algorithm>

#include "MXml.h"
#include "MVector3.h"
#include "MUID.h"

#include "XCutSceneKeyFrameUpdator.h"

#include "MTstring.h"
#include "MEvent.h"

using namespace std;

class XCutSceneObject;

//////////////////////////////////////////////////////////////////////////

class XCutSceneObjectController
{
public:
	enum TYPE
	{
		// CutSceneObject 공용
		E_MOVE = 0,
		E_ANIMATION,
		E_TALENT,

		// 카메라 전용
		E_DOF,
		E_FOLLOW_ACTOR,

		// Event 전용
		E_BGM,
		E_CAPTION,
		E_LETTERBOX,
		E_ENV,
		E_EFFECT,
		E_FADE,
		E_KEYEVENT,

		E_TYPE_NUM
	};


						XCutSceneObjectController(XCutSceneObject* pOwner, TYPE _eType);
	virtual				~XCutSceneObjectController(){}

	virtual bool		ParseFromElement(MXmlElement* pControllerElement) = 0;
	virtual void		SaveToXML( MXmlElement* pObjectElement ) = 0;

	virtual void		Update(float fElapsed) = 0;
	virtual void		ResetUpdate() = 0;
	virtual bool		OnEvent(mint3::MEvent& e) { return false; }

	virtual int			GetKeyCount(){ return 0; }

	TYPE				GetType(){ return m_eType; }
	void				GetRange(float& _f1, float& _f2);

protected:
	XCutSceneObject*	m_pOwner;

	float				m_fStartTime;
	float				m_fEndTime;

private:
	TYPE				m_eType;
};