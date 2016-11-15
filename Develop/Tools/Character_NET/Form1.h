#pragma once

#using <System.dll>
#using <System.Windows.Forms.dll>
#using <System.Xml.dll>

#include "TDef.h"
#include "MainApp.h"
#include "EditItemList.h"
#include "EditAnimation.h"
#include "AnimationEvent.h"

#include "TMeshHitTabController.h"
#include "TTalentEffectTabController.h"
#include "TFrameSliderController.h"
#include "TTalentHelper.h"
#include "TStrings.h"

#include "ColorPickerDlg.h"
#include "PCTalentTreeView.h"
#include "NPCListTabPage.h"
#include "ModelTabPage.h"
#include "ItemTabPage.h"
#include "TalentTabPage.h"
#include "AnimationTabPage.h"
#include "ModelInfoTabPage.h"
#include "EventTabPage.h"
#include "TInitLoadingWindow.h"
#include "MeshInfoTabPage.h"
#include "ReActionAnimationTable.h"

namespace Character_NET {

	using namespace System;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;

	using namespace ColorPickerControls;

	/// <summary>
	/// Form1에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		ReActionAnimationTable^		m_ReActionAnimationTable;

	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
			//m_TalentEventController = gcnew TTalentEventController(this);
			m_MeshHitTabController = new TMeshHitTabController();
			m_TalentEffectTabController = new TTalentEffectTabController();
			m_ColorPickerDialog = gcnew ColorPickerDlg();
			m_AmbientColorPickerDialog = gcnew ColorPickerDlg();
			m_FrameSliderController = new TFrameSliderController();

			Create_ModelTabPage();
			Create_NPCListTabPage();
			Create_PCTalentTab();
			Create_ItemTabPage();
			Create_TalentTabPage();
			Create_AnimationTabPage();
			Create_ModelInfoTabPage();
			Create_EventTabPage();
			Create_MeshInfoTabPage();
		}

		void Create_EventTabPage();
		void Create_ModelInfoTabPage();
		void Create_AnimationTabPage();
		void Create_TalentTabPage();
		void Create_ItemTabPage();
		void Create_NPCListTabPage();
		void Create_ModelTabPage();
		void Create_PCTalentTab();
		void Create_MeshInfoTabPage();

		void ShowReactionAnimationTable();

		void InitLoadingInfoText(string strLoadingInfo);

	private:
		int	GetMouseButtonState(System::Windows::Forms::MouseButtons btn);
	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
			if (m_MeshHitTabController)
			{
				delete m_MeshHitTabController;
			}
			if (m_TalentEffectTabController)
			{
				delete m_TalentEffectTabController;
			}
			SAFE_DELETE(m_FrameSliderController);
		}
	public: EventTabPage^	m_EventTabPage;
	public: ModelInfoTabPage^	m_ModelInfoTabPage;
	public: AnimationTabPage^ m_AnimationTabPage;
	public: TalentTabPage^	m_TalentTabPage;
	public: ItemTabPage^	m_ItemTabPage;
	public: NPCListTabPage^	m_NPCListTabPage;
	public: ModelTabPage^	m_ModelTabPage;
	public: MeshInfoTabPage^	m_MeshInfoTabPage;
	//public: TTalentEventController^ m_TalentEventController;
	public: TMeshHitTabController* m_MeshHitTabController;
	public: TTalentEffectTabController* m_TalentEffectTabController;
	public: TFrameSliderController* m_FrameSliderController;
	public: TInitLoadingWindow m_InitLoadingWindow;

	public: System::Windows::Forms::MenuStrip^  MainmenuStrip;
	public: System::Windows::Forms::StatusStrip^  statusStrip;
	public: System::Windows::Forms::TrackBar^  AnitrackBar;
	private: System::Windows::Forms::Button^  stopbutton;
	public: System::Windows::Forms::Button^  playbutton;

	private: 

	public: System::Windows::Forms::GroupBox^  groupBox3;
	public: System::Windows::Forms::ToolStrip^  toolStrip1;
	public: System::Windows::Forms::Label^  Framelabel;

	public: System::Windows::Forms::GroupBox^  groupBox4;
	public: System::Windows::Forms::Label^  Speedlabel;
	public: System::Windows::Forms::TrackBar^  AniSpeedtrackBar;
	public: System::Windows::Forms::Label^  Framelabel2;

	public: System::Windows::Forms::ToolStripStatusLabel^  CameratoolStripStatusLabel;
	public: System::Windows::Forms::ToolStripButton^  LighrtoolStripButton;
	public: System::Windows::Forms::ToolStripButton^  LightToCameratoolStripButton1;
	public: System::String^				m_ParentName;



	public: System::Windows::Forms::SplitContainer^  splitContainer1;
	public: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage4;



	public: System::Windows::Forms::SplitContainer^  splitContainer2;


















































	public: System::Windows::Forms::TextBox^  LogtextBox;
	public: System::Windows::Forms::ToolStripButton^  DrawBoxtoolStripButton;
	private: System::Windows::Forms::ToolStripButton^  OpenToolStripButton;
	private: System::Windows::Forms::ToolStripButton^  SaveToolStripButton;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator;
	private: System::Windows::Forms::ToolStripMenuItem^  EdirToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator5;
	private: System::Windows::Forms::ToolStripMenuItem^  ToolToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  HelpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  InfomationToolStripMenuItem;
	private: System::Windows::Forms::ToolStripButton^  AmbienttoolStripButton1;
	private: System::Windows::Forms::ToolStripMenuItem^  showBoneToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  ShowNormalToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  showWireToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cameraResetSpaceBarToolStripMenuItem;
	public: System::Windows::Forms::ToolStripMenuItem^  collisionObjectToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  backgroundColorToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  feedbackToolStripMenuItem;




	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
	private: System::Windows::Forms::ToolStripMenuItem^  ReloadToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  selectModelTabToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  selectNPCListTablToolStripMenuItem;
	public: System::Windows::Forms::ToolStripMenuItem^  drawAABBToolStripMenuItem;

	private: System::Windows::Forms::ToolStripMenuItem^  FullScreenToolStripMenuItem;
	public: System::Windows::Forms::CheckBox^  LoopPlaycheckBox;
	private: System::Windows::Forms::ToolStripMenuItem^  LighttToolStripMenuItem;
	public: System::Windows::Forms::ToolStripMenuItem^  lightToolStripMenuItem;
	public: System::Windows::Forms::ToolStripMenuItem^  lightToCameraposToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  ambientToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  AnimToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  animationPlayToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  animationStopToolStripMenuItem;
	public: System::Windows::Forms::ToolStripMenuItem^  animationLoopPlayToolStripMenuItem;
	public: System::Windows::Forms::ToolStripMenuItem^  transitionBlendingToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator6;
	private: System::Windows::Forms::ToolStripMenuItem^  ShowWireShade;
	private: System::Windows::Forms::Button^  AniSpeedResetbutton;
	public: System::Windows::Forms::NumericUpDown^  AniSpeednumericUpDown;

	private: System::Windows::Forms::ToolStripMenuItem^  EditEToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  UndoToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  RedoToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator10;



	private: System::Windows::Forms::TabPage^  tabPage2;
	public: PCTalentTreeView^  m_PCTalenttreeView;




	private: System::Windows::Forms::ToolStripMenuItem^  FeedBack2ToolStripMenuItem;
	public: System::Windows::Forms::CheckBox^  MoveDummycheckBox;


	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator7;
	public: System::Windows::Forms::ToolStripButton^  Unselected_toolStripButton;
	public: System::Windows::Forms::ToolStripButton^  Bottom_toolStripButton;
	public: System::Windows::Forms::ToolStripButton^  Top_toolStripButton;
	public: System::Windows::Forms::ToolStripButton^  X_toolStripButton;
	public: System::Windows::Forms::ToolStripButton^  Y_toolStripButton;
	public: System::Windows::Forms::ToolStripButton^  Z_toolStripButton;
	public: System::Windows::Forms::ToolStripButton^  XY_toolStripButton;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator8;

	public: System::Windows::Forms::ToolStripButton^  CapsuleOpa_toolStripButton;
	private: System::Windows::Forms::ToolStripMenuItem^  Talent_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  Unselect_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  Bottom_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  Top_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  X_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  Y_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  Z_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  XY_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  Opa_toolStripMenuItem;
	public: System::Windows::Forms::ToolStripButton^  TopBottom_toolStripButton;
	private: System::Windows::Forms::ToolStripMenuItem^  TopBottom_toolStripMenuItem;

	private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip2;
	private: System::Windows::Forms::ToolStripMenuItem^  ItemSlot_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotHead_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotFace_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotHands_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotFeet_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotBody_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotLeg_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotLfinger_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotRfinger_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotNeck_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotCharm_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotLweapon_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotRweapon_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotReserved1_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotReserved2_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotReserved3_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SlotReserved4_ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  dummy1hpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_loc_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_1hs_l_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_1hs_r_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_1hb_l_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_1hb_r_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_1hp_l_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_1hp_r_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_2hd_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_pol_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_2hs_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_2hp_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_arc_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_gpd_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_shield_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_lh_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  LW_arrow_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  RW_loc_toolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem21;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem22;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem23;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem24;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem25;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem26;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem27;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem28;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem29;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem30;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem31;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem32;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem33;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem34;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem35;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem36;

	private: System::Windows::Forms::ToolStripMenuItem^  UnEquipToolStripMenuItem;
	public: System::Windows::Forms::ToolStripStatusLabel^  Status_toolStripStatusLabel;

	public: System::Windows::Forms::PictureBox^  MainView;






	private: System::Windows::Forms::ToolStripMenuItem^  FileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  OpenToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
	private: System::Windows::Forms::ToolStripMenuItem^  SaveToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SaveAsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator3;
	private: System::Windows::Forms::ToolStripMenuItem^  TalentInfoSave_Click;
	private: System::Windows::Forms::ToolStripMenuItem^  SaveNPCHitInfo;
	private: System::Windows::Forms::ToolStripMenuItem^  miTalentExport;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator4;
	private: System::Windows::Forms::ToolStripMenuItem^  eXitToolStripMenuItem;

	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator9;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  ViewreferenceMeshToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  SetReferenceMeshToolStripMenuItem;

	public: System::Windows::Forms::ToolStripMenuItem^  partsHitCapsuleToolStripMenuItem;
	public: System::Windows::Forms::CheckBox^  checkShowEffect;
	private: System::Windows::Forms::ToolStripMenuItem^  PartsColorPreviewMenuItem;













	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Button^  button4;
	private: System::Windows::Forms::Button^  button5;
	private: System::Windows::Forms::Button^  button6;
	private: System::Windows::Forms::Button^  button7;
	private: System::Windows::Forms::Button^  button8;
	private: System::Windows::Forms::Button^  button9;
	private: System::Windows::Forms::Button^  button10;








	private: System::Windows::Forms::ToolStripMenuItem^  BuffEffectSave;


























	private: System::Windows::Forms::Button^  button11;
	public: System::Windows::Forms::PropertyGrid^  propertyGrid1;
	private: System::Windows::Forms::Button^  button12;
	private: System::Windows::Forms::Button^  button13;
	public: System::Windows::Forms::TreeView^  treeView1;
	public: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem2;


	private: System::Windows::Forms::ToolStripMenuItem^  SaveAlltoolStripMenuItem;
	public: System::Windows::Forms::ToolStripProgressBar^  Status_toolStripProgressBar;

	// 저장할 파일들 표시용 라벨
	private: System::Windows::Forms::ToolStripStatusLabel^  TalentPosInfoStatusLabel;
	private: System::Windows::Forms::ToolStripStatusLabel^  TalentExtStatusLabel;
	private: System::Windows::Forms::ToolStripStatusLabel^  TalentEffectStatusLabel;
	private: System::Windows::Forms::ToolStripStatusLabel^  TalentHitInfoStatusLabel;
	private: System::Windows::Forms::ToolStripStatusLabel^  MeshInfoStatusLabel;
	private: System::Windows::Forms::ToolStripStatusLabel^  BuffEffectInfoStatusLabel;
	private: System::Windows::Forms::ToolStripStatusLabel^  ChangeMtrlItemStatusLabel;
	private: System::Windows::Forms::ToolStripStatusLabel^  ChangeMtrlNPCStatusLabel;

	 // 파일메뉴
	private: System::Windows::Forms::ToolStripMenuItem^  AllTalentTime;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator12;
	private: System::Windows::Forms::ContextMenuStrip^  NPCcontextMenuStrip;
	private: System::Windows::Forms::ToolStripMenuItem^  COLTEDitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  ScriptEditToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  DisableSpeculartoolStripMenuItem;



















	public:	ColorPickerDlg^			m_ColorPickerDialog;
	public:	ColorPickerDlg^			m_AmbientColorPickerDialog;




	private: System::Windows::Forms::ToolStripMenuItem^  ToolStripMenuItemExecuteValidator;







	private: System::Windows::Forms::ToolStripMenuItem^  hDRToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  drawGridToolStripMenuItem;
private: System::Windows::Forms::TabPage^  tabPage1;
private: System::Windows::Forms::ToolStripMenuItem^  selectPCTalentTabltoolStripMenuItem;




public: System::Windows::Forms::Button^  pausebutton;
public: System::Windows::Forms::Panel^  TalentAniNameBar;
public: System::Windows::Forms::TabControl^  tabControl2;
public: System::Windows::Forms::TabPage^  Itemtab;
public: System::Windows::Forms::TabPage^  Talenttab;
public: System::Windows::Forms::TabPage^  Animationtab;
public: System::Windows::Forms::TabPage^  ModelInfotab;


public: System::Windows::Forms::TabPage^  ItemPreviewtabPage;
private: System::Windows::Forms::Button^  basicMesh_button;
public: 
private: System::Windows::Forms::Button^  Refresh_button;
private: System::Windows::Forms::Button^  ItemOpenbutton;
public: System::Windows::Forms::TreeView^  ItemPreviewtreeView;
private: 
public: System::Windows::Forms::TabPage^  Effecttab;


public: 


private: 

public: 



private: 



public: 



private: 


public: 



private: 

public: 

private: 

public: 




private: 

public: System::Windows::Forms::TabPage^  TalentEvent;
public: System::Windows::Forms::TabPage^  PartsJudgment;
private: System::Windows::Forms::TabControl^  HitTabControl;
public: 
public: System::Windows::Forms::TabPage^  CollisionTabPage;
private: 

public: 
public: System::Windows::Forms::Button^  btnMeshColPasteCapsule;
private: 
public: System::Windows::Forms::Button^  btnMeshColCopyCapsule;
public: System::Windows::Forms::PropertyGrid^  CollisionCapsuleAtt;
private: System::Windows::Forms::Button^  Del_Collision;
public: 

private: System::Windows::Forms::Button^  Add_CollisionCapsule;
public: 

public: 


public: 


private: 
public: System::Windows::Forms::TextBox^  CollisionModelName;
private: System::Windows::Forms::Label^  label15;
public: 

public: System::Windows::Forms::TabPage^  tabPage5;
private: 
public: System::Windows::Forms::TextBox^  PartsHitModelName;
private: System::Windows::Forms::Label^  label10;
public: 

public: System::Windows::Forms::Button^  btnMeshHitCapsulePaste;
private: 
public: System::Windows::Forms::Button^  btnMeshHitCapsuleCopy;
private: System::Windows::Forms::Button^  Add_PartsGroup;
public: 
public: System::Windows::Forms::PropertyGrid^  PartsHitCapsuleAtt;
private: 
private: System::Windows::Forms::Button^  Del_PartsCapsule;
public: 
private: System::Windows::Forms::Button^  Add_PartsCapsule;

private: 
public: System::Windows::Forms::TabPage^  SubMtrltab;
private: System::Windows::Forms::Button^  buttonDel;
public: 
private: System::Windows::Forms::Button^  buttonApply;
public: System::Windows::Forms::ListBox^  listBoxMtrlList;
private: 
private: System::Windows::Forms::GroupBox^  groupBox17;
public: 
private: System::Windows::Forms::GroupBox^  groupBoxMtrlList;
public: System::Windows::Forms::TextBox^  textBoxCurrentMtrl;
private: 
public: System::Windows::Forms::TreeView^  treeViewMeshList;
private: System::Windows::Forms::GroupBox^  groupBoxExport;
public: 
private: System::Windows::Forms::Button^  buttonExport;
private: System::Windows::Forms::CheckBox^  checkBoxShowBone;
private: System::Windows::Forms::ToolStripMenuItem^  DataValidatorToolStripMenuItem;
public: System::Windows::Forms::Panel^  dummy_loc_panel;
public: System::Windows::Forms::ToolStripMenuItem^  showPlayerModelToolStripMenuItem;
private: System::Windows::Forms::ToolStripSplitButton^  ReLoad_toolStripSplitButton;
public: 
private: System::Windows::Forms::ToolStripMenuItem^  effectxmlToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  soundxmlToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  talentxmlToolStripMenuItem;


private: System::Windows::Forms::ToolStripMenuItem^  buffeffectinfoxmlToolStripMenuItem;
public: System::Windows::Forms::ToolStripMenuItem^  DrawInteractionAABBToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  showTestHitEffectDirToolStripMenuItem;
private: System::Windows::Forms::TabPage^  HitEffect_tabPage;

public: System::Windows::Forms::Button^  capsule_Paste_button;
private: 

public: System::Windows::Forms::Button^  capsule_Copy_button;
public: System::Windows::Forms::PropertyGrid^  HitEffect_propertyGrid;
private: 


public: 

private: System::Windows::Forms::Button^  capsule_Del_button;
public: 
private: 

private: System::Windows::Forms::Button^  capsule_Add_button;
public: 


public: System::Windows::Forms::TextBox^  HitEffectModelName;
private: 

private: 

private: 

private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::ToolStripMenuItem^  showCenterAxisToolStripMenuItem;

private: 





public: 

public: 

public: 




private: 






public: System::Windows::Forms::TabPage^  MeshInfotab;
private: System::Windows::Forms::ToolStripMenuItem^  motionxxxmlToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  npcxmlToolStripMenuItem;
public: System::Windows::Forms::CheckBox^  WaterEffect_checkBox;
private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator11;
public: System::Windows::Forms::ToolStripButton^  bp4;
public: System::Windows::Forms::ToolStripButton^  bp1;
private: 

public: System::Windows::Forms::ToolStripButton^  bp2;
public: System::Windows::Forms::ToolStripButton^  bp3;

private: System::Windows::Forms::ToolStripMenuItem^  showWeaponEffectToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  effectSelectToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  effectSelectDeleteToolStripMenuItem;
private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator13;
private: System::Windows::Forms::ToolStripMenuItem^  AnimationBlend_ToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  OvelayAnimation_ToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  ReActionAnimation_ToolStripMenuItem;
private: System::Windows::Forms::SplitContainer^  splitContainer3;
private: System::Windows::Forms::SplitContainer^  splitContainer4;
private: System::Windows::Forms::SplitContainer^  splitContainer5;
private: System::Windows::Forms::ToolStripMenuItem^  PlayerNPCAnimationInfoToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  partsColorTestToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  SkinColorTestToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  HairColorTestToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  ItemColorTestToolStripMenuItem;
private: System::Windows::Forms::FontDialog^  fontDialog1;
private: System::Windows::Forms::SplitContainer^  splitContainer6;
private: System::Windows::Forms::GroupBox^  groupBox13;
public: System::Windows::Forms::TextBox^  CurrentID;
private: 
public: System::Windows::Forms::TabControl^  TalentEffectList_tabControl;
private: System::Windows::Forms::TabPage^  tabPage3;
public: 
public: ::TreeViewMS::TreeViewMS^  TalentEffectTreeView;
private: 
private: System::Windows::Forms::Button^  btnPasteEffectInfo;
public: 
private: System::Windows::Forms::Button^  btnCopyEffectInfo;
public: System::Windows::Forms::Button^  Delattbutton;
private: 
public: System::Windows::Forms::Button^  AddEffectInfoBtn;
private: System::Windows::Forms::TabPage^  tabPage6;
public: 
public: System::Windows::Forms::Button^  Disable_button;
private: 
public: System::Windows::Forms::Button^  Enable_button;
public: System::Windows::Forms::TreeView^  HitEffectPosDirtreeView;
private: System::Windows::Forms::Label^  label24;
public: 
private: System::Windows::Forms::Label^  label18;
private: System::Windows::Forms::Button^  RefSelectBtn;
public: System::Windows::Forms::TextBox^  RefTextBox;
private: 
private: System::Windows::Forms::Button^  button1;
public: 
private: System::Windows::Forms::GroupBox^  groupBox10;
public: System::Windows::Forms::PropertyGrid^  TalentEffectAttGrid;
public: System::Windows::Forms::Button^  Deref_button;
private: System::Windows::Forms::ToolStripMenuItem^  showLogWindowToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem4;
public: ::TreeViewMS::TreeViewMS^  CollisionCapsulesView;
public: ::TreeViewMS::TreeViewMS^  PartsHitCapsulesView;
public: ::TreeViewMS::TreeViewMS^  HitEffect_treeView;
private: System::Windows::Forms::CheckBox^  showHitEffectCapsule_checkBox;
private: System::Windows::Forms::Button^  Add_CollisionBox;
private: System::Windows::Forms::ToolStripMenuItem^  showNPCHitCapsuleGroupMenuItem;



	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
