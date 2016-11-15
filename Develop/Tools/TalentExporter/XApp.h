#ifndef _XAPP_H
#define _XAPP_H

#include "RD3DApplication.h"
#include "RActorNode.h"
#include "RActor.h"
#include "RMeshCollision.h"
#include "XTalentInfo.h"
#include "XTalentInfoMgr.h"
#include "TENPCInfo.h"
#include "XMotionMgr.h"

using namespace rs3;

class XTalentInfo;
class TENPCInfo;

/////////////////////////////////////////////////////////////////////////////////////////////////
/// 어플리케이션 클래스
class XApp : public RD3DApplication {
protected:
	struct _MoveSegment
	{
		int		nTimeIndex;
		vec3	vPos;
		float	fRot;
		_MoveSegment() : nTimeIndex(-1), vPos(vec3::ZERO), fRot(0.0f) {}
	};

	struct _TalentServerInfo
	{
		int						nTalentID;
		vector<_MoveSegment>	vecCastingSegment;
		vector<_MoveSegment>	vecCastingLoopSegment;
		vector<_MoveSegment>	vecMoveSegment;
		vector<_MoveSegment>	vecExtraMoveSegment;
		vector<_MoveSegment>	vecExtra2MoveSegment;
		vector<_MoveSegment>	vecExtra3MoveSegment;
		_TalentServerInfo() : nTalentID(0)	{}
	};

	struct _AniSeq
	{
		tstring		strAniName;
		float		fSpeed;
		int			nFrameOver;
		_AniSeq()
		{
			fSpeed = 1.0f;
			nFrameOver = -1;
		}
	};

	struct _NPCHitInfo_NodeSeg
	{
		uint32				nTime;
		vector<string>		vecCapsuleParentNames;
		vector<MCapsule>	vecCapsules;
	};

	enum _NPCHitInfo_StateType
	{
		ST_TALENT = 0,
		ST_ANI
	};

	struct _NPCHitInfo_State
	{
		_NPCHitInfo_StateType		nStateType;
		int							nTalentID;
		tstring						strAniName;
		vector<_NPCHitInfo_NodeSeg>	vecSegments;
		_NPCHitInfo_State()
		{
			nStateType = ST_TALENT;
			nTalentID = 0;
		}
	};

	struct _NPCHitInfo
	{
		int							nNPCID;
		bool						bSimpleCapsule;
		MCapsule					capSimple;
		vector<_NPCHitInfo_State>	vecStates;

		_NPCHitInfo() : nNPCID(0), bSimpleCapsule(true)	{}
	};
			

	vec3					m_vLastPos;
	float					m_fLastRot;

	char					m_szCmdLine[1024];
	virtual bool			OnCreate();
	virtual int				MainLoop();

	vector<_AniSeq>			MakeAniSequences(tstring& strAniName);
	void					MakeTalentServerInfo(int nTalentID, TECharacter& actor, vector<_AniSeq>& vecCastingSeqs, vector<_AniSeq>& vecCastingLoopSeqs, vector<_AniSeq>& vecAniSeqs, vector<_AniSeq>& vecExtraAniSeqs, vector<_AniSeq>& vecExtra2AniSeqs, vector<_AniSeq>& vecExtra3AniSeqs, vector<_TalentServerInfo>& vecTalentServerInfo);

	bool					SetActorAni(TECharacter& actor, _AniSeq& aniseq, bool bCheckDummyLoc);
	void					SetActorFrame(TECharacter& actor, int nFrame);
	void					AddTalentServerInfoNode(vector<_MoveSegment>& vecMove, int nTimeIndex, RActorNode* pLocActorNode, vec3& vLastPos, float& fLastRot, MMatrix& matAccum, MMatrix& matScale);
	void					AddTalentServerInfoNode(vector<_MoveSegment>& vecMove, vector<MMatrix>& vecMoveMatrix, vec3& vLastPos, float& fLastRot, MMatrix& matAccum, MMatrix& matScale);
	vector<_NPCHitInfo_NodeSeg> MakeStateSegments(TECharacter& actor, vector<_AniSeq>& vecAniSeqs);

	void 					AddNPCInfoSegNode( int nTimeIndex, rs3::RMeshCollision* pMeshCollision, TECharacter &actor, vector<XApp::_NPCHitInfo_NodeSeg>& vecTalentSegs );
	void 					Save(const char* szFileName, vector<_TalentServerInfo>& vecTalentServerInfo);

	void					SaveMoveSegment( vector<_MoveSegment> &vecMoveSegment, MXmlElement * pTalentInfo, tstring strName );
	void 					SaveNPCHitInfo(const char* szFileName, vector<_NPCHitInfo>& vecNPCHitInfo);

	bool 					SetupActor( TECharacter &actor, TENPCInfo* pNPCInfo );
	void 					ExportNPCHitInfo();
	void 					AddNPCHitInfoState( _NPCHitInfo &new_hit_info, TECharacter& actor, XTalentInfo* pTalentInfo, tstring strAniName );

	bool					CalAnimationPositionInfo( TECharacter &actor, vector<_AniSeq> &vecAniSeqs, vector<_MoveSegment> &vecMove, bool bExtraAni = false );
				
public:
							XApp();
	virtual					~XApp();
	virtual char*			GetName()				{ return "TalentExporter"; }

	void					Init(LPSTR lpCmdLine);
	void					Export();

	XTalentInfoMgr			m_TalentInfoMgr;
	TENPCInfoMgr			m_NPCInfoMgr;
	XMotionMgr				m_MotionMgr;
};


#endif	// _XAPP_H