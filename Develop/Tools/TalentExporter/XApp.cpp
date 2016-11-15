#include "stdafx.h"
#include "XApp.h"
#include "REngine.h"
#include "RConfiguration.h"
#include "XTalentInfo.h"
#include "MDebug.h"
#include "RActor.h"
#include "RAnimation.h"
#include "RMeshMgr.h"
#include "XMotion.h"
#include "MCollision.h"
#include "RAnimationResource.h"
#include "XMeshInfoLoader.h"
#include "TETransformControllerManager.h"

#define T_PATH_SHADER				"../../data/Shader/;../../data/Shader/components/"
#define T_PATH_MODEL				_T("../../data/model/")
#define T_PATH_MONSTER				_T("../../data/Model/Monster/")
#define T_FILENAME_TALENT			_T("../../data/system/talent.xml")
#define T_FILENAME_TALENT_EXT		_T("../../data/system/talent_ext.xml")
#define T_FILENAME_TALENTHITINFO	_T("../../data/system/talent_hit_info.xml")
#define T_FILENAME_NPC				_T("../../data/system/npc.xml")
#define T_FILENAME_NPC_CHANGEMTRL	_T("../../data/system/npc_changemtrl.xml")
#define T_FILENAME_MOTION			_T("../../data/system/motion.xml")
#define T_FILENAME_MOTION_NPC		_T("../../data/system/motion_npc.xml")
#define T_FILENAME_MOTION_PC		_T("../../data/system/motion_pc.xml")
#define T_FILENAME_MESH_INFO		_T("../../data/system/mesh_info.xml")

#define T_EXPORT_OUT_FILENAME		_T("../GameServer/system/talent_pos_info.xml")
#define T_EXPORT_OUT_NPC_HITINFO	_T("../GameServer/system/npc_hit_info.xml")

#define T_PATH_VALIDATOR			_T("../GameServer/Validator/")

XApp::XApp()
{
	USES_CONVERSION_EX;

	rs3::REngine::GetConfig().m_bFullScreen = false;
	rs3::REngine::GetConfig().m_nWidth = 640;
	rs3::REngine::GetConfig().m_nHeight = 480;

	MCreateFileSystem( MFILEACCESS_GENERIC, "../../Data");

	CSStrings::Init();

	m_szCmdLine[0] = 0;
}

XApp::~XApp()
{

}

void XApp::Init(LPSTR lpCmdLine)
{
	strcpy_s(m_szCmdLine, lpCmdLine);

}

bool XApp::OnCreate()
{
	// Talent
	if (m_TalentInfoMgr.Load(T_FILENAME_TALENT, T_FILENAME_TALENT_EXT, T_FILENAME_TALENTHITINFO) == false)
	{
		mlog("Fail To Load Talent Files\n");
	}

	// npc
	if (m_NPCInfoMgr.Load() == false)
	{
		mlog("Fail To Load NPC File\n");
	}

	// motion
	m_MotionMgr.Init(T_FILENAME_MOTION, T_FILENAME_MOTION_PC, T_FILENAME_MOTION_NPC);

	return true;
}


int XApp::MainLoop()
{
	CreateDirectory(T_PATH_VALIDATOR, NULL);

	wstring strPath = T_PATH_VALIDATOR + wstring(L"pre_talent_pos_info.xml");
	bool bCopy = CopyFile(T_EXPORT_OUT_FILENAME, strPath.c_str(), false);

	Export();
	//ExportNPCHitInfo();

	return 0;
}