// 		bool	bAniPause;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::Button^  SpeedOkbutton;
			System::Windows::Forms::ToolStripMenuItem^  LW_quiver_toolStripMenuItem;
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->MainmenuStrip = (gcnew System::Windows::Forms::MenuStrip());
			this->FileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->OpenToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SaveAsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->SaveAlltoolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->SaveToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->TalentInfoSave_Click = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SaveNPCHitInfo = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->BuffEffectSave = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator12 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->miTalentExport = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->AllTalentTime = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator4 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->eXitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->EditEToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->UndoToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->RedoToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator10 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->EdirToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showBoneToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ShowNormalToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showWireToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->drawGridToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->partsHitCapsuleToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem2 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem4 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showPlayerModelToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showTestHitEffectDirToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showCenterAxisToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showWeaponEffectToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->effectSelectToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->effectSelectDeleteToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showNPCHitCapsuleGroupMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ShowWireShade = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->DisableSpeculartoolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator5 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->cameraResetSpaceBarToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->collisionObjectToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->ReloadToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->selectModelTabToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->selectNPCListTablToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->selectPCTalentTabltoolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->drawAABBToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->DrawInteractionAABBToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->FullScreenToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->showLogWindowToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator9 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ViewreferenceMeshToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SetReferenceMeshToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->hDRToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->partsColorTestToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SkinColorTestToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->HairColorTestToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ItemColorTestToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LighttToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->lightToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->lightToCameraposToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ambientToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->AnimToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->animationPlayToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->animationStopToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->animationLoopPlayToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->transitionBlendingToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator13 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->AnimationBlend_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->OvelayAnimation_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ReActionAnimation_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ToolToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->backgroundColorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->feedbackToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->FeedBack2ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->PartsColorPreviewMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ToolStripMenuItemExecuteValidator = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->DataValidatorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->PlayerNPCAnimationInfoToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Talent_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Unselect_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Bottom_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Top_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->TopBottom_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->X_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Y_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Z_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->XY_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->Opa_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->HelpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->InfomationToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->statusStrip = (gcnew System::Windows::Forms::StatusStrip());
			this->Status_toolStripProgressBar = (gcnew System::Windows::Forms::ToolStripProgressBar());
			this->Status_toolStripStatusLabel = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->CameratoolStripStatusLabel = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->AnitrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->stopbutton = (gcnew System::Windows::Forms::Button());
			this->playbutton = (gcnew System::Windows::Forms::Button());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->WaterEffect_checkBox = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxShowBone = (gcnew System::Windows::Forms::CheckBox());
			this->TalentAniNameBar = (gcnew System::Windows::Forms::Panel());
			this->pausebutton = (gcnew System::Windows::Forms::Button());
			this->checkShowEffect = (gcnew System::Windows::Forms::CheckBox());
			this->MoveDummycheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->LoopPlaycheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->Framelabel2 = (gcnew System::Windows::Forms::Label());
			this->Framelabel = (gcnew System::Windows::Forms::Label());
			this->dummy_loc_panel = (gcnew System::Windows::Forms::Panel());
			this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
			this->OpenToolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->SaveToolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->DrawBoxtoolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator6 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->LighrtoolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->LightToCameratoolStripButton1 = (gcnew System::Windows::Forms::ToolStripButton());
			this->AmbienttoolStripButton1 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator7 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->Unselected_toolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->Bottom_toolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->Top_toolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->TopBottom_toolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->X_toolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->Y_toolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->Z_toolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->XY_toolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->CapsuleOpa_toolStripButton = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator8 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->ReLoad_toolStripSplitButton = (gcnew System::Windows::Forms::ToolStripSplitButton());
			this->effectxmlToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->soundxmlToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->talentxmlToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->buffeffectinfoxmlToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->motionxxxmlToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->npcxmlToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator11 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->bp1 = (gcnew System::Windows::Forms::ToolStripButton());
			this->bp2 = (gcnew System::Windows::Forms::ToolStripButton());
			this->bp3 = (gcnew System::Windows::Forms::ToolStripButton());
			this->bp4 = (gcnew System::Windows::Forms::ToolStripButton());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->AniSpeedResetbutton = (gcnew System::Windows::Forms::Button());
			this->AniSpeednumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->Speedlabel = (gcnew System::Windows::Forms::Label());
			this->AniSpeedtrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->splitContainer2 = (gcnew System::Windows::Forms::SplitContainer());
			this->MainView = (gcnew System::Windows::Forms::PictureBox());
			this->tabControl2 = (gcnew System::Windows::Forms::TabControl());
			this->Itemtab = (gcnew System::Windows::Forms::TabPage());
			this->Talenttab = (gcnew System::Windows::Forms::TabPage());
			this->Animationtab = (gcnew System::Windows::Forms::TabPage());
			this->ModelInfotab = (gcnew System::Windows::Forms::TabPage());
			this->ItemPreviewtabPage = (gcnew System::Windows::Forms::TabPage());
			this->basicMesh_button = (gcnew System::Windows::Forms::Button());
			this->Refresh_button = (gcnew System::Windows::Forms::Button());
			this->ItemOpenbutton = (gcnew System::Windows::Forms::Button());
			this->ItemPreviewtreeView = (gcnew System::Windows::Forms::TreeView());
			this->Effecttab = (gcnew System::Windows::Forms::TabPage());
			this->splitContainer6 = (gcnew System::Windows::Forms::SplitContainer());
			this->groupBox13 = (gcnew System::Windows::Forms::GroupBox());
			this->Deref_button = (gcnew System::Windows::Forms::Button());
			this->CurrentID = (gcnew System::Windows::Forms::TextBox());
			this->TalentEffectList_tabControl = (gcnew System::Windows::Forms::TabControl());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->TalentEffectTreeView = (gcnew ::TreeViewMS::TreeViewMS());
			this->btnPasteEffectInfo = (gcnew System::Windows::Forms::Button());
			this->btnCopyEffectInfo = (gcnew System::Windows::Forms::Button());
			this->Delattbutton = (gcnew System::Windows::Forms::Button());
			this->AddEffectInfoBtn = (gcnew System::Windows::Forms::Button());
			this->tabPage6 = (gcnew System::Windows::Forms::TabPage());
			this->Disable_button = (gcnew System::Windows::Forms::Button());
			this->Enable_button = (gcnew System::Windows::Forms::Button());
			this->HitEffectPosDirtreeView = (gcnew System::Windows::Forms::TreeView());
			this->label24 = (gcnew System::Windows::Forms::Label());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->RefSelectBtn = (gcnew System::Windows::Forms::Button());
			this->RefTextBox = (gcnew System::Windows::Forms::TextBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->groupBox10 = (gcnew System::Windows::Forms::GroupBox());
			this->TalentEffectAttGrid = (gcnew System::Windows::Forms::PropertyGrid());
			this->TalentEvent = (gcnew System::Windows::Forms::TabPage());
			this->PartsJudgment = (gcnew System::Windows::Forms::TabPage());
			this->HitTabControl = (gcnew System::Windows::Forms::TabControl());
			this->CollisionTabPage = (gcnew System::Windows::Forms::TabPage());
			this->CollisionModelName = (gcnew System::Windows::Forms::TextBox());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->splitContainer3 = (gcnew System::Windows::Forms::SplitContainer());
			this->Add_CollisionBox = (gcnew System::Windows::Forms::Button());
			this->CollisionCapsulesView = (gcnew ::TreeViewMS::TreeViewMS());
			this->Del_Collision = (gcnew System::Windows::Forms::Button());
			this->Add_CollisionCapsule = (gcnew System::Windows::Forms::Button());
			this->btnMeshColPasteCapsule = (gcnew System::Windows::Forms::Button());
			this->btnMeshColCopyCapsule = (gcnew System::Windows::Forms::Button());
			this->CollisionCapsuleAtt = (gcnew System::Windows::Forms::PropertyGrid());
			this->tabPage5 = (gcnew System::Windows::Forms::TabPage());
			this->splitContainer4 = (gcnew System::Windows::Forms::SplitContainer());
			this->showHitEffectCapsule_checkBox = (gcnew System::Windows::Forms::CheckBox());
			this->PartsHitCapsulesView = (gcnew ::TreeViewMS::TreeViewMS());
			this->btnMeshHitCapsulePaste = (gcnew System::Windows::Forms::Button());
			this->btnMeshHitCapsuleCopy = (gcnew System::Windows::Forms::Button());
			this->Add_PartsCapsule = (gcnew System::Windows::Forms::Button());
			this->Add_PartsGroup = (gcnew System::Windows::Forms::Button());
			this->Del_PartsCapsule = (gcnew System::Windows::Forms::Button());
			this->PartsHitCapsuleAtt = (gcnew System::Windows::Forms::PropertyGrid());
			this->PartsHitModelName = (gcnew System::Windows::Forms::TextBox());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->HitEffect_tabPage = (gcnew System::Windows::Forms::TabPage());
			this->splitContainer5 = (gcnew System::Windows::Forms::SplitContainer());
			this->HitEffect_treeView = (gcnew ::TreeViewMS::TreeViewMS());
			this->capsule_Paste_button = (gcnew System::Windows::Forms::Button());
			this->capsule_Copy_button = (gcnew System::Windows::Forms::Button());
			this->capsule_Add_button = (gcnew System::Windows::Forms::Button());
			this->capsule_Del_button = (gcnew System::Windows::Forms::Button());
			this->HitEffect_propertyGrid = (gcnew System::Windows::Forms::PropertyGrid());
			this->HitEffectModelName = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SubMtrltab = (gcnew System::Windows::Forms::TabPage());
			this->buttonDel = (gcnew System::Windows::Forms::Button());
			this->buttonApply = (gcnew System::Windows::Forms::Button());
			this->listBoxMtrlList = (gcnew System::Windows::Forms::ListBox());
			this->groupBox17 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBoxMtrlList = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxCurrentMtrl = (gcnew System::Windows::Forms::TextBox());
			this->treeViewMeshList = (gcnew System::Windows::Forms::TreeView());
			this->groupBoxExport = (gcnew System::Windows::Forms::GroupBox());
			this->buttonExport = (gcnew System::Windows::Forms::Button());
			this->MeshInfotab = (gcnew System::Windows::Forms::TabPage());
			this->NPCcontextMenuStrip = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->COLTEDitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ScriptEditToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->contextMenuStrip2 = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->ItemSlot_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotHead_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotFace_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotHands_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotFeet_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotBody_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotLeg_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotLfinger_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotRfinger_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotNeck_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotCharm_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotLweapon_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_loc_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_1hs_l_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_1hs_r_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_1hb_l_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_1hb_r_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_1hp_l_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_1hp_r_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_2hd_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_pol_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_2hs_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_2hp_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_arc_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_gpd_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_shield_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_lh_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LW_arrow_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotRweapon_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->RW_loc_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem21 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem22 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem23 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem24 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem25 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem26 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem27 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem28 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem29 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem30 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem31 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem32 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem33 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem34 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem35 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItem36 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotReserved1_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotReserved2_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotReserved3_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SlotReserved4_ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->UnEquipToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->LogtextBox = (gcnew System::Windows::Forms::TextBox());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->button6 = (gcnew System::Windows::Forms::Button());
			this->button7 = (gcnew System::Windows::Forms::Button());
			this->button8 = (gcnew System::Windows::Forms::Button());
			this->button9 = (gcnew System::Windows::Forms::Button());
			this->button10 = (gcnew System::Windows::Forms::Button());
			this->button11 = (gcnew System::Windows::Forms::Button());
			this->propertyGrid1 = (gcnew System::Windows::Forms::PropertyGrid());
			this->button12 = (gcnew System::Windows::Forms::Button());
			this->button13 = (gcnew System::Windows::Forms::Button());
			this->treeView1 = (gcnew System::Windows::Forms::TreeView());
			this->fontDialog1 = (gcnew System::Windows::Forms::FontDialog());
			SpeedOkbutton = (gcnew System::Windows::Forms::Button());
			LW_quiver_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->MainmenuStrip->SuspendLayout();
			this->statusStrip->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AnitrackBar))->BeginInit();
			this->groupBox3->SuspendLayout();
			this->toolStrip1->SuspendLayout();
			this->groupBox4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AniSpeednumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AniSpeedtrackBar))->BeginInit();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->splitContainer2->Panel1->SuspendLayout();
			this->splitContainer2->Panel2->SuspendLayout();
			this->splitContainer2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MainView))->BeginInit();
			this->tabControl2->SuspendLayout();
			this->ItemPreviewtabPage->SuspendLayout();
			this->Effecttab->SuspendLayout();
			this->splitContainer6->Panel1->SuspendLayout();
			this->splitContainer6->Panel2->SuspendLayout();
			this->splitContainer6->SuspendLayout();
			this->groupBox13->SuspendLayout();
			this->TalentEffectList_tabControl->SuspendLayout();
			this->tabPage3->SuspendLayout();
			this->tabPage6->SuspendLayout();
			this->groupBox10->SuspendLayout();
			this->PartsJudgment->SuspendLayout();
			this->HitTabControl->SuspendLayout();
			this->CollisionTabPage->SuspendLayout();
			this->splitContainer3->Panel1->SuspendLayout();
			this->splitContainer3->Panel2->SuspendLayout();
			this->splitContainer3->SuspendLayout();
			this->tabPage5->SuspendLayout();
			this->splitContainer4->Panel1->SuspendLayout();
			this->splitContainer4->Panel2->SuspendLayout();
			this->splitContainer4->SuspendLayout();
			this->HitEffect_tabPage->SuspendLayout();
			this->splitContainer5->Panel1->SuspendLayout();
			this->splitContainer5->Panel2->SuspendLayout();
			this->splitContainer5->SuspendLayout();
			this->SubMtrltab->SuspendLayout();
			this->groupBox17->SuspendLayout();
			this->groupBoxExport->SuspendLayout();
			this->NPCcontextMenuStrip->SuspendLayout();
			this->contextMenuStrip2->SuspendLayout();
			this->SuspendLayout();
			// 
			// SpeedOkbutton
			// 
			SpeedOkbutton->Location = System::Drawing::Point(132, 61);
			SpeedOkbutton->Name = L"SpeedOkbutton";
			SpeedOkbutton->Size = System::Drawing::Size(48, 23);
			SpeedOkbutton->TabIndex = 2;
			SpeedOkbutton->Text = L"적용";
			SpeedOkbutton->UseVisualStyleBackColor = true;
			SpeedOkbutton->Click += gcnew System::EventHandler(this, &Form1::SpeedOkbutton_Click);
			// 
			// LW_quiver_toolStripMenuItem
			// 
			LW_quiver_toolStripMenuItem->Name = L"LW_quiver_toolStripMenuItem";
			LW_quiver_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			LW_quiver_toolStripMenuItem->Text = L"Dummy_quiver";
			LW_quiver_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// MainmenuStrip
			// 
			this->MainmenuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(8) {this->FileToolStripMenuItem, 
				this->EditEToolStripMenuItem, this->EdirToolStripMenuItem, this->LighttToolStripMenuItem, this->AnimToolStripMenuItem, this->ToolToolStripMenuItem, 
				this->Talent_toolStripMenuItem, this->HelpToolStripMenuItem});
			this->MainmenuStrip->Location = System::Drawing::Point(0, 0);
			this->MainmenuStrip->Name = L"MainmenuStrip";
			this->MainmenuStrip->Size = System::Drawing::Size(1080, 24);
			this->MainmenuStrip->TabIndex = 3;
			this->MainmenuStrip->Text = L"MainmenuStrip";
			// 
			// FileToolStripMenuItem
			// 
			this->FileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(14) {this->OpenToolStripMenuItem, 
				this->SaveAsToolStripMenuItem, this->toolStripSeparator2, this->SaveAlltoolStripMenuItem, this->toolStripSeparator3, this->SaveToolStripMenuItem, 
				this->TalentInfoSave_Click, this->SaveNPCHitInfo, this->BuffEffectSave, this->toolStripSeparator12, this->miTalentExport, this->AllTalentTime, 
				this->toolStripSeparator4, this->eXitToolStripMenuItem});
			this->FileToolStripMenuItem->Name = L"FileToolStripMenuItem";
			this->FileToolStripMenuItem->Size = System::Drawing::Size(57, 20);
			this->FileToolStripMenuItem->Text = L"파일(&F)";
			this->FileToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::FileToolStripMenuItem_Click);
			// 
			// OpenToolStripMenuItem
			// 
			this->OpenToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"OpenToolStripMenuItem.Image")));
			this->OpenToolStripMenuItem->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->OpenToolStripMenuItem->Name = L"OpenToolStripMenuItem";
			this->OpenToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::O));
			this->OpenToolStripMenuItem->Size = System::Drawing::Size(313, 22);
			this->OpenToolStripMenuItem->Text = L"모델 열기(&O)";
			this->OpenToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::OpenMenuItem_Click);
			// 
			// SaveAsToolStripMenuItem
			// 
			this->SaveAsToolStripMenuItem->Name = L"SaveAsToolStripMenuItem";
			this->SaveAsToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::A));
			this->SaveAsToolStripMenuItem->Size = System::Drawing::Size(313, 22);
			this->SaveAsToolStripMenuItem->Text = L"다른 이름으로 저장(&A)";
			this->SaveAsToolStripMenuItem->Visible = false;
			this->SaveAsToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SaveAsToolStripMenuItem_Click);
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(310, 6);
			// 
			// SaveAlltoolStripMenuItem
			// 
			this->SaveAlltoolStripMenuItem->Name = L"SaveAlltoolStripMenuItem";
			this->SaveAlltoolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::S));
			this->SaveAlltoolStripMenuItem->Size = System::Drawing::Size(313, 22);
			this->SaveAlltoolStripMenuItem->Text = L"전체저장";
			this->SaveAlltoolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SaveAlltoolStripMenuItem_Click);
			// 
			// toolStripSeparator3
			// 
			this->toolStripSeparator3->Name = L"toolStripSeparator3";
			this->toolStripSeparator3->Size = System::Drawing::Size(310, 6);
			// 
			// SaveToolStripMenuItem
			// 
			this->SaveToolStripMenuItem->AutoToolTip = true;
			this->SaveToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"SaveToolStripMenuItem.Image")));
			this->SaveToolStripMenuItem->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->SaveToolStripMenuItem->Name = L"SaveToolStripMenuItem";
			this->SaveToolStripMenuItem->Size = System::Drawing::Size(313, 22);
			this->SaveToolStripMenuItem->Text = L"모델 저장(*.elu.animation.xml 등)";
			this->SaveToolStripMenuItem->ToolTipText = L"*.elu.animation.xml, talent_ext.xml, talent_effect_info.xml, talent_hit_info.xml," 
				L" Mesh_info.xml";
			this->SaveToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SaveMenuItem_Click);
			// 
			// TalentInfoSave_Click
			// 
			this->TalentInfoSave_Click->AutoToolTip = true;
			this->TalentInfoSave_Click->Name = L"TalentInfoSave_Click";
			this->TalentInfoSave_Click->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::T));
			this->TalentInfoSave_Click->Size = System::Drawing::Size(313, 22);
			this->TalentInfoSave_Click->Text = L"탤런트 저장(talent_ext.xml 등)";
			this->TalentInfoSave_Click->ToolTipText = L"talent_ext.xml, talent_effect_info.xml, talent_hit_info.xml";
			this->TalentInfoSave_Click->Click += gcnew System::EventHandler(this, &Form1::TalentInfoSave);
			// 
			// SaveNPCHitInfo
			// 
			this->SaveNPCHitInfo->Name = L"SaveNPCHitInfo";
			this->SaveNPCHitInfo->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::H));
			this->SaveNPCHitInfo->Size = System::Drawing::Size(313, 22);
			this->SaveNPCHitInfo->Text = L"NPC 충돌,판정 저장(mesh_info.xml)";
			this->SaveNPCHitInfo->ToolTipText = L"mesh_hit_info.xml 저장";
			this->SaveNPCHitInfo->Click += gcnew System::EventHandler(this, &Form1::SaveNPCHitInfo_Click);
			// 
			// BuffEffectSave
			// 
			this->BuffEffectSave->Name = L"BuffEffectSave";
			this->BuffEffectSave->Size = System::Drawing::Size(313, 22);
			// 
			// toolStripSeparator12
			// 
			this->toolStripSeparator12->Name = L"toolStripSeparator12";
			this->toolStripSeparator12->Size = System::Drawing::Size(310, 6);
			// 
			// miTalentExport
			// 
			this->miTalentExport->Name = L"miTalentExport";
			this->miTalentExport->Size = System::Drawing::Size(313, 22);
			this->miTalentExport->Text = L"탤런트 이동값 전체 저장(talent_pos_info.xml)";
			this->miTalentExport->Click += gcnew System::EventHandler(this, &Form1::miTalentExport_Click);
			// 
			// AllTalentTime
			// 
			this->AllTalentTime->Name = L"AllTalentTime";
			this->AllTalentTime->Size = System::Drawing::Size(313, 22);
			this->AllTalentTime->Text = L"탤런트 시간 전체 저장(talent_ext.xml)";
			this->AllTalentTime->Click += gcnew System::EventHandler(this, &Form1::AllTalentTime_Click);
			// 
			// toolStripSeparator4
			// 
			this->toolStripSeparator4->Name = L"toolStripSeparator4";
			this->toolStripSeparator4->Size = System::Drawing::Size(310, 6);
			// 
			// eXitToolStripMenuItem
			// 
			this->eXitToolStripMenuItem->Name = L"eXitToolStripMenuItem";
			this->eXitToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::X));
			this->eXitToolStripMenuItem->Size = System::Drawing::Size(313, 22);
			this->eXitToolStripMenuItem->Text = L"끝내기(&X)";
			this->eXitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::eXitToolStripMenuItem_Click);
			// 
			// EditEToolStripMenuItem
			// 
			this->EditEToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->UndoToolStripMenuItem, 
				this->RedoToolStripMenuItem, this->toolStripSeparator10});
			this->EditEToolStripMenuItem->Name = L"EditEToolStripMenuItem";
			this->EditEToolStripMenuItem->Size = System::Drawing::Size(57, 20);
			this->EditEToolStripMenuItem->Text = L"편집(&E)";
			// 
			// UndoToolStripMenuItem
			// 
			this->UndoToolStripMenuItem->Name = L"UndoToolStripMenuItem";
			this->UndoToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Z));
			this->UndoToolStripMenuItem->Size = System::Drawing::Size(182, 22);
			this->UndoToolStripMenuItem->Text = L"실행 취소(&U)";
			this->UndoToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::UndoToolStripMenuItem_Click);
			// 
			// RedoToolStripMenuItem
			// 
			this->RedoToolStripMenuItem->Name = L"RedoToolStripMenuItem";
			this->RedoToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Y));
			this->RedoToolStripMenuItem->Size = System::Drawing::Size(182, 22);
			this->RedoToolStripMenuItem->Text = L"다시 실행(&R)";
			this->RedoToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::RedoToolStripMenuItem_Click);
			// 
			// toolStripSeparator10
			// 
			this->toolStripSeparator10->Name = L"toolStripSeparator10";
			this->toolStripSeparator10->Size = System::Drawing::Size(179, 6);
			// 
			// EdirToolStripMenuItem
			// 
			this->EdirToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(30) {this->showBoneToolStripMenuItem, 
				this->ShowNormalToolStripMenuItem, this->showWireToolStripMenuItem, this->drawGridToolStripMenuItem, this->partsHitCapsuleToolStripMenuItem, 
				this->toolStripMenuItem2, this->toolStripMenuItem4, this->showPlayerModelToolStripMenuItem, this->showTestHitEffectDirToolStripMenuItem, 
				this->showCenterAxisToolStripMenuItem, this->showWeaponEffectToolStripMenuItem, this->showNPCHitCapsuleGroupMenuItem, this->ShowWireShade, 
				this->DisableSpeculartoolStripMenuItem, this->toolStripSeparator5, this->cameraResetSpaceBarToolStripMenuItem, this->collisionObjectToolStripMenuItem, 
				this->toolStripSeparator1, this->ReloadToolStripMenuItem, this->selectModelTabToolStripMenuItem, this->selectNPCListTablToolStripMenuItem, 
				this->selectPCTalentTabltoolStripMenuItem, this->drawAABBToolStripMenuItem, this->DrawInteractionAABBToolStripMenuItem, this->FullScreenToolStripMenuItem, 
				this->showLogWindowToolStripMenuItem, this->toolStripSeparator9, this->toolStripMenuItem1, this->hDRToolStripMenuItem, this->partsColorTestToolStripMenuItem});
			this->EdirToolStripMenuItem->Name = L"EdirToolStripMenuItem";
			this->EdirToolStripMenuItem->Size = System::Drawing::Size(58, 20);
			this->EdirToolStripMenuItem->Text = L"보기(&V)";
			// 
			// showBoneToolStripMenuItem
			// 
			this->showBoneToolStripMenuItem->Name = L"showBoneToolStripMenuItem";
			this->showBoneToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::B));
			this->showBoneToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->showBoneToolStripMenuItem->Text = L"ShowBone";
			this->showBoneToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::showBoneToolStripMenuItem_Click);
			// 
			// ShowNormalToolStripMenuItem
			// 
			this->ShowNormalToolStripMenuItem->CheckOnClick = true;
			this->ShowNormalToolStripMenuItem->Name = L"ShowNormalToolStripMenuItem";
			this->ShowNormalToolStripMenuItem->ShortcutKeyDisplayString = L"";
			this->ShowNormalToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::N));
			this->ShowNormalToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->ShowNormalToolStripMenuItem->Text = L"ShowNormal";
			this->ShowNormalToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::ShowNormalToolStripMenuItem_Click);
			// 
			// showWireToolStripMenuItem
			// 
			this->showWireToolStripMenuItem->CheckOnClick = true;
			this->showWireToolStripMenuItem->Name = L"showWireToolStripMenuItem";
			this->showWireToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::W));
			this->showWireToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->showWireToolStripMenuItem->Text = L"ShowWire";
			this->showWireToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::showWireToolStripMenuItem_Click);
			// 
			// drawGridToolStripMenuItem
			// 
			this->drawGridToolStripMenuItem->Checked = true;
			this->drawGridToolStripMenuItem->CheckOnClick = true;
			this->drawGridToolStripMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->drawGridToolStripMenuItem->Name = L"drawGridToolStripMenuItem";
			this->drawGridToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::G));
			this->drawGridToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->drawGridToolStripMenuItem->Text = L"ShowGrid";
			this->drawGridToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::drawGridToolStripMenuItem_Click);
			// 
			// partsHitCapsuleToolStripMenuItem
			// 
			this->partsHitCapsuleToolStripMenuItem->CheckOnClick = true;
			this->partsHitCapsuleToolStripMenuItem->Name = L"partsHitCapsuleToolStripMenuItem";
			this->partsHitCapsuleToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->partsHitCapsuleToolStripMenuItem->Text = L"히트 캡슐 보여주기";
			this->partsHitCapsuleToolStripMenuItem->CheckedChanged += gcnew System::EventHandler(this, &Form1::PartsHitCapsuleStripMenuItem_CheckedChanged);
			this->partsHitCapsuleToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::partsHitCapsuleToolStripMenuItem_Click);
			// 
			// toolStripMenuItem2
			// 
			this->toolStripMenuItem2->CheckOnClick = true;
			this->toolStripMenuItem2->Name = L"toolStripMenuItem2";
			this->toolStripMenuItem2->Size = System::Drawing::Size(223, 22);
			this->toolStripMenuItem2->Text = L"충돌 캡슐 보여주기";
			this->toolStripMenuItem2->CheckedChanged += gcnew System::EventHandler(this, &Form1::MeshColCapsuleStripMenuItem_CheckedChanged);
			// 
			// toolStripMenuItem4
			// 
			this->toolStripMenuItem4->CheckOnClick = true;
			this->toolStripMenuItem4->Name = L"toolStripMenuItem4";
			this->toolStripMenuItem4->Size = System::Drawing::Size(223, 22);
			this->toolStripMenuItem4->Text = L"히트 이펙트 캡슐 보여주기";
			this->toolStripMenuItem4->CheckedChanged += gcnew System::EventHandler(this, &Form1::toolStripMenuItem4_CheckedChanged);
			// 
			// showPlayerModelToolStripMenuItem
			// 
			this->showPlayerModelToolStripMenuItem->CheckOnClick = true;
			this->showPlayerModelToolStripMenuItem->Name = L"showPlayerModelToolStripMenuItem";
			this->showPlayerModelToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->showPlayerModelToolStripMenuItem->Text = L"ShowPlayerModel";
			this->showPlayerModelToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::showPlayerModelToolStripMenuItem_Click);
			// 
			// showTestHitEffectDirToolStripMenuItem
			// 
			this->showTestHitEffectDirToolStripMenuItem->CheckOnClick = true;
			this->showTestHitEffectDirToolStripMenuItem->Name = L"showTestHitEffectDirToolStripMenuItem";
			this->showTestHitEffectDirToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->showTestHitEffectDirToolStripMenuItem->Text = L"ShowTestHitEffect";
			this->showTestHitEffectDirToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::showTestHitEffectDirToolStripMenuItem_Click);
			// 
			// showCenterAxisToolStripMenuItem
			// 
			this->showCenterAxisToolStripMenuItem->CheckOnClick = true;
			this->showCenterAxisToolStripMenuItem->Name = L"showCenterAxisToolStripMenuItem";
			this->showCenterAxisToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->showCenterAxisToolStripMenuItem->Text = L"ShowCenterAxis";
			this->showCenterAxisToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::showCenterAxisToolStripMenuItem_Click);
			// 
			// showWeaponEffectToolStripMenuItem
			// 
			this->showWeaponEffectToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->effectSelectToolStripMenuItem, 
				this->effectSelectDeleteToolStripMenuItem});
			this->showWeaponEffectToolStripMenuItem->Name = L"showWeaponEffectToolStripMenuItem";
			this->showWeaponEffectToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->showWeaponEffectToolStripMenuItem->Text = L"ShowWeaponEffect";
			// 
			// effectSelectToolStripMenuItem
			// 
			this->effectSelectToolStripMenuItem->Name = L"effectSelectToolStripMenuItem";
			this->effectSelectToolStripMenuItem->Size = System::Drawing::Size(168, 22);
			this->effectSelectToolStripMenuItem->Text = L"EffectSelect";
			this->effectSelectToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::effectSelectToolStripMenuItem_Click);
			// 
			// effectSelectDeleteToolStripMenuItem
			// 
			this->effectSelectDeleteToolStripMenuItem->Name = L"effectSelectDeleteToolStripMenuItem";
			this->effectSelectDeleteToolStripMenuItem->Size = System::Drawing::Size(168, 22);
			this->effectSelectDeleteToolStripMenuItem->Text = L"EffectSelectDelete";
			this->effectSelectDeleteToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::effectSelectDeleteToolStripMenuItem_Click);
			// 
			// showNPCHitCapsuleGroupMenuItem
			// 
			this->showNPCHitCapsuleGroupMenuItem->CheckOnClick = true;
			this->showNPCHitCapsuleGroupMenuItem->Name = L"showNPCHitCapsuleGroupMenuItem";
			this->showNPCHitCapsuleGroupMenuItem->Size = System::Drawing::Size(223, 22);
			this->showNPCHitCapsuleGroupMenuItem->Text = L"ShowNPC_HitCapsuleGroup";
			this->showNPCHitCapsuleGroupMenuItem->Click += gcnew System::EventHandler(this, &Form1::showNPCHitCapsuleGroupMenuItem_Click);
			// 
			// ShowWireShade
			// 
			this->ShowWireShade->CheckOnClick = true;
			this->ShowWireShade->Name = L"ShowWireShade";
			this->ShowWireShade->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Q));
			this->ShowWireShade->Size = System::Drawing::Size(223, 22);
			this->ShowWireShade->Text = L"Wire + Shade";
			this->ShowWireShade->Click += gcnew System::EventHandler(this, &Form1::ShowWireShade_Click);
			// 
			// DisableSpeculartoolStripMenuItem
			// 
			this->DisableSpeculartoolStripMenuItem->CheckOnClick = true;
			this->DisableSpeculartoolStripMenuItem->Name = L"DisableSpeculartoolStripMenuItem";
			this->DisableSpeculartoolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->DisableSpeculartoolStripMenuItem->Text = L"DisableSpecular";
			this->DisableSpeculartoolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::DisableSpeculartoolStripMenuItem_Click);
			// 
			// toolStripSeparator5
			// 
			this->toolStripSeparator5->Name = L"toolStripSeparator5";
			this->toolStripSeparator5->Size = System::Drawing::Size(220, 6);
			// 
			// cameraResetSpaceBarToolStripMenuItem
			// 
			this->cameraResetSpaceBarToolStripMenuItem->Name = L"cameraResetSpaceBarToolStripMenuItem";
			this->cameraResetSpaceBarToolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::F4;
			this->cameraResetSpaceBarToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->cameraResetSpaceBarToolStripMenuItem->Text = L"CameraReset";
			this->cameraResetSpaceBarToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cameraResetSpaceBarToolStripMenuItem_Click);
			// 
			// collisionObjectToolStripMenuItem
			// 
			this->collisionObjectToolStripMenuItem->CheckOnClick = true;
			this->collisionObjectToolStripMenuItem->Name = L"collisionObjectToolStripMenuItem";
			this->collisionObjectToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D));
			this->collisionObjectToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->collisionObjectToolStripMenuItem->Text = L"Collision Object";
			this->collisionObjectToolStripMenuItem->CheckedChanged += gcnew System::EventHandler(this, &Form1::collisionObjectToolStripMenuItem_CheckedChanged);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(220, 6);
			// 
			// ReloadToolStripMenuItem
			// 
			this->ReloadToolStripMenuItem->Name = L"ReloadToolStripMenuItem";
			this->ReloadToolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::F5;
			this->ReloadToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->ReloadToolStripMenuItem->Text = L"Reload";
			this->ReloadToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::ReloadToolStripMenuItem_Click);
			// 
			// selectModelTabToolStripMenuItem
			// 
			this->selectModelTabToolStripMenuItem->Name = L"selectModelTabToolStripMenuItem";
			this->selectModelTabToolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::F1;
			this->selectModelTabToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->selectModelTabToolStripMenuItem->Text = L"Select Model Tab";
			this->selectModelTabToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::selectModelTabToolStripMenuItem_Click);
			// 
			// selectNPCListTablToolStripMenuItem
			// 
			this->selectNPCListTablToolStripMenuItem->Name = L"selectNPCListTablToolStripMenuItem";
			this->selectNPCListTablToolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::F2;
			this->selectNPCListTablToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->selectNPCListTablToolStripMenuItem->Text = L"Select NPCListTabl";
			this->selectNPCListTablToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::selectNPCListTablToolStripMenuItem_Click);
			// 
			// selectPCTalentTabltoolStripMenuItem
			// 
			this->selectPCTalentTabltoolStripMenuItem->Name = L"selectPCTalentTabltoolStripMenuItem";
			this->selectPCTalentTabltoolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::F3;
			this->selectPCTalentTabltoolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->selectPCTalentTabltoolStripMenuItem->Text = L"Select PC탈렌트탭";
			this->selectPCTalentTabltoolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::selectPCTalentTabltoolStripMenuItem_Click);
			// 
			// drawAABBToolStripMenuItem
			// 
			this->drawAABBToolStripMenuItem->CheckOnClick = true;
			this->drawAABBToolStripMenuItem->Name = L"drawAABBToolStripMenuItem";
			this->drawAABBToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::E));
			this->drawAABBToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->drawAABBToolStripMenuItem->Text = L"DrawAABB";
			this->drawAABBToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::DrawBoxtoolStripButton_Click);
			// 
			// DrawInteractionAABBToolStripMenuItem
			// 
			this->DrawInteractionAABBToolStripMenuItem->Name = L"DrawInteractionAABBToolStripMenuItem";
			this->DrawInteractionAABBToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->DrawInteractionAABBToolStripMenuItem->Text = L"DrawInteractionAABB";
			this->DrawInteractionAABBToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::DrawInteractionAABBToolStripMenuItem_Click);
			// 
			// FullScreenToolStripMenuItem
			// 
			this->FullScreenToolStripMenuItem->Name = L"FullScreenToolStripMenuItem";
			this->FullScreenToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::E));
			this->FullScreenToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->FullScreenToolStripMenuItem->Text = L"전체화면";
			this->FullScreenToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::FullscreenToolStripMenuItem_Click);
			// 
			// showLogWindowToolStripMenuItem
			// 
			this->showLogWindowToolStripMenuItem->Checked = true;
			this->showLogWindowToolStripMenuItem->CheckOnClick = true;
			this->showLogWindowToolStripMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->showLogWindowToolStripMenuItem->Name = L"showLogWindowToolStripMenuItem";
			this->showLogWindowToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->showLogWindowToolStripMenuItem->Text = L"로그창 보이기";
			this->showLogWindowToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::showLogWindowToolStripMenuItem_Click);
			// 
			// toolStripSeparator9
			// 
			this->toolStripSeparator9->Name = L"toolStripSeparator9";
			this->toolStripSeparator9->Size = System::Drawing::Size(220, 6);
			// 
			// toolStripMenuItem1
			// 
			this->toolStripMenuItem1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->ViewreferenceMeshToolStripMenuItem, 
				this->SetReferenceMeshToolStripMenuItem});
			this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
			this->toolStripMenuItem1->Size = System::Drawing::Size(223, 22);
			this->toolStripMenuItem1->Text = L"Reference Mesh";
			// 
			// ViewreferenceMeshToolStripMenuItem
			// 
			this->ViewreferenceMeshToolStripMenuItem->Name = L"ViewreferenceMeshToolStripMenuItem";
			this->ViewreferenceMeshToolStripMenuItem->Size = System::Drawing::Size(185, 22);
			this->ViewreferenceMeshToolStripMenuItem->Text = L"Reference Mesh 보기";
			this->ViewreferenceMeshToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::ViewReferenceMeshToolStripMenuItem_Click);
			// 
			// SetReferenceMeshToolStripMenuItem
			// 
			this->SetReferenceMeshToolStripMenuItem->Name = L"SetReferenceMeshToolStripMenuItem";
			this->SetReferenceMeshToolStripMenuItem->Size = System::Drawing::Size(185, 22);
			this->SetReferenceMeshToolStripMenuItem->Text = L"Reference Mesh 설정";
			this->SetReferenceMeshToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SetReferenceMeshToolStripMenuItem_Click);
			// 
			// hDRToolStripMenuItem
			// 
			this->hDRToolStripMenuItem->Name = L"hDRToolStripMenuItem";
			this->hDRToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::H));
			this->hDRToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->hDRToolStripMenuItem->Text = L"HDR";
			this->hDRToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::hDRToolStripMenuItem_Click);
			// 
			// partsColorTestToolStripMenuItem
			// 
			this->partsColorTestToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->SkinColorTestToolStripMenuItem, 
				this->HairColorTestToolStripMenuItem, this->ItemColorTestToolStripMenuItem});
			this->partsColorTestToolStripMenuItem->Name = L"partsColorTestToolStripMenuItem";
			this->partsColorTestToolStripMenuItem->Size = System::Drawing::Size(223, 22);
			this->partsColorTestToolStripMenuItem->Text = L"파츠 컬러 테스트";
			// 
			// SkinColorTestToolStripMenuItem
			// 
			this->SkinColorTestToolStripMenuItem->Name = L"SkinColorTestToolStripMenuItem";
			this->SkinColorTestToolStripMenuItem->Size = System::Drawing::Size(176, 22);
			this->SkinColorTestToolStripMenuItem->Text = L"피부색 테스트";
			this->SkinColorTestToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SkinColorTestToolStripMenuItem_Click);
			// 
			// HairColorTestToolStripMenuItem
			// 
			this->HairColorTestToolStripMenuItem->Name = L"HairColorTestToolStripMenuItem";
			this->HairColorTestToolStripMenuItem->Size = System::Drawing::Size(176, 22);
			this->HairColorTestToolStripMenuItem->Text = L"머리색 테스트";
			this->HairColorTestToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::HairColorTestToolStripMenuItem_Click);
			// 
			// ItemColorTestToolStripMenuItem
			// 
			this->ItemColorTestToolStripMenuItem->Name = L"ItemColorTestToolStripMenuItem";
			this->ItemColorTestToolStripMenuItem->Size = System::Drawing::Size(176, 22);
			this->ItemColorTestToolStripMenuItem->Text = L"아이템 컬러 테스트";
			this->ItemColorTestToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::ItemColorTestToolStripMenuItem_Click);
			// 
			// LighttToolStripMenuItem
			// 
			this->LighttToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->lightToolStripMenuItem, 
				this->lightToCameraposToolStripMenuItem, this->ambientToolStripMenuItem});
			this->LighttToolStripMenuItem->Name = L"LighttToolStripMenuItem";
			this->LighttToolStripMenuItem->Size = System::Drawing::Size(55, 20);
			this->LighttToolStripMenuItem->Text = L"라이트";
			// 
			// lightToolStripMenuItem
			// 
			this->lightToolStripMenuItem->CheckOnClick = true;
			this->lightToolStripMenuItem->Name = L"lightToolStripMenuItem";
			this->lightToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::L));
			this->lightToolStripMenuItem->Size = System::Drawing::Size(216, 22);
			this->lightToolStripMenuItem->Text = L"LightOn/Off";
			this->lightToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::lightToolStripMenuItem_Click);
			// 
			// lightToCameraposToolStripMenuItem
			// 
			this->lightToCameraposToolStripMenuItem->CheckOnClick = true;
			this->lightToCameraposToolStripMenuItem->Name = L"lightToCameraposToolStripMenuItem";
			this->lightToCameraposToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::P));
			this->lightToCameraposToolStripMenuItem->Size = System::Drawing::Size(216, 22);
			this->lightToCameraposToolStripMenuItem->Text = L"LightToCamerapos";
			this->lightToCameraposToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::lightToCameraposToolStripMenuItem_Click);
			// 
			// ambientToolStripMenuItem
			// 
			this->ambientToolStripMenuItem->Name = L"ambientToolStripMenuItem";
			this->ambientToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::M));
			this->ambientToolStripMenuItem->Size = System::Drawing::Size(216, 22);
			this->ambientToolStripMenuItem->Text = L"Ambient 설정";
			this->ambientToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::AmbienttoolStripButton1_Click);
			// 
			// AnimToolStripMenuItem
			// 
			this->AnimToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(8) {this->animationPlayToolStripMenuItem, 
				this->animationStopToolStripMenuItem, this->animationLoopPlayToolStripMenuItem, this->transitionBlendingToolStripMenuItem, this->toolStripSeparator13, 
				this->AnimationBlend_ToolStripMenuItem, this->OvelayAnimation_ToolStripMenuItem, this->ReActionAnimation_ToolStripMenuItem});
			this->AnimToolStripMenuItem->Name = L"AnimToolStripMenuItem";
			this->AnimToolStripMenuItem->Size = System::Drawing::Size(79, 20);
			this->AnimToolStripMenuItem->Text = L"애니메이션";
			// 
			// animationPlayToolStripMenuItem
			// 
			this->animationPlayToolStripMenuItem->Name = L"animationPlayToolStripMenuItem";
			this->animationPlayToolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::F6;
			this->animationPlayToolStripMenuItem->Size = System::Drawing::Size(212, 22);
			this->animationPlayToolStripMenuItem->Text = L"AnimationPlay";
			this->animationPlayToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::animationPlayToolStripMenuItem_Click);
			// 
			// animationStopToolStripMenuItem
			// 
			this->animationStopToolStripMenuItem->Name = L"animationStopToolStripMenuItem";
			this->animationStopToolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::F7;
			this->animationStopToolStripMenuItem->Size = System::Drawing::Size(212, 22);
			this->animationStopToolStripMenuItem->Text = L"AnimationStop";
			this->animationStopToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::animationStopToolStripMenuItem_Click);
			// 
			// animationLoopPlayToolStripMenuItem
			// 
			this->animationLoopPlayToolStripMenuItem->CheckOnClick = true;
			this->animationLoopPlayToolStripMenuItem->Name = L"animationLoopPlayToolStripMenuItem";
			this->animationLoopPlayToolStripMenuItem->ShortcutKeys = System::Windows::Forms::Keys::F8;
			this->animationLoopPlayToolStripMenuItem->Size = System::Drawing::Size(212, 22);
			this->animationLoopPlayToolStripMenuItem->Text = L"AnimationLoopPlay";
			this->animationLoopPlayToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::animationLoopPlayToolStripMenuItem_Click);
			// 
			// transitionBlendingToolStripMenuItem
			// 
			this->transitionBlendingToolStripMenuItem->Checked = true;
			this->transitionBlendingToolStripMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->transitionBlendingToolStripMenuItem->Name = L"transitionBlendingToolStripMenuItem";
			this->transitionBlendingToolStripMenuItem->Size = System::Drawing::Size(212, 22);
			this->transitionBlendingToolStripMenuItem->Text = L"Transition Blending";
			this->transitionBlendingToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::transitionBlendingToolStripMenuItem_Click);
			// 
			// toolStripSeparator13
			// 
			this->toolStripSeparator13->Name = L"toolStripSeparator13";
			this->toolStripSeparator13->Size = System::Drawing::Size(209, 6);
			// 
			// AnimationBlend_ToolStripMenuItem
			// 
			this->AnimationBlend_ToolStripMenuItem->Name = L"AnimationBlend_ToolStripMenuItem";
			this->AnimationBlend_ToolStripMenuItem->Size = System::Drawing::Size(212, 22);
			this->AnimationBlend_ToolStripMenuItem->Text = L"애니메이션 블렌딩값 설정";
			this->AnimationBlend_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::AnimationBlend_ToolStripMenuItem_Click);
			// 
			// OvelayAnimation_ToolStripMenuItem
			// 
			this->OvelayAnimation_ToolStripMenuItem->Name = L"OvelayAnimation_ToolStripMenuItem";
			this->OvelayAnimation_ToolStripMenuItem->Size = System::Drawing::Size(212, 22);
			this->OvelayAnimation_ToolStripMenuItem->Text = L"오버레이 애니메이션 설정";
			this->OvelayAnimation_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::OvelayAnimation_ToolStripMenuItem_Click);
			// 
			// ReActionAnimation_ToolStripMenuItem
			// 
			this->ReActionAnimation_ToolStripMenuItem->Name = L"ReActionAnimation_ToolStripMenuItem";
			this->ReActionAnimation_ToolStripMenuItem->Size = System::Drawing::Size(212, 22);
			this->ReActionAnimation_ToolStripMenuItem->Text = L"리액션 애니메이션 설정";
			this->ReActionAnimation_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::ReActionAnimation_ToolStripMenuItem_Click);
			// 
			// ToolToolStripMenuItem
			// 
			this->ToolToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {this->backgroundColorToolStripMenuItem, 
				this->feedbackToolStripMenuItem, this->FeedBack2ToolStripMenuItem, this->PartsColorPreviewMenuItem, this->ToolStripMenuItemExecuteValidator, 
				this->DataValidatorToolStripMenuItem, this->PlayerNPCAnimationInfoToolStripMenuItem});
			this->ToolToolStripMenuItem->Name = L"ToolToolStripMenuItem";
			this->ToolToolStripMenuItem->Size = System::Drawing::Size(58, 20);
			this->ToolToolStripMenuItem->Text = L"도구(&T)";
			// 
			// backgroundColorToolStripMenuItem
			// 
			this->backgroundColorToolStripMenuItem->Name = L"backgroundColorToolStripMenuItem";
			this->backgroundColorToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>(((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Alt) 
				| System::Windows::Forms::Keys::C));
			this->backgroundColorToolStripMenuItem->Size = System::Drawing::Size(275, 22);
			this->backgroundColorToolStripMenuItem->Text = L"Background Color";
			this->backgroundColorToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::backgroundColorToolStripMenuItem_Click);
			// 
			// feedbackToolStripMenuItem
			// 
			this->feedbackToolStripMenuItem->Name = L"feedbackToolStripMenuItem";
			this->feedbackToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::F));
			this->feedbackToolStripMenuItem->Size = System::Drawing::Size(275, 22);
			this->feedbackToolStripMenuItem->Text = L"피드백(티켓생성하기)";
			this->feedbackToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::feedbackToolStripMenuItem_Click);
			// 
			// FeedBack2ToolStripMenuItem
			// 
			this->FeedBack2ToolStripMenuItem->Name = L"FeedBack2ToolStripMenuItem";
			this->FeedBack2ToolStripMenuItem->Size = System::Drawing::Size(275, 22);
			this->FeedBack2ToolStripMenuItem->Text = L"MDN으로(CharacterTool 정보로 가기)";
			this->FeedBack2ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::FeedBack2ToolStripMenuItem_Click);
			// 
			// PartsColorPreviewMenuItem
			// 
			this->PartsColorPreviewMenuItem->Name = L"PartsColorPreviewMenuItem";
			this->PartsColorPreviewMenuItem->Size = System::Drawing::Size(275, 22);
			this->PartsColorPreviewMenuItem->Text = L"파츠컬러 미리보기";
			this->PartsColorPreviewMenuItem->Click += gcnew System::EventHandler(this, &Form1::PartsColorPreviewMenuItem_Click);
			// 
			// ToolStripMenuItemExecuteValidator
			// 
			this->ToolStripMenuItemExecuteValidator->Name = L"ToolStripMenuItemExecuteValidator";
			this->ToolStripMenuItemExecuteValidator->ShortcutKeys = System::Windows::Forms::Keys::F9;
			this->ToolStripMenuItemExecuteValidator->Size = System::Drawing::Size(275, 22);
			this->ToolStripMenuItemExecuteValidator->Text = L"리소스 밸리데이터 실행";
			this->ToolStripMenuItemExecuteValidator->Click += gcnew System::EventHandler(this, &Form1::ToolStripMenuItemExecuteValidator_Click);
			// 
			// DataValidatorToolStripMenuItem
			// 
			this->DataValidatorToolStripMenuItem->Name = L"DataValidatorToolStripMenuItem";
			this->DataValidatorToolStripMenuItem->Size = System::Drawing::Size(275, 22);
			this->DataValidatorToolStripMenuItem->Text = L"데이터 밸리데이터 실행";
			this->DataValidatorToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::DataValidatorToolStripMenuItem_Click);
			// 
			// PlayerNPCAnimationInfoToolStripMenuItem
			// 
			this->PlayerNPCAnimationInfoToolStripMenuItem->Name = L"PlayerNPCAnimationInfoToolStripMenuItem";
			this->PlayerNPCAnimationInfoToolStripMenuItem->Size = System::Drawing::Size(275, 22);
			this->PlayerNPCAnimationInfoToolStripMenuItem->Text = L"인간형 NPC 애니메이션 정보 설정";
			this->PlayerNPCAnimationInfoToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::PlayerNPCAnimationInfoToolStripMenuItem_Click);
			// 
			// Talent_toolStripMenuItem
			// 
			this->Talent_toolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(9) {this->Unselect_toolStripMenuItem, 
				this->Bottom_toolStripMenuItem, this->Top_toolStripMenuItem, this->TopBottom_toolStripMenuItem, this->X_toolStripMenuItem, this->Y_toolStripMenuItem, 
				this->Z_toolStripMenuItem, this->XY_toolStripMenuItem, this->Opa_toolStripMenuItem});
			this->Talent_toolStripMenuItem->Name = L"Talent_toolStripMenuItem";
			this->Talent_toolStripMenuItem->Size = System::Drawing::Size(79, 20);
			this->Talent_toolStripMenuItem->Text = L"탈렌트편집";
			// 
			// Unselect_toolStripMenuItem
			// 
			this->Unselect_toolStripMenuItem->Name = L"Unselect_toolStripMenuItem";
			this->Unselect_toolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D0));
			this->Unselect_toolStripMenuItem->Size = System::Drawing::Size(178, 22);
			this->Unselect_toolStripMenuItem->Text = L"해제";
			this->Unselect_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::Unselect_toolStripMenuItem_Click);
			// 
			// Bottom_toolStripMenuItem
			// 
			this->Bottom_toolStripMenuItem->Name = L"Bottom_toolStripMenuItem";
			this->Bottom_toolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D1));
			this->Bottom_toolStripMenuItem->Size = System::Drawing::Size(178, 22);
			this->Bottom_toolStripMenuItem->Text = L"1";
			this->Bottom_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::Bottom_toolStripMenuItem_Click);
			// 
			// Top_toolStripMenuItem
			// 
			this->Top_toolStripMenuItem->Name = L"Top_toolStripMenuItem";
			this->Top_toolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D2));
			this->Top_toolStripMenuItem->Size = System::Drawing::Size(178, 22);
			this->Top_toolStripMenuItem->Text = L"2";
			this->Top_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::Top_toolStripMenuItem_Click);
			// 
			// TopBottom_toolStripMenuItem
			// 
			this->TopBottom_toolStripMenuItem->Name = L"TopBottom_toolStripMenuItem";
			this->TopBottom_toolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D3));
			this->TopBottom_toolStripMenuItem->Size = System::Drawing::Size(178, 22);
			this->TopBottom_toolStripMenuItem->Text = L"1+2";
			this->TopBottom_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::TopBottom_toolStripMenuItem_Click);
			// 
			// X_toolStripMenuItem
			// 
			this->X_toolStripMenuItem->Name = L"X_toolStripMenuItem";
			this->X_toolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D4));
			this->X_toolStripMenuItem->Size = System::Drawing::Size(178, 22);
			this->X_toolStripMenuItem->Text = L"X";
			this->X_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::X_toolStripMenuItem_Click);
			// 
			// Y_toolStripMenuItem
			// 
			this->Y_toolStripMenuItem->Name = L"Y_toolStripMenuItem";
			this->Y_toolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D5));
			this->Y_toolStripMenuItem->Size = System::Drawing::Size(178, 22);
			this->Y_toolStripMenuItem->Text = L"Y";
			this->Y_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::Y_toolStripMenuItem_Click);
			// 
			// Z_toolStripMenuItem
			// 
			this->Z_toolStripMenuItem->Name = L"Z_toolStripMenuItem";
			this->Z_toolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D6));
			this->Z_toolStripMenuItem->Size = System::Drawing::Size(178, 22);
			this->Z_toolStripMenuItem->Text = L"Z";
			this->Z_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::Z_toolStripMenuItem_Click);
			// 
			// XY_toolStripMenuItem
			// 
			this->XY_toolStripMenuItem->Name = L"XY_toolStripMenuItem";
			this->XY_toolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D7));
			this->XY_toolStripMenuItem->Size = System::Drawing::Size(178, 22);
			this->XY_toolStripMenuItem->Text = L"XY";
			this->XY_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::XY_toolStripMenuItem_Click);
			// 
			// Opa_toolStripMenuItem
			// 
			this->Opa_toolStripMenuItem->Name = L"Opa_toolStripMenuItem";
			this->Opa_toolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D8));
			this->Opa_toolStripMenuItem->Size = System::Drawing::Size(178, 22);
			this->Opa_toolStripMenuItem->Text = L"CapsuleOpa";
			this->Opa_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::Opa_toolStripMenuItem_Click);
			// 
			// HelpToolStripMenuItem
			// 
			this->HelpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->InfomationToolStripMenuItem});
			this->HelpToolStripMenuItem->Name = L"HelpToolStripMenuItem";
			this->HelpToolStripMenuItem->Size = System::Drawing::Size(72, 20);
			this->HelpToolStripMenuItem->Text = L"도움말(&H)";
			// 
			// InfomationToolStripMenuItem
			// 
			this->InfomationToolStripMenuItem->Name = L"InfomationToolStripMenuItem";
			this->InfomationToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::A));
			this->InfomationToolStripMenuItem->Size = System::Drawing::Size(165, 22);
			this->InfomationToolStripMenuItem->Text = L"정보(&A)...";
			this->InfomationToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::InfomationToolStripMenuItem_Click);
			// 
			// statusStrip
			// 
			this->statusStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->Status_toolStripProgressBar, 
				this->Status_toolStripStatusLabel, this->CameratoolStripStatusLabel});
			this->statusStrip->Location = System::Drawing::Point(0, 720);
			this->statusStrip->Name = L"statusStrip";
			this->statusStrip->Size = System::Drawing::Size(1080, 22);
			this->statusStrip->TabIndex = 4;
			this->statusStrip->Text = L"statusStrip1";
			// 
			// Status_toolStripProgressBar
			// 
			this->Status_toolStripProgressBar->Name = L"Status_toolStripProgressBar";
			this->Status_toolStripProgressBar->Size = System::Drawing::Size(200, 16);
			this->Status_toolStripProgressBar->Visible = false;
			// 
			// Status_toolStripStatusLabel
			// 
			this->Status_toolStripStatusLabel->AutoSize = false;
			this->Status_toolStripStatusLabel->Name = L"Status_toolStripStatusLabel";
			this->Status_toolStripStatusLabel->Size = System::Drawing::Size(30, 17);
			this->Status_toolStripStatusLabel->Text = L"0%";
			this->Status_toolStripStatusLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->Status_toolStripStatusLabel->Visible = false;
			// 
			// CameratoolStripStatusLabel
			// 
			this->CameratoolStripStatusLabel->AutoSize = false;
			this->CameratoolStripStatusLabel->BackColor = System::Drawing::SystemColors::Control;
			this->CameratoolStripStatusLabel->Name = L"CameratoolStripStatusLabel";
			this->CameratoolStripStatusLabel->Size = System::Drawing::Size(350, 17);
			this->CameratoolStripStatusLabel->Text = L"Camera pos : 0.0f, 0.0f, 0.0f";
			this->CameratoolStripStatusLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// AnitrackBar
			// 
			this->AnitrackBar->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->AnitrackBar->LargeChange = 50;
			this->AnitrackBar->Location = System::Drawing::Point(6, 10);
			this->AnitrackBar->Maximum = 100;
			this->AnitrackBar->Name = L"AnitrackBar";
			this->AnitrackBar->Size = System::Drawing::Size(651, 45);
			this->AnitrackBar->TabIndex = 0;
			this->AnitrackBar->TickFrequency = 50;
			this->AnitrackBar->TickStyle = System::Windows::Forms::TickStyle::TopLeft;
			this->AnitrackBar->Scroll += gcnew System::EventHandler(this, &Form1::AnitrackBar_Scroll);
			this->AnitrackBar->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::AnitrackBar_MouseDown);
			this->AnitrackBar->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::AnitrackBar_MouseUp);
			// 
			// stopbutton
			// 
			this->stopbutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->stopbutton->Location = System::Drawing::Point(599, 60);
			this->stopbutton->Name = L"stopbutton";
			this->stopbutton->Size = System::Drawing::Size(55, 23);
			this->stopbutton->TabIndex = 6;
			this->stopbutton->Text = L"Stop";
			this->stopbutton->UseVisualStyleBackColor = true;
			this->stopbutton->Click += gcnew System::EventHandler(this, &Form1::stopbutton_Click);
			// 
			// playbutton
			// 
			this->playbutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->playbutton->Location = System::Drawing::Point(538, 60);
			this->playbutton->Name = L"playbutton";
			this->playbutton->Size = System::Drawing::Size(55, 23);
			this->playbutton->TabIndex = 7;
			this->playbutton->Text = L"Play";
			this->playbutton->UseVisualStyleBackColor = true;
			this->playbutton->Click += gcnew System::EventHandler(this, &Form1::playbutton_Click);
			// 
			// groupBox3
			// 
			this->groupBox3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox3->Controls->Add(this->WaterEffect_checkBox);
			this->groupBox3->Controls->Add(this->checkBoxShowBone);
			this->groupBox3->Controls->Add(this->TalentAniNameBar);
			this->groupBox3->Controls->Add(this->playbutton);
			this->groupBox3->Controls->Add(this->pausebutton);
			this->groupBox3->Controls->Add(this->checkShowEffect);
			this->groupBox3->Controls->Add(this->MoveDummycheckBox);
			this->groupBox3->Controls->Add(this->LoopPlaycheckBox);
			this->groupBox3->Controls->Add(this->Framelabel2);
			this->groupBox3->Controls->Add(this->Framelabel);
			this->groupBox3->Controls->Add(this->stopbutton);
			this->groupBox3->Controls->Add(this->AnitrackBar);
			this->groupBox3->Controls->Add(this->dummy_loc_panel);
			this->groupBox3->Location = System::Drawing::Point(218, 574);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(660, 90);
			this->groupBox3->TabIndex = 9;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"FrameSlider";
			// 
			// WaterEffect_checkBox
			// 
			this->WaterEffect_checkBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->WaterEffect_checkBox->AutoSize = true;
			this->WaterEffect_checkBox->Location = System::Drawing::Point(444, 70);
			this->WaterEffect_checkBox->Name = L"WaterEffect_checkBox";
			this->WaterEffect_checkBox->Size = System::Drawing::Size(86, 17);
			this->WaterEffect_checkBox->TabIndex = 19;
			this->WaterEffect_checkBox->Text = L"Water Effect";
			this->WaterEffect_checkBox->UseVisualStyleBackColor = true;
			this->WaterEffect_checkBox->CheckedChanged += gcnew System::EventHandler(this, &Form1::WaterEffect_checkBox_CheckedChanged);
			// 
			// checkBoxShowBone
			// 
			this->checkBoxShowBone->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->checkBoxShowBone->AutoSize = true;
			this->checkBoxShowBone->Location = System::Drawing::Point(107, 70);
			this->checkBoxShowBone->Name = L"checkBoxShowBone";
			this->checkBoxShowBone->Size = System::Drawing::Size(78, 17);
			this->checkBoxShowBone->TabIndex = 17;
			this->checkBoxShowBone->Text = L"ShowBone";
			this->checkBoxShowBone->UseVisualStyleBackColor = true;
			this->checkBoxShowBone->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBoxShowBone_CheckedChanged);
			// 
			// TalentAniNameBar
			// 
			this->TalentAniNameBar->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->TalentAniNameBar->Location = System::Drawing::Point(14, 44);
			this->TalentAniNameBar->Name = L"TalentAniNameBar";
			this->TalentAniNameBar->Size = System::Drawing::Size(634, 10);
			this->TalentAniNameBar->TabIndex = 16;
			// 
			// pausebutton
			// 
			this->pausebutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->pausebutton->Location = System::Drawing::Point(538, 60);
			this->pausebutton->Name = L"pausebutton";
			this->pausebutton->Size = System::Drawing::Size(55, 24);
			this->pausebutton->TabIndex = 15;
			this->pausebutton->Text = L"Pause";
			this->pausebutton->UseVisualStyleBackColor = true;
			this->pausebutton->Click += gcnew System::EventHandler(this, &Form1::pausebutton_Click);
			// 
			// checkShowEffect
			// 
			this->checkShowEffect->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->checkShowEffect->AutoSize = true;
			this->checkShowEffect->Checked = true;
			this->checkShowEffect->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkShowEffect->Location = System::Drawing::Point(382, 70);
			this->checkShowEffect->Name = L"checkShowEffect";
			this->checkShowEffect->Size = System::Drawing::Size(54, 17);
			this->checkShowEffect->TabIndex = 13;
			this->checkShowEffect->Text = L"Effect";
			this->checkShowEffect->UseVisualStyleBackColor = true;
			this->checkShowEffect->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkShowEffect_CheckedChanged);
			// 
			// MoveDummycheckBox
			// 
			this->MoveDummycheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->MoveDummycheckBox->AutoSize = true;
			this->MoveDummycheckBox->Checked = true;
			this->MoveDummycheckBox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->MoveDummycheckBox->Location = System::Drawing::Point(202, 70);
			this->MoveDummycheckBox->Name = L"MoveDummycheckBox";
			this->MoveDummycheckBox->Size = System::Drawing::Size(88, 17);
			this->MoveDummycheckBox->TabIndex = 12;
			this->MoveDummycheckBox->Text = L"MoveDummy";
			this->MoveDummycheckBox->UseVisualStyleBackColor = true;
			this->MoveDummycheckBox->CheckedChanged += gcnew System::EventHandler(this, &Form1::MoveDummycheckBox_CheckedChanged);
			// 
			// LoopPlaycheckBox
			// 
			this->LoopPlaycheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->LoopPlaycheckBox->AutoSize = true;
			this->LoopPlaycheckBox->Location = System::Drawing::Point(305, 70);
			this->LoopPlaycheckBox->Name = L"LoopPlaycheckBox";
			this->LoopPlaycheckBox->Size = System::Drawing::Size(70, 17);
			this->LoopPlaycheckBox->TabIndex = 10;
			this->LoopPlaycheckBox->Text = L"LoopPlay";
			this->LoopPlaycheckBox->UseVisualStyleBackColor = true;
			this->LoopPlaycheckBox->Click += gcnew System::EventHandler(this, &Form1::LoopPlaycheckBox_Click);
			this->LoopPlaycheckBox->CheckedChanged += gcnew System::EventHandler(this, &Form1::LoopPlaycheckBox_CheckedChanged);
			// 
			// Framelabel2
			// 
			this->Framelabel2->AutoSize = true;
			this->Framelabel2->Location = System::Drawing::Point(6, 75);
			this->Framelabel2->Name = L"Framelabel2";
			this->Framelabel2->Size = System::Drawing::Size(36, 13);
			this->Framelabel2->TabIndex = 9;
			this->Framelabel2->Text = L"Frame";
			// 
			// Framelabel
			// 
			this->Framelabel->AutoSize = true;
			this->Framelabel->Location = System::Drawing::Point(6, 58);
			this->Framelabel->Name = L"Framelabel";
			this->Framelabel->Size = System::Drawing::Size(131, 13);
			this->Framelabel->TabIndex = 8;
			this->Framelabel->Text = L"Aniname : 0/0 frame (0ms)";
			// 
			// dummy_loc_panel
			// 
			this->dummy_loc_panel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->dummy_loc_panel->Location = System::Drawing::Point(41, 71);
			this->dummy_loc_panel->Name = L"dummy_loc_panel";
			this->dummy_loc_panel->Size = System::Drawing::Size(126, 16);
			this->dummy_loc_panel->TabIndex = 18;
			// 
			// toolStrip1
			// 
			this->toolStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(25) {this->OpenToolStripButton, 
				this->SaveToolStripButton, this->toolStripSeparator, this->DrawBoxtoolStripButton, this->toolStripSeparator6, this->LighrtoolStripButton, 
				this->LightToCameratoolStripButton1, this->AmbienttoolStripButton1, this->toolStripSeparator7, this->Unselected_toolStripButton, 
				this->Bottom_toolStripButton, this->Top_toolStripButton, this->TopBottom_toolStripButton, this->X_toolStripButton, this->Y_toolStripButton, 
				this->Z_toolStripButton, this->XY_toolStripButton, this->CapsuleOpa_toolStripButton, this->toolStripSeparator8, this->ReLoad_toolStripSplitButton, 
				this->toolStripSeparator11, this->bp1, this->bp2, this->bp3, this->bp4});
			this->toolStrip1->Location = System::Drawing::Point(0, 24);
			this->toolStrip1->Name = L"toolStrip1";
			this->toolStrip1->Size = System::Drawing::Size(1080, 25);
			this->toolStrip1->TabIndex = 8;
			this->toolStrip1->Text = L"toolStrip1";
			this->toolStrip1->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &Form1::toolStrip1_ItemClicked);
			// 
			// OpenToolStripButton
			// 
			this->OpenToolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->OpenToolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"OpenToolStripButton.Image")));
			this->OpenToolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->OpenToolStripButton->Name = L"OpenToolStripButton";
			this->OpenToolStripButton->Size = System::Drawing::Size(23, 22);
			this->OpenToolStripButton->Text = L"열기(&O)";
			this->OpenToolStripButton->Click += gcnew System::EventHandler(this, &Form1::OpentoolStripButton_Click);
			// 
			// SaveToolStripButton
			// 
			this->SaveToolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->SaveToolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"SaveToolStripButton.Image")));
			this->SaveToolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->SaveToolStripButton->Name = L"SaveToolStripButton";
			this->SaveToolStripButton->Size = System::Drawing::Size(23, 22);
			this->SaveToolStripButton->Text = L"전체저장";
			this->SaveToolStripButton->Click += gcnew System::EventHandler(this, &Form1::SaveToolStripButton_Click);
			// 
			// toolStripSeparator
			// 
			this->toolStripSeparator->Name = L"toolStripSeparator";
			this->toolStripSeparator->Size = System::Drawing::Size(6, 25);
			// 
			// DrawBoxtoolStripButton
			// 
			this->DrawBoxtoolStripButton->CheckOnClick = true;
			this->DrawBoxtoolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->DrawBoxtoolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"DrawBoxtoolStripButton.Image")));
			this->DrawBoxtoolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->DrawBoxtoolStripButton->Name = L"DrawBoxtoolStripButton";
			this->DrawBoxtoolStripButton->Size = System::Drawing::Size(23, 22);
			this->DrawBoxtoolStripButton->Text = L"새로 만들기(&N)";
			this->DrawBoxtoolStripButton->ToolTipText = L"AABB 박스 그리기";
			this->DrawBoxtoolStripButton->Click += gcnew System::EventHandler(this, &Form1::DrawBoxtoolStripButton_Click);
			// 
			// toolStripSeparator6
			// 
			this->toolStripSeparator6->Name = L"toolStripSeparator6";
			this->toolStripSeparator6->Size = System::Drawing::Size(6, 25);
			// 
			// LighrtoolStripButton
			// 
			this->LighrtoolStripButton->CheckOnClick = true;
			this->LighrtoolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->LighrtoolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"LighrtoolStripButton.Image")));
			this->LighrtoolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->LighrtoolStripButton->Name = L"LighrtoolStripButton";
			this->LighrtoolStripButton->Size = System::Drawing::Size(23, 22);
			this->LighrtoolStripButton->Text = L"LightOn/Off";
			this->LighrtoolStripButton->ToolTipText = L"라이트 끄고 켜기";
			this->LighrtoolStripButton->Click += gcnew System::EventHandler(this, &Form1::LighrtoolStripButton_Click);
			// 
			// LightToCameratoolStripButton1
			// 
			this->LightToCameratoolStripButton1->CheckOnClick = true;
			this->LightToCameratoolStripButton1->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->LightToCameratoolStripButton1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"LightToCameratoolStripButton1.Image")));
			this->LightToCameratoolStripButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->LightToCameratoolStripButton1->Name = L"LightToCameratoolStripButton1";
			this->LightToCameratoolStripButton1->Size = System::Drawing::Size(23, 22);
			this->LightToCameratoolStripButton1->Text = L"LightToCamera";
			this->LightToCameratoolStripButton1->ToolTipText = L"카메라위치에서 라이트비추기";
			this->LightToCameratoolStripButton1->Click += gcnew System::EventHandler(this, &Form1::LightToCameratoolStripButton1_Click);
			// 
			// AmbienttoolStripButton1
			// 
			this->AmbienttoolStripButton1->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->AmbienttoolStripButton1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"AmbienttoolStripButton1.Image")));
			this->AmbienttoolStripButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->AmbienttoolStripButton1->Name = L"AmbienttoolStripButton1";
			this->AmbienttoolStripButton1->Size = System::Drawing::Size(23, 22);
			this->AmbienttoolStripButton1->Text = L"파츠";
			this->AmbienttoolStripButton1->ToolTipText = L"파츠 컬러 설정";
			this->AmbienttoolStripButton1->Click += gcnew System::EventHandler(this, &Form1::PartsColorPreviewMenuItem_Click);
			// 
			// toolStripSeparator7
			// 
			this->toolStripSeparator7->Name = L"toolStripSeparator7";
			this->toolStripSeparator7->Size = System::Drawing::Size(6, 25);
			// 
			// Unselected_toolStripButton
			// 
			this->Unselected_toolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->Unselected_toolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Unselected_toolStripButton.Image")));
			this->Unselected_toolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->Unselected_toolStripButton->Name = L"Unselected_toolStripButton";
			this->Unselected_toolStripButton->Size = System::Drawing::Size(35, 22);
			this->Unselected_toolStripButton->Text = L"해제";
			this->Unselected_toolStripButton->Click += gcnew System::EventHandler(this, &Form1::Unselected_toolStripButton_Click);
			// 
			// Bottom_toolStripButton
			// 
			this->Bottom_toolStripButton->CheckOnClick = true;
			this->Bottom_toolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->Bottom_toolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Bottom_toolStripButton.Image")));
			this->Bottom_toolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->Bottom_toolStripButton->Name = L"Bottom_toolStripButton";
			this->Bottom_toolStripButton->Size = System::Drawing::Size(25, 22);
			this->Bottom_toolStripButton->Text = L"[1]";
			this->Bottom_toolStripButton->Click += gcnew System::EventHandler(this, &Form1::Bottom_toolStripButton_Click);
			// 
			// Top_toolStripButton
			// 
			this->Top_toolStripButton->CheckOnClick = true;
			this->Top_toolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->Top_toolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Top_toolStripButton.Image")));
			this->Top_toolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->Top_toolStripButton->Name = L"Top_toolStripButton";
			this->Top_toolStripButton->Size = System::Drawing::Size(25, 22);
			this->Top_toolStripButton->Text = L"[2]";
			this->Top_toolStripButton->Click += gcnew System::EventHandler(this, &Form1::Top_toolStripButton_Click);
			// 
			// TopBottom_toolStripButton
			// 
			this->TopBottom_toolStripButton->CheckOnClick = true;
			this->TopBottom_toolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->TopBottom_toolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"TopBottom_toolStripButton.Image")));
			this->TopBottom_toolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->TopBottom_toolStripButton->Name = L"TopBottom_toolStripButton";
			this->TopBottom_toolStripButton->Size = System::Drawing::Size(39, 22);
			this->TopBottom_toolStripButton->Text = L"[1+2]";
			this->TopBottom_toolStripButton->Click += gcnew System::EventHandler(this, &Form1::TopBottom_toolStripButton_Click);
			// 
			// X_toolStripButton
			// 
			this->X_toolStripButton->CheckOnClick = true;
			this->X_toolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->X_toolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"X_toolStripButton.Image")));
			this->X_toolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->X_toolStripButton->Name = L"X_toolStripButton";
			this->X_toolStripButton->Size = System::Drawing::Size(23, 22);
			this->X_toolStripButton->Text = L"X";
			this->X_toolStripButton->Click += gcnew System::EventHandler(this, &Form1::X_toolStripButton_Click);
			// 
			// Y_toolStripButton
			// 
			this->Y_toolStripButton->CheckOnClick = true;
			this->Y_toolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->Y_toolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Y_toolStripButton.Image")));
			this->Y_toolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->Y_toolStripButton->Name = L"Y_toolStripButton";
			this->Y_toolStripButton->Size = System::Drawing::Size(23, 22);
			this->Y_toolStripButton->Text = L"Y";
			this->Y_toolStripButton->Click += gcnew System::EventHandler(this, &Form1::Y_toolStripButton_Click);
			// 
			// Z_toolStripButton
			// 
			this->Z_toolStripButton->CheckOnClick = true;
			this->Z_toolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->Z_toolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Z_toolStripButton.Image")));
			this->Z_toolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->Z_toolStripButton->Name = L"Z_toolStripButton";
			this->Z_toolStripButton->Size = System::Drawing::Size(23, 22);
			this->Z_toolStripButton->Text = L"Z";
			this->Z_toolStripButton->Click += gcnew System::EventHandler(this, &Form1::Z_toolStripButton_Click);
			// 
			// XY_toolStripButton
			// 
			this->XY_toolStripButton->CheckOnClick = true;
			this->XY_toolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->XY_toolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"XY_toolStripButton.Image")));
			this->XY_toolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->XY_toolStripButton->Name = L"XY_toolStripButton";
			this->XY_toolStripButton->Size = System::Drawing::Size(25, 22);
			this->XY_toolStripButton->Text = L"XY";
			this->XY_toolStripButton->Click += gcnew System::EventHandler(this, &Form1::XY_toolStripButton_Click);
			// 
			// CapsuleOpa_toolStripButton
			// 
			this->CapsuleOpa_toolStripButton->CheckOnClick = true;
			this->CapsuleOpa_toolStripButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->CapsuleOpa_toolStripButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"CapsuleOpa_toolStripButton.Image")));
			this->CapsuleOpa_toolStripButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->CapsuleOpa_toolStripButton->Name = L"CapsuleOpa_toolStripButton";
			this->CapsuleOpa_toolStripButton->Size = System::Drawing::Size(54, 22);
			this->CapsuleOpa_toolStripButton->Text = L"CapOpa";
			// 
			// toolStripSeparator8
			// 
			this->toolStripSeparator8->Name = L"toolStripSeparator8";
			this->toolStripSeparator8->Size = System::Drawing::Size(6, 25);
			// 
			// ReLoad_toolStripSplitButton
			// 
			this->ReLoad_toolStripSplitButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->ReLoad_toolStripSplitButton->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->effectxmlToolStripMenuItem, 
				this->soundxmlToolStripMenuItem, this->talentxmlToolStripMenuItem, this->buffeffectinfoxmlToolStripMenuItem, this->motionxxxmlToolStripMenuItem, 
				this->npcxmlToolStripMenuItem});
			this->ReLoad_toolStripSplitButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"ReLoad_toolStripSplitButton.Image")));
			this->ReLoad_toolStripSplitButton->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->ReLoad_toolStripSplitButton->Name = L"ReLoad_toolStripSplitButton";
			this->ReLoad_toolStripSplitButton->Size = System::Drawing::Size(59, 22);
			this->ReLoad_toolStripSplitButton->Text = L"리로드";
			this->ReLoad_toolStripSplitButton->ButtonClick += gcnew System::EventHandler(this, &Form1::ReLoad_toolStripSplitButton_ButtonClick);
			// 
			// effectxmlToolStripMenuItem
			// 
			this->effectxmlToolStripMenuItem->Name = L"effectxmlToolStripMenuItem";
			this->effectxmlToolStripMenuItem->Size = System::Drawing::Size(179, 22);
			this->effectxmlToolStripMenuItem->Text = L"effect.xml";
			this->effectxmlToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::effectxmlToolStripMenuItem_Click);
			// 
			// soundxmlToolStripMenuItem
			// 
			this->soundxmlToolStripMenuItem->Name = L"soundxmlToolStripMenuItem";
			this->soundxmlToolStripMenuItem->Size = System::Drawing::Size(179, 22);
			this->soundxmlToolStripMenuItem->Text = L"sound.xml";
			this->soundxmlToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::soundxmlToolStripMenuItem_Click);
			// 
			// talentxmlToolStripMenuItem
			// 
			this->talentxmlToolStripMenuItem->Name = L"talentxmlToolStripMenuItem";
			this->talentxmlToolStripMenuItem->Size = System::Drawing::Size(179, 22);
			this->talentxmlToolStripMenuItem->Text = L"talent.xml";
			this->talentxmlToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::talentxmlToolStripMenuItem_Click);
			// 
			// buffeffectinfoxmlToolStripMenuItem
			// 
			this->buffeffectinfoxmlToolStripMenuItem->Name = L"buffeffectinfoxmlToolStripMenuItem";
			this->buffeffectinfoxmlToolStripMenuItem->Size = System::Drawing::Size(179, 22);
			this->buffeffectinfoxmlToolStripMenuItem->Text = L"buff_effect_info.xml";
			this->buffeffectinfoxmlToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::buffeffectinfoxmlToolStripMenuItem_Click);
			// 
			// motionxxxmlToolStripMenuItem
			// 
			this->motionxxxmlToolStripMenuItem->Name = L"motionxxxmlToolStripMenuItem";
			this->motionxxxmlToolStripMenuItem->Size = System::Drawing::Size(179, 22);
			this->motionxxxmlToolStripMenuItem->Text = L"motion_xx.xml";
			this->motionxxxmlToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::motionxxxmlToolStripMenuItem_Click);
			// 
			// npcxmlToolStripMenuItem
			// 
			this->npcxmlToolStripMenuItem->Name = L"npcxmlToolStripMenuItem";
			this->npcxmlToolStripMenuItem->Size = System::Drawing::Size(179, 22);
			this->npcxmlToolStripMenuItem->Text = L"npc.xml";
			this->npcxmlToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::npcxmlToolStripMenuItem_Click);
			// 
			// toolStripSeparator11
			// 
			this->toolStripSeparator11->Name = L"toolStripSeparator11";
			this->toolStripSeparator11->Size = System::Drawing::Size(6, 25);
			// 
			// bp1
			// 
			this->bp1->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->bp1->Enabled = false;
			this->bp1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"bp1.Image")));
			this->bp1->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->bp1->Name = L"bp1";
			this->bp1->Size = System::Drawing::Size(39, 22);
			this->bp1->Text = L"[bp1]";
			this->bp1->ToolTipText = L"브레이커블 파츠 1";
			this->bp1->Click += gcnew System::EventHandler(this, &Form1::bp1_Click);
			// 
			// bp2
			// 
			this->bp2->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->bp2->Enabled = false;
			this->bp2->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"bp2.Image")));
			this->bp2->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->bp2->Name = L"bp2";
			this->bp2->Size = System::Drawing::Size(39, 22);
			this->bp2->Text = L"[bp2]";
			this->bp2->ToolTipText = L"브레이커블 파츠 2";
			this->bp2->Click += gcnew System::EventHandler(this, &Form1::bp2_Click);
			// 
			// bp3
			// 
			this->bp3->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->bp3->Enabled = false;
			this->bp3->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"bp3.Image")));
			this->bp3->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->bp3->Name = L"bp3";
			this->bp3->Size = System::Drawing::Size(39, 22);
			this->bp3->Text = L"[bp3]";
			this->bp3->ToolTipText = L"브레이커블 파츠 3";
			this->bp3->Click += gcnew System::EventHandler(this, &Form1::bp3_Click);
			// 
			// bp4
			// 
			this->bp4->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->bp4->Enabled = false;
			this->bp4->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"bp4.Image")));
			this->bp4->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->bp4->Name = L"bp4";
			this->bp4->Size = System::Drawing::Size(39, 22);
			this->bp4->Text = L"[bp4]";
			this->bp4->ToolTipText = L"브레이커블 파츠 4";
			this->bp4->Click += gcnew System::EventHandler(this, &Form1::bp4_Click);
			// 
			// groupBox4
			// 
			this->groupBox4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->groupBox4->Controls->Add(this->AniSpeedResetbutton);
			this->groupBox4->Controls->Add(this->AniSpeednumericUpDown);
			this->groupBox4->Controls->Add(SpeedOkbutton);
			this->groupBox4->Controls->Add(this->Speedlabel);
			this->groupBox4->Controls->Add(this->AniSpeedtrackBar);
			this->groupBox4->Location = System::Drawing::Point(884, 574);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(186, 90);
			this->groupBox4->TabIndex = 10;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Speed";
			// 
			// AniSpeedResetbutton
			// 
			this->AniSpeedResetbutton->Location = System::Drawing::Point(78, 61);
			this->AniSpeedResetbutton->Name = L"AniSpeedResetbutton";
			this->AniSpeedResetbutton->Size = System::Drawing::Size(48, 23);
			this->AniSpeedResetbutton->TabIndex = 4;
			this->AniSpeedResetbutton->Text = L"리셋";
			this->AniSpeedResetbutton->UseVisualStyleBackColor = true;
			this->AniSpeedResetbutton->Click += gcnew System::EventHandler(this, &Form1::AniSpeedResetbutton_Click);
			// 
			// AniSpeednumericUpDown
			// 
			this->AniSpeednumericUpDown->DecimalPlaces = 2;
			this->AniSpeednumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->AniSpeednumericUpDown->Location = System::Drawing::Point(121, 20);
			this->AniSpeednumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
			this->AniSpeednumericUpDown->Name = L"AniSpeednumericUpDown";
			this->AniSpeednumericUpDown->Size = System::Drawing::Size(59, 20);
			this->AniSpeednumericUpDown->TabIndex = 3;
			this->AniSpeednumericUpDown->ValueChanged += gcnew System::EventHandler(this, &Form1::AniSpeednumericUpDown_ValueChanged);
			// 
			// Speedlabel
			// 
			this->Speedlabel->AutoSize = true;
			this->Speedlabel->Location = System::Drawing::Point(6, 58);
			this->Speedlabel->Name = L"Speedlabel";
			this->Speedlabel->Size = System::Drawing::Size(53, 13);
			this->Speedlabel->TabIndex = 1;
			this->Speedlabel->Text = L"Speed : 1";
			// 
			// AniSpeedtrackBar
			// 
			this->AniSpeedtrackBar->Location = System::Drawing::Point(6, 20);
			this->AniSpeedtrackBar->Maximum = 50;
			this->AniSpeedtrackBar->Name = L"AniSpeedtrackBar";
			this->AniSpeedtrackBar->Size = System::Drawing::Size(109, 45);
			this->AniSpeedtrackBar->TabIndex = 0;
			this->AniSpeedtrackBar->Value = 10;
			this->AniSpeedtrackBar->Scroll += gcnew System::EventHandler(this, &Form1::AniSpeedtrackBar_Scroll);
			// 
			// splitContainer1
			// 
			this->splitContainer1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->splitContainer1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->splitContainer1->FixedPanel = System::Windows::Forms::FixedPanel::Panel1;
			this->splitContainer1->Location = System::Drawing::Point(0, 52);
			this->splitContainer1->Name = L"splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->tabControl1);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
			this->splitContainer1->Panel2->Controls->Add(this->splitContainer2);
			this->splitContainer1->Size = System::Drawing::Size(1080, 480);
			this->splitContainer1->SplitterDistance = 210;
			this->splitContainer1->TabIndex = 14;
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage4);
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl1->Location = System::Drawing::Point(0, 0);
			this->tabControl1->Multiline = true;
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(206, 476);
			this->tabControl1->TabIndex = 1;
			this->tabControl1->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::tabControl1_SelectedIndexChanged);
			// 
			// tabPage4
			// 
			this->tabPage4->Location = System::Drawing::Point(4, 22);
			this->tabPage4->Name = L"tabPage4";
			this->tabPage4->Size = System::Drawing::Size(198, 450);
			this->tabPage4->TabIndex = 1;
			this->tabPage4->Text = L"모델";
			this->tabPage4->ToolTipText = L"모텔";
			this->tabPage4->UseVisualStyleBackColor = true;
			// 
			// tabPage1
			// 
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(198, 454);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"NPC리스트";
			this->tabPage1->ToolTipText = L"NPC";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// tabPage2
			// 
			this->tabPage2->AutoScroll = true;
			this->tabPage2->AutoScrollMinSize = System::Drawing::Size(200, 500);
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Size = System::Drawing::Size(198, 454);
			this->tabPage2->TabIndex = 2;
			this->tabPage2->Text = L"PC 탈렌트";
			this->tabPage2->ToolTipText = L"탈렌트";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// splitContainer2
			// 
			this->splitContainer2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->splitContainer2->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->splitContainer2->FixedPanel = System::Windows::Forms::FixedPanel::Panel2;
			this->splitContainer2->Location = System::Drawing::Point(0, 0);
			this->splitContainer2->Name = L"splitContainer2";
			// 
			// splitContainer2.Panel1
			// 
			this->splitContainer2->Panel1->Controls->Add(this->MainView);
			// 
			// splitContainer2.Panel2
			// 
			this->splitContainer2->Panel2->Controls->Add(this->tabControl2);
			this->splitContainer2->Size = System::Drawing::Size(866, 480);
			this->splitContainer2->SplitterDistance = 569;
			this->splitContainer2->TabIndex = 0;
			// 
			// MainView
			// 
			this->MainView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->MainView->Location = System::Drawing::Point(0, 0);
			this->MainView->Name = L"MainView";
			this->MainView->Size = System::Drawing::Size(565, 476);
			this->MainView->TabIndex = 2;
			this->MainView->TabStop = false;
			this->MainView->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::MainForm_MouseWheel);
			this->MainView->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &Form1::MainView_PreviewKeyDown);
			this->MainView->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::MainView_MouseMove);
			this->MainView->Click += gcnew System::EventHandler(this, &Form1::MainView_Click);
			this->MainView->Resize += gcnew System::EventHandler(this, &Form1::MainView_Resize);
			this->MainView->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::MainView_MouseClick_1);
			this->MainView->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::MainView_MouseDown);
			this->MainView->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::MainView_MouseUp);
			this->MainView->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::MainForm_KeyDown);
			// 
			// tabControl2
			// 
			this->tabControl2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl2->Appearance = System::Windows::Forms::TabAppearance::FlatButtons;
			this->tabControl2->Controls->Add(this->Itemtab);
			this->tabControl2->Controls->Add(this->Talenttab);
			this->tabControl2->Controls->Add(this->Animationtab);
			this->tabControl2->Controls->Add(this->ModelInfotab);
			this->tabControl2->Controls->Add(this->ItemPreviewtabPage);
			this->tabControl2->Controls->Add(this->Effecttab);
			this->tabControl2->Controls->Add(this->TalentEvent);
			this->tabControl2->Controls->Add(this->PartsJudgment);
			this->tabControl2->Controls->Add(this->SubMtrltab);
			this->tabControl2->Controls->Add(this->MeshInfotab);
			this->tabControl2->ItemSize = System::Drawing::Size(57, 20);
			this->tabControl2->Location = System::Drawing::Point(0, 0);
			this->tabControl2->Multiline = true;
			this->tabControl2->Name = L"tabControl2";
			this->tabControl2->SelectedIndex = 0;
			this->tabControl2->Size = System::Drawing::Size(289, 478);
			this->tabControl2->SizeMode = System::Windows::Forms::TabSizeMode::Fixed;
			this->tabControl2->TabIndex = 3;
			this->tabControl2->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::Control1_Click);
			// 
			// Itemtab
			// 
			this->Itemtab->AutoScroll = true;
			this->Itemtab->AutoScrollMinSize = System::Drawing::Size(200, 500);
			this->Itemtab->Location = System::Drawing::Point(4, 70);
			this->Itemtab->Name = L"Itemtab";
			this->Itemtab->Padding = System::Windows::Forms::Padding(3);
			this->Itemtab->Size = System::Drawing::Size(281, 404);
			this->Itemtab->TabIndex = 0;
			this->Itemtab->Text = L"아이템";
			this->Itemtab->UseVisualStyleBackColor = true;
			// 
			// Talenttab
			// 
			this->Talenttab->AutoScroll = true;
			this->Talenttab->AutoScrollMinSize = System::Drawing::Size(200, 500);
			this->Talenttab->Location = System::Drawing::Point(4, 70);
			this->Talenttab->Name = L"Talenttab";
			this->Talenttab->Padding = System::Windows::Forms::Padding(3);
			this->Talenttab->Size = System::Drawing::Size(281, 408);
			this->Talenttab->TabIndex = 1;
			this->Talenttab->Text = L"판정";
			this->Talenttab->UseVisualStyleBackColor = true;
			// 
			// Animationtab
			// 
			this->Animationtab->AutoScroll = true;
			this->Animationtab->AutoScrollMinSize = System::Drawing::Size(200, 500);
			this->Animationtab->Location = System::Drawing::Point(4, 70);
			this->Animationtab->Name = L"Animationtab";
			this->Animationtab->Padding = System::Windows::Forms::Padding(3);
			this->Animationtab->Size = System::Drawing::Size(281, 408);
			this->Animationtab->TabIndex = 2;
			this->Animationtab->Text = L"애니메이션";
			this->Animationtab->UseVisualStyleBackColor = true;
			// 
			// ModelInfotab
			// 
			this->ModelInfotab->AutoScroll = true;
			this->ModelInfotab->AutoScrollMinSize = System::Drawing::Size(200, 500);
			this->ModelInfotab->Location = System::Drawing::Point(4, 70);
			this->ModelInfotab->Name = L"ModelInfotab";
			this->ModelInfotab->Padding = System::Windows::Forms::Padding(3);
			this->ModelInfotab->Size = System::Drawing::Size(281, 408);
			this->ModelInfotab->TabIndex = 4;
			this->ModelInfotab->Text = L"모델정보";
			this->ModelInfotab->UseVisualStyleBackColor = true;
			// 
			// ItemPreviewtabPage
			// 
			this->ItemPreviewtabPage->AutoScroll = true;
			this->ItemPreviewtabPage->AutoScrollMinSize = System::Drawing::Size(200, 500);
			this->ItemPreviewtabPage->Controls->Add(this->basicMesh_button);
			this->ItemPreviewtabPage->Controls->Add(this->Refresh_button);
			this->ItemPreviewtabPage->Controls->Add(this->ItemOpenbutton);
			this->ItemPreviewtabPage->Controls->Add(this->ItemPreviewtreeView);
			this->ItemPreviewtabPage->Location = System::Drawing::Point(4, 70);
			this->ItemPreviewtabPage->Name = L"ItemPreviewtabPage";
			this->ItemPreviewtabPage->Size = System::Drawing::Size(281, 408);
			this->ItemPreviewtabPage->TabIndex = 7;
			this->ItemPreviewtabPage->Text = L"아템미리보기";
			this->ItemPreviewtabPage->UseVisualStyleBackColor = true;
			// 
			// basicMesh_button
			// 
			this->basicMesh_button->Location = System::Drawing::Point(184, 3);
			this->basicMesh_button->Name = L"basicMesh_button";
			this->basicMesh_button->Size = System::Drawing::Size(70, 49);
			this->basicMesh_button->TabIndex = 12;
			this->basicMesh_button->Text = L"기본메쉬";
			this->basicMesh_button->UseVisualStyleBackColor = true;
			this->basicMesh_button->Click += gcnew System::EventHandler(this, &Form1::basicMesh_button_Click);
			// 
			// Refresh_button
			// 
			this->Refresh_button->Location = System::Drawing::Point(3, 3);
			this->Refresh_button->Name = L"Refresh_button";
			this->Refresh_button->Size = System::Drawing::Size(138, 49);
			this->Refresh_button->TabIndex = 11;
			this->Refresh_button->Text = L"읽기";
			this->Refresh_button->UseVisualStyleBackColor = true;
			this->Refresh_button->Click += gcnew System::EventHandler(this, &Form1::Refresh_button_Click);
			// 
			// ItemOpenbutton
			// 
			this->ItemOpenbutton->AutoSize = true;
			this->ItemOpenbutton->Enabled = false;
			this->ItemOpenbutton->Location = System::Drawing::Point(151, 3);
			this->ItemOpenbutton->Name = L"ItemOpenbutton";
			this->ItemOpenbutton->Size = System::Drawing::Size(27, 23);
			this->ItemOpenbutton->TabIndex = 4;
			this->ItemOpenbutton->Text = L"...";
			this->ItemOpenbutton->UseVisualStyleBackColor = true;
			this->ItemOpenbutton->Click += gcnew System::EventHandler(this, &Form1::ItemOpenbutton_Click);
			// 
			// ItemPreviewtreeView
			// 
			this->ItemPreviewtreeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->ItemPreviewtreeView->HideSelection = false;
			this->ItemPreviewtreeView->Location = System::Drawing::Point(5, 58);
			this->ItemPreviewtreeView->Name = L"ItemPreviewtreeView";
			this->ItemPreviewtreeView->Size = System::Drawing::Size(239, 439);
			this->ItemPreviewtreeView->TabIndex = 3;
			this->ItemPreviewtreeView->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &Form1::ItemPreviewtreeView_NodeMouseClick);
			// 
			// Effecttab
			// 
			this->Effecttab->AutoScroll = true;
			this->Effecttab->AutoScrollMinSize = System::Drawing::Size(200, 500);
			this->Effecttab->Controls->Add(this->splitContainer6);
			this->Effecttab->Location = System::Drawing::Point(4, 70);
			this->Effecttab->Name = L"Effecttab";
			this->Effecttab->Padding = System::Windows::Forms::Padding(3);
			this->Effecttab->Size = System::Drawing::Size(281, 408);
			this->Effecttab->TabIndex = 11;
			this->Effecttab->Text = L"이펙트";
			this->Effecttab->UseVisualStyleBackColor = true;
			// 
			// splitContainer6
			// 
			this->splitContainer6->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->splitContainer6->BackColor = System::Drawing::Color::White;
			this->splitContainer6->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->splitContainer6->Location = System::Drawing::Point(3, 3);
			this->splitContainer6->Name = L"splitContainer6";
			this->splitContainer6->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer6.Panel1
			// 
			this->splitContainer6->Panel1->Controls->Add(this->groupBox13);
			// 
			// splitContainer6.Panel2
			// 
			this->splitContainer6->Panel2->Controls->Add(this->groupBox10);
			this->splitContainer6->Size = System::Drawing::Size(241, 501);
			this->splitContainer6->SplitterDistance = 268;
			this->splitContainer6->TabIndex = 22;
			// 
			// groupBox13
			// 
			this->groupBox13->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox13->Controls->Add(this->Deref_button);
			this->groupBox13->Controls->Add(this->CurrentID);
			this->groupBox13->Controls->Add(this->TalentEffectList_tabControl);
			this->groupBox13->Controls->Add(this->label24);
			this->groupBox13->Controls->Add(this->label18);
			this->groupBox13->Controls->Add(this->RefSelectBtn);
			this->groupBox13->Controls->Add(this->RefTextBox);
			this->groupBox13->Controls->Add(this->button1);
			this->groupBox13->Location = System::Drawing::Point(2, 3);
			this->groupBox13->Name = L"groupBox13";
			this->groupBox13->Size = System::Drawing::Size(234, 260);
			this->groupBox13->TabIndex = 21;
			this->groupBox13->TabStop = false;
			this->groupBox13->Text = L"탤런트 이펙트";
			// 
			// Deref_button
			// 
			this->Deref_button->Enabled = false;
			this->Deref_button->Location = System::Drawing::Point(136, 43);
			this->Deref_button->Name = L"Deref_button";
			this->Deref_button->Size = System::Drawing::Size(49, 21);
			this->Deref_button->TabIndex = 30;
			this->Deref_button->Text = L"역참조";
			this->Deref_button->UseVisualStyleBackColor = true;
			this->Deref_button->Click += gcnew System::EventHandler(this, &Form1::Deref_button_Click);
			// 
			// CurrentID
			// 
			this->CurrentID->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->CurrentID->Location = System::Drawing::Point(51, 44);
			this->CurrentID->Name = L"CurrentID";
			this->CurrentID->ReadOnly = true;
			this->CurrentID->Size = System::Drawing::Size(82, 20);
			this->CurrentID->TabIndex = 29;
			this->CurrentID->DoubleClick += gcnew System::EventHandler(this, &Form1::CurrentID_DoubleClick);
			// 
			// TalentEffectList_tabControl
			// 
			this->TalentEffectList_tabControl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->TalentEffectList_tabControl->Controls->Add(this->tabPage3);
			this->TalentEffectList_tabControl->Controls->Add(this->tabPage6);
			this->TalentEffectList_tabControl->Location = System::Drawing::Point(6, 95);
			this->TalentEffectList_tabControl->Name = L"TalentEffectList_tabControl";
			this->TalentEffectList_tabControl->SelectedIndex = 0;
			this->TalentEffectList_tabControl->Size = System::Drawing::Size(222, 162);
			this->TalentEffectList_tabControl->TabIndex = 28;
			this->TalentEffectList_tabControl->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::TalentEffectList_tabControl_SelectedIndexChanged);
			// 
			// tabPage3
			// 
			this->tabPage3->Controls->Add(this->TalentEffectTreeView);
			this->tabPage3->Controls->Add(this->btnPasteEffectInfo);
			this->tabPage3->Controls->Add(this->btnCopyEffectInfo);
			this->tabPage3->Controls->Add(this->Delattbutton);
			this->tabPage3->Controls->Add(this->AddEffectInfoBtn);
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Padding = System::Windows::Forms::Padding(3);
			this->tabPage3->Size = System::Drawing::Size(214, 136);
			this->tabPage3->TabIndex = 0;
			this->tabPage3->Text = L"이펙트";
			this->tabPage3->UseVisualStyleBackColor = true;
			// 
			// TalentEffectTreeView
			// 
			this->TalentEffectTreeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->TalentEffectTreeView->Location = System::Drawing::Point(6, 6);
			this->TalentEffectTreeView->Name = L"TalentEffectTreeView";
			this->TalentEffectTreeView->SelectedNodes = (cli::safe_cast<System::Collections::ArrayList^  >(resources->GetObject(L"TalentEffectTreeView.SelectedNodes")));
			this->TalentEffectTreeView->Size = System::Drawing::Size(202, 102);
			this->TalentEffectTreeView->TabIndex = 33;
			this->TalentEffectTreeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &Form1::TalentEffectTreeView_AfterSelect);
			this->TalentEffectTreeView->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::TalentEffectTreeView_KeyDown);
			// 
			// btnPasteEffectInfo
			// 
			this->btnPasteEffectInfo->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->btnPasteEffectInfo->Location = System::Drawing::Point(147, 111);
			this->btnPasteEffectInfo->Name = L"btnPasteEffectInfo";
			this->btnPasteEffectInfo->Size = System::Drawing::Size(50, 23);
			this->btnPasteEffectInfo->TabIndex = 32;
			this->btnPasteEffectInfo->Text = L"붙여넣기";
			this->btnPasteEffectInfo->UseVisualStyleBackColor = true;
			this->btnPasteEffectInfo->Click += gcnew System::EventHandler(this, &Form1::btnPasteEffectInfo_Click);
			// 
			// btnCopyEffectInfo
			// 
			this->btnCopyEffectInfo->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->btnCopyEffectInfo->Location = System::Drawing::Point(98, 111);
			this->btnCopyEffectInfo->Name = L"btnCopyEffectInfo";
			this->btnCopyEffectInfo->Size = System::Drawing::Size(46, 23);
			this->btnCopyEffectInfo->TabIndex = 31;
			this->btnCopyEffectInfo->Text = L"복사";
			this->btnCopyEffectInfo->UseVisualStyleBackColor = true;
			this->btnCopyEffectInfo->Click += gcnew System::EventHandler(this, &Form1::btnCopyEffectInfo_Click);
			// 
			// Delattbutton
			// 
			this->Delattbutton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Delattbutton->Location = System::Drawing::Point(49, 111);
			this->Delattbutton->Name = L"Delattbutton";
			this->Delattbutton->Size = System::Drawing::Size(46, 23);
			this->Delattbutton->TabIndex = 29;
			this->Delattbutton->Text = L"삭제";
			this->Delattbutton->UseVisualStyleBackColor = true;
			this->Delattbutton->Click += gcnew System::EventHandler(this, &Form1::Delattbutton_Click);
			// 
			// AddEffectInfoBtn
			// 
			this->AddEffectInfoBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->AddEffectInfoBtn->Location = System::Drawing::Point(1, 111);
			this->AddEffectInfoBtn->Name = L"AddEffectInfoBtn";
			this->AddEffectInfoBtn->Size = System::Drawing::Size(46, 23);
			this->AddEffectInfoBtn->TabIndex = 28;
			this->AddEffectInfoBtn->Text = L"추가";
			this->AddEffectInfoBtn->UseVisualStyleBackColor = true;
			this->AddEffectInfoBtn->Click += gcnew System::EventHandler(this, &Form1::AddEffectInfoBtn_Click);
			// 
			// tabPage6
			// 
			this->tabPage6->Controls->Add(this->Disable_button);
			this->tabPage6->Controls->Add(this->Enable_button);
			this->tabPage6->Controls->Add(this->HitEffectPosDirtreeView);
			this->tabPage6->Location = System::Drawing::Point(4, 22);
			this->tabPage6->Name = L"tabPage6";
			this->tabPage6->Padding = System::Windows::Forms::Padding(3);
			this->tabPage6->Size = System::Drawing::Size(231, 136);
			this->tabPage6->TabIndex = 1;
			this->tabPage6->Text = L"피격이펙트위치방향";
			this->tabPage6->UseVisualStyleBackColor = true;
			// 
			// Disable_button
			// 
			this->Disable_button->Location = System::Drawing::Point(58, 111);
			this->Disable_button->Name = L"Disable_button";
			this->Disable_button->Size = System::Drawing::Size(65, 23);
			this->Disable_button->TabIndex = 33;
			this->Disable_button->Text = L"사용안함";
			this->Disable_button->UseVisualStyleBackColor = true;
			this->Disable_button->Click += gcnew System::EventHandler(this, &Form1::Disable_button_Click);
			// 
			// Enable_button
			// 
			this->Enable_button->Location = System::Drawing::Point(6, 110);
			this->Enable_button->Name = L"Enable_button";
			this->Enable_button->Size = System::Drawing::Size(46, 23);
			this->Enable_button->TabIndex = 32;
			this->Enable_button->Text = L"사용";
			this->Enable_button->UseVisualStyleBackColor = true;
			this->Enable_button->Click += gcnew System::EventHandler(this, &Form1::Enable_button_Click);
			// 
			// HitEffectPosDirtreeView
			// 
			this->HitEffectPosDirtreeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->HitEffectPosDirtreeView->FullRowSelect = true;
			this->HitEffectPosDirtreeView->Location = System::Drawing::Point(3, 2);
			this->HitEffectPosDirtreeView->Name = L"HitEffectPosDirtreeView";
			this->HitEffectPosDirtreeView->Size = System::Drawing::Size(225, 103);
			this->HitEffectPosDirtreeView->TabIndex = 31;
			this->HitEffectPosDirtreeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &Form1::HitEffectPosDirtreeView_AfterSelect);
			// 
			// label24
			// 
			this->label24->AutoSize = true;
			this->label24->Location = System::Drawing::Point(12, 47);
			this->label24->Name = L"label24";
			this->label24->Size = System::Drawing::Size(18, 13);
			this->label24->TabIndex = 17;
			this->label24->Text = L"ID";
			// 
			// label18
			// 
			this->label18->AutoSize = true;
			this->label18->Location = System::Drawing::Point(10, 72);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(24, 13);
			this->label18->TabIndex = 22;
			this->label18->Text = L"Ref";
			// 
			// RefSelectBtn
			// 
			this->RefSelectBtn->Location = System::Drawing::Point(136, 68);
			this->RefSelectBtn->Name = L"RefSelectBtn";
			this->RefSelectBtn->Size = System::Drawing::Size(25, 21);
			this->RefSelectBtn->TabIndex = 21;
			this->RefSelectBtn->Text = L"...";
			this->RefSelectBtn->UseVisualStyleBackColor = true;
			this->RefSelectBtn->Click += gcnew System::EventHandler(this, &Form1::RefSelectBtn_Click);
			// 
			// RefTextBox
			// 
			this->RefTextBox->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->RefTextBox->ForeColor = System::Drawing::Color::DarkRed;
			this->RefTextBox->Location = System::Drawing::Point(51, 68);
			this->RefTextBox->Name = L"RefTextBox";
			this->RefTextBox->ReadOnly = true;
			this->RefTextBox->Size = System::Drawing::Size(82, 20);
			this->RefTextBox->TabIndex = 20;
			this->RefTextBox->DoubleClick += gcnew System::EventHandler(this, &Form1::RefTextBox_DoubleClick);
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(5, 16);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(170, 21);
			this->button1->TabIndex = 0;
			this->button1->Text = L"Talent Effect Default Setting";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::TalentEffectBase_Click);
			// 
			// groupBox10
			// 
			this->groupBox10->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox10->Controls->Add(this->TalentEffectAttGrid);
			this->groupBox10->Location = System::Drawing::Point(2, 3);
			this->groupBox10->Name = L"groupBox10";
			this->groupBox10->Size = System::Drawing::Size(234, 221);
			this->groupBox10->TabIndex = 20;
			this->groupBox10->TabStop = false;
			this->groupBox10->Text = L"탤런트 이펙트 속성";
			// 
			// TalentEffectAttGrid
			// 
			this->TalentEffectAttGrid->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->TalentEffectAttGrid->Location = System::Drawing::Point(8, 14);
			this->TalentEffectAttGrid->Name = L"TalentEffectAttGrid";
			this->TalentEffectAttGrid->PropertySort = System::Windows::Forms::PropertySort::Categorized;
			this->TalentEffectAttGrid->Size = System::Drawing::Size(217, 201);
			this->TalentEffectAttGrid->TabIndex = 1;
			this->TalentEffectAttGrid->Visible = false;
			this->TalentEffectAttGrid->PropertyValueChanged += gcnew System::Windows::Forms::PropertyValueChangedEventHandler(this, &Form1::TalentEffectAttGrid_PropertyValueChanged);
			// 
			// TalentEvent
			// 
			this->TalentEvent->AutoScroll = true;
			this->TalentEvent->AutoScrollMinSize = System::Drawing::Size(200, 500);
			this->TalentEvent->Location = System::Drawing::Point(4, 70);
			this->TalentEvent->Name = L"TalentEvent";
			this->TalentEvent->Padding = System::Windows::Forms::Padding(3);
			this->TalentEvent->Size = System::Drawing::Size(281, 408);
			this->TalentEvent->TabIndex = 9;
			this->TalentEvent->Text = L"이벤트";
			this->TalentEvent->UseVisualStyleBackColor = true;
			// 
			// PartsJudgment
			// 
			this->PartsJudgment->AutoScroll = true;
			this->PartsJudgment->AutoScrollMinSize = System::Drawing::Size(200, 500);
			this->PartsJudgment->Controls->Add(this->HitTabControl);
			this->PartsJudgment->Location = System::Drawing::Point(4, 70);
			this->PartsJudgment->Name = L"PartsJudgment";
			this->PartsJudgment->Padding = System::Windows::Forms::Padding(3);
			this->PartsJudgment->Size = System::Drawing::Size(281, 408);
			this->PartsJudgment->TabIndex = 10;
			this->PartsJudgment->Text = L"충돌, 판정";
			this->PartsJudgment->UseVisualStyleBackColor = true;
			// 
			// HitTabControl
			// 
			this->HitTabControl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->HitTabControl->Controls->Add(this->CollisionTabPage);
			this->HitTabControl->Controls->Add(this->tabPage5);
			this->HitTabControl->Controls->Add(this->HitEffect_tabPage);
			this->HitTabControl->Location = System::Drawing::Point(6, 6);
			this->HitTabControl->Name = L"HitTabControl";
			this->HitTabControl->SelectedIndex = 0;
			this->HitTabControl->Size = System::Drawing::Size(234, 494);
			this->HitTabControl->TabIndex = 0;
			this->HitTabControl->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::HitTabControl_SelectedIndexChanged);
			// 
			// CollisionTabPage
			// 
			this->CollisionTabPage->Controls->Add(this->CollisionModelName);
			this->CollisionTabPage->Controls->Add(this->label15);
			this->CollisionTabPage->Controls->Add(this->splitContainer3);
			this->CollisionTabPage->Location = System::Drawing::Point(4, 22);
			this->CollisionTabPage->Name = L"CollisionTabPage";
			this->CollisionTabPage->Padding = System::Windows::Forms::Padding(3);
			this->CollisionTabPage->Size = System::Drawing::Size(226, 468);
			this->CollisionTabPage->TabIndex = 0;
			this->CollisionTabPage->Text = L"충돌";
			this->CollisionTabPage->UseVisualStyleBackColor = true;
			// 
			// CollisionModelName
			// 
			this->CollisionModelName->Location = System::Drawing::Point(76, 12);
			this->CollisionModelName->Name = L"CollisionModelName";
			this->CollisionModelName->Size = System::Drawing::Size(115, 20);
			this->CollisionModelName->TabIndex = 3;
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Location = System::Drawing::Point(12, 15);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(58, 13);
			this->label15->TabIndex = 2;
			this->label15->Text = L"모델 이름";
			// 
			// splitContainer3
			// 
			this->splitContainer3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->splitContainer3->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->splitContainer3->Location = System::Drawing::Point(6, 54);
			this->splitContainer3->Name = L"splitContainer3";
			this->splitContainer3->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer3.Panel1
			// 
			this->splitContainer3->Panel1->Controls->Add(this->Add_CollisionBox);
			this->splitContainer3->Panel1->Controls->Add(this->CollisionCapsulesView);
			this->splitContainer3->Panel1->Controls->Add(this->Del_Collision);
			this->splitContainer3->Panel1->Controls->Add(this->Add_CollisionCapsule);
			this->splitContainer3->Panel1->Controls->Add(this->btnMeshColPasteCapsule);
			this->splitContainer3->Panel1->Controls->Add(this->btnMeshColCopyCapsule);
			// 
			// splitContainer3.Panel2
			// 
			this->splitContainer3->Panel2->Controls->Add(this->CollisionCapsuleAtt);
			this->splitContainer3->Size = System::Drawing::Size(214, 408);
			this->splitContainer3->SplitterDistance = 196;
			this->splitContainer3->TabIndex = 7;
			// 
			// Add_CollisionBox
			// 
			this->Add_CollisionBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Add_CollisionBox->Location = System::Drawing::Point(84, 146);
			this->Add_CollisionBox->Name = L"Add_CollisionBox";
			this->Add_CollisionBox->Size = System::Drawing::Size(72, 20);
			this->Add_CollisionBox->TabIndex = 25;
			this->Add_CollisionBox->Text = L"박스추가";
			this->Add_CollisionBox->UseVisualStyleBackColor = true;
			this->Add_CollisionBox->Click += gcnew System::EventHandler(this, &Form1::Add_CollisionBox_Click);
			// 
			// CollisionCapsulesView
			// 
			this->CollisionCapsulesView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->CollisionCapsulesView->Location = System::Drawing::Point(3, 3);
			this->CollisionCapsulesView->Name = L"CollisionCapsulesView";
			this->CollisionCapsulesView->SelectedNodes = (cli::safe_cast<System::Collections::ArrayList^  >(resources->GetObject(L"CollisionCapsulesView.SelectedNodes")));
			this->CollisionCapsulesView->Size = System::Drawing::Size(206, 131);
			this->CollisionCapsulesView->TabIndex = 24;
			this->CollisionCapsulesView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &Form1::MeshColtreeView_AfterSelect);
			this->CollisionCapsulesView->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::CollisionCapsulesView_KeyDown);
			// 
			// Del_Collision
			// 
			this->Del_Collision->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Del_Collision->Location = System::Drawing::Point(165, 170);
			this->Del_Collision->Name = L"Del_Collision";
			this->Del_Collision->Size = System::Drawing::Size(72, 20);
			this->Del_Collision->TabIndex = 2;
			this->Del_Collision->Text = L"삭제";
			this->Del_Collision->UseVisualStyleBackColor = true;
			this->Del_Collision->Click += gcnew System::EventHandler(this, &Form1::Del_CollisionCapsule_Click);
			// 
			// Add_CollisionCapsule
			// 
			this->Add_CollisionCapsule->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Add_CollisionCapsule->Location = System::Drawing::Point(3, 146);
			this->Add_CollisionCapsule->Name = L"Add_CollisionCapsule";
			this->Add_CollisionCapsule->Size = System::Drawing::Size(72, 20);
			this->Add_CollisionCapsule->TabIndex = 1;
			this->Add_CollisionCapsule->Text = L"캡슐추가";
			this->Add_CollisionCapsule->UseVisualStyleBackColor = true;
			this->Add_CollisionCapsule->Click += gcnew System::EventHandler(this, &Form1::Add_CollisionCapsule_Click);
			// 
			// btnMeshColPasteCapsule
			// 
			this->btnMeshColPasteCapsule->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->btnMeshColPasteCapsule->Location = System::Drawing::Point(84, 171);
			this->btnMeshColPasteCapsule->Name = L"btnMeshColPasteCapsule";
			this->btnMeshColPasteCapsule->Size = System::Drawing::Size(72, 20);
			this->btnMeshColPasteCapsule->TabIndex = 22;
			this->btnMeshColPasteCapsule->Text = L"붙여넣기";
			this->btnMeshColPasteCapsule->UseVisualStyleBackColor = true;
			this->btnMeshColPasteCapsule->Click += gcnew System::EventHandler(this, &Form1::btnMeshColPasteCapsule_Click);
			// 
			// btnMeshColCopyCapsule
			// 
			this->btnMeshColCopyCapsule->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->btnMeshColCopyCapsule->Location = System::Drawing::Point(3, 171);
			this->btnMeshColCopyCapsule->Name = L"btnMeshColCopyCapsule";
			this->btnMeshColCopyCapsule->Size = System::Drawing::Size(72, 20);
			this->btnMeshColCopyCapsule->TabIndex = 21;
			this->btnMeshColCopyCapsule->Text = L"복사";
			this->btnMeshColCopyCapsule->UseVisualStyleBackColor = true;
			this->btnMeshColCopyCapsule->Click += gcnew System::EventHandler(this, &Form1::btnMeshColCopyCapsule_Click);
			// 
			// CollisionCapsuleAtt
			// 
			this->CollisionCapsuleAtt->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->CollisionCapsuleAtt->Location = System::Drawing::Point(3, 3);
			this->CollisionCapsuleAtt->Name = L"CollisionCapsuleAtt";
			this->CollisionCapsuleAtt->PropertySort = System::Windows::Forms::PropertySort::Categorized;
			this->CollisionCapsuleAtt->Size = System::Drawing::Size(206, 200);
			this->CollisionCapsuleAtt->TabIndex = 3;
			this->CollisionCapsuleAtt->PropertyValueChanged += gcnew System::Windows::Forms::PropertyValueChangedEventHandler(this, &Form1::MeshColCapsule_PropertyValueChanged);
			// 
			// tabPage5
			// 
			this->tabPage5->Controls->Add(this->splitContainer4);
			this->tabPage5->Controls->Add(this->PartsHitModelName);
			this->tabPage5->Controls->Add(this->label10);
			this->tabPage5->Location = System::Drawing::Point(4, 22);
			this->tabPage5->Name = L"tabPage5";
			this->tabPage5->Padding = System::Windows::Forms::Padding(3);
			this->tabPage5->Size = System::Drawing::Size(243, 468);
			this->tabPage5->TabIndex = 1;
			this->tabPage5->Text = L"판정";
			this->tabPage5->UseVisualStyleBackColor = true;
			// 
			// splitContainer4
			// 
			this->splitContainer4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->splitContainer4->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->splitContainer4->Location = System::Drawing::Point(3, 39);
			this->splitContainer4->Name = L"splitContainer4";
			this->splitContainer4->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer4.Panel1
			// 
			this->splitContainer4->Panel1->Controls->Add(this->showHitEffectCapsule_checkBox);
			this->splitContainer4->Panel1->Controls->Add(this->PartsHitCapsulesView);
			this->splitContainer4->Panel1->Controls->Add(this->btnMeshHitCapsulePaste);
			this->splitContainer4->Panel1->Controls->Add(this->btnMeshHitCapsuleCopy);
			this->splitContainer4->Panel1->Controls->Add(this->Add_PartsCapsule);
			this->splitContainer4->Panel1->Controls->Add(this->Add_PartsGroup);
			this->splitContainer4->Panel1->Controls->Add(this->Del_PartsCapsule);
			// 
			// splitContainer4.Panel2
			// 
			this->splitContainer4->Panel2->Controls->Add(this->PartsHitCapsuleAtt);
			this->splitContainer4->Size = System::Drawing::Size(236, 426);
			this->splitContainer4->SplitterDistance = 214;
			this->splitContainer4->TabIndex = 6;
			// 
			// showHitEffectCapsule_checkBox
			// 
			this->showHitEffectCapsule_checkBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->showHitEffectCapsule_checkBox->AutoSize = true;
			this->showHitEffectCapsule_checkBox->Font = (gcnew System::Drawing::Font(L"Gulim", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->showHitEffectCapsule_checkBox->Location = System::Drawing::Point(3, 196);
			this->showHitEffectCapsule_checkBox->Name = L"showHitEffectCapsule_checkBox";
			this->showHitEffectCapsule_checkBox->Size = System::Drawing::Size(168, 15);
			this->showHitEffectCapsule_checkBox->TabIndex = 26;
			this->showHitEffectCapsule_checkBox->Text = L"연결된 히트 이펙트 보여주기";
			this->showHitEffectCapsule_checkBox->UseVisualStyleBackColor = true;
			this->showHitEffectCapsule_checkBox->CheckedChanged += gcnew System::EventHandler(this, &Form1::showHitEffectCapsule_checkBox_CheckedChanged);
			// 
			// PartsHitCapsulesView
			// 
			this->PartsHitCapsulesView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->PartsHitCapsulesView->Location = System::Drawing::Point(3, -10);
			this->PartsHitCapsulesView->Name = L"PartsHitCapsulesView";
			this->PartsHitCapsulesView->SelectedNodes = (cli::safe_cast<System::Collections::ArrayList^  >(resources->GetObject(L"PartsHitCapsulesView.SelectedNodes")));
			this->PartsHitCapsulesView->Size = System::Drawing::Size(228, 151);
			this->PartsHitCapsulesView->TabIndex = 25;
			this->PartsHitCapsulesView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &Form1::PartsHittreeView_AfterSelect);
			this->PartsHitCapsulesView->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::PartsHitCapsulesView_KeyDown);
			// 
			// btnMeshHitCapsulePaste
			// 
			this->btnMeshHitCapsulePaste->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->btnMeshHitCapsulePaste->Location = System::Drawing::Point(79, 171);
			this->btnMeshHitCapsulePaste->Name = L"btnMeshHitCapsulePaste";
			this->btnMeshHitCapsulePaste->Size = System::Drawing::Size(70, 21);
			this->btnMeshHitCapsulePaste->TabIndex = 24;
			this->btnMeshHitCapsulePaste->Text = L"붙여넣기";
			this->btnMeshHitCapsulePaste->UseVisualStyleBackColor = true;
			this->btnMeshHitCapsulePaste->Click += gcnew System::EventHandler(this, &Form1::btnMeshHitCapsulePaste_Click);
			// 
			// btnMeshHitCapsuleCopy
			// 
			this->btnMeshHitCapsuleCopy->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->btnMeshHitCapsuleCopy->Location = System::Drawing::Point(3, 171);
			this->btnMeshHitCapsuleCopy->Name = L"btnMeshHitCapsuleCopy";
			this->btnMeshHitCapsuleCopy->Size = System::Drawing::Size(70, 21);
			this->btnMeshHitCapsuleCopy->TabIndex = 23;
			this->btnMeshHitCapsuleCopy->Text = L"복사";
			this->btnMeshHitCapsuleCopy->UseVisualStyleBackColor = true;
			this->btnMeshHitCapsuleCopy->Click += gcnew System::EventHandler(this, &Form1::btnMeshHitCapsuleCopy_Click);
			// 
			// Add_PartsCapsule
			// 
			this->Add_PartsCapsule->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Add_PartsCapsule->Location = System::Drawing::Point(79, 144);
			this->Add_PartsCapsule->Name = L"Add_PartsCapsule";
			this->Add_PartsCapsule->Size = System::Drawing::Size(64, 21);
			this->Add_PartsCapsule->TabIndex = 1;
			this->Add_PartsCapsule->Text = L"캡슐추가";
			this->Add_PartsCapsule->UseVisualStyleBackColor = true;
			this->Add_PartsCapsule->Click += gcnew System::EventHandler(this, &Form1::Add_PartsCapsule_Click);
			// 
			// Add_PartsGroup
			// 
			this->Add_PartsGroup->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Add_PartsGroup->Location = System::Drawing::Point(3, 144);
			this->Add_PartsGroup->Name = L"Add_PartsGroup";
			this->Add_PartsGroup->Size = System::Drawing::Size(70, 21);
			this->Add_PartsGroup->TabIndex = 4;
			this->Add_PartsGroup->Text = L"그룹추가";
			this->Add_PartsGroup->UseVisualStyleBackColor = true;
			this->Add_PartsGroup->Click += gcnew System::EventHandler(this, &Form1::Add_PartsGroup_Click);
			// 
			// Del_PartsCapsule
			// 
			this->Del_PartsCapsule->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Del_PartsCapsule->Location = System::Drawing::Point(149, 144);
			this->Del_PartsCapsule->Name = L"Del_PartsCapsule";
			this->Del_PartsCapsule->Size = System::Drawing::Size(42, 21);
			this->Del_PartsCapsule->TabIndex = 2;
			this->Del_PartsCapsule->Text = L"삭제";
			this->Del_PartsCapsule->UseVisualStyleBackColor = true;
			this->Del_PartsCapsule->Click += gcnew System::EventHandler(this, &Form1::Del_PartsCapsule_Click);
			// 
			// PartsHitCapsuleAtt
			// 
			this->PartsHitCapsuleAtt->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->PartsHitCapsuleAtt->Location = System::Drawing::Point(2, 3);
			this->PartsHitCapsuleAtt->Name = L"PartsHitCapsuleAtt";
			this->PartsHitCapsuleAtt->PropertySort = System::Windows::Forms::PropertySort::Categorized;
			this->PartsHitCapsuleAtt->Size = System::Drawing::Size(231, 201);
			this->PartsHitCapsuleAtt->TabIndex = 3;
			this->PartsHitCapsuleAtt->PropertyValueChanged += gcnew System::Windows::Forms::PropertyValueChangedEventHandler(this, &Form1::PartsHitCapsule_PropertyValueChanged);
			// 
			// PartsHitModelName
			// 
			this->PartsHitModelName->Location = System::Drawing::Point(76, 12);
			this->PartsHitModelName->Name = L"PartsHitModelName";
			this->PartsHitModelName->Size = System::Drawing::Size(115, 20);
			this->PartsHitModelName->TabIndex = 1;
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(12, 15);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(58, 13);
			this->label10->TabIndex = 0;
			this->label10->Text = L"모델 이름";
			// 
			// HitEffect_tabPage
			// 
			this->HitEffect_tabPage->Controls->Add(this->splitContainer5);
			this->HitEffect_tabPage->Controls->Add(this->HitEffectModelName);
			this->HitEffect_tabPage->Controls->Add(this->label1);
			this->HitEffect_tabPage->Location = System::Drawing::Point(4, 22);
			this->HitEffect_tabPage->Name = L"HitEffect_tabPage";
			this->HitEffect_tabPage->Size = System::Drawing::Size(226, 468);
			this->HitEffect_tabPage->TabIndex = 2;
			this->HitEffect_tabPage->Text = L"판정이펙트";
			this->HitEffect_tabPage->UseVisualStyleBackColor = true;
			// 
			// splitContainer5
			// 
			this->splitContainer5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->splitContainer5->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->splitContainer5->Location = System::Drawing::Point(4, 39);
			this->splitContainer5->Name = L"splitContainer5";
			this->splitContainer5->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer5.Panel1
			// 
			this->splitContainer5->Panel1->Controls->Add(this->HitEffect_treeView);
			this->splitContainer5->Panel1->Controls->Add(this->capsule_Paste_button);
			this->splitContainer5->Panel1->Controls->Add(this->capsule_Copy_button);
			this->splitContainer5->Panel1->Controls->Add(this->capsule_Add_button);
			this->splitContainer5->Panel1->Controls->Add(this->capsule_Del_button);
			// 
			// splitContainer5.Panel2
			// 
			this->splitContainer5->Panel2->Controls->Add(this->HitEffect_propertyGrid);
			this->splitContainer5->Size = System::Drawing::Size(218, 425);
			this->splitContainer5->SplitterDistance = 207;
			this->splitContainer5->TabIndex = 7;
			// 
			// HitEffect_treeView
			// 
			this->HitEffect_treeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->HitEffect_treeView->Location = System::Drawing::Point(3, 3);
			this->HitEffect_treeView->Name = L"HitEffect_treeView";
			this->HitEffect_treeView->SelectedNodes = (cli::safe_cast<System::Collections::ArrayList^  >(resources->GetObject(L"HitEffect_treeView.SelectedNodes")));
			this->HitEffect_treeView->Size = System::Drawing::Size(210, 145);
			this->HitEffect_treeView->TabIndex = 25;
			this->HitEffect_treeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &Form1::HitEffect_treeView_AfterSelect);
			// 
			// capsule_Paste_button
			// 
			this->capsule_Paste_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->capsule_Paste_button->Location = System::Drawing::Point(79, 181);
			this->capsule_Paste_button->Name = L"capsule_Paste_button";
			this->capsule_Paste_button->Size = System::Drawing::Size(70, 21);
			this->capsule_Paste_button->TabIndex = 24;
			this->capsule_Paste_button->Text = L"붙여넣기";
			this->capsule_Paste_button->UseVisualStyleBackColor = true;
			this->capsule_Paste_button->Click += gcnew System::EventHandler(this, &Form1::capsule_Paste_button_Click);
			// 
			// capsule_Copy_button
			// 
			this->capsule_Copy_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->capsule_Copy_button->Location = System::Drawing::Point(3, 181);
			this->capsule_Copy_button->Name = L"capsule_Copy_button";
			this->capsule_Copy_button->Size = System::Drawing::Size(70, 21);
			this->capsule_Copy_button->TabIndex = 23;
			this->capsule_Copy_button->Text = L"복사";
			this->capsule_Copy_button->UseVisualStyleBackColor = true;
			this->capsule_Copy_button->Click += gcnew System::EventHandler(this, &Form1::capsule_Copy_button_Click);
			// 
			// capsule_Add_button
			// 
			this->capsule_Add_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->capsule_Add_button->Location = System::Drawing::Point(3, 154);
			this->capsule_Add_button->Name = L"capsule_Add_button";
			this->capsule_Add_button->Size = System::Drawing::Size(70, 21);
			this->capsule_Add_button->TabIndex = 1;
			this->capsule_Add_button->Text = L"캡슐추가";
			this->capsule_Add_button->UseVisualStyleBackColor = true;
			this->capsule_Add_button->Click += gcnew System::EventHandler(this, &Form1::capsule_Add_button_Click);
			// 
			// capsule_Del_button
			// 
			this->capsule_Del_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->capsule_Del_button->Location = System::Drawing::Point(79, 154);
			this->capsule_Del_button->Name = L"capsule_Del_button";
			this->capsule_Del_button->Size = System::Drawing::Size(70, 21);
			this->capsule_Del_button->TabIndex = 2;
			this->capsule_Del_button->Text = L"캡슐삭제";
			this->capsule_Del_button->UseVisualStyleBackColor = true;
			this->capsule_Del_button->Click += gcnew System::EventHandler(this, &Form1::capsule_Del_button_Click);
			// 
			// HitEffect_propertyGrid
			// 
			this->HitEffect_propertyGrid->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->HitEffect_propertyGrid->Location = System::Drawing::Point(1, 3);
			this->HitEffect_propertyGrid->Name = L"HitEffect_propertyGrid";
			this->HitEffect_propertyGrid->PropertySort = System::Windows::Forms::PropertySort::Categorized;
			this->HitEffect_propertyGrid->Size = System::Drawing::Size(212, 206);
			this->HitEffect_propertyGrid->TabIndex = 3;
			this->HitEffect_propertyGrid->PropertyValueChanged += gcnew System::Windows::Forms::PropertyValueChangedEventHandler(this, &Form1::HitEffect_propertyGrid_PropertyValueChanged);
			// 
			// HitEffectModelName
			// 
			this->HitEffectModelName->Location = System::Drawing::Point(76, 12);
			this->HitEffectModelName->Name = L"HitEffectModelName";
			this->HitEffectModelName->Size = System::Drawing::Size(115, 20);
			this->HitEffectModelName->TabIndex = 3;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 15);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(58, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"모델 이름";
			// 
			// SubMtrltab
			// 
			this->SubMtrltab->AutoScroll = true;
			this->SubMtrltab->AutoScrollMinSize = System::Drawing::Size(200, 500);
			this->SubMtrltab->Controls->Add(this->buttonDel);
			this->SubMtrltab->Controls->Add(this->buttonApply);
			this->SubMtrltab->Controls->Add(this->listBoxMtrlList);
			this->SubMtrltab->Controls->Add(this->groupBox17);
			this->SubMtrltab->Controls->Add(this->groupBoxExport);
			this->SubMtrltab->Location = System::Drawing::Point(4, 70);
			this->SubMtrltab->Name = L"SubMtrltab";
			this->SubMtrltab->Padding = System::Windows::Forms::Padding(3);
			this->SubMtrltab->Size = System::Drawing::Size(281, 408);
			this->SubMtrltab->TabIndex = 11;
			this->SubMtrltab->Text = L"재질 조합";
			this->SubMtrltab->UseVisualStyleBackColor = true;
			// 
			// buttonDel
			// 
			this->buttonDel->Location = System::Drawing::Point(135, 408);
			this->buttonDel->Name = L"buttonDel";
			this->buttonDel->Size = System::Drawing::Size(109, 28);
			this->buttonDel->TabIndex = 7;
			this->buttonDel->Text = L"-";
			this->buttonDel->UseVisualStyleBackColor = true;
			// 
			// buttonApply
			// 
			this->buttonApply->Location = System::Drawing::Point(17, 408);
			this->buttonApply->Name = L"buttonApply";
			this->buttonApply->Size = System::Drawing::Size(109, 28);
			this->buttonApply->TabIndex = 6;
			this->buttonApply->Text = L"Apply";
			this->buttonApply->UseVisualStyleBackColor = true;
			this->buttonApply->Click += gcnew System::EventHandler(this, &Form1::buttonApply_Click);
			// 
			// listBoxMtrlList
			// 
			this->listBoxMtrlList->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->listBoxMtrlList->FormattingEnabled = true;
			this->listBoxMtrlList->Location = System::Drawing::Point(15, 316);
			this->listBoxMtrlList->Name = L"listBoxMtrlList";
			this->listBoxMtrlList->Size = System::Drawing::Size(183, 30);
			this->listBoxMtrlList->TabIndex = 5;
			this->listBoxMtrlList->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::listBoxMtrlList_MouseDoubleClick);
			// 
			// groupBox17
			// 
			this->groupBox17->Controls->Add(this->groupBoxMtrlList);
			this->groupBox17->Controls->Add(this->textBoxCurrentMtrl);
			this->groupBox17->Controls->Add(this->treeViewMeshList);
			this->groupBox17->Location = System::Drawing::Point(2, 4);
			this->groupBox17->Name = L"groupBox17";
			this->groupBox17->Size = System::Drawing::Size(258, 447);
			this->groupBox17->TabIndex = 7;
			this->groupBox17->TabStop = false;
			this->groupBox17->Text = L"Material TreeView";
			// 
			// groupBoxMtrlList
			// 
			this->groupBoxMtrlList->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBoxMtrlList->Location = System::Drawing::Point(6, 295);
			this->groupBoxMtrlList->Name = L"groupBoxMtrlList";
			this->groupBoxMtrlList->Size = System::Drawing::Size(240, 143);
			this->groupBoxMtrlList->TabIndex = 2;
			this->groupBoxMtrlList->TabStop = false;
			this->groupBoxMtrlList->Text = L"Sub Material List";
			// 
			// textBoxCurrentMtrl
			// 
			this->textBoxCurrentMtrl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBoxCurrentMtrl->Location = System::Drawing::Point(13, 264);
			this->textBoxCurrentMtrl->Name = L"textBoxCurrentMtrl";
			this->textBoxCurrentMtrl->ReadOnly = true;
			this->textBoxCurrentMtrl->Size = System::Drawing::Size(222, 20);
			this->textBoxCurrentMtrl->TabIndex = 1;
			// 
			// treeViewMeshList
			// 
			this->treeViewMeshList->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->treeViewMeshList->HideSelection = false;
			this->treeViewMeshList->Location = System::Drawing::Point(13, 21);
			this->treeViewMeshList->Name = L"treeViewMeshList";
			this->treeViewMeshList->Size = System::Drawing::Size(231, 237);
			this->treeViewMeshList->TabIndex = 0;
			this->treeViewMeshList->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &Form1::treeViewMeshList_AfterSelect);
			// 
			// groupBoxExport
			// 
			this->groupBoxExport->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBoxExport->Controls->Add(this->buttonExport);
			this->groupBoxExport->Location = System::Drawing::Point(3, 441);
			this->groupBoxExport->Name = L"groupBoxExport";
			this->groupBoxExport->Size = System::Drawing::Size(223, 62);
			this->groupBoxExport->TabIndex = 5;
			this->groupBoxExport->TabStop = false;
			this->groupBoxExport->Text = L"ExportToXML";
			// 
			// buttonExport
			// 
			this->buttonExport->Location = System::Drawing::Point(68, 23);
			this->buttonExport->Name = L"buttonExport";
			this->buttonExport->Size = System::Drawing::Size(112, 28);
			this->buttonExport->TabIndex = 6;
			this->buttonExport->Text = L"Export";
			this->buttonExport->UseVisualStyleBackColor = true;
			this->buttonExport->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::buttonExport_MouseDown);
			// 
			// MeshInfotab
			// 
			this->MeshInfotab->AutoScroll = true;
			this->MeshInfotab->AutoScrollMinSize = System::Drawing::Size(200, 500);
			this->MeshInfotab->Location = System::Drawing::Point(4, 70);
			this->MeshInfotab->Name = L"MeshInfotab";
			this->MeshInfotab->Size = System::Drawing::Size(281, 408);
			this->MeshInfotab->TabIndex = 12;
			this->MeshInfotab->Text = L"메쉬정보";
			this->MeshInfotab->UseVisualStyleBackColor = true;
			// 
			// NPCcontextMenuStrip
			// 
			this->NPCcontextMenuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->COLTEDitToolStripMenuItem, 
				this->ScriptEditToolStripMenuItem});
			this->NPCcontextMenuStrip->Name = L"NPCcontextMenuStrip";
			this->NPCcontextMenuStrip->Size = System::Drawing::Size(150, 48);
			// 
			// COLTEDitToolStripMenuItem
			// 
			this->COLTEDitToolStripMenuItem->Name = L"COLTEDitToolStripMenuItem";
			this->COLTEDitToolStripMenuItem->Size = System::Drawing::Size(149, 22);
			this->COLTEDitToolStripMenuItem->Text = L"COLT 편집";
			this->COLTEDitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::COLTEDitToolStripMenuItem_Click);
			// 
			// ScriptEditToolStripMenuItem
			// 
			this->ScriptEditToolStripMenuItem->Name = L"ScriptEditToolStripMenuItem";
			this->ScriptEditToolStripMenuItem->Size = System::Drawing::Size(149, 22);
			this->ScriptEditToolStripMenuItem->Text = L"스크립트 편집";
			this->ScriptEditToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::ScriptEditToolStripMenuItem_Click);
			// 
			// contextMenuStrip2
			// 
			this->contextMenuStrip2->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->ItemSlot_toolStripMenuItem, 
				this->UnEquipToolStripMenuItem});
			this->contextMenuStrip2->Name = L"contextMenuStrip2";
			this->contextMenuStrip2->Size = System::Drawing::Size(159, 48);
			// 
			// ItemSlot_toolStripMenuItem
			// 
			this->ItemSlot_toolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(16) {this->SlotHead_toolStripMenuItem, 
				this->SlotFace_ToolStripMenuItem, this->SlotHands_ToolStripMenuItem, this->SlotFeet_ToolStripMenuItem, this->SlotBody_ToolStripMenuItem, 
				this->SlotLeg_ToolStripMenuItem, this->SlotLfinger_ToolStripMenuItem, this->SlotRfinger_ToolStripMenuItem, this->SlotNeck_ToolStripMenuItem, 
				this->SlotCharm_ToolStripMenuItem, this->SlotLweapon_ToolStripMenuItem, this->SlotRweapon_ToolStripMenuItem, this->SlotReserved1_ToolStripMenuItem, 
				this->SlotReserved2_ToolStripMenuItem, this->SlotReserved3_ToolStripMenuItem, this->SlotReserved4_ToolStripMenuItem});
			this->ItemSlot_toolStripMenuItem->Name = L"ItemSlot_toolStripMenuItem";
			this->ItemSlot_toolStripMenuItem->Size = System::Drawing::Size(158, 22);
			this->ItemSlot_toolStripMenuItem->Text = L"아이템슬롯결정";
			// 
			// SlotHead_toolStripMenuItem
			// 
			this->SlotHead_toolStripMenuItem->Name = L"SlotHead_toolStripMenuItem";
			this->SlotHead_toolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotHead_toolStripMenuItem->Text = L"HEAD";
			this->SlotHead_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// SlotFace_ToolStripMenuItem
			// 
			this->SlotFace_ToolStripMenuItem->Name = L"SlotFace_ToolStripMenuItem";
			this->SlotFace_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotFace_ToolStripMenuItem->Text = L"FACE";
			this->SlotFace_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// SlotHands_ToolStripMenuItem
			// 
			this->SlotHands_ToolStripMenuItem->Name = L"SlotHands_ToolStripMenuItem";
			this->SlotHands_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotHands_ToolStripMenuItem->Text = L"HANDS";
			this->SlotHands_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// SlotFeet_ToolStripMenuItem
			// 
			this->SlotFeet_ToolStripMenuItem->Name = L"SlotFeet_ToolStripMenuItem";
			this->SlotFeet_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotFeet_ToolStripMenuItem->Text = L"FEET";
			this->SlotFeet_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// SlotBody_ToolStripMenuItem
			// 
			this->SlotBody_ToolStripMenuItem->Name = L"SlotBody_ToolStripMenuItem";
			this->SlotBody_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotBody_ToolStripMenuItem->Text = L"BODY";
			this->SlotBody_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// SlotLeg_ToolStripMenuItem
			// 
			this->SlotLeg_ToolStripMenuItem->Name = L"SlotLeg_ToolStripMenuItem";
			this->SlotLeg_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotLeg_ToolStripMenuItem->Text = L"LEG";
			this->SlotLeg_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// SlotLfinger_ToolStripMenuItem
			// 
			this->SlotLfinger_ToolStripMenuItem->Name = L"SlotLfinger_ToolStripMenuItem";
			this->SlotLfinger_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotLfinger_ToolStripMenuItem->Text = L"LFINGER";
			this->SlotLfinger_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// SlotRfinger_ToolStripMenuItem
			// 
			this->SlotRfinger_ToolStripMenuItem->Name = L"SlotRfinger_ToolStripMenuItem";
			this->SlotRfinger_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotRfinger_ToolStripMenuItem->Text = L"RFINGER";
			this->SlotRfinger_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// SlotNeck_ToolStripMenuItem
			// 
			this->SlotNeck_ToolStripMenuItem->Name = L"SlotNeck_ToolStripMenuItem";
			this->SlotNeck_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotNeck_ToolStripMenuItem->Text = L"NECK";
			this->SlotNeck_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// SlotCharm_ToolStripMenuItem
			// 
			this->SlotCharm_ToolStripMenuItem->Name = L"SlotCharm_ToolStripMenuItem";
			this->SlotCharm_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotCharm_ToolStripMenuItem->Text = L"CHARM";
			this->SlotCharm_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// SlotLweapon_ToolStripMenuItem
			// 
			this->SlotLweapon_ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(17) {this->LW_loc_toolStripMenuItem, 
				this->LW_1hs_l_toolStripMenuItem, this->LW_1hs_r_toolStripMenuItem, this->LW_1hb_l_toolStripMenuItem, this->LW_1hb_r_toolStripMenuItem, 
				this->LW_1hp_l_toolStripMenuItem, this->LW_1hp_r_toolStripMenuItem, this->LW_2hd_toolStripMenuItem, this->LW_pol_toolStripMenuItem, 
				this->LW_2hs_toolStripMenuItem, this->LW_2hp_toolStripMenuItem, this->LW_arc_toolStripMenuItem, this->LW_gpd_toolStripMenuItem, 
				this->LW_shield_toolStripMenuItem, this->LW_lh_toolStripMenuItem, this->LW_arrow_toolStripMenuItem, LW_quiver_toolStripMenuItem});
			this->SlotLweapon_ToolStripMenuItem->Name = L"SlotLweapon_ToolStripMenuItem";
			this->SlotLweapon_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotLweapon_ToolStripMenuItem->Text = L"LWEAPON";
			this->SlotLweapon_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// LW_loc_toolStripMenuItem
			// 
			this->LW_loc_toolStripMenuItem->Name = L"LW_loc_toolStripMenuItem";
			this->LW_loc_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_loc_toolStripMenuItem->Text = L"Dummy_loc";
			this->LW_loc_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_1hs_l_toolStripMenuItem
			// 
			this->LW_1hs_l_toolStripMenuItem->Name = L"LW_1hs_l_toolStripMenuItem";
			this->LW_1hs_l_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_1hs_l_toolStripMenuItem->Text = L"Dummy_1hs_l";
			this->LW_1hs_l_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_1hs_r_toolStripMenuItem
			// 
			this->LW_1hs_r_toolStripMenuItem->Name = L"LW_1hs_r_toolStripMenuItem";
			this->LW_1hs_r_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_1hs_r_toolStripMenuItem->Text = L"Dummy_1hs_r";
			this->LW_1hs_r_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_1hb_l_toolStripMenuItem
			// 
			this->LW_1hb_l_toolStripMenuItem->Name = L"LW_1hb_l_toolStripMenuItem";
			this->LW_1hb_l_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_1hb_l_toolStripMenuItem->Text = L"Dummy_1hb_l";
			this->LW_1hb_l_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_1hb_r_toolStripMenuItem
			// 
			this->LW_1hb_r_toolStripMenuItem->Name = L"LW_1hb_r_toolStripMenuItem";
			this->LW_1hb_r_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_1hb_r_toolStripMenuItem->Text = L"Dummy_1hb_r";
			this->LW_1hb_r_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_1hp_l_toolStripMenuItem
			// 
			this->LW_1hp_l_toolStripMenuItem->Name = L"LW_1hp_l_toolStripMenuItem";
			this->LW_1hp_l_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_1hp_l_toolStripMenuItem->Text = L"Dummy_1hp_l";
			this->LW_1hp_l_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_1hp_r_toolStripMenuItem
			// 
			this->LW_1hp_r_toolStripMenuItem->Name = L"LW_1hp_r_toolStripMenuItem";
			this->LW_1hp_r_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_1hp_r_toolStripMenuItem->Text = L"Dummy_1hp_r";
			this->LW_1hp_r_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_2hd_toolStripMenuItem
			// 
			this->LW_2hd_toolStripMenuItem->Name = L"LW_2hd_toolStripMenuItem";
			this->LW_2hd_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_2hd_toolStripMenuItem->Text = L"Dummy_2hd";
			this->LW_2hd_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_pol_toolStripMenuItem
			// 
			this->LW_pol_toolStripMenuItem->Name = L"LW_pol_toolStripMenuItem";
			this->LW_pol_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_pol_toolStripMenuItem->Text = L"Dummy_pol";
			this->LW_pol_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_2hs_toolStripMenuItem
			// 
			this->LW_2hs_toolStripMenuItem->Name = L"LW_2hs_toolStripMenuItem";
			this->LW_2hs_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_2hs_toolStripMenuItem->Text = L"Dummy_2hs";
			this->LW_2hs_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_2hp_toolStripMenuItem
			// 
			this->LW_2hp_toolStripMenuItem->Name = L"LW_2hp_toolStripMenuItem";
			this->LW_2hp_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_2hp_toolStripMenuItem->Text = L"Dummy_2hp";
			this->LW_2hp_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_arc_toolStripMenuItem
			// 
			this->LW_arc_toolStripMenuItem->Name = L"LW_arc_toolStripMenuItem";
			this->LW_arc_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_arc_toolStripMenuItem->Text = L"Dummy_arc";
			this->LW_arc_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_gpd_toolStripMenuItem
			// 
			this->LW_gpd_toolStripMenuItem->Name = L"LW_gpd_toolStripMenuItem";
			this->LW_gpd_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_gpd_toolStripMenuItem->Text = L"Dummy_gpd";
			this->LW_gpd_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_shield_toolStripMenuItem
			// 
			this->LW_shield_toolStripMenuItem->Name = L"LW_shield_toolStripMenuItem";
			this->LW_shield_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_shield_toolStripMenuItem->Text = L"Dummy_shield";
			this->LW_shield_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_lh_toolStripMenuItem
			// 
			this->LW_lh_toolStripMenuItem->Name = L"LW_lh_toolStripMenuItem";
			this->LW_lh_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_lh_toolStripMenuItem->Text = L"Dummy_lh";
			this->LW_lh_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// LW_arrow_toolStripMenuItem
			// 
			this->LW_arrow_toolStripMenuItem->Name = L"LW_arrow_toolStripMenuItem";
			this->LW_arrow_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->LW_arrow_toolStripMenuItem->Text = L"Dummy_arrow";
			this->LW_arrow_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::LW_loc_toolStripMenuItem_Click);
			// 
			// SlotRweapon_ToolStripMenuItem
			// 
			this->SlotRweapon_ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(17) {this->RW_loc_toolStripMenuItem, 
				this->toolStripMenuItem21, this->toolStripMenuItem22, this->toolStripMenuItem23, this->toolStripMenuItem24, this->toolStripMenuItem25, 
				this->toolStripMenuItem26, this->toolStripMenuItem27, this->toolStripMenuItem28, this->toolStripMenuItem29, this->toolStripMenuItem30, 
				this->toolStripMenuItem31, this->toolStripMenuItem32, this->toolStripMenuItem33, this->toolStripMenuItem34, this->toolStripMenuItem35, 
				this->toolStripMenuItem36});
			this->SlotRweapon_ToolStripMenuItem->Name = L"SlotRweapon_ToolStripMenuItem";
			this->SlotRweapon_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotRweapon_ToolStripMenuItem->Text = L"RWEAPON";
			this->SlotRweapon_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// RW_loc_toolStripMenuItem
			// 
			this->RW_loc_toolStripMenuItem->Name = L"RW_loc_toolStripMenuItem";
			this->RW_loc_toolStripMenuItem->Size = System::Drawing::Size(155, 22);
			this->RW_loc_toolStripMenuItem->Text = L"Dummy_loc";
			this->RW_loc_toolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem21
			// 
			this->toolStripMenuItem21->Name = L"toolStripMenuItem21";
			this->toolStripMenuItem21->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem21->Text = L"Dummy_1hs_l";
			this->toolStripMenuItem21->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem22
			// 
			this->toolStripMenuItem22->Name = L"toolStripMenuItem22";
			this->toolStripMenuItem22->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem22->Text = L"Dummy_1hs_r";
			this->toolStripMenuItem22->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem23
			// 
			this->toolStripMenuItem23->Name = L"toolStripMenuItem23";
			this->toolStripMenuItem23->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem23->Text = L"Dummy_1hb_l";
			this->toolStripMenuItem23->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem24
			// 
			this->toolStripMenuItem24->Name = L"toolStripMenuItem24";
			this->toolStripMenuItem24->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem24->Text = L"Dummy_1hb_r";
			this->toolStripMenuItem24->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem25
			// 
			this->toolStripMenuItem25->Name = L"toolStripMenuItem25";
			this->toolStripMenuItem25->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem25->Text = L"Dummy_1hp_l";
			this->toolStripMenuItem25->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem26
			// 
			this->toolStripMenuItem26->Name = L"toolStripMenuItem26";
			this->toolStripMenuItem26->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem26->Text = L"Dummy_1hp_r";
			this->toolStripMenuItem26->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem27
			// 
			this->toolStripMenuItem27->Name = L"toolStripMenuItem27";
			this->toolStripMenuItem27->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem27->Text = L"Dummy_2hd";
			this->toolStripMenuItem27->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem28
			// 
			this->toolStripMenuItem28->Name = L"toolStripMenuItem28";
			this->toolStripMenuItem28->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem28->Text = L"Dummy_pol";
			this->toolStripMenuItem28->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem29
			// 
			this->toolStripMenuItem29->Name = L"toolStripMenuItem29";
			this->toolStripMenuItem29->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem29->Text = L"Dummy_2hs";
			this->toolStripMenuItem29->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem30
			// 
			this->toolStripMenuItem30->Name = L"toolStripMenuItem30";
			this->toolStripMenuItem30->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem30->Text = L"Dummy_2hp";
			this->toolStripMenuItem30->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem31
			// 
			this->toolStripMenuItem31->Name = L"toolStripMenuItem31";
			this->toolStripMenuItem31->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem31->Text = L"Dummy_arc";
			this->toolStripMenuItem31->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem32
			// 
			this->toolStripMenuItem32->Name = L"toolStripMenuItem32";
			this->toolStripMenuItem32->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem32->Text = L"Dummy_gpd";
			this->toolStripMenuItem32->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem33
			// 
			this->toolStripMenuItem33->Name = L"toolStripMenuItem33";
			this->toolStripMenuItem33->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem33->Text = L"Dummy_shield";
			this->toolStripMenuItem33->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem34
			// 
			this->toolStripMenuItem34->Name = L"toolStripMenuItem34";
			this->toolStripMenuItem34->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem34->Text = L"Dummy_lh";
			this->toolStripMenuItem34->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem35
			// 
			this->toolStripMenuItem35->Name = L"toolStripMenuItem35";
			this->toolStripMenuItem35->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem35->Text = L"Dummy_arrow";
			this->toolStripMenuItem35->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// toolStripMenuItem36
			// 
			this->toolStripMenuItem36->Name = L"toolStripMenuItem36";
			this->toolStripMenuItem36->Size = System::Drawing::Size(155, 22);
			this->toolStripMenuItem36->Text = L"Dummy_quiver";
			this->toolStripMenuItem36->Click += gcnew System::EventHandler(this, &Form1::RW_loc_toolStripMenuItem_Click);
			// 
			// SlotReserved1_ToolStripMenuItem
			// 
			this->SlotReserved1_ToolStripMenuItem->Name = L"SlotReserved1_ToolStripMenuItem";
			this->SlotReserved1_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotReserved1_ToolStripMenuItem->Text = L"RESERVED1";
			this->SlotReserved1_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// SlotReserved2_ToolStripMenuItem
			// 
			this->SlotReserved2_ToolStripMenuItem->Name = L"SlotReserved2_ToolStripMenuItem";
			this->SlotReserved2_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotReserved2_ToolStripMenuItem->Text = L"RESERVED2";
			this->SlotReserved2_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// SlotReserved3_ToolStripMenuItem
			// 
			this->SlotReserved3_ToolStripMenuItem->Name = L"SlotReserved3_ToolStripMenuItem";
			this->SlotReserved3_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotReserved3_ToolStripMenuItem->Text = L"RESERVED3";
			this->SlotReserved3_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// SlotReserved4_ToolStripMenuItem
			// 
			this->SlotReserved4_ToolStripMenuItem->Name = L"SlotReserved4_ToolStripMenuItem";
			this->SlotReserved4_ToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->SlotReserved4_ToolStripMenuItem->Text = L"RESERVED4";
			this->SlotReserved4_ToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::SlotHead_toolStripMenuItem_Click);
			// 
			// UnEquipToolStripMenuItem
			// 
			this->UnEquipToolStripMenuItem->Name = L"UnEquipToolStripMenuItem";
			this->UnEquipToolStripMenuItem->Size = System::Drawing::Size(158, 22);
			this->UnEquipToolStripMenuItem->Text = L"아이템해제";
			// 
			// LogtextBox
			// 
			this->LogtextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->LogtextBox->Font = (gcnew System::Drawing::Font(L"Gulim", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(129)));
			this->LogtextBox->Location = System::Drawing::Point(6, 670);
			this->LogtextBox->Multiline = true;
			this->LogtextBox->Name = L"LogtextBox";
			this->LogtextBox->ReadOnly = true;
			this->LogtextBox->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->LogtextBox->Size = System::Drawing::Size(1070, 47);
			this->LogtextBox->TabIndex = 15;
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(129, 72);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(46, 23);
			this->button2->TabIndex = 10;
			this->button2->Text = L"등록";
			this->button2->UseVisualStyleBackColor = true;
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(129, 71);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(46, 23);
			this->button3->TabIndex = 11;
			this->button3->Text = L"삭제";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Visible = false;
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(5, 16);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(170, 21);
			this->button4->TabIndex = 0;
			this->button4->Text = L"Talent Effect Default";
			this->button4->UseVisualStyleBackColor = true;
			// 
			// button5
			// 
			this->button5->Location = System::Drawing::Point(129, 72);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(46, 23);
			this->button5->TabIndex = 10;
			this->button5->Text = L"등록";
			this->button5->UseVisualStyleBackColor = true;
			// 
			// button6
			// 
			this->button6->Location = System::Drawing::Point(129, 71);
			this->button6->Name = L"button6";
			this->button6->Size = System::Drawing::Size(46, 23);
			this->button6->TabIndex = 11;
			this->button6->Text = L"삭제";
			this->button6->UseVisualStyleBackColor = true;
			this->button6->Visible = false;
			// 
			// button7
			// 
			this->button7->Location = System::Drawing::Point(5, 16);
			this->button7->Name = L"button7";
			this->button7->Size = System::Drawing::Size(170, 21);
			this->button7->TabIndex = 0;
			this->button7->Text = L"Talent Effect Default";
			this->button7->UseVisualStyleBackColor = true;
			// 
			// button8
			// 
			this->button8->Location = System::Drawing::Point(129, 72);
			this->button8->Name = L"button8";
			this->button8->Size = System::Drawing::Size(46, 23);
			this->button8->TabIndex = 10;
			this->button8->Text = L"등록";
			this->button8->UseVisualStyleBackColor = true;
			// 
			// button9
			// 
			this->button9->Location = System::Drawing::Point(129, 71);
			this->button9->Name = L"button9";
			this->button9->Size = System::Drawing::Size(46, 23);
			this->button9->TabIndex = 11;
			this->button9->Text = L"삭제";
			this->button9->UseVisualStyleBackColor = true;
			this->button9->Visible = false;
			// 
			// button10
			// 
			this->button10->Location = System::Drawing::Point(5, 16);
			this->button10->Name = L"button10";
			this->button10->Size = System::Drawing::Size(170, 21);
			this->button10->TabIndex = 0;
			this->button10->Text = L"Talent Effect Default";
			this->button10->UseVisualStyleBackColor = true;
			// 
			// button11
			// 
			this->button11->Location = System::Drawing::Point(13, 170);
			this->button11->Name = L"button11";
			this->button11->Size = System::Drawing::Size(70, 21);
			this->button11->TabIndex = 4;
			this->button11->Text = L"그룹추가";
			this->button11->UseVisualStyleBackColor = true;
			// 
			// propertyGrid1
			// 
			this->propertyGrid1->Location = System::Drawing::Point(10, 197);
			this->propertyGrid1->Name = L"propertyGrid1";
			this->propertyGrid1->Size = System::Drawing::Size(184, 263);
			this->propertyGrid1->TabIndex = 3;
			// 
			// button12
			// 
			this->button12->Location = System::Drawing::Point(159, 170);
			this->button12->Name = L"button12";
			this->button12->Size = System::Drawing::Size(42, 21);
			this->button12->TabIndex = 2;
			this->button12->Text = L"삭제";
			this->button12->UseVisualStyleBackColor = true;
			// 
			// button13
			// 
			this->button13->Location = System::Drawing::Point(89, 170);
			this->button13->Name = L"button13";
			this->button13->Size = System::Drawing::Size(64, 21);
			this->button13->TabIndex = 1;
			this->button13->Text = L"캡슐추가";
			this->button13->UseVisualStyleBackColor = true;
			// 
			// treeView1
			// 
			this->treeView1->LineColor = System::Drawing::Color::Empty;
			this->treeView1->Location = System::Drawing::Point(12, 22);
			this->treeView1->Name = L"treeView1";
			this->treeView1->Size = System::Drawing::Size(182, 142);
			this->treeView1->TabIndex = 0;
			// 
			// Form1
			// 
			this->AllowDrop = true;
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Inherit;
			this->ClientSize = System::Drawing::Size(1080, 742);
			this->Controls->Add(this->LogtextBox);
			this->Controls->Add(this->splitContainer1);
			this->Controls->Add(this->toolStrip1);
			this->Controls->Add(this->statusStrip);
			this->Controls->Add(this->groupBox4);
			this->Controls->Add(this->MainmenuStrip);
			this->Controls->Add(this->groupBox3);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->KeyPreview = true;
			this->MainMenuStrip = this->MainmenuStrip;
			this->Name = L"Form1";
			this->Text = L"CharacterTool";
			this->Deactivate += gcnew System::EventHandler(this, &Form1::MainForm_OnDeactivated);
			this->Load += gcnew System::EventHandler(this, &Form1::MainForm_Load);
			this->Activated += gcnew System::EventHandler(this, &Form1::MainForm_OnActivated);
			this->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &Form1::MainForm_DragDrop);
			this->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &Form1::MainForm_DragEnter);
			this->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::MainForm_KeyPress);
			this->Move += gcnew System::EventHandler(this, &Form1::Form1_Move);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::MainForm_FormClosing);
			this->MainmenuStrip->ResumeLayout(false);
			this->MainmenuStrip->PerformLayout();
			this->statusStrip->ResumeLayout(false);
			this->statusStrip->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AnitrackBar))->EndInit();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->toolStrip1->ResumeLayout(false);
			this->toolStrip1->PerformLayout();
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AniSpeednumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AniSpeedtrackBar))->EndInit();
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->ResumeLayout(false);
			this->tabControl1->ResumeLayout(false);
			this->splitContainer2->Panel1->ResumeLayout(false);
			this->splitContainer2->Panel2->ResumeLayout(false);
			this->splitContainer2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MainView))->EndInit();
			this->tabControl2->ResumeLayout(false);
			this->ItemPreviewtabPage->ResumeLayout(false);
			this->ItemPreviewtabPage->PerformLayout();
			this->Effecttab->ResumeLayout(false);
			this->splitContainer6->Panel1->ResumeLayout(false);
			this->splitContainer6->Panel2->ResumeLayout(false);
			this->splitContainer6->ResumeLayout(false);
			this->groupBox13->ResumeLayout(false);
			this->groupBox13->PerformLayout();
			this->TalentEffectList_tabControl->ResumeLayout(false);
			this->tabPage3->ResumeLayout(false);
			this->tabPage6->ResumeLayout(false);
			this->groupBox10->ResumeLayout(false);
			this->PartsJudgment->ResumeLayout(false);
			this->HitTabControl->ResumeLayout(false);
			this->CollisionTabPage->ResumeLayout(false);
			this->CollisionTabPage->PerformLayout();
			this->splitContainer3->Panel1->ResumeLayout(false);
			this->splitContainer3->Panel2->ResumeLayout(false);
			this->splitContainer3->ResumeLayout(false);
			this->tabPage5->ResumeLayout(false);
			this->tabPage5->PerformLayout();
			this->splitContainer4->Panel1->ResumeLayout(false);
			this->splitContainer4->Panel1->PerformLayout();
			this->splitContainer4->Panel2->ResumeLayout(false);
			this->splitContainer4->ResumeLayout(false);
			this->HitEffect_tabPage->ResumeLayout(false);
			this->HitEffect_tabPage->PerformLayout();
			this->splitContainer5->Panel1->ResumeLayout(false);
			this->splitContainer5->Panel2->ResumeLayout(false);
			this->splitContainer5->ResumeLayout(false);
			this->SubMtrltab->ResumeLayout(false);
			this->groupBox17->ResumeLayout(false);
			this->groupBox17->PerformLayout();
			this->groupBoxExport->ResumeLayout(false);
			this->NPCcontextMenuStrip->ResumeLayout(false);
			this->contextMenuStrip2->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

