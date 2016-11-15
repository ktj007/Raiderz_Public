#include "stdafx.h"
#include "XCoverMesh.h"

#include "XCharacter.h"
#include "RMeshNode.h"
#include "MLocale.h"

void XCoverMeshList::AddCoverMeshList( tstring strUserProperty, bool bSub, int nIndex /*= 1 */ )
{
	// 중복 확인
	vector<XCoverMesh>::iterator itCover = m_vecCoverMeshList.begin();
	while(itCover != m_vecCoverMeshList.end())
	{
		if((*itCover).strCoverMeshSlot == strUserProperty &&
			itCover->nIndex == nIndex)
		{
			itCover->bSub = bSub;
			return;													// 이미 가지고 있다.
		}

		itCover++;
	}

	XCoverMesh CoverList;
	CoverList.strCoverMeshSlot	= strUserProperty;
	CoverList.bSub				= bSub;
	CoverList.nIndex			= nIndex;

	m_vecCoverMeshList.push_back(CoverList);
}

void XCoverMeshList::ShowCoverMesh( tstring strUserProperty )
{	
	// 여기는 메쉬를 모두 보여주는 것이다.
	// 바로 보여주기 기능을 실행한다.
	vector<XCoverMesh>::iterator itCover = m_vecCoverMeshList.begin();
	while(itCover != m_vecCoverMeshList.end())
	{
		if((*itCover).strCoverMeshSlot == strUserProperty)
		{
			RActorNode *pActorNode = m_pCharacter->GetActorNode(MLocale::ConvTCHARToAnsi(strUserProperty.c_str()).c_str());

			if(pActorNode)
			{
				if((*itCover).bSub == false)
				{
					pActorNode->SetAllSubMaterialMask(true);
				}
				else
				{
					pActorNode->SetSubMaterialMask(itCover->nIndex, true);
				}
			}

			// 바로 리스트에서 삭제를 한다.
			m_vecCoverMeshList.erase(itCover);
			return;
		}

		itCover++;
	}
}

void XCoverMeshList::HideCoverMesh()
{
	// 여기는 메쉬를 숨기는 기능이 있는 것이다.
	vector<XCoverMesh>::iterator itCover = m_vecCoverMeshList.begin();
	while(itCover != m_vecCoverMeshList.end())
	{
		RActorNode *pActorNode = m_pCharacter->GetActorNode(MLocale::ConvTCHARToAnsi((*itCover).strCoverMeshSlot.c_str()).c_str());

		if(pActorNode)
		{
			if((*itCover).bSub == false)
			{
				pActorNode->SetAllSubMaterialMask(false);
			}
			else
			{
				pActorNode->SetSubMaterialMask(itCover->nIndex, false);
			}
		}

		itCover++;
	}
}

void XCoverMeshList::SetCoverMesh( RMeshNode* pMeshNode, bool bCover )
{
	const TCHAR* SET_COVER	= _T("set_cover");
	const TCHAR* SET_SUBCOVER= _T("set_subcover");

	if(!pMeshNode) return;

	// TODO : Actor wrap 클래스가 필요하다
	for(int i=0;i<pMeshNode->GetUserPropertyCount();i++)
	{
		const tstring& strProperty = MLocale::ConvAnsiToTCHAR(pMeshNode->GetUserPropertyString(i).c_str());
		if(_tcsncmp(strProperty.c_str(),SET_COVER,_tcslen(SET_COVER))==0)
		{
			int n = (int)strProperty.find_first_of(_T("="));
			if(n != tstring::npos)
			{
				tstring strTargetNode = strProperty.substr(n+1);
				int n = (int)strTargetNode.find_first_not_of(_T(" \t\v\n"));	// trim left
				if(n != tstring::npos) strTargetNode = strTargetNode.substr(n);

				if(bCover)
				{
					// 숨기는 것은 일괄처리하므로 리스트에 추가만 한다.
					AddCoverMeshList(strTargetNode, false);
				}
				else
				{
					ShowCoverMesh(strTargetNode);
				}
			}
		}
		else if(_tcsncmp(strProperty.c_str(), SET_SUBCOVER, _tcslen(SET_SUBCOVER))==0)
		{
			int n = (int)strProperty.find_first_of(_T("="));
			if(n != tstring::npos)
			{
				int nIndex = 1;
				tstring strTargetNode = strProperty.substr(n+1);
				int n = (int)strTargetNode.find_first_not_of(_T(" \t\v\n"));	// trim left
				if(n != tstring::npos) strTargetNode = strTargetNode.substr(n);
				n = (int)strTargetNode.find_first_of(_T(" "));
				if(n != tstring::npos) 
				{
					tstring strIndex = strTargetNode.substr(n);
					nIndex = _ttoi(strIndex.c_str()) - 1;					// -1을 해주어야 한다.

					strTargetNode = strTargetNode.substr(0, n);
				}

				if(bCover)
				{
					// 숨기는 것은 일괄처리하므로 리스트에 추가만 한다.
					AddCoverMeshList(strTargetNode, true, nIndex);
				}
				else
				{
					ShowCoverMesh(strTargetNode);
				}
			}
		}
	}

	//------------------------------------------------------------------------
	// 숨기는 메쉬 일괄 처리
	if(bCover)
	{
		HideCoverMesh();
	}
}