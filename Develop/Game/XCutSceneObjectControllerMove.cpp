#include "stdafx.h"
#include "XCutSceneObjectControllerMove.h"

#include "XCutSceneObject.h"
#include "XCutSceneContent.h"

#include "MMath.h"
#include "MLocale.h"

#include "RCameraSceneNode.h"

XCutSceneObjectControllerMove::XCutSceneObjectControllerMove( XCutSceneObject* pOwner )
: XCutSceneObjectController(pOwner, E_MOVE)
, m_bCatmullRom(false)
, m_bUsingCollision(false)
, m_bUsingUp(false)
, m_fLastWarpTime(-1.f)
{

}

XCutSceneObjectControllerMove::~XCutSceneObjectControllerMove()
{

}

bool XCutSceneObjectControllerMove::ParseFromElement( MXmlElement* pControllerElement )
{
	float fTime;
	MVector3 vPos, vDir;
	bool bWarp;

	std::string strTemp;
	_Attribute(strTemp, pControllerElement, "interpolation");
	if (strTemp == "catmull")
	{
		m_bCatmullRom = true;
	}
	_Attribute(strTemp, pControllerElement, "collision");
	if (strTemp == "true")
	{
		m_bUsingCollision = true;
	}

	MXmlElement* pMoveKeyElement = pControllerElement->FirstChildElement("movekey");
	while (pMoveKeyElement)
	{
		_Attribute(fTime, pMoveKeyElement, "time");
		_Attribute(vPos.x, pMoveKeyElement, "x");
		_Attribute(vPos.y, pMoveKeyElement, "y");
		_Attribute(vPos.z, pMoveKeyElement, "z");
		_Attribute(vDir.x, pMoveKeyElement, "dir_x");
		_Attribute(vDir.y, pMoveKeyElement, "dir_y");
		_Attribute(vDir.z, pMoveKeyElement, "dir_z");

		MVector3 vUp(MVector3::AXISZ);
		if (_Attribute(vUp.x, pMoveKeyElement, "up_x"))
		{
			_Attribute(vUp.y, pMoveKeyElement, "up_y");
			_Attribute(vUp.z, pMoveKeyElement, "up_z");
		}

		_Attribute(bWarp, pMoveKeyElement, "warp", false);

		AddKey(fTime, vPos, vDir, vUp, bWarp);
		pMoveKeyElement = pMoveKeyElement->NextSiblingElement();
	}

	SortKey();
	return true;
}

void XCutSceneObjectControllerMove::GetKeyInfo( int nIdx, float& _rOutTime, MVector3& _rOutPos, MVector3& _rOutDir, MVector3& _rOutUp, bool& _rIsWarp )
{
	if(static_cast<int>(m_vKeyInfos.size()) > nIdx)
	{
		SKeyInfo& rKeyInfo = m_vKeyInfos[nIdx];
		_rOutTime = rKeyInfo.fTime;

		_rOutPos = rKeyInfo.vPos;
		_rOutDir = rKeyInfo.vDir;
		_rOutUp = rKeyInfo.vUp;

		_rIsWarp = rKeyInfo.bWarp;
	}
}

void XCutSceneObjectControllerMove::SetKeyInfo( int nIdx, float _fTime, const MVector3& _rPos, const MVector3& _rDir, const MVector3& _rUp, bool _bWarp )
{
	SKeyInfo& rKeyInfo = m_vKeyInfos[nIdx];
	rKeyInfo.fTime = _fTime;
	rKeyInfo.vPos = _rPos;
	rKeyInfo.vDir = _rDir;
	rKeyInfo.vUp = _rUp;
	rKeyInfo.bWarp = _bWarp;
}

void XCutSceneObjectControllerMove::DeleteKey( int nIdx )
{
	if (nIdx < 0)
		return;

	m_vKeyInfos.erase( m_vKeyInfos.begin() + nIdx );
}