public: System::Void MainLoop();

private: System::Void MainForm_Load(System::Object^  sender, System::EventArgs^  e);
private: System::Void MainForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
private: System::Void OpenMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void SaveMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void showBoneToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void ShowNormalToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void showWireToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void cameraResetSpaceBarToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void lightToCameraposToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void MainView_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
private: System::Void MainForm_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
private: System::Void MainForm_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
public: System::Void AnitrackBar_Scroll(System::Object^  sender, System::EventArgs^  e);
private: System::Void Control1_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void stopbutton_Click(System::Object^  sender, System::EventArgs^  e);
public: System::Void playbutton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void MainForm_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
private: System::Void AniSpeedtrackBar_Scroll(System::Object^  sender, System::EventArgs^  e);
private: System::Void backgroundColorToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void lightToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void feedbackToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void OpentoolStripButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void DrawBoxtoolStripButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void LighrtoolStripButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void MainView_Resize(System::Object^  sender, System::EventArgs^  e);
private: System::Void AmbienttoolStripButton1_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void LightToCameratoolStripButton1_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void collisionObjectToolStripMenuItem_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void MainForm_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
private: System::Void MainForm_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
private: System::Void SaveNPCHitInfo_Click(System::Object^  sender, System::EventArgs^  e);
		 //스피드 적용 
