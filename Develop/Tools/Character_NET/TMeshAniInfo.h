#pragma once

#include "TMeshInfo.h"
#include "CSMeshInfo.h"

class TMeshInfoMgr;

//////////////////////////////////////////////////////////////////////////
class TMeshAniInfoMgr
{
private:
	TMeshInfoMgr *				m_pMeshInfo;
	vector<string>				m_vecAnimationTimeList;

	struct stANIMATIONXMLDATA
	{
		string					animationName;
		string					fileName;
		float					fSpeed;
		int						nMaxFrame;

		stANIMATIONXMLDATA()
		{
			fSpeed				= 0.0f;
			nMaxFrame			= 0;
		}
	};

private:
	CSAniInfo *					GetAnimationInfo(CSMeshInfo * pMeshInfo, string strAnimationName);
	float						GetAnimationTime(RAnimation* pAnimation, float fSpeed);
	float						GetAnimationTime(stANIMATIONXMLDATA& animationData);

	stANIMATIONXMLDATA			CheckExistAnimation(const string& strDirPath, const string& strModelName, const string& strAnimationName);
	RAnimation*					CheckExistAnimation(RActor* pActor, const string& strAnimationName);

	void						AnimationTimeChangeView(string strAnimation, float fPreTime, float fCurrentTime);

public:
	TMeshAniInfoMgr(TMeshInfoMgr* pMeshInfo);

	bool						RefreshMeshAniInfo(string strModelName, string strModelPath);
	bool						RefreshMeshAniInfo(RActor* pActor);
};
