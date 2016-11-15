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

class XCutSceneObjectControllerMove : public XCutSceneObjectController
{
public:
						XCutSceneObjectControllerMove(XCutSceneObject* pOwner);
	virtual				~XCutSceneObjectControllerMove();

	virtual bool		ParseFromElement(MXmlElement* pControllerElement);
	virtual void		SaveToXML( MXmlElement* pObjectElement );

	virtual void		Update(float fElapsed){ m_KeyFrameUpdator.UpdateKey(this, fElapsed, m_vKeyInfos); }
	virtual void		ResetUpdate();

	virtual int			GetKeyCount(){ return (int)(m_vKeyInfos.size()); }

	void				GetKeyInfo(int nIdx, float& _rOutTime, MVector3& _rOutPos, MVector3& _rOutDir, MVector3& _rOutUp, bool& _rIsWarp);
	void				SetKeyInfo(int nIdx, float _fTime, const MVector3& _rPos, const MVector3& _rDir, const MVector3& _rUp, bool _bWarp);

	int					GetKeyIndex(float fValue){ return m_KeyFrameUpdator.GetKeyFromTime(fValue, m_vKeyInfos); }
	void				AddKey(float _fTime, const MVector3& _rPos, const MVector3& _rDir, const MVector3& _rUp, bool _bWarp){ m_vKeyInfos.push_back( SKeyInfo(_fTime, _rPos, _rDir, _rUp, _bWarp) ); }
	void				DeleteKey(int nIdx);

	bool				IsUsingCatmullRomInterpolation(){ return m_bCatmullRom; }
	void				SetUsingCatmullRomInterpolation(bool bEnable){ m_bCatmullRom = bEnable; }

	bool				IsUsingCollision(){return m_bUsingCollision; }
	void				SetUsingCollision(bool bEnable){ m_bUsingCollision = bEnable; }

	bool				IsUsingUp(){ return m_bUsingUp; }
	void				SetUsingUp(bool bEnable){ m_bUsingUp = bEnable; }

	void				SortKey(){ m_KeyFrameUpdator.SortKey(m_vKeyInfos, &m_fStartTime, &m_fEndTime); }

private:
	struct SKeyInfo
	{
		SKeyInfo(float _fTime, const MVector3& _rPos, const MVector3& _rDir, const MVector3& _rUp, bool _bWarp)
			: fTime(_fTime), vPos(_rPos), vDir(_rDir), vUp(_rUp), bWarp(_bWarp), bUpdated(false)
		{

		}

		float		fTime;

		MVector3	vPos;
		MVector3	vDir;
		MVector3	vUp;

		bool		bWarp;
		bool		bUpdated;
	};

	std::vector< SKeyInfo > m_vKeyInfos;
	XCutSceneKeyFrameUpdator<SKeyInfo> m_KeyFrameUpdator;

	friend class		XCutSceneKeyFrameUpdator<SKeyInfo>;
	void				OnUpdateBetweenKey(SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext);
	void				OnUpdateOverKey(SKeyInfo* pProcKey);

	bool				m_bCatmullRom;
	bool				m_bUsingCollision;
	bool				m_bUsingUp;
	float				m_fLastWarpTime;
};
