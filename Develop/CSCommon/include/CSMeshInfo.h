#pragma once

#include "MTypes.h"
#include "MFileSystem.h"
#include "CSCommonLib.h"
#include "MTstring.h"

#define HUMAN_RADIUS 40.0f
#define HUMAN_TOP 150.0f

//////////////////////////////////////////////////////////////////////////
// 판정
class CSCOMMON_API CSHitCapsule
{
private:
		bool							m_bAlwaysActive;
		float							m_fActiveTimeBegin;
		float							m_fActiveTimeEnd;

public:
	CSHitCapsule() : m_fActiveTimeBegin(0.0f), m_fActiveTimeEnd(0.0f), m_bAlwaysActive(true), nLinkedBPart(0), m_nHitEffectCapsuleID(-1) {}
		MCapsule						m_Capsule;

		int								nLinkedBPart;
		int								m_nHitEffectCapsuleID;

		void							SetActiveTime(float fBeginTime, float fEndTime);
		bool							CheckActiveTime (float fTime) const;
		bool							IsAlwaysActive() const									{ return m_bAlwaysActive; }
		float							GetActiveTimeBegin() const								{ return m_fActiveTimeBegin; }
		float							GetActiveTimeEnd() const								{ return m_fActiveTimeEnd; }

		bool							IsHitFirst() const										{ return nLinkedBPart != 0; }
};


class CSCOMMON_API CSHitGroup
{
public:
		int								m_nGroupID;
		vector<CSHitCapsule>			m_vecHitCapsules;

		size_t							GetHitCapsuleCount()									{ return m_vecHitCapsules.size(); }
		CSHitCapsule *					GetHitCapsuleByCount(int nIndex);
};

//////////////////////////////////////////////////////////////////////////
// 충돌
class CSCOMMON_API CSColCapsule
{
public:
	MCapsule							m_Capsule;

	tstring								m_strBoneTop;
	tstring								m_strBoneBottom;
};

class CSCOMMON_API CSColBox : public MBox
{
public:
	MMatrix								m_matWorld;

	CSColBox& operator=(const CSColBox& other)	{ vmin = other.vmin; vmax = other.vmax; m_matWorld = other.m_matWorld; return *this; }

};

//////////////////////////////////////////////////////////////////////////
// 애니
class CSCOMMON_API CSAniInfo
{
public:
	tstring								m_strAnimationName;
	float								m_fAnimationTime;

	CSAniInfo()
	{
		m_fAnimationTime			= 0.0f;
	}
};

//////////////////////////////////////////////////////////////////////////
// 메쉬정보
class CSCOMMON_API CSMeshInfo
{
public:
		tstring							m_strMeshName;

		map<int, CSHitGroup>			m_mapHitGroupInfo;
		vector<CSColCapsule>			m_vecColCapsules;
		vector<CSColBox>				m_vecColBox;
		vector<CSAniInfo>				m_vecAniInfo;
		vector<CSColCapsule>			m_vecHitEffectInfo;

		//tstring							m_strReActionAnimationName;
		//float							m_fReActionBlendWeight;
		//vector<tstring>					m_vecNotReActionAnimationList;

		tstring							m_strBPartsStartSound[4];
		tstring							m_strBPartsCollisionSound[4];

		//set<tstring>					m_setNotReActionAnimationList;	// 게임에서 검색용으로 사용

		CSMeshInfo() /*: m_fReActionBlendWeight(0.0f)*/ {}

		int								FindHitEffectCapsuleIndex(int nGroupID, int nCapsuleID);
		int								FindPartsHitCapsuleIndex(int nGroupID, int nIndex);
		CSHitGroup *					FineHitGroup(int nGroupID);

		CSAniInfo*						FindAniInfo(const tstring& aniName);

		//bool							IsExistNotReActionAnimation(const tstring& aniName);
};


class CSCOMMON_API CSMeshInfoMgr
{
public:
		map<tstring, CSMeshInfo>			m_mapMeshInfo;

		CSMeshInfo*						GetInfo(const tstring& key);
		int								GetSize()			{ return (int)m_mapMeshInfo.size();}
};