private: System::Void SpeedOkbutton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void SaveAsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void InfomationToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void ReloadToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void eXitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void selectModelTabToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void selectNPCListTablToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void tabControl1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void FullscreenToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void LoopPlayButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void animationPlayToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void animationStopToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void animationLoopPlayToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void transitionBlendingToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) ;
private: System::Void LoopPlaycheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void LoopPlaycheckBox_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void ShowWireShade_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void AniSpeedResetbutton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void AniSpeednumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void RedoToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void UndoToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void MainView_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e); 
private: System::Void MainView_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e); 
private: System::Void ItemOpenbutton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void MainView_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void EffectInfoNameComboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void TalentEffectAttGrid_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e);
private: System::Void FeedBack2ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Delattbutton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void MoveDummycheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void miTalentExport_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Unselected_toolStripButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void check_XYZ();
private: System::Void Bottom_toolStripButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Top_toolStripButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void TopBottom_toolStripButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void X_toolStripButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Y_toolStripButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Z_toolStripButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void XY_toolStripButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Unselect_toolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Bottom_toolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Top_toolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void TopBottom_toolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void X_toolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Y_toolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Z_toolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void XY_toolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Opa_toolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void ItemPreviewtreeView_NodeMouseClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e);
private: System::Void Refresh_button_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void SlotHead_toolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void LW_loc_toolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void RW_loc_toolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void basicMesh_button_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void TalentEventAttGrid_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e);
private: System::Void TalentEvent_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
private: System::Void TalentInfoSave(System::Object^  sender, System::EventArgs^  e);
private: System::Void CurrentID_DoubleClick(System::Object^  sender, System::EventArgs^  e);
private: System::Void TalentEffectBase_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void ViewReferenceMeshToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void SetReferenceMeshToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Add_PartsCapsule_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void PartsHittreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
private: System::Void Add_PartsGroup_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Del_PartsCapsule_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void PartsHitCapsule_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e);
private: System::Void PartsHitCapsuleStripMenuItem_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void PartsColorPreviewMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void AllTalentTime_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void buttonApply_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void listBoxMtrlList_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
private: System::Void buttonExport_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
private: System::Void MeshColtreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
private: System::Void MeshColCapsuleStripMenuItem_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void MeshColCapsule_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e);
private: System::Void Add_CollisionCapsule_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Del_CollisionCapsule_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void SaveAlltoolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void COLTEDitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->m_NPC.COLTEdit();
		 }
