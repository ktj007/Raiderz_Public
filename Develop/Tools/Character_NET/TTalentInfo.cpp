#include "StdAfx.h"
#include "TTalentInfo.h"
#include "CSTalentInfoParser.h"
#include "XTalentInfoLoader.h"
#include "XTalentInfoDef.h"
#include "CSTalentPosInfoLoader.h"
#include "CSTalentPosInfoSaver.h"
#include "CSHitInfoSaver.h"
#include "RAnimationResource.h"
#include "XMotionMgr.h"

using namespace System::IO;

TTalentInfoMgr::TTalentInfoMgr()
{

}

TTalentInfoMgr::~TTalentInfoMgr()
{
	Clear();
}

void TTalentInfoMgr::Clear()
{
	while(!empty()) 
	{
		CSTalentInfo* pTalentInfo = static_cast<CSTalentInfo*>(begin()->second);
		delete pTalentInfo;
		erase(begin());
	}
}

bool TTalentInfoMgr::Load()
{
	DWORD dwTest;
	CHECK_LOADING_TIME_START(dwTest);
	if(LoadTalent() == false)
	{
		if (System::Windows::Forms::MessageBox::Show("talent.xml 파일이 이상합니다.", "파일 이상."
			,MessageBoxButtons::OK) == System::Windows::Forms::DialogResult::OK)
		{
		}
	}
	CHECK_LOADING_TIME_END(dwTest, "  -- LoadTalent()");
	CHECK_LOADING_TIME_START(dwTest);
	if(LoadTalentPosInfo() == false)
	{
		if (System::Windows::Forms::MessageBox::Show("talent_pos_info.xml 파일이 이상합니다.", "파일 이상."
			,MessageBoxButtons::OK) == System::Windows::Forms::DialogResult::OK)
		{
		}
	}
	CHECK_LOADING_TIME_END(dwTest, "  -- LoadTalentPosInfo()");
	CHECK_LOADING_TIME_START(dwTest);
	if(LoadTalentExt() == false)
	{
		if (System::Windows::Forms::MessageBox::Show("talent_ext.xml 파일이 이상합니다.", "파일 이상."
			,MessageBoxButtons::OK) == System::Windows::Forms::DialogResult::OK)
		{
		}
	}
	CHECK_LOADING_TIME_END(dwTest, "  -- LoadTalentExt()");

	CHECK_LOADING_TIME_START(dwTest);
	XTalentInfoLoader talentInfoLoader;
	if(talentInfoLoader.LoadHitInfo(this, FILENAME_TALENT_HIT_INFO) == false)
	{
		if (System::Windows::Forms::MessageBox::Show("talent_hit_info.xml 파일이 이상합니다.", "파일 이상."
			,MessageBoxButtons::OK) == System::Windows::Forms::DialogResult::OK)
		{
		}
	}
	CHECK_LOADING_TIME_END(dwTest, "  -- LoadHitInfo()");

	// cooking
	CSTalentInfoMgr::Cooking();

	return true;
}

bool TTalentInfoMgr::LoadTalent()
{
	MXml xml;

	if (!xml.LoadFile(SERVER_FILENAME_TALENTINFO))
	{
		mlog("에러 : Failed To Load talent.xml\r");
		return false;
	}
	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT))
		{
			ParseTalent(pElement, &xml);
		}
	}
	xml.Clear();

	return true;
}

bool TTalentInfoMgr::LoadTalentPosInfo()
{
	CSTalentPosInfoLoader loader;
	if (loader.Load(this, SERVER_FILENAME_TALENT_POS_INFO) == false)
	{
		mlog("에러 : Failed To Load talent_pos_info.xml\r");
		return false;
	}

	return true;
}

bool TTalentInfoMgr::LoadTalentExt()
{
	TTalentInfoExtParser talentInfoExtParser;

	MXml xml;

	if(!xml.LoadFile(SERVER_FILENAME_TALENT_EXT)) 
	{
		mlog("에러 : Failed To Load talent_ext.xml\r");
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_XML_TAG_MAIET).FirstChildElement().Element();

	for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
	{
		if(!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT))
		{
			talentInfoExtParser.Parse(this, &xml, pElement);
		}
	}

	return true;
}



