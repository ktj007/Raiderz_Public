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

class XCutSceneObjectControllerAnimation : public XCutSceneObjectController
{
public:
						XCutSceneObjectControllerAnimation(XCutSceneObject* pOwner) : XCutSceneObjectController(pOwner, E_ANIMATION) {}
	virtual				~XCutSceneObjectControllerAnimation(){}

	virtual bool		ParseFromElement(MXmlElement* pControllerElement);
	virtual void		SaveToXML( MXmlElement* pObjectElement );

	virtual void		Update(float fElapsed){ m_KeyFrameUpdator.UpdateKey(this, fElapsed, m_vKeyInfos); }
	virtual void		ResetUpdate();

	virtual int			GetKeyCount(){ return (int)(m_vKeyInfos.size()); }

	void				GetKeyInfo(int nIdx, float& _rOutTime, std::tstring& _rAniName);
	void				SetKeyInfo(int nIdx, float _fTime, const std::tstring& _rAniName);

	int					GetKeyIndex(float fValue){ return m_KeyFrameUpdator.GetKeyFromTime(fValue, m_vKeyInfos); }
	void				AddKey(float _fTime, const std::tstring& _rAniName){ m_vKeyInfos.push_back( SKeyInfo(_fTime, _rAniName) ); }
	void				DeleteKey(int nIdx);

	void				SortKey(){ m_KeyFrameUpdator.SortKey(m_vKeyInfos, &m_fStartTime, &m_fEndTime); }

private:
	struct SKeyInfo
	{
		SKeyInfo(float _fTime, const std::tstring& _strAniName)
			: fTime(_fTime), strAniName(_strAniName), bUpdated(false)
		{
		}

		float		fTime;
		std::tstring	strAniName;
		bool		bUpdated;
	};

	std::vector< SKeyInfo > m_vKeyInfos;
	XCutSceneKeyFrameUpdator<SKeyInfo> m_KeyFrameUpdator;

	friend class		XCutSceneKeyFrameUpdator<SKeyInfo>;
	void				OnUpdateBetweenKey(SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext);
	void				OnUpdateOverKey(SKeyInfo* pProcKey);
};
