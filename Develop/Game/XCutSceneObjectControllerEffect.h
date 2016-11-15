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

enum CONTROLLER_EFFECT_TYPE
{
	CRET_NONE = 0,
	CRET_MODEL,
	CRET_SOUND,
	CRET_CAMERA,
	CRET_POSTEFFECT,
	CRET_DECALEFFECT,

	CRET_MAX
};

struct stControllerEffectData
{
	CONTROLLER_EFFECT_TYPE	type;

	MUID				uidEffect;

	bool				bFollow;
	bool				bTerrainAlign;
	bool				bTerrainPos;

	tstring				strResourceName;
	tstring				strBoneName;

	stControllerEffectData()
	{
		type			= CRET_NONE;

		uidEffect.SetZero();

		bFollow			= false;
		bTerrainAlign	= false;
		bTerrainPos		= false;

		strResourceName.clear();
		strBoneName.clear();
	}
};

class XCutSceneObjectControllerEffect : public XCutSceneObjectController
{
private:
	struct SKeyInfo
	{
		SKeyInfo(float _fTime, float _DurationTime, const tstring& _strEffectName, const tstring& _strEffectInfo, vec3 _pos, vec3 _dir, vec3 _up)
			: fTime(_fTime), fDurationTime(_DurationTime), strEffectName(_strEffectName), strEffectInfo(_strEffectInfo), vPos(_pos), vDir(_dir), vUp(_up), m_bProced(false)
		{
		}

		int				nIndex;

		float			fTime;
		float			fDurationTime;

		vec3			vPos;
		vec3			vDir;
		vec3			vUp;

		tstring			strEffectName;
		tstring			strEffectInfo;

		bool			m_bProced;
	};

	struct stEffectData
	{
		CONTROLLER_EFFECT_TYPE	type;

		MUID			uidEffect;
		float			fDurationTime;

		stEffectData()
		{
			type			= CRET_NONE;
			uidEffect		= MUID::ZERO;
			fDurationTime	= -1.0f;
		}
	};

	std::vector< SKeyInfo > m_vKeyInfos;
	XCutSceneKeyFrameUpdator<SKeyInfo> m_KeyFrameUpdator;

	std::vector<stEffectData>	m_vecEffectData;

private:
	friend class		XCutSceneKeyFrameUpdator<SKeyInfo>;
	void				OnUpdateBetweenKey(SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext);
	void				OnUpdateOverKey(SKeyInfo* pProcKey);

	void				CreateEffect(SKeyInfo* keyInfo);
	void				DestroyEffect(MUID uid);
	void				DestroyAllEffect();

	void				UpdateEffect(float fDelta);

public:
	XCutSceneObjectControllerEffect(XCutSceneObject* pOwner) : XCutSceneObjectController(pOwner, E_EFFECT) {}
	virtual				~XCutSceneObjectControllerEffect();																						

	virtual bool		ParseFromElement(MXmlElement* pControllerElement);
	virtual void		SaveToXML( MXmlElement* pObjectElement );

	virtual void		Update(float fElapsed);
	virtual void		ResetUpdate();

	virtual int			GetKeyCount(){ return (int)(m_vKeyInfos.size()); }

	void				GetKeyInfo(int nIdx, float& _rOutTime, float& _rOutDurationTime, vec3& _rpos, vec3& _rdir, vec3& _rup, std::tstring& _rEffectName, std::tstring& _rEffectInfo);
	void				SetKeyInfo(int nIdx, float _fTime, float _fDurationTime, vec3 _pos, vec3 _dir, vec3 _up, const std::tstring& _rEffectName, const std::tstring& _rEffectInfo);

	int					GetKeyIndex(float fValue){ return m_KeyFrameUpdator.GetKeyFromTime(fValue, m_vKeyInfos); }
	void				AddKey(float _fTime, float _DurationTime, const tstring& _strName, const tstring& _strInfo, vec3 _pos, vec3 _dir, vec3 _up);
	void				DeleteKey(int nIdx);

	void				SortKey();
};