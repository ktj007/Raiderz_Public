#pragma once

#include "MTypes.h"
#include "MXml.h"
#include <map>
#include "XCutSceneObjectController.h"
#include "XCutSceneObjectControllerEffect.h"

class XCutSceneObjectController;
typedef std::vector< XCutSceneObjectController* > VEC_CUTSCENE_CONTROLLERS;
class XCutSceneClapper;

namespace rs3
{
	class RCameraSceneNode;
}

//////////////////////////////////////////////////////////////////////////

class XCutSceneObjectEntity
{
public:
									XCutSceneObjectEntity(){}
	virtual							~XCutSceneObjectEntity(){}

	virtual void					SetTransform(const MMatrix& mat, bool bUsingCollision) = 0;
	virtual rs3::RCameraSceneNode*	GetRelatedCamera() = 0;

	virtual void					SetAnimation(const std::tstring& _rAnimationName){}
	virtual void					OnUseTalent(const int nTalent){}
	virtual void					PlaySound(const TCHAR* _pSzSoundName){}
	virtual void					ShowCaption(const TCHAR* chCaption, const TCHAR* chFontName, int nSize, bool bBold = false, bool bItalic = false) {}
	virtual void					HideCaption() {}
	virtual void					ShowKeyEvent(const TCHAR* chKeyEvent, const float fPosX, const float fPosY,  const TCHAR* chFontName, int nSize, bool bBold = false, bool bItalic = false) {}
	virtual void					HideKeyEvent() {}
	virtual void					SetLetterBox(bool bShow, float fRatioW, float fRatioH) {}
	virtual void					SetEnvironment(int nEnvTime, int nEnvWeather, const std::tstring& _rEnvSetName, bool bFading){}
	virtual void					CreateEffect(tstring& strEffectName, tstring& strEffectInfo, vec3& pos, vec3& dir, vec3& up, vector<stControllerEffectData>* pResultData) {}
	virtual void					DestroyEffect(MUID uidEffect) {}

	virtual void					FadeUpdate( float fFadingAlpha )	{}
	virtual void					FadeEnd()	{}

	virtual void					ResetUpdate(){}

	virtual void					CutSceneSkip() {}
};

//////////////////////////////////////////////////////////////////////////

class XCutSceneObject
{
public:

	// type
	enum TYPE
	{
		E_CAMERA = 0,
		E_ACTOR,
		E_EVENT
	};
	static const TCHAR* GetTypeStr(TYPE _eType)
	{
		static const TCHAR* pSzTypes[] = 
		{
			_T("camera"),
			_T("actor"),
			_T("event")
		};

		return pSzTypes[_eType];
	}


									XCutSceneObject(XCutSceneObjectEntity* pEntity, XCutSceneClapper* pClapper, TYPE _eType);
	virtual							~XCutSceneObject();

	virtual bool					ParseFromElement(MXmlElement* pObjectElement) = 0;
	virtual void					AddAttributeOnSave(MXmlElement* pObjectElement) = 0;
	virtual bool					OnEvent(mint3::MEvent& e);

	TYPE							GetType(){ return m_eType; }
	const TCHAR*					GetTypeStr(){ return GetTypeStr(m_eType); }
	XCutSceneClapper*				GetClapper(){ return m_pCutSceneClapper; }
	XCutSceneObjectEntity*			GetEntity()	{ return m_pCutSceneObjectEntity; }

	VEC_CUTSCENE_CONTROLLERS&		GetControllers(){ return m_vecCutSceneControllers; }
	void							AddController(XCutSceneObjectController* _pNewController);
	XCutSceneObjectController*		GetController(int nControllerId){ return m_vecCutSceneControllers[nControllerId]; }
	void							RemoveController(int nControllerId);
	const std::tstring&				GetName(){ return m_strName; }
	void							SetName(const std::tstring& _rStrName){ m_strName = _rStrName; }

	void							Update(float fElapsed);
	void							ResetUpdate();

protected:
	XCutSceneObjectEntity*			m_pCutSceneObjectEntity;
	XCutSceneClapper*				m_pCutSceneClapper;
	std::tstring					m_strName;

private:
	TYPE							m_eType;
	VEC_CUTSCENE_CONTROLLERS		m_vecCutSceneControllers;
};

//////////////////////////////////////////////////////////////////////////

class XCutSceneObjectCamera : public XCutSceneObject
{
public:
									XCutSceneObjectCamera(XCutSceneObjectEntity* pEntity, XCutSceneClapper* pClapper);

	virtual bool					ParseFromElement(MXmlElement* pObjectElement);
	virtual void					AddAttributeOnSave(MXmlElement* pObjectElement){}
};

//////////////////////////////////////////////////////////////////////////

class XCutSceneObjectActor : public XCutSceneObject
{
public:
									XCutSceneObjectActor(XCutSceneObjectEntity* pEntity, XCutSceneClapper* pClapper);

	virtual bool					ParseFromElement(MXmlElement* pObjectElement);
	virtual void					AddAttributeOnSave(MXmlElement* pObjectElement);

	void							SetType(const std::tstring& _rStrType){ m_strType = _rStrType; }
	const std::tstring&				GetType(){ return m_strType; }

	static int						GetActorIdFromEntityInfo(const std::tstring& _rFromEntityCreationId);

private:
	std::tstring					m_strType;
};

//////////////////////////////////////////////////////////////////////////

class XCutSceneObjectEvent : public XCutSceneObject
{
public:
									XCutSceneObjectEvent(XCutSceneObjectEntity* pEntity, XCutSceneClapper* pClapper);

	virtual bool					ParseFromElement(MXmlElement* pObjectElement);
	virtual void					AddAttributeOnSave(MXmlElement* pObjectElement){}

	void							AddCutSceneSkipObjectControllerKeyEvent();
};
