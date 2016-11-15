#include "stdafx.h"
#include "RMeshAnimationMgr.h"
#include "MDebug.h"
#include "RAnimationManager.h"
#include "RAnimationResource.h"
#include "RRule.h"

namespace rs3 {

RMeshAnimationMgr::RMeshAnimationMgr() {
	m_nIDCount = 0;
	m_AnimationBlendTable.Reset( this );
}

RMeshAnimationMgr::~RMeshAnimationMgr() {
	DelAll();
}


RAnimation* RMeshAnimationMgr::Add(const char* name, const char* filename, bool bBackground, bool bLoadMonitoring, RResource::LoadingController* pLoadingController) 
{
	RAnimationResource* node = REngine::GetSceneManager().CreateResource < RAnimationResource > ( filename, bLoadMonitoring );
	//REngine::GetSceneManagerPtr()->AddLoadingList( node, bBackground, pLoadingController );
	node->BuildAll(bBackground,pLoadingController);

	RAnimation* pAnimation = new RAnimation( string(name), node, m_nIDCount );

	m_nIDCount++;

	m_NodeHashList.PushBack( string(name), pAnimation );

	return pAnimation;
}

void RMeshAnimationMgr::Delete(const char* name)
{
	RAnimation* pAnimation = m_NodeHashList.Find( name );
	if(pAnimation)
	{
		REngine::GetSceneManager().ReleaseResource( pAnimation->GetResource() );
		m_NodeHashList.Delete(name);
	}
}

void RMeshAnimationMgr::DelAll() {

	RAnimationHashList_Iter node;
	for(node = m_NodeHashList.begin(); node != m_NodeHashList.end(); ) 
	{
		RAnimation* pANode = *node;

		if( REngine::GetSceneManagerPtr() )
			REngine::GetSceneManagerPtr()->ReleaseResource( pANode->GetResource() );

		delete pANode;
		node = m_NodeHashList.erase(node);
	}
	m_NodeHashList.Clear();

	m_AnimationBlendTable.Clear();	// 블렌드 테이블 비움 - 090608, OZ

	m_nIDCount = 0;
}

RAnimation* RMeshAnimationMgr::GetAnimation(const char* name) 
{
	if(!name)	return NULL;

	if(name[0]==0) return NULL;

	if(m_NodeHashList.empty())	return NULL;

	RAnimation* pAni = m_NodeHashList.Find(name);
	return pAni;
}

RAnimation* RMeshAnimationMgr::GetAnimation(int nID)
{
	RAnimationHashList_Iter node;

	if( m_NodeHashList.empty() ) 
		return NULL;

	for(node = m_NodeHashList.begin(); node != m_NodeHashList.end(); ++node) {
		if( (*node)->GetID() == nID )
			return *node;
	}
	return NULL;
}

bool RMeshAnimationMgr::AddXmlDesc(const char* xmlFileName, bool bBackground, bool bLoadMonitoring, bool bExtern, const char* szFilter)
{
	MXml xml;
	bool bOk = xml.LoadFile(xmlFileName);
	if(!bOk)
		return false;

	if ( bExtern == false)
		m_AnimationBlendTable.Clear();


	string strPath = MGetPurePath(xmlFileName);

	MXmlElement* pRootNode = xml.Doc()->FirstChildElement();

	MXmlElement* pChild = pRootNode->FirstChildElement();

	size_t nFilterLen = strlen( szFilter);

	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement()) {

		char NodeName[256] = {0,};
		char IDName[256] = {0,};
		char FileName[256] = {0,};
		char MotionLoopTypeID[256] = {0,};
		AnimationLoopType MLoopType = RAniLoopType_Loop;
		float speed =1.0f;

		strcpy(NodeName, pChild->Value());

		if ( strcmp( NodeName, "CopyFromXml")==0)
		{
			char Filter[256];
			_Attribute(FileName, pChild, "filename");
			_Attribute(Filter, pChild, "filter");

			string path = MGetPathFileNameFromRelativePath( xmlFileName, FileName);
			AddXmlDesc( path.c_str(), bBackground, bLoadMonitoring, true, Filter);
		}

		else if(strcmp(NodeName, "AddAnimation")==0)
		{
			_Attribute(IDName, pChild, "name");

			if ( nFilterLen > 2)
			{
				if ( strncmp( IDName, szFilter, nFilterLen) != 0)
					continue;

				strcpy_s( IDName, IDName + nFilterLen);
			}

			_Attribute(FileName, pChild, "filename");
			_Attribute(MotionLoopTypeID, pChild, "motion_loop_type");
			_Attribute(&speed, pChild, "speed");

			if(strcmp(MotionLoopTypeID,"normal")==0) {
			}
			else if(strcmp(MotionLoopTypeID,"loop")==0) {
				MLoopType = RAniLoopType_Loop;
			}
			else if(strcmp(MotionLoopTypeID,"lastframe")==0) {
				MLoopType = RAniLoopType_HoldLastFrame;
			}

			MFile pFile; //Apparently someone at maiet thinks opening a file from ANYWHERE is a good idea for disk I/O and/or CPU... idiots...

			string realFileName = FileName;
			if(pFile.Open(FileName))
			{
					realFileName = pFile.GetFileName();
					strPath = MGetPurePath(realFileName);
			}
			string PathFileName = MGetPathFileNameFromRelativePath( strPath, FileName);
			RAnimation* pAni = NULL;
			pAni = Add( IDName, PathFileName.c_str(), bBackground, bLoadMonitoring, NULL );
			
			// TODO: 애니메이션 로딩이벤트를 받을 필요가 있으면 처리하자
			if(pAni)
			{
				pAni->SetAnimationLoopType( MLoopType );
				pAni->SetSpeed(speed);

				// event 추가
				pAni->LoadEvent(pChild);
			}

			if (pAni == NULL)
			{
				mlog("ani %s file loading failure !!!\n", PathFileName.c_str());
			}
		}
		else if(strcmp(NodeName, "Animation")==0) {

			_Attribute(IDName, pChild, "name");

			if ( nFilterLen > 2)
			{
				if ( strncmp( IDName, szFilter, nFilterLen) != 0)
					continue;

				strcpy_s( IDName, IDName + nFilterLen);
			}
			RAnimation* pAni = NULL;
			pAni = GetAnimation(IDName);
			if(pAni)
			{
				// event 추가
				pAni->LoadEvent(pChild);
			}
		}
		else
		if( strcmp(NodeName, "AnimationTransitions")==0) {
			
			m_AnimationBlendTable.LoadXml( this, pChild, szFilter);
		}
	}

