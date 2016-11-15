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

class XCutSceneObjectControllerTalent : public XCutSceneObjectController
{
public:
						XCutSceneObjectControllerTalent(XCutSceneObject* pOwner) : XCutSceneObjectController(pOwner, E_TALENT) {}
	virtual				~XCutSceneObjectControllerTalent(){}

	virtual bool		ParseFromElement(MXmlElement* pControllerElement);
	virtual void		SaveToXML( MXmlElement* pObjectElement );

	virtual void		Update(float fElapsed){ m_KeyFrameUpdator.UpdateKey(this, fElapsed, m_vKeyInfos); }
	virtual void		ResetUpdate();

	virtual int			GetKeyCount(){ return (int)(m_vKeyInfos.size()); }

	void				GetKeyInfo(int nIdx, float& _rOutTime, int& nTalentID);
	void				SetKeyInfo(int nIdx, float _fTime, int nTalentID);

	int					GetKeyIndex(float fValue){ return m_KeyFrameUpdator.GetKeyFromTime(fValue, m_vKeyInfos); }
	void				AddKey(float _fTime, const int _nTalentID){ m_vKeyInfos.push_back( SKeyInfo(_fTime, _nTalentID) ); }
	void				DeleteKey(int nIdx);

	void				SortKey(){ m_KeyFrameUpdator.SortKey(m_vKeyInfos, &m_fStartTime, &m_fEndTime); }

private:
	struct SKeyInfo
	{
		SKeyInfo(float _fTime, const int _nTalentID)
			: fTime(_fTime), nTalentID(_nTalentID), bUpdated(false)
		{
		}

		float	fTime;
		int		nTalentID;
		bool	bUpdated;
	};

	std::vector< SKeyInfo > m_vKeyInfos;
	XCutSceneKeyFrameUpdator<SKeyInfo> m_KeyFrameUpdator;

	friend class		XCutSceneKeyFrameUpdator<SKeyInfo>;
	void				OnUpdateBetweenKey(SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext);
	void				OnUpdateOverKey(SKeyInfo* pProcKey);
};
