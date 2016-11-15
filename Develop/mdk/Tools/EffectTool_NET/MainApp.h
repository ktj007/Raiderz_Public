#pragma once

#include "Util.h"
#include "TCamera.h"
#include "MRTTI.h"
#include "REmitter.h"
#include "REffectActor.h"
#include "REffectSceneNode.h"
#include "REffectBase.h"
#include "RTrail.h"
#include "RRenderingCallbackInterface.h"

extern bool g_bModified;
extern bool g_bIgnoreValueChangedMessage;

namespace rs3 {
	class RS_API RDeviceD3D;
	class RS_API RFont;
	class RS_API RTextSceneNode;
	class RS_API RTrail;

	enum EMITTER_TYPE;
};

using namespace System::Windows::Forms;


using namespace rs3;
/**
 자료 관리 - 맵, 사용자 데이터
 커맨드 관리 - 모든 동작은 커맨드로 이루어 진다. ( Undo, Redo의 가능 )
*/

class TSceneManager;

class CMainApplication  : public RRenderingCallbackInterface
{
	/// <summary>
	/// 메인 애플리케이션 클래스로서 핵심 클래스, 메인폼에 종속되어있다.
	/// </summary>

	typedef vector<tstring> EFFECTLIST;

public:
	static const char*	EFFECT_FILE_EXT;
	static const size_t	EFFECT_FILE_EXT_LENGTH;

	TCHAR				szModuleFileName[_MAX_DIR];

	vector<string>		m_strEffectList;	//effectlist.xml에 기술된것

	bool				m_bShowPlatform;
	bool				m_bAxis;
	bool				m_bGrid;
	bool				m_bShowEmitter;		// 이미터, 액터, AABB
	bool				m_bAnimationLoop;
	bool				m_bShowLight;		// Light 범위

public:
	CMainApplication();
	virtual ~CMainApplication();
	
	//////////////////////////////////////////////////////////////////////////
	// 프레임웍/초기화/종료
	bool	Init(HWND hWnd);
	void	InitDevice(HWND hWnd);
	void	InitState(int nWidth, int nHeight, bool bReset=true);
	void	Destroy();

	void	OnLostDevice();
	void	OnResetDevice();

	//////////////////////////////////////////////////////////////////////////
	// View 관련
	bool	OnDraw();
	void	OnUpdate();
	void	OnUpdateKeys();

	void	CameraReset();
	void	CameraPosCal();

	void	InitEffectList();
	bool	SetEffectListTreeView();
	void	SetEffectListTreeViewFilter(System::String^ strFilter);

	void	UpdateKey(float time);
	void	UpdateCameraKey(float time);

	void	OnMouseMove(int stat, RVector2 pos, int delta);
	
	void	SetClearColor(DWORD col);
	void	SetFogSettingEnable(bool bEnable);
	void	SetHDREnable(bool bEnable);
	void	SetFillMode();
	void	SetLightToggle();
	void	SetLightToCameraToggle();
	void	SetLightAmbient(const RVector4& ambient);

	void		DrawBoxSelectedNode();
	void		DrawEmitter(bool bShowEmitter);
	void		DrawActor(bool bShowActor);
	void		DrawPlatform();
	void		DrawInfoText();
	void		DrawEmitterAABB(bool bShowAABB);
	void		DrawLightAABB(bool bShowAABB);

	void		OnLoopAnimation();
	void		StartButton();
	void		StopButton();
	void		PauseButton();


	//////////////////////////////////////////////////////////////////////////
	// 이펙트 편집
	void			UndoCommand();
	void			RedoCommand();

	System::String^ GetProperNodeName( System::String^ strNodeName );

	TreeNode^		SearchNode(TreeNodeCollection^ nodes, System::String^ str);

	bool			CreateEmitter( TreeNode^ parentTreeNode, EMITTER_TYPE type);
	bool			CreateTrail( TreeNode^ parentTreeNode );
	bool			CreateLight( TreeNode^ parentTreeNode, bool bSpotLight );
	bool			CreateEffectSceneNode( TreeNode^ parentTreeNode );

	void			AttachChild( RSceneNode* pNode, TreeNode^ parentTreeNode);
	void			MoveSceneNode( TreeNode^ treeNode, TreeNode^ parentTreeNode );
	void			CopySceneNode( TreeNode^ treeNode, TreeNode^ parentTreeNode );
	void			LinkSceneNode( TreeNode^ treeNode, TreeNode^ parentTreeNode );

	void			MoveUpDownSceneNode( TreeNode^ treeNode, bool bUp );	///< bUp true 이면 up, false 이면 down

	void			MarkSameNamedNodes();	///< 같은 이름을 가진 노드를 마크한다
	void			MarkSameNamedNodes(TreeNodeCollection^ nodes);	///< 같은 이름을 가진 노드를 마크한다
	void			ApplyToAllSameNamed( RSceneNode* pNode );		///< 같은 이름 노드에게 적용

	// Child는 child-node 중 REffectBase를 상속받은 계통을 이야기함.
	void			DeleteChild(TreeNode^ treeNode);
//	bool			OnChildFileOpen(System::String^ strFileName);
	bool			OnChildFileOpen(TreeNode^ treeNode, const char* FileName);

	bool			ImportSceneXML(TreeNode^ treeNode, const char* FileName);