private: System::Void ScriptEditToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->m_NPC.ScriptEdit();
		 }
private: System::Void DisableSpeculartoolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->m_TMenu.OnShowSpecular();
		 }
private: System::Void TalentSoundAtt_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e) {
			 if(m_EventTabPage->m_TalentEventController->IsExistenceTalentEvent() == false)
			 {
				 System::Windows::Forms::MessageBox::Show(L"탤런트를 선택하세요!", "에러");
				 return;
			 }

			 //g_pMainApp->SetPropertyTalentSound(CurrentID->Text, EffectInfoNameComboBox->Text, false);
		 }
private: System::Void SaveToolStripButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->SaveAll();
		 }
private: System::Void RefSelectBtn_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void RefTextBox_DoubleClick(System::Object^  sender, System::EventArgs^  e);
private: System::Void btnMeshColCopyCapsule_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_MeshHitTabController->CopyColCapsule();
		 }
private: System::Void btnMeshColPasteCapsule_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_MeshHitTabController->PasteColCapsule();
		 }
private: System::Void CollisionCapsulesView_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 if (e->Control)
			 {
				 if (e->KeyCode == Keys::C)
				 {
					 m_MeshHitTabController->CopyColCapsule();
				 }
				 else if (e->KeyCode == Keys::V)
				 {
					 m_MeshHitTabController->PasteColCapsule();
				 }
			 }
		 }
