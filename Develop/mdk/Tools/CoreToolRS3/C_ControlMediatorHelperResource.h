#pragma once

#include "C_ControlMediator.h"
#include "RMesh.h"
//#include "M_ToolActor.h"

class CEntityObject;
class CWorkSpaceManager;

namespace rs3 { class RToolActor; class RToolDecal; }

class CORETOOL_API CControlMediatorHelperNewMapResource
{
public:
	CControlMediatorHelperNewMapResource(){}
	virtual ~CControlMediatorHelperNewMapResource(){}

	virtual bool			IsKeyEventHandlerAcceptable() = 0;

	virtual const char*		GetNewMapName() = 0;
	virtual void			OpenNewMapDlg() = 0;
};

class CORETOOL_API CControlMediatorHelperResource
{
public:

	void			RefreshZone();									///< Zone Resource 새로고침

	void			ResetRefMesh(const char* _pMeshName);			///< Reset Ref Mesh With Name
	void			ShowRefMeshToggle();							///< Ref Mesh View Toggle
	void			UpdateRefMesh();
	bool			IsCreateRefMesh( const MVector3* _pContactPos );

	void			DeleteTempFile();
	void			SetAutoAnimationProperty();						///< 애니메이션이 있는 액터들의 자동 애니메이션 프로퍼티를 설정해줌

	void			SortZoneXmlWithRuby(MXml& _refZoneXml);
	void			SortZoneXmlAndSave(const char* _pSzFileName, MXml& _refZoneXml);

	void			SetMediatorAfterSceneLoaded(MAP_EDIT_TYPE _eEditType = MAP_EDIT_TYPE_SCENE);

	enum			MAP_CONFIG_FILE { MAP_CONFIG_LOAD = 0, MAP_CONFIG_SAVE, MAP_CONFIG_SAVE_AS };
	void			ManageConfigFileRelatedMap(const char* _pStrName, MAP_CONFIG_FILE _type, bool _bNeedEditInfoFile);

	enum			MINI_MAP { MINI_DELETE = 0, MINI_JUST_CAMERA_SET, MINI_NONE_CAMERA_SET, MINI_CREATE };
	void			CreateMiniMapAndSetCamera(const char* _szFileName, MINI_MAP _type);

	// static
	static void		CopyToServerZoneFolder(const char* _pStrFileName);
	static void		CopyToServerSystemFolder(const char* _pStrFileName);
	static std::string MakeServerZoneFile( const char* _pStrFileName );

	// informed actor loader
	struct SExtraInformedActorLoaderParam
	{
		SExtraInformedActorLoaderParam()
		{
			Clear();
		}

		void Clear()
		{
			m_pActorToLoad = NULL;
			m_pInformedXmlElement = NULL;
			m_bAddToSceneNode = false;

			m_strOutActorName.clear();
			m_strOutAnimationName.clear();
		}

		rs3::RActor*	m_pActorToLoad;
		MXmlElement*	m_pInformedXmlElement;
		bool			m_bAddToSceneNode;

		rs3::RVector3	m_vOutPos;
		rs3::RVector3	m_vOutDir;
		std::string		m_strOutActorName;
		std::string		m_strOutAnimationName;
	};

