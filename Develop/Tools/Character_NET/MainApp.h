#pragma once

#include "RDeviceD3D.h"
#include "RActor.h"
#include "RMesh.h"
#include "RMeshNode.h"
#include "MDebug.h"
#include "RMeshMgr.h"
#include "TCamera.h"
#include "RPointLightSceneNode.h"
#include "RDirectionalLightSceneNode.h"
#include "RRenderingCallbackInterface.h"

#include "XItemData.h"
#include "TItem.h"

#include "RTextSceneNode.h"
#include "TAnimationEventHandler.h"

#include "TPlayMotion.h"

// 클라쪽 소스
#include "TCharacter.h"
#include "TNPCInfo.h"
#include "TBuffInfo.h"
#include "TEffectInfoMgr.h"
#include "TEffectManager.h"

#include "TTalentEffectViewer.h"
#include "TPartsHitInfoViewer.h"
#include "TMeshColInfoViewer.h"
#include "TMeshHitEffectInfoViewer.h"

//#include "SaveMgr.h"
#include "TTalentInfo.h"
#include "TRealSound.h"

#include "TTabAnimation.h"
#include "TTabModel.h"
#include "TTabModelInfo.h"
#include "TTabItem.h"

#include "TEquipment.h"
#include "TNPC.h"
#include "TMtrl.h"
#include "TMenu.h"

#include "TSwordTrailSampling.h"

#include "THelper.h"

#include "TProjectile.h"
#include "TMagicArea.h"

#include "TTalentAnimation.h"
#include "TNormalAnimation.h"
#include "TAnimationBlendTable.h"

#include "TActorMgr.h"

#include "TEquipItems.h"

#include "TCameraEffect.h"
//namespace physx { class MPhysX; }
using namespace rs3;

namespace rs3{
	class RRotationController;
	class RRotationController;
	class RMaterialResource;
};

//#define  CHECK_LOADING_TIME
inline void CheckLoadingTimeStart(DWORD& _dwTime)
{
#ifdef CHECK_LOADING_TIME
	_dwTime = GetTickCount();
#endif
}

inline void CheckLoadingTimeEnd(DWORD& _dwTime, string _strMessage)
{
#ifdef CHECK_LOADING_TIME
	mlog("%s : %d ms\r", _strMessage.c_str(), GetTickCount() - _dwTime);
#endif
}

#define  CHECK_LOADING_TIME_START(_dwTime) CheckLoadingTimeStart(_dwTime)
#define  CHECK_LOADING_TIME_END(_dwTime, _strMessage) CheckLoadingTimeEnd(_dwTime, _strMessage)

/**
 자료 관리 - 맵, 사용자 데이터
 커맨드 관리 - 모든 동작은 커맨드로 이루어 진다. ( Undo, Redo의 가능 )
*/

//class ICommand;	// 당분간 사용되지 않을 것이나, 고려하여 작성될 것
class CDocument;
class CCmdBuffer;
class TTalentEventMgr;

struct delGroupCapsuleData
{
	int			nGroupID;
	int			nCapsuleID;

	delGroupCapsuleData()
	{
		nGroupID = -1;
		nCapsuleID = -1;
	}

	static bool descending(delGroupCapsuleData &a, delGroupCapsuleData &b)
	{
		if(a.nGroupID > b.nGroupID)
		{
			return true;
		}
		else if(a.nGroupID == b.nGroupID)
		{
			return a.nCapsuleID > b.nCapsuleID;
		}

		return false;
	}
};


enum EVENT_MSG
{
	EVENT_MSG_STOP_ANIMATION = 10,
	EVENT_MSG_PLAY_ANIMATION = 11,
	EVENT_MSG_LIGHT_TO_CAMERA = 12
};

enum _PCTalentTreeNodeType
{
	NT_WEAPON_MODE = 0,		// 무기별 노드
	NT_TALENT,				// 탤런트 노드
	NT_TALENT_MODE,			// 모드 탤런트 노드
	NT_ETC					// 미분류
};