void XApp::Export()
{
	vector<_TalentServerInfo>	vecTalentServerInfo;

	XTalentInfoMgr& talentinfo_mgr = m_TalentInfoMgr;

	for( XTalentInfoMgr::iterator itorTalentInfo = talentinfo_mgr.begin(); itorTalentInfo != talentinfo_mgr.end(); ++itorTalentInfo)
	{
		XTalentInfo* pTalentInfo = static_cast<XTalentInfo*>((*itorTalentInfo).second);
		if (pTalentInfo->m_setNPCID.empty()) continue;

		if(pTalentInfo->m_nID == 210701101)
		{
			int a = 0;
		}
		TENPCInfo* pNPCInfo = m_NPCInfoMgr.Get(*pTalentInfo->m_setNPCID.begin());
		if (pNPCInfo == NULL) continue;

		TECharacter actor;
		if (SetupActor(actor, pNPCInfo) == false) continue;

		mlog("- 탤런트 익스포터 %d   ", pTalentInfo->m_nID);
		
		vector<_AniSeq>	vecCastingSeqs = MakeAniSequences(pTalentInfo->m_szCastingAnimation);
		vector<_AniSeq>	vecCastingLoopSeqs = MakeAniSequences(pTalentInfo->m_szCastingLoopAnimation);
		vector<_AniSeq>	vecAniSeqs = MakeAniSequences(pTalentInfo->m_szUseAnimation);
		vector<_AniSeq>	vecExtraAniSeqs = MakeAniSequences(pTalentInfo->m_szExtraActAnimation);
		vector<_AniSeq>	vecExtra2AniSeqs = MakeAniSequences(pTalentInfo->m_szExtraActAnimation2);
		vector<_AniSeq>	vecExtra3AniSeqs = MakeAniSequences(pTalentInfo->m_szExtraActAnimation3);

		MakeTalentServerInfo(pTalentInfo->m_nID, actor, vecCastingSeqs, vecCastingLoopSeqs, vecAniSeqs, vecExtraAniSeqs, vecExtra2AniSeqs, vecExtra3AniSeqs, vecTalentServerInfo);

		mlog("   -----------> 완료\n");
	}

	USES_CONVERSION_EX;
	Save(W2A_EX(T_EXPORT_OUT_FILENAME, 0), vecTalentServerInfo);

	mlog("Export OK!\n");
}

void XApp::ExportNPCHitInfo()
{
	vector<_NPCHitInfo>	vecNPCHitInfo;

	XTalentInfoMgr& talentinfo_mgr = m_TalentInfoMgr;

	for (TENPCInfoMgr::iterator itor = m_NPCInfoMgr.begin(); itor != m_NPCInfoMgr.end(); ++itor)
	{
		TENPCInfo* pNPCInfo = static_cast<TENPCInfo*>((*itor).second);

		TECharacter actor;
		if (SetupActor(actor, pNPCInfo) == false) continue;

		rs3::RMeshCollision* pMeshCollision = actor.m_pMesh->m_pMeshCollision;

		_NPCHitInfo new_hit_info;
		new_hit_info.nNPCID = pNPCInfo->nID;
		if(pMeshCollision && !pMeshCollision->empty())
		{
			new_hit_info.bSimpleCapsule = false;

			// 탤런트별로 추린다.
			for( XTalentInfoMgr::iterator itorTalentInfo = talentinfo_mgr.begin(); itorTalentInfo != talentinfo_mgr.end(); ++itorTalentInfo)
			{
				XTalentInfo* pTalentInfo = static_cast<XTalentInfo*>((*itorTalentInfo).second);
				if (pTalentInfo->m_setNPCID.empty()) continue;

				if (*pTalentInfo->m_setNPCID.begin() != pNPCInfo->nID) continue;

				AddNPCHitInfoState(new_hit_info, actor, pTalentInfo, L"" );
			}

			// 애니메이션 별로 추린다.
			AddNPCHitInfoState(new_hit_info, actor, NULL, L"idle" );
			AddNPCHitInfoState(new_hit_info, actor, NULL, L"walk" );
			AddNPCHitInfoState(new_hit_info, actor, NULL, L"run" );
		}
		else
		{
			new_hit_info.bSimpleCapsule = true;

			RBox b;
			actor.m_pMesh->GetStaticSkeletonBoundingBox(b);
			if (b.maxx <= -FLT_MAX)
			{
				b.vmax.Set(0, 0, 0);
				b.vmin.Set(0, 0, 0);
			}
			RVector c = b.GetCenter();

			float rad, bot, top;

			rad = b.maxx - c.x;
			if (rad < b.maxy - c.y)
				rad = b.maxy - c.y;

			rad *= 1.2f;

			bot = rad;
			if (bot > c.z)
				bot = c.z;

			top = b.maxz - rad;
			if (top < c.z)
				top = c.z;

			top += top * 0.1f;


			new_hit_info.capSimple = MCapsule(RVector(0, 0, bot), RVector(0, 0, top), rad);
		}


		vecNPCHitInfo.push_back(new_hit_info);
	}


	USES_CONVERSION_EX;
	SaveNPCHitInfo(W2A_EX(T_EXPORT_OUT_NPC_HITINFO, 0), vecNPCHitInfo);

	mlog("ExportNPCHitInfo OK!\n");
}