	return true;
}

bool RMeshAnimationMgr::SaveToXml(const char* szFileName)
{
	bool bRet = true;
	string strAnimationXmlFileName( szFileName );
	strAnimationXmlFileName += ".animation.xml";
	if (SaveAnimation(strAnimationXmlFileName.c_str()) == false) bRet = false;

	string strAnimationEventXmlFileName( szFileName );
	strAnimationEventXmlFileName += ".animationevent.xml";
	if (SaveAnimationEvent(strAnimationEventXmlFileName.c_str()) == false) bRet = false;

	return bRet;
}

bool RMeshAnimationMgr::SaveAnimation(const char* szFileName)
{
	MXml xml;

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);

	MXmlElement *pRootElement = new MXmlElement("XML");
	xml.Doc()->LinkEndChild(pRootElement);

	for(RAnimationHashList_Iter it = m_NodeHashList.begin(); it != m_NodeHashList.end();it++)
	{
		RAnimation* pAnimation = *it;

		MXmlElement *pAnimationElement = new MXmlElement("AddAnimation");
		pRootElement->LinkEndChild(pAnimationElement);
		_SetAttribute(pAnimationElement,"name",pAnimation->GetAliasName());

		// 상대경로
		string strRelativePath = MGetRelativePathFileName( szFileName, pAnimation->GetResource()->GetName() );
		_SetAttribute(pAnimationElement,"filename",strRelativePath);
		_SetAttribute(pAnimationElement,"motion_loop_type",
			(pAnimation->GetAnimationLoopType()==RAniLoopType_Loop) ? "loop" : "lastframe" );
		_SetAttribute(pAnimationElement,"speed", pAnimation->GetSpeed());
	}

	m_AnimationBlendTable.SaveXml( pRootElement );

	return xml.SaveFile(szFileName);
}

bool RMeshAnimationMgr::SaveAnimationEvent(const char* szFileName)
{
	MXml xml;

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);

	MXmlElement *pRootElement = new MXmlElement("XML");
	xml.Doc()->LinkEndChild(pRootElement);

	for(RAnimationHashList_Iter it = m_NodeHashList.begin(); it != m_NodeHashList.end();it++)
	{
		RAnimation* pAnimation = *it;
		if(pAnimation->m_animationEvents.empty()) continue;

		MXmlElement *pAnimationElement = new MXmlElement("Animation");
		pRootElement->LinkEndChild(pAnimationElement);
		_SetAttribute(pAnimationElement,"name",pAnimation->GetAliasName());

		pAnimation->SaveEvent(pAnimationElement);
	}

	return xml.SaveFile(szFileName);
}

BLENDDATA RMeshAnimationMgr::GetAnimationBlendType( const char* source, const char* target )
{
	return m_AnimationBlendTable.GetAnimationTransData( source, target );
}

BLENDDATA RMeshAnimationMgr::GetAnimationBlendType( int sourceId, int targetId )
{
	return m_AnimationBlendTable.GetAnimationTransData( sourceId, targetId );
}

bool RMeshAnimationMgr::IsLoadingCompleted()
{
	RAnimationHashList_Iter node;

	if( m_NodeHashList.empty() ) 
		return true;

	for(node = m_NodeHashList.begin(); node != m_NodeHashList.end(); ++node)
	{
		if( (*node)->GetResource()->IsLoadingCompleted() == false )
			return false;
	}
	return true;
}

void RMeshAnimationMgr::OnLoadingComplete(RResource*)
{

}

bool RMeshAnimationMgr::LoadAnimationInformationsToAnimationMgr( const string& EluFileName,bool bExistAutoAnimation,bool bBackgournd )
{
	string AnimationSetXMLFileName = RRule::Animation::GetAnimationSetXMLFileNameByEluFileName(EluFileName);
	bool bExistAnimationListFile = this->AddXmlDesc( AnimationSetXMLFileName.c_str(), bBackgournd);

	string AnimationEventXMLFileName = RRule::Animation::GetAnimationEventXMLFileByEluFileName(EluFileName);
	bool bRetAniEvent = this->AddXmlDesc( AnimationEventXMLFileName.c_str(), bBackgournd, false, true );

	if( bExistAutoAnimation )
	{
		string AnimationName = RRule::Animation::GetAutoAnimationNameByEluFileName(EluFileName);
		RAnimation* pAnimation = this->GetAnimation(AnimationName.c_str());
		if ( pAnimation == NULL )
		{
			string AnimationFileName = RRule::Animation::GetAutoAnimationFileNameByEluFileName(EluFileName);
			this->Add(AnimationName.c_str(), AnimationFileName.c_str(), bBackgournd, false, NULL );
		}
	}

	return bExistAnimationListFile;
}

}