void XCutSceneObjectControllerMove::SaveToXML( MXmlElement* pObjectElement )
{
	MXmlElement* pMoveElement = new MXmlElement("move");

	if (m_bCatmullRom)
	{
		_SetAttribute(pMoveElement, "interpolation", "catmull");
	}
	else
	{
		_SetAttribute(pMoveElement, "interpolation", "linear");
	}

	if (m_bUsingCollision)
	{
		_SetAttribute(pMoveElement, "collision", "true");
	}
	else
	{
		_SetAttribute(pMoveElement, "collision", "false");
	}

	size_t nTotalKey = m_vKeyInfos.size();
	for (size_t i = 0; i < nTotalKey; ++i)
	{
		MXmlElement* pMoveKey = new MXmlElement("movekey");
		_SetAttribute(pMoveKey, "time", m_vKeyInfos[i].fTime);
		_SetAttribute(pMoveKey, "x", m_vKeyInfos[i].vPos.x);
		_SetAttribute(pMoveKey, "y", m_vKeyInfos[i].vPos.y);
		_SetAttribute(pMoveKey, "z", m_vKeyInfos[i].vPos.z);

		_SetAttribute(pMoveKey, "dir_x", m_vKeyInfos[i].vDir.x);
		_SetAttribute(pMoveKey, "dir_y", m_vKeyInfos[i].vDir.y);
		_SetAttribute(pMoveKey, "dir_z", m_vKeyInfos[i].vDir.z);

		if (m_bUsingUp)
		{
			_SetAttribute(pMoveKey, "up_x", m_vKeyInfos[i].vUp.x);
			_SetAttribute(pMoveKey, "up_y", m_vKeyInfos[i].vUp.y);
			_SetAttribute(pMoveKey, "up_z", m_vKeyInfos[i].vUp.z);
		}

		_SetAttribute(pMoveKey, "warp", m_vKeyInfos[i].bWarp);

		pMoveElement->LinkEndChild(pMoveKey);
	}

	pObjectElement->LinkEndChild(pMoveElement);
}

void XCutSceneObjectControllerMove::OnUpdateBetweenKey( SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext )
{
	MMatrix matSrc;
	matSrc.SetLocalMatrix(pCurrent->vPos, pCurrent->vDir, pCurrent->vUp);
	if (pNext->bWarp)
	{
		if (pCurrent->bUpdated == false)
		{
			pCurrent->bUpdated = true;
			m_fLastWarpTime = pCurrent->fTime;
			m_pOwner->GetEntity()->SetTransform(matSrc, m_bUsingCollision);
		}
		return;
	}

	MMatrix matDest;
	matDest.SetLocalMatrix(pNext->vPos, pNext->vDir, pNext->vUp);

	// for direction
	MQuat qSrc(matSrc);
	MQuat qDest(matDest);
	MQuat qResult;
	qResult.Slerp(qSrc, qDest, fIntervalRatio);
	MMatrix matResult = qResult.Conv2Matrix();

	// for position
	if (m_bCatmullRom)
	{
		MVector3 vPrePos, vNextNextPos;
		if (pPre)
		{
			vPrePos = pPre->vPos;
		}
		else
		{
			vPrePos = pCurrent->vPos;
		}

		if (pNextNext)
		{
			vNextNextPos = pNextNext->vPos;
		}
		else
		{
			vNextNextPos = pNext->vPos;
		}

		matResult.SetTranslation( MMath::CatmullRomInterpolation(vPrePos, pCurrent->vPos, pNext->vPos, vNextNextPos, fIntervalRatio) );
	}
	else
	{
		matResult.SetTranslation( MMath::LinearInterpolation(pCurrent->vPos, pNext->vPos, fIntervalRatio) );
	}

	m_pOwner->GetEntity()->SetTransform(matResult, m_bUsingCollision);
}

void XCutSceneObjectControllerMove::OnUpdateOverKey( SKeyInfo* pProcKey )
{
	if (pProcKey->fTime <= m_fLastWarpTime)
		return;

	MMatrix mat;
	mat.SetLocalMatrix(pProcKey->vPos, pProcKey->vDir, pProcKey->vUp);
	m_pOwner->GetEntity()->SetTransform(mat, m_bUsingCollision);
}

void XCutSceneObjectControllerMove::ResetUpdate()
{
	m_fLastWarpTime = -1.f;
	m_KeyFrameUpdator.Reset();
	for (std::vector< SKeyInfo >::iterator itr = m_vKeyInfos.begin(); itr != m_vKeyInfos.end(); ++itr)
	{
		itr->bUpdated = false;
	}
}