private: System::Void btnMeshHitCapsuleCopy_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_MeshHitTabController->CopyHitCapsule();
		 }
private: System::Void btnMeshHitCapsulePaste_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_MeshHitTabController->PasteHitCapsule();
		 }
private: System::Void PartsHitCapsulesView_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 if (e->Control)
			 {
				 if (e->KeyCode == Keys::C)
				 {
					 m_MeshHitTabController->CopyHitCapsule();
				 }
				 else if (e->KeyCode == Keys::V)
				 {
					 m_MeshHitTabController->PasteHitCapsule();
				 }
			 }
		 }
private: System::Void TalentEffectTreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) {
			 m_TalentEffectTabController->OnEffectTreeView_AfterSelect(e);
		 }
private: System::Void AddEffectInfoBtn_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentEffectTabController->OnAddEffectInfoBtn_Click();
		 }
private: System::Void btnCopyEffectInfo_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentEffectTabController->CopyEffectInfo();
		 }
private: System::Void btnPasteEffectInfo_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentEffectTabController->PasteEffectInfo();
		 }
private: System::Void TalentEffectTreeView_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			 if (e->Control)
			 {
				 if (e->KeyCode == Keys::C)
				 {
					 m_TalentEffectTabController->CopyEffectInfo();
				 }
				 else if (e->KeyCode == Keys::V)
				 {
					 m_TalentEffectTabController->PasteEffectInfo();
				 }
			 }
		 }