enum TAB_MTRLSTATE
{
	TAB_MTRLSTATE_NONE = -1,
	TAB_MTRLSTATE_PC = 0,
	TAB_MTRLSTATE_NPC = 1,
	TAB_MTRLSTATE_PC_TALENT = 2,
	TAB_MTRLSTATE_ITEM = 3
};

enum TAB_TABSTATE
{
	TAB_TABSTATE_NONE = 0,
	TAB_TABSTATE_ITEM = 2,
};

enum CheckModifyData
{
	CHECK_MODIFY_FILENAME_EFFECT,
	CHECK_MODIFY_FILENAME_SOUND,
	CHECK_MODIFY_FILENAME_TALENT,
	CHECK_MODIFY_FILENAME_NPC_INFO,
	CHECK_MODIFY_FILENAME_BUFF_EFFECT_INFO,
	CHECK_MODIFY_FILENAME_MOTION_INFO
};

public ref class PCTalentTreeNodeTag
{
public:
	public: _PCTalentTreeNodeType nNodeType;
	public: int nTalentID;
	public: int nTalentMode;
	public: WEAPON_TYPE nWeaponType;

	PCTalentTreeNodeTag(_PCTalentTreeNodeType nNodeType) : nNodeType(nNodeType), nTalentID(0), nTalentMode(0), nWeaponType(WEAPON_NONE) {}
};

class CMainApplication  : public RRenderingCallbackInterface
{
	/// <summary>
	/// 메인 애플리케이션 클래스로서 핵심 클래스, 메인폼에 종속되어있다.
	/// </summary>
	string				m_strPath;
protected:
	CDocument *m_pDocument;
	CCmdBuffer *m_pCmdBuffer;

	HWND				m_hWnd;
	RSceneManager*		m_pSceneManager;
	TCharacter*			m_pActor;
	TCharacter*			m_pSampleActor;

	CSTalentInfo*		m_pTalentInfo;

	TEquipItems			m_SampleActorItem;

	// actor controllers
	RRotationController* m_pRotationControllerHorizonal;
	RRotationController* m_pRotationControllerVertical;

	TCamera*			m_pCamera;
	TCamera*			m_pLight;		//< 라이트 위치용으로 카메라 하나더.
	TCameraEffect		m_CameraEffect;

	RFont*				m_pFont;
	bool				m_bShowGrid;
	//RFont*				m_pTextFont;
	//RTextSceneNode*		m_pTextNode;

	TItemManager		m_ItemMgr;				///< 아이템 관리자

	//bool				m_bShowBones;
	//bool				m_bVertexNormal;
	//bool				m_bShowSpecular;
	//bool				m_bShowAABB;
	//RFILLMODE			m_FillMode;
	//bool				m_bShowWire;
	bool				m_bLightToCamerapos;

	bool				m_bFromMoveCheck;

	RTimer				m_Timer;
	TPlayMotion*		m_pMotion;

	TSoulHuntAnimationEventListener m_eventListener;

	unsigned int		m_time;
	list<RAnimation*>	m_returnAnimationList;

	vector<tstring>	m_vItemModelNames;

	TTalentEffectViewer	m_EffectViewControl;
	TPartsHitInfoViewer m_PartsHitInfoViewControl;
	TMeshColInfoViewer	m_MeshColInfoViewControl;
	TMeshHitEffectInfoViewer m_MeshHitEffectInfoViewControl;

	RDeviceD3D*			m_pDevice;

	bool				m_bLighting;
	RVector4			m_Ambient;

	// 애니메이션 플래그들
	bool				m_bLoopAnimation;
	bool				m_bTransitionBlending;

	bool				m_bShowCenterAxis;

	int					m_nPreFrame;

public:
	TNPC				m_NPC;
	TMtrl				m_Mtrl;

	TTabAnimation		m_TabAnimation;
	TTabModel			m_TabModel;
	TTabModelInfo		m_TabModelInfo;
	TTabItem			m_TabItem;

	TMenu				m_TMenu;

	TActorMgr*			m_pActorMgr;