vector<XApp::_AniSeq> XApp::MakeAniSequences(tstring& strAniName)
{
	vector<XApp::_AniSeq> ret;

	if(strAniName.empty())
		return ret;

	// 모션인지 체크
	if (XMotionMgr::IsTalentMotionName(strAniName.c_str()))
	{
		XMotion* pCurrMotion = m_MotionMgr.GetMotion(strAniName.c_str());
		if (pCurrMotion == NULL) return ret;

		vector<XMotionAniSequence*>& vecMotionSeq = pCurrMotion->m_vecAniSequence[MT_DEFAULT];

		for (size_t n = 0; n < vecMotionSeq.size(); n++)
		{
			_AniSeq new_seq;
			new_seq.strAniName = vecMotionSeq[n]->GetAniName();
			new_seq.fSpeed = vecMotionSeq[n]->GetSpeed();
			new_seq.nFrameOver = vecMotionSeq[n]->GetFrameOver();
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

bool XApp::SetActorAni(TECharacter& actor, _AniSeq& aniseq, bool bCheckDummyLoc)
{
	tstring strCurrAniName = aniseq.strAniName;

	// 애니메이션 세팅
	USES_CONVERSION_EX;
	RAnimation* pAnimation = actor.GetAnimation(W2A_EX(strCurrAniName.c_str(), 0));
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

void XApp::AddTalentServerInfoNode( vector<_MoveSegment>& vecMove, int nTimeIndex, RActorNode* pLocActorNode, vec3& vLastPos, float& fLastRot, MMatrix& matAccum, MMatrix& matScale )
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

void XApp::AddTalentServerInfoNode( vector<_MoveSegment>& vecMove, vector<MMatrix>& vecMoveMatrix, vec3& vLastPos, float& fLastRot, MMatrix& matAccum, MMatrix& matScale )
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
void XApp::SetActorFrame(TECharacter& actor, int nFrame)
{
	actor.SetFrame(nFrame);
	RSceneNodeUpdateVisitor visitor;
	actor.Traverse(&visitor);
	//actor.Update();

	RActorNode* pLocActorNode = actor.GetActorNode("dummy_loc");
	if (pLocActorNode) pLocActorNode->ForceUpdateAnimation();
}

void XApp::MakeTalentServerInfo( int nTalentID, TECharacter& actor, vector<_AniSeq>& vecCastingSeqs, vector<_AniSeq>& vecCastingLoopSeqs, vector<_AniSeq>& vecAniSeqs, vector<_AniSeq>& vecExtraAniSeqs, vector<_AniSeq>& vecExtra2AniSeqs, vector<_AniSeq>& vecExtra3AniSeqs, vector<_TalentServerInfo>& vecTalentServerInfo )
{
	_TalentServerInfo		new_talent_info;
	new_talent_info.nTalentID = nTalentID;

	RMeshAnimationMgr* pAMGR = &actor.m_pMesh->m_AniMgr;

	bool bCalSucesse = false;
	bCalSucesse |= CalAnimationPositionInfo(actor, vecCastingSeqs, new_talent_info.vecCastingSegment, true);
	bCalSucesse |= CalAnimationPositionInfo(actor, vecCastingLoopSeqs, new_talent_info.vecCastingLoopSegment, true);
	bCalSucesse |= CalAnimationPositionInfo(actor, vecAniSeqs, new_talent_info.vecMoveSegment);
	bCalSucesse |= CalAnimationPositionInfo(actor, vecExtraAniSeqs, new_talent_info.vecExtraMoveSegment, true);
	bCalSucesse |= CalAnimationPositionInfo(actor, vecExtra2AniSeqs, new_talent_info.vecExtra2MoveSegment, true);
	bCalSucesse |= CalAnimationPositionInfo(actor, vecExtra3AniSeqs, new_talent_info.vecExtra3MoveSegment, true);

	if (bCalSucesse)
	{
		vecTalentServerInfo.push_back(new_talent_info);
	}
}


#define TALENT_XML_TAG_MAIET				"maiet"
#define TALENT_XML_TAG_TALENT_INFO			"TALENT_INFO"
#define TALENT_XML_TAG_MOVE_SEGMENT			"MoveSegment"
#define TALENT_XML_TAG_POS					"Pos"
#define TALENT_XML_TAG_ROT					"Rot"

#define TALENT_XML_ATTR_ID					"id"
#define TALENT_XML_ATTR_TIME_INDEX			"TimeIndex"

void XApp::Save(const char* szFileName, vector<_TalentServerInfo>& vecTalentServerInfo)
{
	// npc 탤런트만..
	//<?xml version="1.0" encoding="UTF-8" ?>
	//	<maiet>
	//	<TALENT_INFO id="1" ani_time="1">
	//	<MoveSegment TimeIndex="0">
	//	<Pos>0 100 0</Pos>
	//	<Rot>0.0</Rot>
	//	</MoveSegment>
	//	</TALENT_INFO>
	//	</maiet>

	MXml xml;
	MXmlDocument* root = xml.Doc();

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	root->LinkEndChild(pDec);

	MXmlElement* pMaiet = new MXmlElement(TALENT_XML_TAG_MAIET);

	for (size_t i = 0; i < vecTalentServerInfo.size(); i++)
	{
		_TalentServerInfo info = vecTalentServerInfo[i];

		MXmlElement *pTalentInfo = new MXmlElement(TALENT_XML_TAG_TALENT_INFO);	


		_SetAttribute(pTalentInfo, TALENT_XML_ATTR_ID,	info.nTalentID);

		pMaiet->LinkEndChild(pTalentInfo);

		SaveMoveSegment(vecTalentServerInfo[i].vecCastingSegment, pTalentInfo, _T(TALENT_XML_TAG_CASTING_MOVE_SEGMENT));
		SaveMoveSegment(vecTalentServerInfo[i].vecCastingLoopSegment, pTalentInfo, _T(TALENT_XML_TAG_CASTINGLOOP_MOVE_SEGMENT));
		SaveMoveSegment(vecTalentServerInfo[i].vecMoveSegment, pTalentInfo, _T(TALENT_XML_TAG_MOVE_SEGMENT));
		SaveMoveSegment(vecTalentServerInfo[i].vecExtraMoveSegment, pTalentInfo, _T(TALENT_XML_TAG_EXTRA_MOVE_SEGMENT));
		SaveMoveSegment(vecTalentServerInfo[i].vecExtra2MoveSegment, pTalentInfo, _T(TALENT_XML_TAG_EXTRA2_MOVE_SEGMENT));
		SaveMoveSegment(vecTalentServerInfo[i].vecExtra3MoveSegment, pTalentInfo, _T(TALENT_XML_TAG_EXTRA3_MOVE_SEGMENT));
	}

	root->LinkEndChild(pMaiet);

	xml.SaveFile(szFileName);	
}


#define NPC_HITINFO_XML_TAG_MAIET			"maiet"
#define NPC_HITINFO_XML_TAG_NPC_HITINFO		"NPC_HIT_INFO"
#define NPC_HITINFO_XML_TAG_SEGMENT			"Segment"
#define NPC_HITINFO_XML_TAG_CAPSULE			"Capsule"
#define NPC_HITINFO_XML_TAG_STATE			"State"

#define NPC_HITINFO_XML_ATTR_ID				"id"
#define NPC_HITINFO_XML_ATTR_TALENT_ID		"talent_id"
#define NPC_HITINFO_XML_ATTR_TYPE			"type"
#define NPC_HITINFO_XML_ATTR_TIME			"time"
#define NPC_HITINFO_XML_ATTR_SIMPLE			"simple"
#define NPC_HITINFO_XML_ATTR_PARENT_NAME	"parent_name"
#define NPC_HITINFO_XML_ATTR_ANI_NAME		"ani_name"


void XApp::SaveNPCHitInfo( const char* szFileName, vector<_NPCHitInfo>& vecNPCHitInfo )
{
	//<NPC_HIT_INFO id="1" Simple="true">
		//	<Capsule>0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000</Capsule>
	//</NPC_HIT_INFO>
	//<NPC_HIT_INFO id="2" Simple="false">
		//<State type="talent" talent_id="200012">
		//	<Segment Time="0">
		//		<Capsule ParentName="Bip01 L UpperArm">461.018097 -365.240417 16.486767 274.584351 -399.281525 761.597290 131.513763</Capsule>
		//		<Capsule ParentName="Bip01 R UpperArm">-422.329987 -61.431339 96.345711 -268.352295 -374.938568 781.261780 131.513763</Capsule>
		//		<Capsule ParentName="Bip01 L Thigh">245.414932 603.396118 49.038898 216.355301 308.226624 651.139099 147.690277</Capsule>
		//		<Capsule ParentName="Bip01 R Thigh">-271.000732 213.972748 -16.353918 -217.967682 270.613190 650.335266 147.690445</Capsule>
		//	</Segment>
		//</Talent>
	//</NPC_HIT_INFO>

	MXml xml;
	MXmlDocument* root = xml.Doc();

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	root->LinkEndChild(pDec);

	MXmlElement* pMaiet = new MXmlElement(NPC_HITINFO_XML_TAG_MAIET);

	for (size_t i = 0; i < vecNPCHitInfo.size(); i++)
	{
		_NPCHitInfo info = vecNPCHitInfo[i];
		TENPCInfo* pNPCInfo = m_NPCInfoMgr.Get(info.nNPCID);
		if (pNPCInfo == NULL) continue;

		MXmlElement* pNPCInfoElement = new MXmlElement(NPC_HITINFO_XML_TAG_NPC_HITINFO);	
		_SetAttribute(pNPCInfoElement, NPC_HITINFO_XML_ATTR_ID,	info.nNPCID);
		_SetAttribute(pNPCInfoElement, NPC_HITINFO_XML_ATTR_SIMPLE,	info.bSimpleCapsule);

		if (info.bSimpleCapsule)
		{
			MXmlElement* pCapsuleElement = new MXmlElement(NPC_HITINFO_XML_TAG_CAPSULE);
			char szText[512]="";
			sprintf(szText, "%f %f %f %f %f %f %f", 
				info.capSimple.bottom.x, info.capSimple.bottom.y, info.capSimple.bottom.z,
				info.capSimple.top.x, info.capSimple.top.y, info.capSimple.top.z, 
				info.capSimple.radius);
			_SetContents(pCapsuleElement, szText);
			pNPCInfoElement->LinkEndChild(pCapsuleElement);
		}
		else
		{
			for (size_t j = 0; j < info.vecStates.size(); j++)
			{
				_NPCHitInfo_State& state_info = info.vecStates[j];

				MXmlElement* pStateElement = new MXmlElement(NPC_HITINFO_XML_TAG_STATE);

				if (state_info.nStateType == ST_TALENT)
				{
					_SetAttribute(pStateElement, NPC_HITINFO_XML_ATTR_TYPE, "talent");
					_SetAttribute(pStateElement, NPC_HITINFO_XML_ATTR_TALENT_ID, state_info.nTalentID);
				}
				else if (state_info.nStateType == ST_ANI)
				{
					_SetAttribute(pStateElement, NPC_HITINFO_XML_ATTR_TYPE, "ani");
					_SetAttribute(pStateElement, NPC_HITINFO_XML_ATTR_ANI_NAME, state_info.strAniName.c_str());
				}


				for (size_t k = 0; k < state_info.vecSegments.size(); k++)
				{
					_NPCHitInfo_NodeSeg& talent_seg = state_info.vecSegments[k];
					MXmlElement* pSegmentElement = new MXmlElement(NPC_HITINFO_XML_TAG_SEGMENT);
					_SetAttribute(pSegmentElement, NPC_HITINFO_XML_ATTR_TIME, (int)talent_seg.nTime);

					for (size_t l = 0; l < talent_seg.vecCapsules.size(); l++)
					{
						MCapsule cap = talent_seg.vecCapsules[l];
						char buf[256];
						sprintf(buf, "%f %f %f %f %f %f %f", cap.bottom.x, cap.bottom.y, cap.bottom.z, cap.top.x, cap.top.y, cap.top.z, cap.radius);

						MXmlElement* pCapsuleElement = new MXmlElement(NPC_HITINFO_XML_TAG_CAPSULE);
						_SetContents(pCapsuleElement, buf);
						_SetAttribute(pCapsuleElement, NPC_HITINFO_XML_ATTR_PARENT_NAME, talent_seg.vecCapsuleParentNames[l].c_str());

						pSegmentElement->LinkEndChild(pCapsuleElement);
					}

					pStateElement->LinkEndChild(pSegmentElement);
				}
				pNPCInfoElement->LinkEndChild(pStateElement);
			}
		}

		pMaiet->LinkEndChild(pNPCInfoElement);
	}

	root->LinkEndChild(pMaiet);

	xml.SaveFile(szFileName);	
}

vector<XApp::_NPCHitInfo_NodeSeg> XApp::MakeStateSegments( TECharacter& actor, vector<_AniSeq>& vecAniSeqs)
{
	vector<XApp::_NPCHitInfo_NodeSeg> ret;

	RMeshAnimationMgr* pAMGR = &actor.m_pMesh->m_AniMgr;
	rs3::RMeshCollision* pMeshCollision = actor.m_pMesh->m_pMeshCollision;

	int nTimeIndex = 0;

	vec3 vLastPos = vec3::ZERO;
	float fLastRot = 0.0f;

	RAnimationController& ac = actor.GetAnimationController();
	bool bIsMotion = !(vecAniSeqs.size() == 1);

	for (size_t ani_index = 0; ani_index < vecAniSeqs.size(); ani_index++)
	{
		_AniSeq& currAniSeq = vecAniSeqs[ani_index];

		if (SetActorAni(actor, currAniSeq, false) == false) continue;

		const int FRAME_STEP = (int)( (4800 * currAniSeq.fSpeed) /10);
		int nMaxFrame = ac.GetMaxFrame();
		if (currAniSeq.nFrameOver > 0) nMaxFrame = currAniSeq.nFrameOver;

		for (size_t f = 0; (int)f < nMaxFrame; f += FRAME_STEP)
		{
			SetActorFrame(actor, (int)f);

			AddNPCInfoSegNode(nTimeIndex, pMeshCollision, actor, ret);

			nTimeIndex++;
		}

		// 마지막 더미 위치 세팅
		SetActorFrame(actor, nMaxFrame);

		int nRemainderFrame = nMaxFrame % FRAME_STEP;

		if (ani_index == vecAniSeqs.size() - 1 && nRemainderFrame > 0)
		{
			AddNPCInfoSegNode(nTimeIndex, pMeshCollision, actor, ret);
		}
	}

	return ret;
}

bool XApp::SetupActor( TECharacter &actor, TENPCInfo* pNPCInfo )
{
	tstring strMeshFileName = T_PATH_MODEL + pNPCInfo->strMeshPath + pNPCInfo->strMeshName + L".elu";

	if (actor.Create(strMeshFileName.c_str()) == false)
	{
		return false;
	}
	actor.SetScale(vec3(pNPCInfo->fScale,pNPCInfo->fScale,pNPCInfo->fScale));
	if (actor.m_pMesh == NULL) return false;

	return true;
}

void XApp::AddNPCInfoSegNode( int nTimeIndex, rs3::RMeshCollision* pMeshCollision, TECharacter &actor, vector<XApp::_NPCHitInfo_NodeSeg>& vecTalentSegs )
{
	_NPCHitInfo_NodeSeg new_talent_seg;
	new_talent_seg.nTime = nTimeIndex * 100;

	vector<RCapsule> vCapsule;
	vector<RActorNode*> vNodes;

	for(size_t j=0;j<pMeshCollision->size();j++)
	{
		MCollision* pCollision = pMeshCollision->at(j);
		MCapsule c, cc;

		cc = MCapsule(MVector3(0, 0, 0), MVector3(0, 0, pCollision->m_fHeight), pCollision->m_fRadius);
		c = cc.Multiply(pCollision->m_matLocal);

		vCapsule.push_back(c);

		RActorNode* n = actor.GetActorNode(pCollision->m_strParentName.c_str());
		vNodes.push_back(n);
	}

	RAnimation* pAnimation = actor.GetAnimation();
	if (pAnimation != NULL)
	{
		RAnimationController& ac = actor.GetAnimationController();

		for (size_t j=0; j<vCapsule.size(); j++)
		{
			RCapsule c;
			RActorNode* an = vNodes[j];
			an->ForceUpdateAnimation();
			c = vCapsule[j].Multiply(an->GetWorldTransform());

			new_talent_seg.vecCapsules.push_back(c);
			new_talent_seg.vecCapsuleParentNames.push_back(vNodes[j]->GetNodeName());
		}
	}

	vecTalentSegs.push_back(new_talent_seg);
}

void XApp::AddNPCHitInfoState( _NPCHitInfo &new_hit_info, TECharacter& actor, XTalentInfo* pTalentInfo, tstring strAniName )
{
	_NPCHitInfo_State new_state;
	vector<_AniSeq>	vecAniSeqs;

	if (pTalentInfo)
	{
		new_state.nStateType = ST_TALENT;
		new_state.nTalentID = pTalentInfo->m_nID;
		vecAniSeqs = MakeAniSequences(pTalentInfo->m_szUseAnimation);
	}
	else
	{
		new_state.nStateType = ST_ANI;
		new_state.strAniName = strAniName;
		vecAniSeqs = MakeAniSequences(strAniName);
	}

	new_state.vecSegments = MakeStateSegments(actor, vecAniSeqs);
	new_hit_info.vecStates.push_back(new_state);
}

bool XApp::CalAnimationPositionInfo( TECharacter &actor, vector<_AniSeq> &vecAniSeqs, vector<_MoveSegment> &vecMove, bool bExtraAni /*= false */ )
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

		actor.AddActiveActorNode(L"dummy_loc");
		RActorNode* pLocActorNode = actor.GetActorNode("dummy_loc");
		if(pLocActorNode == NULL)
			continue;

		const int FRAME_STEP = (int)( (4800 * currAniSeq.fSpeed) /10);
		int nMaxFrame = actor.GetAnimationMaxFrame();
		if (currAniSeq.nFrameOver > 0) 
			nMaxFrame = currAniSeq.nFrameOver;

		if(bExtraAni == false)
		{
			// ExtraAni만 여러번 하지 않는다.
			vector< MMatrix > vecMatrix;
			actor.GetSamplingActorNodeData(L"dummy_loc", 0, nMaxFrame, FRAME_STEP, vecMatrix);
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
			actor.GetSamplingActorNodeData(L"dummy_loc", nMaxFrame, nMaxFrame, FRAME_STEP, vecMatrix);
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

void XApp::SaveMoveSegment( vector<_MoveSegment> &vecMoveSegment, MXmlElement * pTalentInfo, tstring strName )
{
	USES_CONVERSION_EX;
	for (size_t j = 0; j < vecMoveSegment.size(); j++)
	{
		int nTimeIndex = vecMoveSegment[j].nTimeIndex;
		vec3 vPos = vecMoveSegment[j].vPos;
		float fRot = vecMoveSegment[j].fRot;

		MXmlElement* pMovSeg = new MXmlElement(W2A_EX(strName.c_str(), 0));
		_SetAttribute(pMovSeg, TALENT_XML_ATTR_TIME_INDEX,	nTimeIndex);

		pTalentInfo->LinkEndChild(pMovSeg);

		MXmlElement* pPos = new MXmlElement(TALENT_XML_TAG_POS);
		char szText[512]="";
		sprintf_s(szText, "%f %f %f", vPos.x, vPos.y, vPos.z);
		_SetContents(pPos, szText);
		pMovSeg->LinkEndChild(pPos);

		MXmlElement* pRot = new MXmlElement(TALENT_XML_TAG_ROT);
		sprintf_s(szText, "%f", fRot);
		_SetContents(pRot, szText);
		pMovSeg->LinkEndChild(pRot);
	}
}