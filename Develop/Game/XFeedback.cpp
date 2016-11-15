#include "stdafx.h"
#include "XFeedback.h"

XFeedbackInfoMgr::XFeedbackInfoMgr() : m_bShowFeedback(false)
{

}

XFeedbackInfoMgr::~XFeedbackInfoMgr()
{
	Clear();
}

bool XFeedbackInfoMgr::LoadFromXmlStream( const wchar_t* szStream )
{
	MXml xml;
	if(!xml.LoadStream(MLocale::ConvUTF16ToAnsi(szStream).c_str()))
	{
		return false;
	}


	MXmlElement* pElement = xml.DocHandle().FirstChild("XML").FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), "Feedback"))
		{
			ParseFeedback(pElement, &xml);
		}
	}
	return true;
}

void XFeedbackInfoMgr::ParseFeedback( MXmlElement* pFeedbackElement, MXml* pXml )
{
	XFeedbackInfo* pNewFeedbackInfo = new XFeedbackInfo();

	wstring strValue;

	_Attribute(pNewFeedbackInfo->m_strUserID, pFeedbackElement, "userid", pXml);
	_Attribute(pNewFeedbackInfo->m_strName, pFeedbackElement, "name", pXml);
	_Attribute(pNewFeedbackInfo->m_strDateTime, pFeedbackElement, "date", pXml);
	_Attribute(&pNewFeedbackInfo->m_nFieldID, pFeedbackElement, "field");
	_Attribute(pNewFeedbackInfo->m_strTag, pFeedbackElement, "tag", pXml);

	if (_Attribute(strValue, pFeedbackElement, "pos"))
	{
		swscanf_s(strValue.c_str(), L"%f %f %f", &pNewFeedbackInfo->m_vPos.x, 
			&pNewFeedbackInfo->m_vPos.y, &pNewFeedbackInfo->m_vPos.z);
	}

	if (_Attribute(strValue, pFeedbackElement, "dir"))
	{
		swscanf_s(strValue.c_str(), L"%f %f %f", &pNewFeedbackInfo->m_vDir.x, 
			&pNewFeedbackInfo->m_vDir.y, &pNewFeedbackInfo->m_vDir.z);
	}

	MXmlElement* pCommentElement = pFeedbackElement->FirstChildElement("Comment");
	if (pCommentElement)
	{
		string strComment;
		_Contents(strComment, pCommentElement, pXml);

		pNewFeedbackInfo->m_strComment = MLocale::ConvAnsiToUTF16(strComment.c_str());
	}

	{
		cml2::MCriticalSection::Guard guard(&m_Lock);

		FEEDBACK_LIST* pFeedbackList = FindFeedbackList(pNewFeedbackInfo->m_nFieldID);

		if (pFeedbackList == NULL)
		{
			pFeedbackList = new FEEDBACK_LIST();
		}
		pFeedbackList->push_back(pNewFeedbackInfo);

		m_FeedbackMap.insert(FEEDBACKS_MAP::value_type(pNewFeedbackInfo->m_nFieldID, pFeedbackList));
	}
}

void XFeedbackInfoMgr::Clear()
{
	cml2::MCriticalSection::Guard guard(&m_Lock);

	for (FEEDBACKS_MAP::iterator itor = m_FeedbackMap.begin(); itor != m_FeedbackMap.end(); ++itor)
	{
		FEEDBACK_LIST* pList = (*itor).second;

		for (FEEDBACK_LIST::iterator itor2 = pList->begin(); itor2 != pList->end(); ++itor2)
		{
			delete (*itor2);
		}

		delete pList;
	}
	m_FeedbackMap.clear();
}

XFeedbackInfoMgr::FEEDBACK_LIST* XFeedbackInfoMgr::FindFeedbackList( int nFieldID )
{
	FEEDBACKS_MAP::iterator itor = m_FeedbackMap.find(nFieldID);
	if (itor != m_FeedbackMap.end())
	{
		return (*itor).second;
	}
	return NULL;
}

#include "XMyPlayer.h"
#include "XGame.h"

void XFeedbackInfoMgr::MakeObjects()
{
	if (!m_bShowFeedback) return;

	cml2::MCriticalSection::Guard guard(&m_Lock);

	int nFieldID = gvar.World.GetFieldID();
	vec3 vPos = gvar.Game.pMyPlayer->GetPosition();

	FEEDBACK_LIST* pList = FindFeedbackList(nFieldID);
	if (pList == NULL) return;

	const float TOLER_DIST = SECTOR_SIZE * 3;

	for (FEEDBACK_LIST::iterator itor = pList->begin(); itor != pList->end(); ++itor)
	{
		XFeedbackInfo* pInfo = (*itor);
		float fDist = pInfo->m_vPos.DistanceTo(vPos);
		if (fDist > TOLER_DIST) 
		{
			if (pInfo->m_uidObject != MUID::ZERO)
			{
				XObject* pObj = gg.omgr->Find(pInfo->m_uidObject);
				if (pObj)
				{
					pObj->DeleteMe();
				}

				pInfo->m_uidObject = MUID::ZERO;
			}
			continue;
		}

		if (pInfo->m_uidObject != MUID::ZERO)
		{
			XObject* pObj = gg.omgr->Find(pInfo->m_uidObject);
			if (pObj != NULL) continue;
		}

		MUID uidNew = gg.currentgamestate->CreateFeedbackObject(pInfo->m_vPos, pInfo->m_vDir,
			pInfo->m_strName,
			pInfo->m_strComment);
		pInfo->m_uidObject = uidNew;
	}
}

void XFeedbackInfoMgr::SetShowFeedback( bool bShow )
{
	m_bShowFeedback = bShow;

	if (!m_bShowFeedback)
	{
		ClearObjects();
	}
	else
	{
		MakeObjects();
	}
}

void XFeedbackInfoMgr::ClearObjects()
{
	int nFieldID = gvar.World.GetFieldID();
	vec3 vPos = gvar.Game.pMyPlayer->GetPosition();

	FEEDBACK_LIST* pList = FindFeedbackList(nFieldID);
	if (pList == NULL) return;

	for (FEEDBACK_LIST::iterator itor = pList->begin(); itor != pList->end(); ++itor)
	{
		XFeedbackInfo* pInfo = (*itor);
		if (pInfo->m_uidObject != MUID::ZERO)
		{
			XObject* pObj = gg.omgr->Find(pInfo->m_uidObject);
			if (pObj)
			{
				pObj->DeleteMe();
			}

			pInfo->m_uidObject = MUID::ZERO;
		}
	}
}