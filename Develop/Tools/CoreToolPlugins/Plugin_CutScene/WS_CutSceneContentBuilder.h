#pragma once

#include "WS_CutScene.h"
#include "WS_CutSceneXCutSceneWrapper.h"
#include "../../Game/XCutSceneContent.h"

class CCutSceneContentBuilder : public XCutSceneContentBuilder
{
public: 
	CCutSceneContentBuilder(XCutSceneContent* pCutSceneContent);
	virtual ~CCutSceneContentBuilder();

protected:
	virtual bool OnClapperCreated(XCutSceneClapper* pClapper)
	{
		return true;
	}

	virtual XCutSceneObjectEntity* OnCreateCameraEntity()
	{
		return CXCutSceneWrapper::CreateCameraObjectEntity();
	}

	virtual XCutSceneObjectEntity* OnCreateActorEntity(const std::string& _rStrType)
	{
		return CXCutSceneWrapper::CreateActorObjectEntity(_rStrType);
	}

	virtual XCutSceneObjectEntity* OnCreateEventEntity(MXmlElement* _pEventElement)
	{
		return CXCutSceneWrapper::CreateEventObjectEntity(_pEventElement, m_pCutSceneContent);
	}

protected:
	XCutSceneContent*	m_pCutSceneContent;
};