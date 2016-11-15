#pragma once

#include "RSceneManager.h"
#include "RViewFrustum.h"
#include "M_ToolTerrain.h"
#include "RSkyManager.h"
#include "REnvironment.h"
#include "MRTTI.h"

#include <vector>
#include <string>

class CControlMediatorHelperResource;
class CLogicObject_SceneNode;

namespace rs3 {

class RTerrain;
class RTreeSystem;
class RSceneSky;
class RSkyManager;
class RDirectionalLightSceneNode;
class RSceneNodeInstance;

typedef std::multimap<float, RPICKINFO, less<float> > RPICKINFO_MAP;

class RSceneNodeEnvModifier
{
public:
	RSceneNodeEnvModifier(){}
	virtual ~RSceneNodeEnvModifier(){}

	virtual void Clear() = 0;
	virtual void Update(rs3::RCollisionTree* pCollisionTree) = 0;
	virtual void SetEnvModifier(rs3::REnvironment::ETime eTime, rs3::REnvironment::EWeather eWeather, bool bFade, float fFadingTime, RCollisionTree* pCollisionTree =  NULL) = 0;
	virtual void ResetRootScene(const rs3::RSceneNode* pNodeRootToAttach) = 0;
};

template < typename T_EnvModifierImpl >
class RSceneNodeEnvModifierImpl : public RSceneNodeEnvModifier
{
public:
	RSceneNodeEnvModifierImpl()
	{
		m_pModifierImpl = new T_EnvModifierImpl(REnvironmentSet::GetInstancePtr(),rs3::REngine::GetSceneManager().GetCurrentRootSceneNode(false));
	}

	virtual ~RSceneNodeEnvModifierImpl()
	{
		delete m_pModifierImpl;
	}

	virtual void Clear()
	{
		m_pModifierImpl->Clear();
	}

	virtual void Update(rs3::RCollisionTree* pCollisionTree)
	{
		m_pModifierImpl->Update(pCollisionTree);
	}

	T_EnvModifierImpl* GetEnvModifierImpl()
	{
		return m_pModifierImpl;
	}

	virtual void SetEnvModifier(rs3::REnvironment::ETime eTime, rs3::REnvironment::EWeather eWeather, bool bFade, float fFadingTime, RCollisionTree* pCollisionTree = NULL)
	{
		m_pModifierImpl->SetCurrentEnvSet(eTime, eWeather, bFade, fFadingTime, pCollisionTree);
	}

	virtual void ResetRootScene(const rs3::RSceneNode* pNodeRootToAttach)
	{
		m_pModifierImpl->Clear();
		delete m_pModifierImpl;
		m_pModifierImpl = new T_EnvModifierImpl(REnvironmentSet::GetInstancePtr(),rs3::REngine::GetSceneManager().GetCurrentRootSceneNode(false));

		m_pModifierImpl->AttachNightNodeController(pNodeRootToAttach);
	}

private:
	T_EnvModifierImpl* m_pModifierImpl;
};

class CORETOOL_API RToolSceneManager : public RSceneManager
{
	friend class CControlMediatorHelperResource;	// for pick_ignore and error message

public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸 / 리셋(Clear)
	//
	RToolSceneManager(CControlMediator*);
	virtual ~RToolSceneManager();
	virtual void Clear();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 씬노드 팩토리 함수 및 파일이름으로부터 생성함수. 툴 전용 파생 씬노드 생성등에 사용
	//
	virtual RSceneNode*			CreateSceneNode(const char *szNodeName);

	// File로부터 입력씬노드의 정보를 넣어줌. 백그라운드로딩은 안하더라..
	bool						LoadSceneNodeFromFile(RSceneNode* pSceneNode,const char* szFileName);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// SceneNode 생성 팩토리 함수 및 파일이름으로부터의 씬노드 생성함수
	//
	virtual bool				SaveRootSceneToXML(const char *szFileName );
	void						SaveRootSceneToXML(const char *szFileName, MXml& _refMXml);

	/// max 파일로 병합 스크립트를 익스포트
	void						ExportMaxScript(const char* szMaxScriptFileName);
	void						SaveResourceListToXML(const char *szFileName);

	//////////////////////////////////////////////////////////////////////////
	// update
	virtual void				Update();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// SceneNode 피킹함수
	//
	bool						Pick(const RVector &origin,const RVector &to, RPICKINFO_MAP& _refPickInfoOut, bool _bSelectMode);
	bool						PickFrustum(RSceneNodeList *pOutList, RViewFrustum &kFrustum);
	const RVector*				GetVerticalContactPos( const RVector& _refPos, const RSceneNode* _pSceneNodeToIgnore );
	const RVector*				GetVerticalContactPos( const RVector& _refPos, const RSceneNode* _pSceneNodeToIgnore, RVector* const _outNormal );
// 	RSceneNode*					GetPickIgnoredSceneNode(){ return m_pPickingIgnoreSceneNode; }

	//////////////////////////////////////////////////////////////////////////
	// 툴 로직오브젝트 관련
	CLogicObject_SceneNode*		CreateLogicObject( RSceneNode* pSceneNode );
	bool						DestroyLogicObject( RSceneNode* pSceneNode );

	bool						AttachLogicObject( RSceneNode* pSceneNode );	///< LogicObject 를 만들어 붙여주고 편집가능하도록 LogicObjectList에 등록한다
	bool						DetachLogicObject( RSceneNode* pSceneNode );	///< LogicObject 를 삭제하고, 편집 가능목록에서 삭제한다
	
