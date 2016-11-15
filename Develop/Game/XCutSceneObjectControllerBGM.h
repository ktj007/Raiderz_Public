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

class XCutSceneObjectControllerBGM : public XCutSceneObjectController
{
public:
						XCutSceneObjectControllerBGM(XCutSceneObject* pOwner) : XCutSceneObjectController(pOwner, E_BGM), m_nUID(-1) {}
	virtual				~XCutSceneObjectControllerBGM(){}

	virtual bool		ParseFromElement(MXmlElement* pControllerElement);
	virtual void		SaveToXML( MXmlElement* pObjectElement );

	virtual void		Update(float fElapsed){ m_KeyFrameUpdator.UpdateKey(this, fElapsed, m_vKeyInfos); }
	virtual void		ResetUpdate();

	void				SetUID(int _nUid){ m_nUID = _nUid; }
	int					GetUID(){ return m_nUID; }

	virtual int			GetKeyCount(){ return (int)(m_vKeyInfos.size()); }

	void				GetKeyInfo(int nIdx, float& _rOutTime, std::tstring& _rFileName, bool& _rbLoop, float& _rfVolume);
	void				SetKeyInfo(int nIdx, float _fTime, const std::tstring& _rFileName, bool _bLoop, float _fVolume);

	int					GetKeyIndex(float fValue){ return m_KeyFrameUpdator.GetKeyFromTime(fValue, m_vKeyInfos); }
	void				AddKey(float _fTime, const std::tstring& _rFileName, bool _bLoop, float _fVolume, int* _pCustomUID = NULL);
	void				DeleteKey(int nIdx);

	void				SortKey(){ m_KeyFrameUpdator.SortKey(m_vKeyInfos, &m_fStartTime, &m_fEndTime); }

private:
	int					m_nUID;
	struct SKeyInfo
	{
		SKeyInfo(float _fTime, const std::tstring& _rFileName, bool _bLoop, float _fVolume, int _uid)
			: fTime(_fTime), bUpdated(false), strFileName(_rFileName), bLoop(_bLoop), fVolume(_fVolume), nUid(_uid)
		{
		}

		float			fTime;
		bool			bUpdated;

		std::tstring		strFileName;
		bool			bLoop;
		float			fVolume;
		int				nUid;
	};

	std::vector< SKeyInfo > m_vKeyInfos;
	XCutSceneKeyFrameUpdator<SKeyInfo> m_KeyFrameUpdator;

	friend class		XCutSceneKeyFrameUpdator<SKeyInfo>;
	void				OnUpdateBetweenKey(SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext);
	void				OnUpdateOverKey(SKeyInfo* pProcKey);
};