	TBuffInfoMgr		m_BuffInfo;

public:
	//physx::MPhysX*		m_pPhx;
	TTalentInfoMgr		m_TTalentInfoMgr;
	TRealSound			m_pSoundMgr;

	string				m_strModelFullPathName;

	vector<string>		m_strEffectList;

	bool				m_bRenderActorCollision;

	bool				m_bMoveDummy;
	bool				m_bShowEffect;
	bool				m_bShowWaterEffect;

	int					m_nMousePrevX;
	int					m_nMousePrevY;
	

	RVector				m_vTop;
	RVector				m_vBottom;

	TTalentEffectMgr *	m_pTalentEffectMgr;
	TTalentEventMgr *	m_pEventMgr;
	TEffectInfoMgr *	m_pEffectInfoMgr;
	TEffectManager *	m_pEffectManager;

	bool				m_bShowPartsHitCapsule;
	bool				m_bShowPartsHitCapsuleLinkedHitEffect;
	bool				m_bShowMeshColCapsule;
	bool				m_bShowMeshHitEffectCapsule;
	bool				m_bShowNPCHitCapsuleGroup;

	vector<TProjectile*> m_vecProjectile;
	TMagicArea			m_MagicArea;

	bool				m_bPlayPauseButtonStateByShourtCut;

	//------------------------------------------------------------------------
	// [3/6/2008 jun] 재질 조합
	RActorNode*			m_pSelectedActorNode;
	RMaterial*			m_pSelectedMaterial;

	TAB_MTRLSTATE		m_SelectedTab;	// 현재 선택된 탭
	TAB_TABSTATE		m_SelectedTab2;	// 우측 탭리스트에서의 인덱스 // 3 : item

	TSwordTrailSampling	m_SwordTrailSampling;

	bool				m_bTalentAnimation;
	TTalentAnimation	m_TalentAnimation;
	TNormalAnimation	m_NormalAnimation;
	TAnimationBlendTable m_AnimationBledTable;

	bool				m_bCameraMove;

	TEquipItems			m_EquipItems;

	string				m_strWeaponEffectName[WEAPON_EFFECT_DUMMY_MAX_COUNT_TOOL];
	int					m_nWeaponEffectBuffID;
	int					m_nEnchantBuffEquipType;
	string				m_strEnchantBuffEquipType;

protected:
	bool	InitSceneManager();
	void	InitCamera();
	void	InitPhysx();
	void	InitFont();

	void	DestroySceneManager();

	void	Update(float fElapsed);
	void	UpdateLightToCamera(float time);
	void	UpdateProjectile(float fElapsed);
	void	UpdateMagicArea(float fElapsed);

	void	DrawPlatform();
	void	DrawBoxSelectedNode();
	void	DrawExistDummyLoc();

	void	PartsColorPreview();
	void	AmbientColorPreview();

	bool	CreateActor( const char* FileName );
	
	// RRenderingCallbackInterface
public:
	virtual void DeviceSetting();
	virtual void PreOrderRender();
	virtual void PostOrderRender();

public:
	CMainApplication();
	virtual ~CMainApplication();
	
	bool	Init(HWND hWnd);
	void	InitState(int nWidth, int nHeight, bool bReset=true);
	void	Destroy();

	void	SetHDREnable(bool bEnable = false);
	void	CameraReset();
	void	Reset();

	string	SetCurrentDir();

	void	OnLostDevice();
	void	OnResetDevice();

	bool	OnDraw();				///< Form에서 불려짐

	void DrawPartsHitCapsule();
	void DrawHitArea();
	void DrawHitEffectPosDir();
	void	OnUpdate();				///< Form에서 불려짐

	void	OnOffLight();
	void	OnDirLight();
	void	OffDirLight();


//	void	OnExcuteCommand(ICommand *pCmd);

	bool	InitDevice(HWND hWnd);

	void	InitItemList();
	void	InitMotionList();
	void	InitPCTalentList();

	static void	McvLog(const char* szText, MLogLevel nLevel = MLOG_LEVEL_INFORMATION);