TTalentInfo* TTalentInfoMgr::Get( int nID, int nMode, bool bModeExactly )
{
	return static_cast<TTalentInfo*>(CSTalentInfoMgr::Find(nID, nMode, bModeExactly));
}

void TTalentInfoMgr::ParseTalent(MXmlElement* pElement, MXml* pXml)
{
	TTalentInfoParser talentInfoParser;

	string strName;
	int id = -1;
	int mode = 0;

	TTalentInfo* pNewInfo = NULL;
	bool new_talent = false;

	bool bLoop=false;
	bool bCancelEnabled = false;
	bool bWeaponAni = false;

	CSTalentInfoParserHelper talentInfoParserHelper;
	CSTalentInfoParserHelper::TalentInfoID talentInfoID = talentInfoParserHelper.ParseTalentID(pXml, pElement);
	id = talentInfoID.nID;
	mode = talentInfoID.nMode;

	if (mode > 0)
	{
		if (Exist(id, mode))
		{
			pNewInfo = Get(id, mode);
		}
	}
	else
	{
		pNewInfo = Get(id);
	}


	if (pNewInfo == NULL)
	{
		new_talent = true;
		pNewInfo = new TTalentInfo();

		if (mode > 0)
		{
			TTalentInfo* pDefaultTalentInfo = Get(id);
			if (pDefaultTalentInfo)
			{
				pDefaultTalentInfo->CopyAll(pNewInfo);
			}
		}
	}

	pNewInfo->m_nID = id;	
	pNewInfo->m_nMode = mode;	

	// 클라이언트, 서버 공통으로 읽는 값
	talentInfoParser.ParseTalentCommon(pNewInfo, pElement, pXml);

	//클라이언트
	talentInfoParser.ParseTalentClient(pNewInfo, pXml, pElement);

	// 서버
	talentInfoParser.ParseTalentServer(pNewInfo, pXml, pElement);

	// 툴
	talentInfoParser.ParseTalentTool(pNewInfo, pXml, pElement);
		

	if (new_talent)
	{
		Insert(pNewInfo);
	}
}



bool TTalentInfoMgr::SaveTalentPosInfo()
{
	CSTalentPosInfoSaver saver;
	saver.Save(this, SERVER_FILENAME_TALENT_POS_INFO);

	return true;
}

bool TTalentInfoMgr::SaveTalentHitInfo()
{
	_SortHitInfos();

	CSHitInfoSaver hitInfoSaver;
	bool bSave = hitInfoSaver.SaveHitInfo(this, FILENAME_TALENT_HIT_INFO);

	if (bSave)
	{
		File::Copy(FILENAME_TALENT_HIT_INFO, SERVER_FILENAME_TALENT_HIT_INFO, true);
	}

	return bSave;
}