	/// 해당 노드및 모든 하위 노드에 편집가능하도록 LogicObject 붙이기
	void						AttachLogicObjectTraverse( RSceneNode* pSceneNodeTree );

	/// 툴에서 편집가능하도록 한 객체인 CLogicObject 를 얻는다
	CLogicObject*				GetLogicObjectLinkedTo( const RSceneNode* pSceneNode );


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 이름으로 씬노드 찾기
	//
	RSceneNode*					GetSceneNodeByName(const char* _pSzSceneNodeName);
	string						GetEffectNameFromFileName( const string& strEffectFileName );	///< 확장자 제거한 이펙트 이름 얻기
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 프라이머리 카메라 렌더링
	// 와이어 프레임 렌더링 모드
	virtual void				RenderPrimaryCamera();
	void						SetWireFrameDrawMode( bool bFlag ) { m_bWireFrameDrawMode = bFlag; }
	bool						GetWireFrameDrawMode() { return m_bWireFrameDrawMode; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 에러메시지
	//
	void						AddErrorMessage(const char* _pSzMessage){ m_vecErrorMessage_.push_back(_pSzMessage); }
	std::vector<std::string>&	GetErrorMessage(){ return m_vecErrorMessage_; }


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 로드 루트씬
	//
	bool						LoadEditableSceneXML(RSceneNode *pSceneNodeToLoad, const char* szFileName);
	bool						LoadEditableSceneXML(const char *szFileName);
//	bool						LoadXML(RSceneNode *pSceneNodeToLoad, MXmlElement &element, bool bBackgroundCreation) override;

	//////////////////////////////////////////////////////////////////////////
	// SceneNodeInstance
	bool						MakeSceneNodeInstanceEditable(RSceneNodeInstance* pSceneInstance, RSceneNodeList* pOutList );

	/// import scene as SceneNodeInstance
	void						ImportScene(const char *szFileName);

	//////////////////////////////////////////////////////////////////////////
	// 밤/낮 관련
	template< typename T_EnvModifierImpl >
	void CreateSceneNodeEnvModifier()
	{
		DestroySceneNodeEnvModifier();
		m_pSceneNodeModifier = new RSceneNodeEnvModifierImpl<T_EnvModifierImpl>();
	}

	void DestroySceneNodeEnvModifier()
	{
		if(m_pSceneNodeModifier)
		{
			m_pSceneNodeModifier->Clear();
			SAFE_DELETE(m_pSceneNodeModifier);
		}
	}

	template< typename T_EnvModifierImpl >
	T_EnvModifierImpl* GetSceneNodeEnvModifier()
	{
		if (NULL == m_pSceneNodeModifier)
			return NULL;

		RSceneNodeEnvModifierImpl<T_EnvModifierImpl>* pImpl = dynamic_cast<RSceneNodeEnvModifierImpl<T_EnvModifierImpl>*>(m_pSceneNodeModifier);
		if (NULL == pImpl)
			return NULL;

		return pImpl->GetEnvModifierImpl();
	}

	void SetEnvModifier(rs3::REnvironment::ETime eTime, rs3::REnvironment::EWeather eWeather, bool bFade, float fFadingTime, RCollisionTree* pCollisionTree)
	{
		if (m_pSceneNodeModifier)
			m_pSceneNodeModifier->SetEnvModifier(eTime, eWeather, bFade, fFadingTime, pCollisionTree);
	}

	void ResetEnvModifierRootScene()
	{
		if (m_pSceneNodeModifier)
			m_pSceneNodeModifier->ResetRootScene(&GetRootSceneNode());
	}

private:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 라인 / 프러스텀과 씬노드와의 피킹. 좀더 빠른 피킹을 위해 AABB를 사용한다.
	//
	RSceneNode*					AABBPicking(const RSceneNodeList *pNodeList, const RVector &origin, const RVector &to, RPICKINFO_MAP& _refPickInfoOut, bool _bSelectMode);
	int							AABBPickingFrustum(RSceneNodeList *pOutList, const RSceneNodeList *pChildNodeList, RViewFrustum &kFrustum);

	void						UpdateRootSceneTransform();

//	RSceneNode*					m_pPickingIgnoreSceneNode;	// indoor 편집시 static mesh 를 무시하기 위한듯한데 property 에 있으므로 불필요하다

	bool						m_bWireFrameDrawMode;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 로딩등의 에러메시지
	//
	std::vector<std::string>	m_vecErrorMessage_;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 씬노드 이름으로 찾기 Functor
	//
	struct SSceneNodeFinder
	{
		static bool FuncGetByName( RSceneNode* pSceneNode )
		{
			if (pSceneNode->GetNodeName() == pSzSceneNodeName )
				return true;
			else
				return false;
		}
		static char* pSzSceneNodeName;
	};

	void						GatherAllActor(const RSceneNode *pSceneNode, RSceneNodeList* pOutList);
	void						GatherAllTrees(const RSceneNode *pSceneNode, RSceneNodeList* pOutList);
	void						ExportTerrainMaxScript(FILE* file);
	void						ExportLightMapActorMaxScript(FILE* file);
	void						ExportSpeedTreeMaxScript(FILE* file);

	//////////////////////////////////////////////////////////////////////////
	// 밤/낮 modifier
	RSceneNodeEnvModifier*		m_pSceneNodeModifier;
	CControlMediator*			m_pControlMediator;
};

}