	bool	CorrectXml(tstring& str);

	void	EventMessage(int index, System::String^ str1 = nullptr);
	void	SelectItemInItemList(int slot);

	//void	ShowAnimationInfo();

	bool	MeshFileOpen(System::String^ FileName);
	bool	MeshFileOpen(const char* FileName);
	RActor*	eluFileOpen(const char* FileName);
	bool	CharFileOpen(const char* FileName);
	void	ReloadCharacter();

	void	GetAnimationPos(bool bReturn = false);

	void	OnMouseMove(int stat, RVector2 pos, int delta);
	void	OnMouseMovePartEdit(int stat, RVector2 &pos, int delta);
	void	OnMouseMoveTalentEdit( int stat, RVector2 &pos, int delta );
	void	OnMouseMoveColEdit(int stat, RVector2 &pos, int delta);
	void	OnMouseMoveBoneNode(int stat, RVector2& pos, int delta);
	void	OnMouseMoveHitEffectEdit(int stat, RVector2 &pos, int delta);

	void	OnMouseClick(int stat, RVector2& pos, int delta);
	void	OnMouseClickBoneNode(int stat, RVector2& pos, int delta);

	void	OnMouseDown(int stat, RVector2 pos, int delta);

	void	OnCameraMouseMove(int stat, RVector2 pos, int delta);

	bool	SelectActorNode(RActorNode* pActorNode);

	void	AniTreeView_AnimationPlay(System::String^ Aniname);
	void	AniTreeView_AnimationPlay(const char* name);

	CSItemData*	GetItemData(int index);

	void	HideAllTabs();
	void	ShowTabs(int index);

	bool	OnPlayMotion(System::String^ str1 = nullptr);
	bool	IsAnimation(System::String^ str1);

	bool	SetAnimation(string strAniName);

	void	SetClearClolor(DWORD col);

	void	SetActorSpineTarget(float fAngle1,float fAngle2);

	bool	SaveAs();
	bool	SaveAnimationXml(const char* szFileName);
	bool	SaveAnimationXml(System::String^ FileName);

	System::Windows::Forms::TreeNode^ SearchNode(System::Windows::Forms::TreeNodeCollection^ nodes, System::String^ str);
	TreeNode^ SearchNode(TreeNodeCollection^ nodes, int nID);

	RActorNode*	GetActorNode(string strBoneName);
	TCharacter*	const GetActor()	{ return m_pActor;}
	void		DeleteActor();
	RDeviceD3D* GetDevice() { return m_pDevice;}
	RSceneManager*	GetSceneMgr()		{ return m_pSceneManager; }
	TCamera*		GetCamera()			{ return m_pCamera; }
	TPlayMotion*	GetMotion()			{ return m_pMotion;}
	RFont*			GetFont()			{ return m_pFont;	}
	TItemManager*	GetItemMgr()		{ return &m_ItemMgr;}
	TCameraEffect&	GetCameraEffect()	{ return m_CameraEffect; }

	void	SetLightAmbient(const RVector4& light);

	void	SetActorCollision();

	void	SetCurrentTalentInfo(CSTalentInfo* pTalentInfo);


	//------------------------------------------------------------------------
	void	RenderMeshCollisionIndex();

	bool	OnPlayAnimation();
	void	OnStopAnimation();
	void	OnPauseAnimation();
	void	OnLoopAnimation();
	void	OnTransitionBlending();
	void	OnFullScreen();

	void	OnPlayAnimationByShourtCut();

	void	RefreshModels();
	void	SetReviewItems();
	SH_ITEM_SLOT	GetNodetoItemslot(string text);

	//--------------------------------------------------------------------------
	// 라이트
	void			SetLightToCamera(bool bLightToCamera);

	// [2007/5/30 isnara] Talent Effect Attribute
	bool SaveTalentEffectInfo();


	//------------------------------------------------------------------------
	// [11/23/2007 isnara] Talent Event Attribute
	bool SaveTalentEventInfo();