private: System::Void Form1_Move(System::Object^  sender, System::EventArgs^  e) {
			 ColorPickerMove();
		 }
private: System::Void AnitrackBar_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 g_pMainApp->m_pSoundMgr.SetMute( RSD_CHANNELGROUP_MASTER, true);
		 }
private: System::Void AnitrackBar_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 g_pMainApp->m_pSoundMgr.SetMute( RSD_CHANNELGROUP_MASTER, false);
		 }
 private: System::Void ColorPickerMove()
		  {
			  System::Drawing::Point Location = this->Location;
			  Location.X += (this->Width - m_ColorPickerDialog->Width/2);
			  Location.Y += m_ColorPickerDialog->Height/2;
			  m_ColorPickerDialog->Location = Location;
		  }
private: System::Void treeViewMeshList_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) {
			 g_pMainApp->treeViewMeshList_NodeMouseClick();
		 }
private: System::Void ToolStripMenuItemExecuteValidator_Click(System::Object^  sender, System::EventArgs^  e) {
			 System::Diagnostics::Process^ proc = gcnew System::Diagnostics::Process();

			 proc->StartInfo->WorkingDirectory = L"..\\ResourceValidator\\";
			 proc->StartInfo->FileName = L"CMD.exe";
			 proc->StartInfo->Arguments = "/c !run_validate.bat";

			 proc->Start();
		 }