	template< typename T_MeshContainer >
	void LoadExtraInformedActor( SExtraInformedActorLoaderParam& _rParam, T_MeshContainer& _rAddedMeshContainer )
	{
		using namespace rs3;

		RActor* pActor = _rParam.m_pActorToLoad;
		MXmlElement* pActorElement = _rParam.m_pInformedXmlElement;
		std::string& strActorProperty = _rParam.m_strOutActorName;

		_ASSERT(pActor);
		_ASSERT(pActorElement);

		// RActor 로딩
		_Attribute(strActorProperty, pActorElement, "elu");
		m_pControlMediator_->GetToolSceneManager()->LoadSceneNodeFromFile( pActor, (CXMLConfig::GetInstance()->pcDataPath + strActorProperty).c_str() );

		// 씬매니저에 붙이기
		if (_rParam.m_bAddToSceneNode)
			REngine::GetSceneManager().AddSceneNode(pActor);

		std::string strActorNode;
		std::string strMesh;
		std::string strParent;
		std::string strColor;
		MXmlElement* pCustomActorNodeElement = pActorElement->FirstChildElement("ACTORNODE");
		while (pCustomActorNodeElement)
		{
			strActorNode.clear();
			strMesh.clear();
			strParent.clear();
			strColor.clear();

			_Attribute(strActorNode, pCustomActorNodeElement, "name");
			_Attribute(strMesh, pCustomActorNodeElement, "mesh");
			_Attribute(strParent, pCustomActorNodeElement, "parent");

			// 기존 액터노드 지우기
			pActor->DeleteActorNode(strActorNode.c_str());

			// 메시 리소스 생성
			RMesh* pMesh = REngine::GetSceneManager().CreateResource<RMesh>( (CXMLConfig::GetInstance()->pcDataPath + strMesh).c_str() );
			pMesh->BuildAll();

			// 메시 리소스로부터 새 액터노드 생성
			RMeshNode* pMeshNode = pMesh->GetNode(strActorNode.c_str());
			if (pMeshNode)
			{
				RActorNode* pParentActorNode = NULL;
				if (!strParent.empty())
					pParentActorNode = pActor->GetActorNode(strParent.c_str());

				RActorNode* pNewActorNode = pActor->AddMeshNode(pMeshNode, pParentActorNode, strActorNode.c_str());
				if (_Attribute(strColor, pCustomActorNodeElement, "color"))
				{
					DWORD dwColor = strtol(strColor.c_str(), NULL, 16);		// XML에 16진수로 저장되어있다.
					dwColor = 0xFF000000 | dwColor;
					pNewActorNode->SetPartsColor(dwColor);
				}

				// 생성에 필요했던 리소스 보관
				_rAddedMeshContainer.push_back(pMesh);
			}
			else
			{
				REngine::GetSceneManager().ReleaseResource(pMesh);
			}

			pCustomActorNodeElement = pCustomActorNodeElement->NextSiblingElement("ACTORNODE");
		}

		// 애니메이션
		_Attribute(_rParam.m_strOutAnimationName, pActorElement, "ani");

		RVector3& vActorPos = _rParam.m_vOutPos;
		RVector3& vActorDir = _rParam.m_vOutDir;

		_Attribute(&vActorPos.x, pActorElement, "x");
		_Attribute(&vActorPos.y, pActorElement, "y");
		_Attribute(&vActorPos.z, pActorElement, "z");
		_Attribute(&vActorDir.x, pActorElement, "dir_x");
		_Attribute(&vActorDir.y, pActorElement, "dir_y");
		_Attribute(&vActorDir.z, pActorElement, "dir_z");

		if (!strActorProperty.empty())
		{
			pActor->SetAnimation(_rParam.m_strOutAnimationName.c_str());
			pActor->SetTransform(vActorPos, -vActorDir);
			pActor->UpdateTransform();
		}

		strActorProperty.clear();
		_Attribute(strActorProperty, pActorElement, "name");
		pActor->SetNodeName(strActorProperty);
	}

	bool IsKeyEventAcceptableInNewMapDlg();

	rs3::RToolDecal* GetToolDecal() { return m_pToolDecal; }
private:

	//Mediator 에 종속적
	friend class CControlMediator;
	CControlMediatorHelperResource(void);
	~CControlMediatorHelperResource(void);

	inline void InitMediatorHelper_(CControlMediator* _pControlMediator)
	{
		m_pControlMediator_ = _pControlMediator;
	}

	//리소스 config 관련 메쏘드
	void SetMediatorWithLoadedTerrain_();
	void SetMediatorWithNoneTerrain_();

	/*--- Private Member -------------------------------*/
	//
	//ControlMediator
	CControlMediator* m_pControlMediator_;

	// Reference Mesh
	rs3::RToolActor*	m_pRefActor_;
	rs3::RToolDecal*	m_pToolDecal;
};