	void			SetEffectName(System::String^ strEffectName);
	void			SetEmitterTexture(System::String^ strName);
	void			SetEmitterSITexture(System::String^ strName);
	bool			ChangeEmitterType(TreeNode^ treeNode, EMITTER_TYPE type);

	void			SetTrailTexture(System::String^ strName);
	void			SetTrailSITexture(System::String^ strName);

	std::string		SetChildName(System::String^ str); //return value : 취소할때 되돌릴이름
	void			RenameEffectForSaveAs(System::String^ name);

	//////////////////////////////////////////////////////////////////////////
	// selection
	void			SetChild(System::String^ strEmitterName);
	void			SetChild(RSceneNode* pSceneNode);
	void			SelectTreeNode(TreeNode^ parentTreeNode, RSceneNode* pSceneNode);

	RSceneNode*			GetSelectedChild();

	REmitter*			GetSelectedEmitter();
	RTrail*				GetSelectedTrail();
	
	REffectActor*		GetSelectedEffectActor()	{ return m_pSelectedEffectActor; }
	REffectSceneNode*	GetCurrentEffectSceneNode()	{ return m_pCurrentEffectSceneNode; }
	RActor*				GetActor()					{ return m_pActor;	}

	//////////////////////////////////////////////////////////////////////////
	// 파일 관련
	void		OnNew();

	bool		LoadEffectFile(System::String^ FileName);
	bool		LoadEffectFile(const char* FileName);

	bool		SaveEffectFile();
	bool		SaveAs();
	bool		SaveEffectFile(const char* szFileName);

	bool		MeshFileOpen(System::String^ strFileName);
	bool		MeshFileOpen(const char* FileName, bool bAddMRUList );
	bool		MeshFileOpenFromMRUList(int index);
	bool		DefaultMeshFileOpen();
	void		DeleteActor();
	void		DeleteEffectInstance();
	bool		eluFileOpen(const char* FileName);
	bool		LoadAniFile(const char* FileName);

	bool		SetAnitreeView();
	void		SetAnitreeViewFilter(System::String^ strFilter);

	bool		SetNodeListTreeView();
	void		SetNodeListTreeViewFilter(System::String^ strFilter);

	void		SetAnimation_Play(System::String^ Name);
	void		SelectedNode(System::String^ strNodeName);

	void		ReloadEffect();
	void		ReloadEffectList();
	void		ReloadActor();
	void		ReSelectNode();
	void		RePlayAnimation();
	bool		ConfirmSave();	// false를 반환하면 취소 - 090602, OZ
	void		SetEmitterNameAutoUpdate(bool bEnable)	{ m_bEmitterNameAutoUpdate = bEnable; }		//Save as 시 emitter 이름을 자동으로 업데이트할지 여부 결정

	void		CopySceneNode( TreeNode^ treeNode );
	void		PasteSceneNode( TreeNode^ parentTreeNode );

	//////////////////////////////////////////////////////////////////////////
	// 기타
	RDeviceD3D* GetDevice() { return m_pDevice;}
	TCamera*	GetCamera() { return m_pCamera; }

	void		AttachEffect( REffectSceneNode* pEffectInst, RSceneNode* pSceneNode, RActorNode* pActorNode , const RMatrix& _rBaseFrame = RMatrix::IDENTITY, bool bIgnoreDir = false );

	static void	McvLog(const char* szText, MLogLevel nLevel = MLOG_LEVEL_INFORMATION);

	// RRenderingCallbackInterface
public:
	virtual void PreOrderRender();
	virtual void PostOrderRender();

protected:
	HWND				m_hWnd;
	TSceneManager*		m_pSceneManager;//////////////////////
	RDeviceD3D*			m_pDevice;

	TCamera*			m_pCamera;

	RActor*				m_pActor;
	RActorNode*			m_pSelectedActorNode;

	RSceneNode*			m_pSelectedChild;
	vector<RSceneNode*>	m_vecSameNamed;		///< 선택된 노드와 같은이름을 가진 노드들
	REffectActor*		m_pSelectedEffectActor;
	REffectSceneNode*	m_pCurrentEffectSceneNode;///////////////////////////
	string				m_strFileName;	///< 편집중인 파일 이름

	RSceneNode*			m_pCopiedSceneNode;	// 복사한 씬노드

	bool				m_bShowAABB;
	RFILLMODE			m_FillMode;

	RFont*				m_pFont;
	RTextSceneNode*		m_pTextNode;

	unsigned int		m_time;

	string				m_loadedActorName;
	string				m_loadedActorAnimationName;
	string				m_loadedActorLinkedDummyName;

	bool				m_bEmitterNameAutoUpdate;

	string				m_EffectListTreeViewFilter;
	string				m_AniTreeViewFilter;
	string				m_NodeListTreeViewFilter;

	bool				m_bUseLighting;
	bool				m_bLightToCameraDir;

	EFFECTLIST			m_EffectList; // 이펙트이름

protected:
	void				RefreshSceneNodeTreeView();
	void				RefreshSceneNodeTreeView( RSceneNode* pSceneNode );
	void				ReorderSceneRendering();
	TreeNode^			MakeTreeNode(RSceneNode* pSceneNode);
	void				SetTreeNodeIcon(RSceneNode* pSceneNode, TreeNode^ treeNode);
	int					GetTreeNodeIcon(RSceneNode* pSceneNode);

	void				UpdateCaption();
	void				UpdateLightToCamera();
};

extern 	CMainApplication *g_pMainApp;
