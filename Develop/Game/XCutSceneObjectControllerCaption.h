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

class XCutSceneObjectControllerCaption : public XCutSceneObjectController
{
private:
	struct SKeyInfo
	{
		SKeyInfo(float _fTime, float _DurationTime, tstring _strScript)
			: fTime(_fTime), fDurationTime(_DurationTime), strScript(_strScript)
		{
		}

		int				nIndex;

		float			fTime;
		float			fDurationTime;

		tstring			strScript;
	};

	struct stCurrentCaption
	{
		int				nIndex;
		bool			bActive;
		tstring			strScript;
		float			fDurationTime;

		stCurrentCaption()
		{
			Init();
		}

		void Init()
		{
			nIndex			= 0;
			bActive			= false;
			fDurationTime	= 0.0f;
			strScript.clear();
		}
	};

	tstring				m_strFontName;
	int					m_nSize;

	bool				m_bBold;
	bool				m_bItalic;

	std::vector< SKeyInfo > m_vKeyInfos;
	XCutSceneKeyFrameUpdator<SKeyInfo> m_KeyFrameUpdator;

	stCurrentCaption	m_CurrentCaption;

private:
	friend class		XCutSceneKeyFrameUpdator<SKeyInfo>;
	void				OnUpdateBetweenKey(SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext);
	void				OnUpdateOverKey(SKeyInfo* pProcKey);

	void				ShowCaption(SKeyInfo* keyInfo, tstring strFontName, int nSize, bool bBold, bool bItalic);
	void				HideCaption();

public:
	XCutSceneObjectControllerCaption(XCutSceneObject* pOwner) : XCutSceneObjectController(pOwner, E_CAPTION), m_nSize(12), m_strFontName(_T("±¼¸²Ã¼")), m_bBold(false), m_bItalic(false) {}
	virtual				~XCutSceneObjectControllerCaption(){}																						

	virtual bool		ParseFromElement(MXmlElement* pControllerElement);
	virtual void		SaveToXML( MXmlElement* pObjectElement );

	virtual void		Update(float fElapsed);
	virtual void		ResetUpdate();

	virtual int			GetKeyCount(){ return (int)(m_vKeyInfos.size()); }

	void				GetKeyInfo(int nIdx, float& _rOutTime, float& _rOutDurationTime, std::tstring& _rScript);
	void				SetKeyInfo(int nIdx, float _fTime, float _fDurationTime, const std::tstring& _rScript);

	int					GetKeyIndex(float fValue){ return m_KeyFrameUpdator.GetKeyFromTime(fValue, m_vKeyInfos); }
	void				AddKey(float _fTime, float _DurationTime, tstring _strScript);
	void				DeleteKey(int nIdx);

	void				SortKey();

	void				SetFontName(tstring strName);
	void				SetSize(int nSize)	{ m_nSize = nSize; }
	void				SetBold(bool bBold) { m_bBold = bBold; }
	void				SetItalic(bool bItalic) { m_bItalic = bItalic; }

	std::tstring		GetFontName()	{ return m_strFontName; }
	int					GetSize()		{ return m_nSize; }
	bool				GetBold()		{ return m_bBold; }
	bool				GetItalic()		{ return m_bItalic; }
};
