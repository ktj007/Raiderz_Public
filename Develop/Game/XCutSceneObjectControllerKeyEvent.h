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

enum KEY {
	KEY_ESC = 0
};

enum EVENT {
	EVENT_SKIP = 0
};

class XCutSceneObjectControllerKeyEvent : public XCutSceneObjectController
{
private:
	struct SKeyInfo
	{
		SKeyInfo() {}
		SKeyInfo(float _fTime, float _DurationTime, int _nKeyType, int _nEvent, tstring _strKyeEventName, tstring _strFont, int _nSize, bool _bBold, bool _bItalic)
			: fTime(_fTime), fDurationTime(_DurationTime), nKeyType(_nKeyType), nEvent(_nEvent), strKyeEventName(_strKyeEventName), strFont(_strFont), nSize(_nSize), bBold(_bBold), bItalic(_bItalic)
		{
		}

		int				nIndex;

		float			fTime;
		float			fDurationTime;
		int				nKeyType;
		int				nEvent;
		tstring			strKyeEventName;
		tstring			strFont;
		int				nSize;
		bool			bBold;
		bool			bItalic;

		void Set(SKeyInfo& keyInfo)
		{
			nIndex				= keyInfo.nIndex;

			fTime				= keyInfo.fTime;
			fDurationTime		= keyInfo.fDurationTime;
			nKeyType			= keyInfo.nKeyType;
			nEvent				= keyInfo.nEvent;
			strKyeEventName		= keyInfo.strKyeEventName;
			strFont				= keyInfo.strFont;
			nSize				= keyInfo.nSize;
			bBold				= keyInfo.bBold;
			bItalic				= keyInfo.bItalic;
		}
	};

	struct stCurrentKeyEvent : public SKeyInfo
	{
		bool			bActive;

		stCurrentKeyEvent()
		{
			Init();
		}

		void Init()
		{
			bActive			= false;
		}
	};

	float				m_fPosX;
	float				m_fPosY;

	std::vector< SKeyInfo > m_vKeyInfos;
	XCutSceneKeyFrameUpdator<SKeyInfo> m_KeyFrameUpdator;

	stCurrentKeyEvent	m_CurrentKeyEvent;

private:
	friend class		XCutSceneKeyFrameUpdator<SKeyInfo>;
	void				OnUpdateBetweenKey(SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext);
	void				OnUpdateOverKey(SKeyInfo* pProcKey);

	void				ShowKeyEvent(SKeyInfo* keyInfo);
	void				HideKeyEvent();

public:
	XCutSceneObjectControllerKeyEvent(XCutSceneObject* pOwner);
	virtual				~XCutSceneObjectControllerKeyEvent(){}

	virtual bool		ParseFromElement(MXmlElement* pControllerElement);
	virtual void		SaveToXML( MXmlElement* pObjectElement );
	virtual	bool		OnEvent(mint3::MEvent& e);

	virtual void		Update(float fElapsed);
	virtual void		ResetUpdate();

	virtual int			GetKeyCount(){ return (int)(m_vKeyInfos.size()); }

	void				GetKeyInfo(int nIdx, float& _rOutTime, float& _rOutDurationTime);
	void				SetKeyInfo(int nIdx, float _fTime, float _fDurationTime);

	int					GetKeyIndex(float fValue){ return m_KeyFrameUpdator.GetKeyFromTime(fValue, m_vKeyInfos); }
	void				AddKey(SKeyInfo& keyInfo);
	void				DeleteKey(int nIdx);

	void				SortKey();

	void				SetPosX(float fPosX)		{ m_fPosX = fPosX; }
	void				SetPosY(float fPosY)		{ m_fPosX = fPosY; }

	float				GetPosX()		{ return m_fPosX; }
	float				GetPosY()		{ return m_fPosY; }
};
