#pragma once

#include <string>
#include <algorithm>

#include "MXml.h"
#include "MVector3.h"
#include "MUID.h"

#include "XCutSceneKeyFrameUpdator.h"

#include "MTstring.h"
#include "XCutSceneObjectController.h"

using namespace std;

class XCutSceneObject;

class XCutSceneObjectControllerDOF : public XCutSceneObjectController
{
public:
						XCutSceneObjectControllerDOF(XCutSceneObject* pOwner) : XCutSceneObjectController(pOwner, E_DOF) {}
	virtual				~XCutSceneObjectControllerDOF(){}

	virtual bool		ParseFromElement(MXmlElement* pControllerElement);
	virtual void		SaveToXML( MXmlElement* pObjectElement );

	virtual void		Update(float fElapsed);
	virtual void		ResetUpdate();

	virtual int			GetKeyCount(){ return (int)(m_vKeyInfos.size()); }

	void				GetKeyInfo(int nIdx, float& _rOutTime, float& _rFocusDist, float& _rFocusRange, float& _rCocSize);
	void				SetKeyInfo(int nIdx, float _fTime, float _fFocusDist, float _fFocusRange, float _fCocSize);

	int					GetKeyIndex(float fValue){ return m_KeyFrameUpdator.GetKeyFromTime(fValue, m_vKeyInfos); }
	void				AddKey(float _fTime, float _fFocusDist, float _fFocusRange, float _fCocSize){ m_vKeyInfos.push_back( SKeyInfo(_fTime, _fFocusDist, _fFocusRange, _fCocSize) ); }
	void				DeleteKey(int nIdx);

	void				SortKey(){ m_KeyFrameUpdator.SortKey(m_vKeyInfos, &m_fStartTime, &m_fEndTime); }

private:
	struct SKeyInfo
	{
		SKeyInfo(float _fTime, float _fFocusDist, float _fFocusRange, float _fCocSize)
			: fTime(_fTime), fFocusDist(_fFocusDist), fFocusRange(_fFocusRange), fCocSize(_fCocSize)
		{
		}

		float		fTime;

		float		fFocusDist;
		float		fFocusRange;
		float		fCocSize;
	};

	std::vector< SKeyInfo > m_vKeyInfos;
	XCutSceneKeyFrameUpdator<SKeyInfo> m_KeyFrameUpdator;

	friend class		XCutSceneKeyFrameUpdator<SKeyInfo>;
	void				OnUpdateBetweenKey(SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext);
	void				OnUpdateOverKey(SKeyInfo* pProcKey);
};
