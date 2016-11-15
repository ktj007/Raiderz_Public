#pragma once

#include <string>
#include <vector>
#include "MTstring.h"

#include "MXml.h"
#include "MEvent.h"

class XCutSceneObject;
typedef std::vector<XCutSceneObject*> VEC_CUTSCNEOBJ;

class XCutSceneObjectEntity;

//////////////////////////////////////////////////////////////////////////

class XCutSceneClapper
{
public:
									XCutSceneClapper() : m_nField(-1), m_fDuration(-1.f){}
									~XCutSceneClapper(){}

	bool							ParseFromElement(MXmlElement* pClapperElement);

	const TCHAR*					GetCutSceneDesc()	const { return m_strCutSceneDesc.c_str(); }
	int								GetField()			const { return m_nField; }
	float							GetDuration()		const { return m_fDuration; }

	void							SetCutSceneDesc(const std::tstring& _rDesc){ m_strCutSceneDesc = _rDesc; }
	void							SetField(int nId){ m_nField = nId; }
	void							SetDuration(float fDuration){ m_fDuration = fDuration; }

	static std::tstring				CreateXMLString(const TCHAR* _pSzName, int _nField, float _fDuration);

private:

	std::tstring					m_strCutSceneDesc;
	int								m_nField;
	float							m_fDuration;
};

//////////////////////////////////////////////////////////////////////////

class XCutSceneContent
{
public:
									XCutSceneContent() : m_pClapper(NULL), m_fCurrentRunningTime(0.f), m_bCutSceneSkip(false) {}
									~XCutSceneContent();

	XCutSceneClapper*				GetClapper(){ return m_pClapper; }
	VEC_CUTSCNEOBJ&					GetCutSceneObjects(){ return m_vCutSceneObjects; }
	XCutSceneObject*				FindCutSceneObject(const std::tstring& _strName);

	void							AddClapper(XCutSceneClapper* pClapper){ _ASSERT(!m_pClapper); m_pClapper = pClapper; }
	void							AddCutSceneObject( XCutSceneObject* pCutSceneObject ){ m_vCutSceneObjects.push_back(pCutSceneObject); }

	void							Run(float fElapsed);
	void							ResetRunning();
	float							GetCurrnetRunningTime(){ return m_fCurrentRunningTime; }

	bool							OnEvent(mint3::MEvent& e);

    void							SetCutSceneSkip(bool bCutSceneSkip);
	bool							IsCutSceneSkip();

private:
	XCutSceneClapper*				m_pClapper;
	VEC_CUTSCNEOBJ					m_vCutSceneObjects;
	float							m_fCurrentRunningTime;
	bool							m_bCutSceneSkip;
};

//////////////////////////////////////////////////////////////////////////

class XCutSceneContentBuilder
{
public:
									XCutSceneContentBuilder(){}
	virtual							~XCutSceneContentBuilder(){}

	bool							Build(const TCHAR* _pSzFileName, XCutSceneContent* _pOutCutSceneContents);
	bool							Build(MXmlElement* pRootElement, XCutSceneContent* _pOutCutSceneContents);

protected:

	// clapper
	virtual bool					OnClapperCreated(XCutSceneClapper* pClapper) = 0;

	// entity
	virtual XCutSceneObjectEntity*	OnCreateCameraEntity() = 0;
	virtual XCutSceneObjectEntity*	OnCreateActorEntity(const std::tstring& _rStrType) = 0;
	virtual XCutSceneObjectEntity*	OnCreateEventEntity(MXmlElement* _pEventElement) = 0;

};
