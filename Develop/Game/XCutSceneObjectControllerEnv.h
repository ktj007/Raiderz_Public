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

class XCutSceneObjectControllerEnv : public XCutSceneObjectController
{
public:
	XCutSceneObjectControllerEnv(XCutSceneObject* pOwner) : XCutSceneObjectController(pOwner, E_ENV) {}
	virtual				~XCutSceneObjectControllerEnv(){}

	virtual bool		ParseFromElement(MXmlElement* pControllerElement);
	virtual void		SaveToXML( MXmlElement* pObjectElement );

	virtual void		Update(float fElapsed){ m_KeyFrameUpdator.UpdateKey(this, fElapsed, m_vKeyInfos); }
	virtual void		ResetUpdate();

	virtual int			GetKeyCount(){ return (int)(m_vKeyInfos.size()); }

	void				GetKeyInfo(int nIdx, float& _rOutTime, int& _rEnvTime, int& _rEnvWeather, std::tstring& _rSetName);
	void				SetKeyInfo(int nIdx, float _fTime, int _nEnvTime, int _nEnvWeather, const std::tstring& _rSetName);

	int					GetKeyIndex(float fValue){ return m_KeyFrameUpdator.GetKeyFromTime(fValue, m_vKeyInfos); }
	void				AddKey(float _fTime, int _nEnvTime, int _nEnvWeather, const std::tstring& _rSetName);
	void				DeleteKey(int nIdx);

	void				SortKey(){ m_KeyFrameUpdator.SortKey(m_vKeyInfos, &m_fStartTime, &m_fEndTime); }

private:
	struct SKeyInfo
	{
		SKeyInfo(float _fTime, int _nEnvTime, int _nEnvWeather, const std::tstring& _rSetName)
			: fTime(_fTime), bUpdated(false), nEnvTime(_nEnvTime), nEnvWeather(_nEnvWeather), strSetName(_rSetName)
		{
		}

		float			fTime;
		bool			bUpdated;

		int				nEnvTime;
		int				nEnvWeather;
		std::tstring		strSetName;
	};

	std::vector< SKeyInfo > m_vKeyInfos;
	XCutSceneKeyFrameUpdator<SKeyInfo> m_KeyFrameUpdator;

	friend class		XCutSceneKeyFrameUpdator<SKeyInfo>;
	void				OnUpdateBetweenKey(SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext);
	void				OnUpdateOverKey(SKeyInfo* pProcKey);
};