	bool	SaveTalentHitInfo();

	void	SaveTalentExt(bool bSaveEvent = false, bool bSaveEffect = false, bool bSaveCol = false);

	void	ClearReturnAni();
	void	SetReturnAni();

	void	SetModelFullPathName(System::String^ ModelName);
	void	SetModelFullPathName(string strModelName);


	void	RotateUpdate(float fDelta);

	void	BasicMesh();

	void	SetProgressBarValue(int value = 0);
	void	SetTalent(CSTalentInfo* pTalentInfo);
	void	SetTalentEffect(CSTalentInfo* pTalentInfo);

	//------------------------------------------------------------------------
	// [1/4/2008 isnara] 부분 판정
	void	SetMeshHitInfo(System::String^ ModelName);
	void	SetPartsHitCapsuleInfo(int nGroupID, int nCapsuleID);
	void	ChangePartsHitCapsuleInfo(int nGroupID, int nCapsuleID);
	int		CreateMeshHitGroupInfo();
	int		CreateMeshHitCapsuleInfo(int nGroupID);
	void	DelMeshHitGroupInfo(int nGroupID);
	void	DelMeshHitCapsuleInfo(int nGroupID, int nCapsuleID);
	void	CheckShowPartsHitCapsule();
	int		GetNextCapsuleIndex(int nGroupID);


	//------------------------------------------------------------------------
	// [1/22/2008 isnara] 초기
	void	InitTalentHitInfo();


	//////////////////////////////////////////////////////////////////////////
	// 이팩트 보여주기
	//
	// reference mesh
	struct REFERENCE_MESH
	{
		REFERENCE_MESH() : m_bUseRefMesh(false), m_pReferenceMesh(NULL) {}

		std::string		m_strRefMeshName;
		bool			m_bUseRefMesh;
		TCharacter*		m_pReferenceMesh;
	} m_ReferenceMesh;

	float			GetExtraMotionTime(CSTalentInfo* pTalentInfo);	//탤런트 ExtraMotion 시간 알아내기
	WEAPON_TYPE		GetWeaponType(CSTalentInfo* pTalentInfo);				//< 탈렌트로 웨폰 타입 알아내기
	string			GetTalentAniName(string szName, int nTalentID);
	bool			CompareTalentTime(CSTalentInfo* pTalentInfo);
	float			GetMotionTime();
	void			CheckCustomMotion();
	void			DeleteCustomMotion( CSTalentInfo* info );
	void			AllTalentTimeRefresh();

	void			TalentTimeRefresh( CSTalentInfo* pTalentInfo, int &prevNPCID, bool &bPlayer );
	void			SetPlayer();

	void			SelectWeaponEffect();
	int				EnchantEquipType2PlayerPartsSlotType(string strType);
	WEAPON_TYPE		EnchantEquipType2WeaponType(string strType);
	void			DeleteWeaponEffect();
	bool			CheckWeaponEffect();

	//------------------------------------------------------------------------
	// [3/6/2008 jun] 재질 조합
	void			treeViewMeshList_NodeMouseClick();

	bool			IsPlayer();

	void			MovePartSegmentBottom(int dx, int dy, int dz);
	void			MovePartSegmentTop(int dx, int dy, int dz);

	//------------------------------------------------------------------------
	void ModelLoad( String^ nodeName, String^ parentName, String^ modelname );
	void PlayerModelLoad(String^ strSex);
	bool IsDefaultPlayerModelLoad( );

	//------------------------------------------------------------------------
	// [4/14/2008 isnara] 충돌 편집
	void	SetMeshColInfo(System::String^ ModelName);
	void	CheckShowMeshColCapsule();
	void	ChangeMeshColInfo(int nCapsuleID);
	int		CreateMeshColCapsuleInfo();
	int		CreateMeshColBoxInfo();
	void	DelMeshColCapsuleInfo(int nCapsuleID);
	void	DelMeshColBoxInfo(int nBoxID);

	void	SetCapsuleRenderState();
	void	EndCapsuleRenderState();

