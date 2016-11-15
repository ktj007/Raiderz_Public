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

#define LETTERBOX_SCRREN_RATIO_W 2.35
#define LETTERBOX_SCRREN_RATIO_H 1

class XCutSceneObjectControllerLetterBox : public XCutSceneObjectController
{
public:
	enum LetterBoxTYPE
	{
		LBT_START,
		LBT_END
	};

private:
	struct SKeyInfo
	{
		SKeyInfo(LetterBoxTYPE _eType, float _fTime)
			: eType(_eType), fTime(_fTime)
		{
		}

		float			fTime;
		LetterBoxTYPE	eType;
	};

	std::vector< SKeyInfo > m_vKeyInfos;
	XCutSceneKeyFrameUpdator<SKeyInfo> m_KeyFrameUpdator;

private:
	friend class		XCutSceneKeyFrameUpdator<SKeyInfo>;
	void				OnUpdateBetweenKey(SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext);
	void				OnUpdateOverKey(SKeyInfo* pProcKey);

	void				SetLetterBox(LetterBoxTYPE eType);

public:
	XCutSceneObjectControllerLetterBox(XCutSceneObject* pOwner) : XCutSceneObjectController(pOwner, E_LETTERBOX) {}
	virtual				~XCutSceneObjectControllerLetterBox(){}

	virtual bool		ParseFromElement(MXmlElement* pControllerElement);
	virtual void		SaveToXML( MXmlElement* pObjectElement );

	virtual void		Update(float fElapsed);
	virtual void		ResetUpdate();

	virtual int			GetKeyCount(){ return (int)(m_vKeyInfos.size()); }

	void				GetKeyInfo(int nIdx, LetterBoxTYPE& _eOutType, float& _rOutTime);
	void				SetKeyInfo(int nIdx, LetterBoxTYPE _eType, float _fTime);

	int					GetKeyIndex(float fValue){ return m_KeyFrameUpdator.GetKeyFromTime(fValue, m_vKeyInfos); }
	void				AddKey(LetterBoxTYPE eType, float _fTime);
	void				DeleteKey(int nIdx);

	void				SortKey(){ m_KeyFrameUpdator.SortKey(m_vKeyInfos, &m_fStartTime); }
};