bool TTalentInfoMgr::TalentPosInfoRefresh(CSTalentInfo* pTalentInfo)
{
	if (pTalentInfo == NULL) return false;
	if (pTalentInfo->m_setNPCID.empty()) return false;

	vector<_TalentServerInfo>	vecTalentServerInfo;

	TNPCInfo* pNPCInfo = g_pMainApp->m_NPC.GetNPCInfo(*pTalentInfo->m_setNPCID.begin());
	if (pNPCInfo == NULL)
		return false;

	TCharacter actor;
	if (SetupActor(actor, pNPCInfo) == false) return false;

	vector<_AniSeq>	vecCastingAniSeqs		= MakeAniSequences(pTalentInfo->m_szCastingAnimation);
	vector<_AniSeq>	vecCastingLoopAniSeqs	= MakeAniSequences(pTalentInfo->m_szCastingLoopAnimation);
	vector<_AniSeq>	vecAniSeqs				= MakeAniSequences(pTalentInfo->m_szUseAnimation);
	vector<_AniSeq>	vecExtraAniSeqs			= MakeAniSequences(pTalentInfo->m_szExtraActAnimation);
	vector<_AniSeq>	vecExtra2AniSeqs		= MakeAniSequences(pTalentInfo->m_szExtraActAnimation2);
	vector<_AniSeq>	vecExtra3AniSeqs		= MakeAniSequences(pTalentInfo->m_szExtraActAnimation3);

	MakeTalentServerInfo(pTalentInfo->m_nID, actor, vecCastingAniSeqs, vecCastingLoopAniSeqs, vecAniSeqs, vecExtraAniSeqs, vecExtra2AniSeqs, vecExtra3AniSeqs, vecTalentServerInfo);

	if(vecTalentServerInfo.size() == 0) 
	{
		if(pTalentInfo->m_MovingInfoList.size())
		{
			pTalentInfo->m_MovingInfoList.clear();
			return true;
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	bool bNew = false;
	// Casting
	if(vecTalentServerInfo[0].vecCastingMoveSegment.size() > 0 &&
		CheckReSetAniPosInfo(pTalentInfo->m_vCastingPos, pTalentInfo->m_fCastingRot, vecTalentServerInfo[0].vecCastingMoveSegment[0]))
	{
		SetPosAniInfo(pTalentInfo->m_vCastingPos, pTalentInfo->m_fCastingRot, vecTalentServerInfo[0].vecCastingMoveSegment[0]);
		bNew = true;
	}

	// Casting Loop
	if(vecTalentServerInfo[0].vecCastingLoopMoveSegment.size() > 0 &&
		CheckReSetAniPosInfo(pTalentInfo->m_vCastingLoopPos, pTalentInfo->m_fCastingLoopRot, vecTalentServerInfo[0].vecCastingLoopMoveSegment[0]))
	{
		SetPosAniInfo(pTalentInfo->m_vCastingLoopPos, pTalentInfo->m_fCastingLoopRot, vecTalentServerInfo[0].vecCastingLoopMoveSegment[0]);
		bNew = true;
	}

	// Use
	if(CheckReSetAniPosInfo(&pTalentInfo->m_MovingInfoList, &vecTalentServerInfo[0].vecUseMoveSegment))
	{
		SetPosAniInfo(&pTalentInfo->m_MovingInfoList, &vecTalentServerInfo[0].vecUseMoveSegment);
		bNew = true;
	}

	// Extra
	if(vecTalentServerInfo[0].vecExtraMoveSegment.size() > 0 &&
		CheckReSetAniPosInfo(pTalentInfo->m_vExtraPos, pTalentInfo->m_fExtraRot, vecTalentServerInfo[0].vecExtraMoveSegment[0]))
	{
		SetPosAniInfo(pTalentInfo->m_vExtraPos, pTalentInfo->m_fExtraRot, vecTalentServerInfo[0].vecExtraMoveSegment[0]);
		bNew = true;
	}

	// Extra2
	if(vecTalentServerInfo[0].vecExtra2MoveSegment.size() > 0 &&
		CheckReSetAniPosInfo(pTalentInfo->m_vExtra2Pos, pTalentInfo->m_fExtra2Rot, vecTalentServerInfo[0].vecExtra2MoveSegment[0]))
	{
		SetPosAniInfo(pTalentInfo->m_vExtra2Pos, pTalentInfo->m_fExtra2Rot, vecTalentServerInfo[0].vecExtra2MoveSegment[0]);
		bNew = true;
	}

	return bNew;
}

void TTalentInfoMgr::AddTalentServerInfoNode( vector<_MoveSegment>& vecMove, int nTimeIndex, RActorNode* pLocActorNode, vec3& vLastPos, float& fLastRot, MMatrix& matAccum, MMatrix& matScale )
{
	_MoveSegment new_move_seg;

	if (pLocActorNode) 
	{
		MMatrix matTemp = pLocActorNode->GetWorldTransform() * matAccum * matScale;
		new_move_seg.vPos = vec3(matTemp._41,matTemp._42,matTemp._43);

		vec3 v1 = vec3(0,-1,0);
		vec3 v2 = vec3(matTemp._21, matTemp._22, matTemp._23);
		new_move_seg.fRot = v1.AngleToXY(v2);
	}
	else
	{
		new_move_seg.vPos = vLastPos;
		new_move_seg.fRot = fLastRot;
	}

	new_move_seg.nTimeIndex = nTimeIndex;

	vecMove.push_back(new_move_seg);

	vLastPos = new_move_seg.vPos;
	fLastRot = new_move_seg.fRot;
}

void TTalentInfoMgr::AddTalentServerInfoNode( vector<_MoveSegment>& vecMove, vector<MMatrix>& vecMoveMatrix, vec3& vLastPos, float& fLastRot, MMatrix& matAccum, MMatrix& matScale )
{
	int nTimeIndex = 0;
	for(vector<MMatrix>::iterator it = vecMoveMatrix.begin(); it != vecMoveMatrix.end(); ++it)
	{
		_MoveSegment new_move_seg;
		
		MMatrix matTemp = (*it) * matAccum * matScale;
		new_move_seg.vPos = vec3(matTemp._41,matTemp._42,matTemp._43);

		vec3 v1 = vec3(0,-1,0);
		vec3 v2 = vec3(matTemp._21, matTemp._22, matTemp._23);
		new_move_seg.fRot = v1.AngleToXY(v2);

		new_move_seg.nTimeIndex = nTimeIndex;
	
		vecMove.push_back(new_move_seg);

		vLastPos = new_move_seg.vPos;
		fLastRot = new_move_seg.fRot;

		++nTimeIndex;
	}
}
// TODO : 
//		: void MakeTalentServerInfo(CSTalentInfo* pTalentInfo, rs3::RActor& actor, vector<_AniSeq>& vecAniSeqs);
void TTalentInfoMgr::MakeTalentServerInfo( int nTalentID, TCharacter& actor, vector<_AniSeq>& vecCastingAniSeqs, vector<_AniSeq>& vecCastingLoopAniSeqs, vector<_AniSeq>& vecUseAniSeqs, vector<_AniSeq>& vecExtraAniSeqs, vector<_AniSeq>& vecExtra2AniSeqs, vector<_AniSeq>& vecExtra3AniSeqs, vector<_TalentServerInfo>& vecTalentServerInfo )
{
	_TalentServerInfo		new_talent_info;
	new_talent_info.nTalentID = nTalentID;

	RMeshAnimationMgr* pAMGR = &actor.m_pMesh->m_AniMgr;

	bool bCalSucesse = false;
	bCalSucesse |= CalAnimationPositionInfo(actor, vecCastingAniSeqs, new_talent_info.vecCastingMoveSegment, true);
	bCalSucesse |= CalAnimationPositionInfo(actor, vecCastingLoopAniSeqs, new_talent_info.vecCastingLoopMoveSegment, true);
	bCalSucesse |= CalAnimationPositionInfo(actor, vecUseAniSeqs, new_talent_info.vecUseMoveSegment);
	bCalSucesse |= CalAnimationPositionInfo(actor, vecExtraAniSeqs, new_talent_info.vecExtraMoveSegment, true);
	bCalSucesse |= CalAnimationPositionInfo(actor, vecExtra2AniSeqs, new_talent_info.vecExtra2MoveSegment, true);
	bCalSucesse |= CalAnimationPositionInfo(actor, vecExtra3AniSeqs, new_talent_info.vecExtra3MoveSegment, true);

	if (bCalSucesse)
	{
		vecTalentServerInfo.push_back(new_talent_info);
	}
}

vector<TTalentInfoMgr::_AniSeq> TTalentInfoMgr::MakeAniSequences(string& strAniName)
{
	vector<_AniSeq> ret;

	// 모션인지 체크
	if (XMotionMgr::IsTalentMotionName(strAniName.c_str()))
	{
		XMotion* pCurrMotion = g_pMainApp->GetMotion()->GetMotionMgr()->GetMotion(strAniName.c_str());
		if (pCurrMotion == NULL) return ret;

		vector<XMotionAniSequence*>& vecMotionSeq = pCurrMotion->m_vecAniSequence[MT_DEFAULT];

		for (size_t n = 0; n < vecMotionSeq.size(); n++)
		{
			_AniSeq new_seq;
			new_seq.strAniName	= vecMotionSeq[n]->GetAniName();
			new_seq.fSpeed		= vecMotionSeq[n]->GetSpeed();
			new_seq.nFrameOver	= vecMotionSeq[n]->GetFrameOver();
			ret.push_back(new_seq);
		}
	}
	else
	{
		_AniSeq new_seq;
		new_seq.strAniName = strAniName;
		ret.push_back(new_seq);
	}

	return ret;
}

bool TTalentInfoMgr::SetupActor( TCharacter &actor, TNPCInfo* pNPCInfo )
{
	string strMeshFileName = PATH_MODEL_TOOL + pNPCInfo->strMeshPath + pNPCInfo->strMeshName + ".elu";
	
	if( !ExistFile(strMeshFileName.c_str()))
	{
		mlog("에러 : 파일이 존재하지 않습니다. - %s \r", strMeshFileName.c_str());
		return false;
	}

	if (actor.Create(strMeshFileName.c_str()) == false)
	{
		return false;
	}
	actor.SetScale(vec3(pNPCInfo->fScale,pNPCInfo->fScale,pNPCInfo->fScale));
	if (actor.m_pMesh == NULL) return false;

	return true;
}

bool TTalentInfoMgr::SetActorAni(TCharacter& actor, _AniSeq& aniseq, bool bCheckDummyLoc)
{
	string strCurrAniName = aniseq.strAniName;

	// 애니메이션 세팅
	RAnimation* pAnimation = actor.GetAnimation((char*)strCurrAniName.c_str());
	if( pAnimation == NULL) return false;
	if (actor.SetAnimation(strCurrAniName) == false) return false;
	actor.SetAnimationSpeed(pAnimation->GetSpeed());

	string name = pAnimation->GetAliasName();

	if (bCheckDummyLoc)
	{
		RAnimationNode* pLocNode = pAnimation->GetResource()->GetNode("dummy_loc");
		if (pLocNode == NULL) return false;
	}

	return true;
}

void TTalentInfoMgr::SetActorFrame(TCharacter& actor, int nFrame)
{
	actor.SetAnimationFrame(nFrame);
	actor.ForceUpdateBaseAnimationTC();
}

bool TTalentInfoMgr::CalAnimationPositionInfo( TCharacter &actor, vector<_AniSeq> &vecAniSeqs, vector<_MoveSegment> &vecMove, bool bEndAni /*= false */ )
{
	int nTimeIndex = 0;

	vec3 vLastPos = vec3::ZERO;
	float fLastRot = 0.0f;

	bool bIsMotion = !(vecAniSeqs.size() == 1);
	bool bExistDummyLoc = false;
	MMatrix matAccum;
	matAccum.MakeIdentity();

	MMatrix matScale;
	matScale.MakeIdentity();
	matScale.SetScale(actor.GetScale());

	for (size_t ani_index = 0; ani_index < vecAniSeqs.size(); ani_index++)
	{
		_AniSeq& currAniSeq = vecAniSeqs[ani_index];

		if (SetActorAni(actor, currAniSeq, true) == true) 
		{
			bExistDummyLoc = true;
		}
		else
		{
			if (bIsMotion == false) 
				break;
		}

		actor.AddActiveActorNode("dummy_loc");
		RActorNode* pLocActorNode = actor.GetActorNode("dummy_loc");
		if(pLocActorNode == NULL)
			continue;

		const int FRAME_STEP = (int)( (4800 * currAniSeq.fSpeed) /10);
		int nMaxFrame = actor.GetAnimationMaxFrame();
		if (currAniSeq.nFrameOver > 0) 
			nMaxFrame = currAniSeq.nFrameOver;

		if(bEndAni == false)
		{
			// ExtraAni만 여러번 하지 않는다.
			vector< MMatrix > vecMatrix;
			actor.GetSamplingActorNodeData("dummy_loc", 0, nMaxFrame, FRAME_STEP, vecMatrix);
			AddTalentServerInfoNode(vecMove, vecMatrix, vLastPos, fLastRot, matAccum, matScale);

			nTimeIndex += vecMatrix.size();

			int nLastIndex = vecMatrix.size();
			if(vecMatrix.empty() == false && nLastIndex > 0)
			{
				matAccum = vecMatrix[nLastIndex - 1] * matAccum;
			}
		}
		else
		{
			// 마지막 더미 위치 세팅
			// 오직 하나
			vector< RMatrix > vecMatrix;
			actor.GetSamplingActorNodeData("dummy_loc", nMaxFrame, nMaxFrame, FRAME_STEP, vecMatrix);
			AddTalentServerInfoNode(vecMove, vecMatrix, vLastPos, fLastRot, matAccum, matScale);

			nTimeIndex++;

			int nLastIndex = vecMatrix.size();
			if(vecMatrix.empty() == false && nLastIndex > 0)
				matAccum = vecMatrix[0] * matAccum;
		}
	}

	// dummy_loc 노드만 있고 실제로 움직이지 않았으면 지워줌
	vLastPos = vec3::ZERO;
	float fDist = 0.0f;
	for (size_t i = 0; i < vecMove.size(); i++)
	{
		_MoveSegment& move_seg = vecMove[i];
		fDist += move_seg.vPos.DistanceToSq(vLastPos);
	}

	bool bRotEqual = true;

	if (vecMove.size() > 0)
	{
		float fRot1 = vecMove[0].fRot;
		float fRot2 = vecMove[vecMove.size()-1].fRot;
		if (MMath::IsZero(fRot1-fRot2, 0.001f) == false)
		{
			bRotEqual = false;
		}
	}

	if (fDist < 0.01f && bRotEqual)
	{
		vecMove.clear();
	}

	return (bExistDummyLoc && vecMove.empty() == false);
}

bool TTalentInfoMgr::CheckTalentInfoModify()
{
	// 수정이 되었다.
	return ReLoad();
}

bool TTalentInfoMgr::ReLoad()
{
	Clear();
	bool bResult = Load();

	GlobalObjects::g_pMainForm->m_NPCListTabPage->ReLoadNPCList();
	GlobalObjects::g_pMainForm->m_PCTalenttreeView->ReLoadPCTalentInfo();

	return bResult;
}

bool TTalentInfoMgr::CheckReSetAniPosInfo( vector<CSTalentInfo::_TALENT_MOVE_INFO>* pTalentMoveInfo, vector<_MoveSegment>* vecMoveSegment )
{
	if (pTalentMoveInfo->size() != vecMoveSegment->size()) 
		return true;


	for( size_t i =0; i < vecMoveSegment->size(); i++)
	{
		CSTalentInfo::_TALENT_MOVE_INFO info;
		info.fRot = (*vecMoveSegment)[i].fRot;
		info.vPos = (*vecMoveSegment)[i].vPos;

		if (i >= pTalentMoveInfo->size())
		{
			return true;
		}

		CSTalentInfo::_TALENT_MOVE_INFO old_info = (*pTalentMoveInfo)[i];
		if (MMath::Equals(old_info.fRot, info.fRot, 0.1f) == false ||
			MMath::Equals(old_info.vPos, info.vPos, 2.0f) == false)
		{
			return true;
		}
	}

	return false;
}

bool TTalentInfoMgr::CheckReSetAniPosInfo( vec3& vPos, float& fDir, _MoveSegment& moveSegment )
{
	if (MMath::Equals(fDir, moveSegment.fRot, 0.1f) == false ||
		MMath::Equals(vPos, moveSegment.vPos, 2.0f) == false)
	{
		return true;
	}

	return false;
}

void TTalentInfoMgr::SetPosAniInfo( vector<CSTalentInfo::_TALENT_MOVE_INFO>* pTalentMoveInfo, vector<_MoveSegment>* vecMoveSegment )
{
	pTalentMoveInfo->clear();
	for( size_t i =0; i < vecMoveSegment->size(); i++)
	{
		CSTalentInfo::_TALENT_MOVE_INFO info;
		info.fRot = (*vecMoveSegment)[i].fRot;
		info.vPos = (*vecMoveSegment)[i].vPos;
		pTalentMoveInfo->push_back(info);
	}
}

void TTalentInfoMgr::SetPosAniInfo( vec3& vOutPos, float& fOutDir, _MoveSegment& moveSegment )
{
	vOutPos = moveSegment.vPos;
	fOutDir	= moveSegment.fRot;
}


void TTalentInfoParser::ParseTalentTool(CSTalentInfo* pNewInfo, MXml* pXml, MXmlElement* pElement)
{
	// 툴은 이름을 문자열테이블키로 찾지 않고, xml에 기술은 그대로 사용한다.
	_Attribute(pNewInfo->m_szName,	 pElement, TALENT_XML_ATTR_NAME, pXml);
}


void TTalentInfoExtParser::_Parse( MXmlElement* pElement, MXml* pXml, CSTalentInfo* pTalentInfo )
{
	XTalentInfoExtParser::_Parse(pElement, pXml, pTalentInfo);
	CSTalentInfoExtParser::_ParseExt_Server(pElement, pXml, pTalentInfo);
}