private: System::Void hDRToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void drawGridToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);

		 public: bool m_bActive;
private: System::Void MainForm_OnActivated(System::Object^  sender, System::EventArgs^  e) {
			 m_bActive = true;
		 }
private: System::Void MainForm_OnDeactivated(System::Object^  sender, System::EventArgs^  e) {
			 m_bActive = false;
		 }
private: System::Void selectPCTalentTabltoolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(tabControl1->SelectedIndex == 2) return;
			 tabControl1->SelectedIndex = 2;
		 }
private: System::Void pausebutton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void checkShowEffect_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->m_bShowEffect = checkShowEffect->Checked;
		 }
private: System::Void MainView_MouseClick_1(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 int istat = GetMouseButtonState(e->Button);
			 g_pMainApp->OnMouseClick(istat, vec2(e->X, e->Y), e->Delta);
		 }
private: System::Void checkBoxShowBone_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {

			 g_pMainApp->m_TMenu.SetShowBoneType(SBS_NONE);
			 g_pMainApp->m_TMenu.SetFillMode(RFILL_SOLID);

			 if(checkBoxShowBone->Checked)
			 {
				 g_pMainApp->m_TMenu.SetShowBoneType(SBS_SIMPLE);
				 g_pMainApp->m_TMenu.SetFillMode(RFILL_WIRE);
			 }
		 }
private: System::Void MainView_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e) {
			 g_pMainApp->MainView_KeyCheck(e);
		 }

private: System::Void DataValidatorToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 System::Diagnostics::Process^ proc = gcnew System::Diagnostics::Process();

			 proc->StartInfo->WorkingDirectory = L"..\\ResourceValidator\\ClientResourceValidator\\";
			 proc->StartInfo->FileName = L"ClientResourceValidator.exe";

			 proc->Start();
		 }
private: System::Void showPlayerModelToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			g_pMainApp->ShowHideSamplePlayerModel(true);
		 }
private: System::Void effectxmlToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(g_pMainApp->FileModifyCheck(CHECK_MODIFY_FILENAME_EFFECT))
			 {
				 mlog("effect.xml 로딩 완료\n");
				 System::Windows::Forms::MessageBox::Show(L"effect.xml 로딩 완료");
			 }
			 else
			 {
				 mlog("effect.xml 로딩 실패\n");
				 System::Windows::Forms::MessageBox::Show(L"effect.xml 로딩 실패");
			 }
		 }
private: System::Void OvelayAnimation_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			 g_pMainApp->OvelayAnimationEditTool();
		 }
private: System::Void soundxmlToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(g_pMainApp->FileModifyCheck(CHECK_MODIFY_FILENAME_SOUND))
			 {
				 mlog("sound.xml 로딩 완료\n");
				 System::Windows::Forms::MessageBox::Show(L"sound.xml 로딩 완료");
			 }
			 else
			 {
				 mlog("sound.xml 로딩 실패\n");
				 System::Windows::Forms::MessageBox::Show(L"sound.xml 로딩 실패");
			 }
		 }
private: System::Void talentxmlToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(g_pMainApp->FileModifyCheck(CHECK_MODIFY_FILENAME_TALENT))
			 {
				 mlog("talent Info 로딩 완료\n");
				 System::Windows::Forms::MessageBox::Show(L"talent Info 로딩 완료");
			 }
			 else
			 {
				 mlog("talent Info 로딩 실패\n");
				 System::Windows::Forms::MessageBox::Show(L"talent Info 로딩 실패");
			 }
		 }
private: System::Void ReLoad_toolStripSplitButton_ButtonClick(System::Object^  sender, System::EventArgs^  e) {
			 bool beffect = g_pMainApp->FileModifyCheck(CHECK_MODIFY_FILENAME_EFFECT);
			 bool bsound = g_pMainApp->FileModifyCheck(CHECK_MODIFY_FILENAME_SOUND);
			 bool btalent = g_pMainApp->FileModifyCheck(CHECK_MODIFY_FILENAME_TALENT);
			 bool bbuffeffect = g_pMainApp->FileModifyCheck(CHECK_MODIFY_FILENAME_BUFF_EFFECT_INFO);
			 bool bmotion = g_pMainApp->FileModifyCheck(CHECK_MODIFY_FILENAME_MOTION_INFO);
			 bool bnpc = g_pMainApp->FileModifyCheck(CHECK_MODIFY_FILENAME_NPC_INFO);

			 if(beffect && bsound && btalent)
			 {
				 mlog("전체 리로딩 완료\n");
				 System::Windows::Forms::MessageBox::Show(L"전체 리로딩 완료");
			 }
			 else if(beffect == false)
			 {
				 mlog("effect.xml 로딩 실패\n");
				 System::Windows::Forms::MessageBox::Show(L"effect.xml 로딩 실패");
			 }
			 else if(bsound == false)
			 {
				 mlog("sound.xml 로딩 실패\n");
				 System::Windows::Forms::MessageBox::Show(L"sound.xml 로딩 실패");
			 }
			 else if(btalent == false)
			 {
				 mlog("talent Info 로딩 실패\n");
				 System::Windows::Forms::MessageBox::Show(L"talent Info 로딩 실패");
			 }
			 else if(bbuffeffect == false)
			 {
				 mlog("buff effect Info 로딩 실패\n");
				 System::Windows::Forms::MessageBox::Show(L"buff effect Info 로딩 실패");
			 }
			 else if(bmotion == false)
			 {	
				 mlog("motion Info 로딩 실패\n");
				 System::Windows::Forms::MessageBox::Show(L"motion Info 로딩 실패");
			 }
			 else if(bnpc == false)
			 {
				 mlog("npc Info 로딩 실패\n");
				 System::Windows::Forms::MessageBox::Show(L"npc Info 로딩 실패");
			 }
		 }
private: System::Void buffeffectinfoxmlToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(g_pMainApp->FileModifyCheck(CHECK_MODIFY_FILENAME_BUFF_EFFECT_INFO))
			 {
				 mlog("buff effect Info 로딩 완료\n");
				 System::Windows::Forms::MessageBox::Show(L"buff effect Info 로딩 완료");
			 }
			 else
			 {
				 mlog("buff effect Info 로딩 실패\n");
				 System::Windows::Forms::MessageBox::Show(L"buff effect Info 로딩 실패");
			 }
		 }
private: System::Void DrawInteractionAABBToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->m_TMenu.OnShowInteractionAABB();
		 }
private: System::Void showTestHitEffectDirToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 static bool bShowTestHitEffectDir = false;
			 bShowTestHitEffectDir = !bShowTestHitEffectDir;
			 g_pMainApp->ShowTestHitEffect(bShowTestHitEffectDir);
		 }
private: System::Void HitTabControl_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void capsule_Add_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_MeshHitTabController->AddHitEffectCapsule_Click(sender, e);
		 }
private: System::Void capsule_Del_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_MeshHitTabController->DelHitEffectCapsule_Click(sender, e);
		 }
private: System::Void capsule_Copy_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_MeshHitTabController->CopyHitEffectCapsule();
		 }
private: System::Void capsule_Paste_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_MeshHitTabController->PasteHitEffectCapsule();
		 }
private: System::Void HitEffect_propertyGrid_PropertyValueChanged(System::Object^  s, System::Windows::Forms::PropertyValueChangedEventArgs^  e);
private: System::Void HitEffect_treeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
private: System::Void AnimationBlend_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			g_pMainApp->AnimationBlendEditTool();
		 }
private: System::Void showCenterAxisToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 static bool bShowCenterAxis = false;
			 bShowCenterAxis = !bShowCenterAxis;
			 g_pMainApp->ShowCenterAxis(bShowCenterAxis);
		 }
//private: System::Void b_part_01_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
//			 g_pMainApp->OnOffBreakableParts(1, b_part_01_ToolStripMenuItem->Checked);
//		 }
//private: System::Void b_part_02_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
//			 g_pMainApp->OnOffBreakableParts(2, b_part_02_ToolStripMenuItem->Checked);
//		 }
//private: System::Void b_part_03_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
//			 g_pMainApp->OnOffBreakableParts(3, b_part_03_ToolStripMenuItem->Checked);
//		 }
//private: System::Void b_part_04_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
//			 g_pMainApp->OnOffBreakableParts(4, b_part_04_ToolStripMenuItem->Checked);
//		 }
private: System::Void HitEffectPosDirtreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) {
			 m_TalentEffectTabController->OnHitEffectPosDirTreeView_AfterSelect(e);
		 }
private: System::Void TalentEffectList_tabControl_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 TalentEffectTreeView->SelectedNode = nullptr;
			 HitEffectPosDirtreeView->SelectedNode = nullptr;
		 }
private: System::Void Enable_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentEffectTabController->OnEnableHitEffectPosDirBtn_Click();
		 }
private: System::Void Disable_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentEffectTabController->OnDisableHitEffectPosDirBtn_Click();
		 }
private: System::Void motionxxxmlToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(g_pMainApp->FileModifyCheck(CHECK_MODIFY_FILENAME_MOTION_INFO))
			 {
				 mlog("motion Info 로딩 완료\n");
				 System::Windows::Forms::MessageBox::Show(L"motion Info 로딩 완료");
			 }
			 else
			 {
				 mlog("motion Info 로딩 실패\n");
				 System::Windows::Forms::MessageBox::Show(L"motion Info 로딩 실패");
			 }
		 }
private: System::Void npcxmlToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(g_pMainApp->FileModifyCheck(CHECK_MODIFY_FILENAME_NPC_INFO))
			 {
				 mlog("npc Info 로딩 완료\n");
				 System::Windows::Forms::MessageBox::Show(L"npc Info 로딩 완료");
			 }
			 else
			 {
				 mlog("npc Info 로딩 실패\n");
				 System::Windows::Forms::MessageBox::Show(L"npc Info 로딩 실패");
			 }
		 }
private: System::Void WaterEffect_checkBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->m_bShowWaterEffect = WaterEffect_checkBox->Checked;			 
		 }
private: System::Void toolStrip1_ItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) {
		 }
private: System::Void bp1_Click(System::Object^  sender, System::EventArgs^  e) {
			 bp1->Checked = !bp1->Checked;
			 g_pMainApp->OnOffBreakableParts(1, bp1->Checked);
		 }
private: System::Void bp2_Click(System::Object^  sender, System::EventArgs^  e) {
			 bp2->Checked = !bp2->Checked;
			 g_pMainApp->OnOffBreakableParts(2, bp2->Checked);
		 }
private: System::Void bp3_Click(System::Object^  sender, System::EventArgs^  e) {
			 bp3->Checked = !bp3->Checked;
			 g_pMainApp->OnOffBreakableParts(3, bp3->Checked);
		 }
private: System::Void bp4_Click(System::Object^  sender, System::EventArgs^  e) {
			 bp4->Checked = !bp4->Checked;
			 g_pMainApp->OnOffBreakableParts(4, bp4->Checked);
		 }

private: System::Void effectSelectToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->SelectWeaponEffect();
		 }
private: System::Void effectSelectDeleteToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->DeleteWeaponEffect();
		 }

private: System::Void ReActionAnimation_ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->ReActionAnimationEditTool();
		 }
private: System::Void PlayerNPCAnimationInfoToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->SetNPCAnimationInfo();
		 }
private: System::Void SkinColorTestToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(g_pMainApp->SkinColorTest())
			 {
				 PartsColorPreviewMenuItem_Click(sender, e);
			 }
		 }
private: System::Void HairColorTestToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(g_pMainApp->HairColorTest())
			 {
				 PartsColorPreviewMenuItem_Click(sender, e);
			 }
		 }
private: System::Void ItemColorTestToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(g_pMainApp->ItemColorTest())
			 {
				 PartsColorPreviewMenuItem_Click(sender, e);
			 }
		 }

private: System::Void Deref_button_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_TalentEffectTabController->OnDeDereferenceTalentEffectIDViewer();
		 }
private: System::Void showLogWindowToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void toolStripMenuItem4_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->m_bShowMeshHitEffectCapsule = toolStripMenuItem4->Checked;
		 }
private: System::Void showHitEffectCapsule_checkBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			g_pMainApp->m_bShowPartsHitCapsuleLinkedHitEffect = showHitEffectCapsule_checkBox->Checked;
		 }
private: System::Void SubMtrltab_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void Add_CollisionBox_Click(System::Object^  sender, System::EventArgs^  e) {
			 m_MeshHitTabController->AddCollisionBox_Click(sender, e);
		 }
private: System::Void showNPCHitCapsuleGroupMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->m_bShowNPCHitCapsuleGroup = showNPCHitCapsuleGroupMenuItem->Checked;

			 g_pMainApp->UpdateShowNPCHitCapsuleGroup(g_pMainApp->m_bShowNPCHitCapsuleGroup);
		 }
private: System::Void FileToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void partsHitCapsuleToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
};
}	//namespace Character_NET
