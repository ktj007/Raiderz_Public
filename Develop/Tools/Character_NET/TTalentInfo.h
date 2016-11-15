#pragma once

#include "main/GTalentDef.h"
#include "XTalentInfoMgr.h"
#include "XTalentInfoParser.h"
#include "XTalentInfoExtParser.h"

class TTalentInfo : public XTalentInfo
{
public:

};

class TTalentInfoMgr;

class TTalentInfoParser : public XTalentInfoParser
{
public:
	TTalentInfoParser() {}
	virtual ~TTalentInfoParser() {}

	using XTalentInfoParser::ParseTalentClient;
	using XTalentInfoParser::ParseTalentServer;

	void ParseTalentTool(CSTalentInfo* pNewInfo, MXml* pXml, MXmlElement* pElement);
};

class TTalentInfoExtParser : public XTalentInfoExtParser
{
protected:
	virtual void _Parse(MXmlElement* pElement, MXml* pXml, CSTalentInfo* pTalentInfo) override;
public:
	TTalentInfoExtParser() {}
	virtual ~TTalentInfoExtParser() {}
};

class TTalentInfoMgr : public CSTalentInfoMgr
{
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
		vector<_MoveSegment>	vecCastingMoveSegment;
		vector<_MoveSegment>	vecCastingLoopMoveSegment;
		vector<_MoveSegment>	vecUseMoveSegment;
		vector<_MoveSegment>	vecExtraMoveSegment;
		vector<_MoveSegment>	vecExtra2MoveSegment;
		vector<_MoveSegment>	vecExtra3MoveSegment;
		_TalentServerInfo() : nTalentID(0)	{}
	};

	struct _AniSeq
	{
		string		strAniName;
		float		fSpeed;
		int			nFrameOver;
		_AniSeq()
		{
			fSpeed = 1.0f;
			nFrameOver = -1;
		}
	};

protected:
	//talent
	void					ParseTalent(MXmlElement* pElement, MXml* pXml);

	bool					SetupActor( TCharacter &actor, TNPCInfo* pNPCInfo );
	vector<_AniSeq>			MakeAniSequences(string& strAniName);
	void 					MakeTalentServerInfo(int nTalentID, TCharacter& actor, vector<_AniSeq>& vecCastingAniSeqs, vector<_AniSeq>& vecCastingLoopAniSeqs, vector<_AniSeq>& vecUseAniSeqs, vector<_AniSeq>& vecExtraAniSeqs, vector<_AniSeq>& vecExtra2AniSeqs, vector<_AniSeq>& vecExtra3AniSeqs, vector<_TalentServerInfo>& vecTalentServerInfo);
	bool 					SetActorAni(TCharacter& actor, _AniSeq& aniseq, bool bCheckDummyLoc);
	void 					AddTalentServerInfoNode(vector<_MoveSegment>& vecMove, int nTimeIndex, RActorNode* pLocActorNode, vec3& vLastPos, float& fLastRot, MMatrix& matAccum, MMatrix& matScale);
	void 					AddTalentServerInfoNode(vector<_MoveSegment>& vecMove, vector<MMatrix>& vecMoveMatrix, vec3& vLastPos, float& fLastRot, MMatrix& matAccum, MMatrix& matScale);

	bool					CalAnimationPositionInfo( TCharacter &actor, vector<_AniSeq> &vecAniSeqs, vector<_MoveSegment> &vecMove, bool bEndAni = false );

	bool					ReLoad();

	bool					CheckReSetAniPosInfo(vector<CSTalentInfo::_TALENT_MOVE_INFO>* pTalentMoveInfo, vector<_MoveSegment>* vecMoveSegment);
	bool					CheckReSetAniPosInfo(vec3& vPos, float& fDir, _MoveSegment& moveSegment);
	void					SetPosAniInfo(vector<CSTalentInfo::_TALENT_MOVE_INFO>* pTalentMoveInfo, vector<_MoveSegment>* vecMoveSegment);
	void					SetPosAniInfo(vec3& vOutPos, float& fOutDir, _MoveSegment& moveSegment);

public:
	using CSTalentInfoMgr::CopyHitFromReference;

	TTalentInfoMgr();
	virtual ~TTalentInfoMgr();

	bool					Load();
	bool					LoadTalentPosInfo();
	bool					LoadTalent();
	bool					LoadTalentExt();

	bool					SaveTalentPosInfo();
	bool					SaveTalentHitInfo();

	TTalentInfo*			Get(int nID, int nMode=0, bool bModeExactly=false);
	void					Clear();

	bool					TalentPosInfoRefresh(CSTalentInfo* pTalentInfo);
	void					SetActorFrame(TCharacter& actor, int nFrame);

	bool					CheckTalentInfoModify();
};