	void	MoveColSegmentBottom(int dx, int dy, int dz);
	void	MoveColSegmentTop(int dx, int dy, int dz);
	//--------------------------------------------------------------
	bool	TalentPosInfoRefresh(CSTalentInfo* pTalentInfo);
	bool	MeshInfoRefresh(string strModelName);
	
	// 저장 관련 함수
	void	SaveAll();
	void	SaveModelAnimation();

	// PhysX
	//RSceneManager* CreateSceneManager();

	int		GetTalentID()
	{
		if (m_pTalentInfo)
		{
			return m_pTalentInfo->m_nID;
		}

		return 0;
	}
	CSTalentInfo* GetTalentInfo() const { return m_pTalentInfo; }
	void SelectTalent(int nTalentID, int nTalentMode);

	void GetEffectInfoList(System::Collections::ArrayList^ effectList);
	void NPCListtreeView_AfterSelect(System::Object^ sender, System::Windows::Forms::TreeViewEventArgs^ e);
	void NPCListtreeView_NodeMouseClick(System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e);
	
	// 브레이커블 파츠
	//void OnOffBreakableParts(bool bChecked);
	void OnOffBreakableParts(int nIndex, bool bChecked);
	int BreakablePartsCount();
	//void CheckBreakableParts();
	//void InitBreakablePartsMenu();
	//System::Windows::Forms::ToolStripMenuItem^ GetBreakablePartsMenuItem(int nIndex);

	// 
	inline void ShowGrid(bool bShowGrid)
	{
		m_bShowGrid = bShowGrid;
	}

	void ModeltreeView_AfterSelect( System::Windows::Forms::TreeViewEventArgs^ e );

	void	DelProjectile();

	void MainView_KeyCheck(System::Windows::Forms::PreviewKeyDownEventArgs^  e);

	void	AnimationEventPlay(RAnimationEvent* pAnimationEvent);

	void ShowHideSamplePlayerModel(bool bShow);

	bool FileModifyCheck(CheckModifyData eModifyName);

	void ShowTestHitEffect(bool bShow);
	void ShowCenterAxis(bool bShow) { m_bShowCenterAxis = bShow; }

	//------------------------------------------------------------------------
	// 판정 캡슐
	void	SetMeshHitEffectInfo(System::String^ ModelName);
	int		CreateMeshHitEffectCapsuleInfo();
	void	DelMeshHitEffectCapsuleInfo(int nCapsuleID);
	void	CheckShowMeshHitEffectCapsule();
	void	ChangeMeshHitEffectCapsuleInfo(int nCapsuleID);

	void	MoveHitEffectSegmentBottom(int dx, int dy, int dz);
	void	MoveHitEffectSegmentTop(int dx, int dy, int dz);
	//------------------------------------------------------------------------

	void	SetMeshInfo(System::String^ ModelName);
	void	GetAnimationList(System::Collections::ArrayList^ arrayAnimationList);
	void	GetAnimationList(vector<string>& vecAnimationList);

	void	AnimationBlendEditTool();
	void	OvelayAnimationEditTool();
	void	ReActionAnimationEditTool();
	void	ReActionAnimationTest();
	void	ReActionAnimationNomalTest();
	void	ReActionAnimationBossTest();

	void	UpdateModelScale();

	void	SetNPCAnimationInfo(String^ strNpcName = nullptr);

	bool	SkinColorTest();
	bool	HairColorTest();
	bool	ItemColorTest();


	void	RenderAxisText(RMatrix mat, float fSize);

	String^	GetSearchCustomEffectInfoName();

	void	UpdateShowNPCHitCapsuleGroup(bool bShow, CSTalentInfo* pTalentInfo = NULL);

	void	StartTalentPosInfoExporter();

	private:
		TClothMgr				m_ClothMgr;	//옷관련 속성창 관리
};

extern 	CMainApplication *g_pMainApp;
RDeviceD3D*  GetDevice();

TreeNode^ SearchNode(TreeNodeCollection^ nodes, String^ str);