#pragma once

#include "MainApp.h"
#include "ManagedUtil.h"
#include "Global.h"
#include "PicForm.h"
#include "ListItemObject.h"
#include "RMesh.h"
#include "DistortionPropertyReflection.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace rs3;
using namespace ReflectivePropertyFormNameSpace;

namespace EffectTool_NET {
	/// <summary>
	/// EffectTabForm에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class EffectTabForm : public System::Windows::Forms::Form
	{
	public:
		EffectTabForm(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~EffectTabForm()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  EffecttabPage;
	private: System::Windows::Forms::TextBox^  textoffsetZ;
	private: System::Windows::Forms::TextBox^  textoffsetY;
	private: System::Windows::Forms::TextBox^  textoffsetX;
	private: System::Windows::Forms::TabPage^  MeshtabPage;
	public: System::Windows::Forms::TabPage^  EmittertabPage;
	public: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::Label^  label40;
	private: System::Windows::Forms::CheckBox^  FixAmountcheckBox;
	private: System::Windows::Forms::CheckBox^  withmecheckBox;
	private: System::Windows::Forms::Label^  label22;
	private: System::Windows::Forms::Label^  label21;
	private: System::Windows::Forms::NumericUpDown^  RotationMaxnumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  RotationMinnumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  EmitterLifefnumericUpDown;
	private: System::Windows::Forms::CheckBox^  RandomVelcheckBox;
	private: System::Windows::Forms::CheckBox^  EmitterDepSortcheckBox;
	private: System::Windows::Forms::CheckBox^  RotationcheckBox;
	private: System::Windows::Forms::CheckBox^  ExplosioncheckBox;
	private: System::Windows::Forms::CheckBox^  GathercheckBox;
	private: System::Windows::Forms::CheckBox^  RandomColorcheckBox;
	private: System::Windows::Forms::CheckBox^  EmitterLoopcheckBox;
	public: System::Windows::Forms::GroupBox^  groupBox6;
	public: System::Windows::Forms::GroupBox^  groupBox5;
	private: System::Windows::Forms::Label^  label32;
	private: System::Windows::Forms::Label^  label33;
	private: System::Windows::Forms::Label^  label34;
	private: System::Windows::Forms::Label^  label29;
	private: System::Windows::Forms::Label^  label30;
	private: System::Windows::Forms::Label^  label31;
	private: System::Windows::Forms::Button^  EmitterTimeDelbutton;
	private: System::Windows::Forms::Button^  EmitterTimeAddbutton;
	private: System::Windows::Forms::NumericUpDown^  EmitterTimeAnglenumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  EmitterTimeRangeZnumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  EmitterTimeRangeYnumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  EmitterTimeRangeXnumericUpDown;
	public: System::Windows::Forms::NumericUpDown^  EmitterTimePositionZnumericUpDown;
	public: System::Windows::Forms::NumericUpDown^  EmitterTimePositionYnumericUpDown;
	public: System::Windows::Forms::NumericUpDown^  EmitterTimePositionXnumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  EmitterTimeAmountnumericUpDown;
	public : System::Windows::Forms::NumericUpDown^  EmitterTimePosnumericUpDown;
	private: System::Windows::Forms::ListBox^  EmitterTimrTracklistBox;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label9;
	public: System::Windows::Forms::TabPage^  ParticletabPage;
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::Label^  label38;
	private: System::Windows::Forms::NumericUpDown^  ParticleVariableNumericUpDown1;
	private: System::Windows::Forms::Button^  ParticleTimeRefineAddbutton;
	private: System::Windows::Forms::Label^  label36;
	private: System::Windows::Forms::NumericUpDown^  ParticleTimeScaleVarnumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  VisibilitynumericUpDown;
	private: System::Windows::Forms::Label^  label35;
	private: System::Windows::Forms::Label^  label25;
	private: System::Windows::Forms::Label^  label24;
	private: System::Windows::Forms::Label^  label23;
	private: System::Windows::Forms::Button^  ParticleTimeDelbutton;
	private: System::Windows::Forms::NumericUpDown^  ParticleTimeVelMaxnumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  ParticleLifemaxnumericUpDown;
	private: System::Windows::Forms::Button^  ColorSelectbutton;
	private: System::Windows::Forms::Label^  label19;
	private: System::Windows::Forms::NumericUpDown^  ParticleLifeMinnumericUpDown;
	private: System::Windows::Forms::Label^  label17;
	private: System::Windows::Forms::NumericUpDown^  ParticleTimeGRatenumericUpDown;
	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::NumericUpDown^  ParticleTimeVelMinnumericUpDown;
	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::Label^  label15;
	private: System::Windows::Forms::NumericUpDown^  ParticleTimeVelDirZnumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  ParticleTimeVelDirYnumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  ParticleTimeVelDirXnumericUpDown;
	private: System::Windows::Forms::NumericUpDown^  ParticleTimeScaleXnumericUpDown;
	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::Button^  ParticleTimeAddbutton;
	public : System::Windows::Forms::NumericUpDown^  ParticleTimePosnumericUpDown;
	private: System::Windows::Forms::ListBox^  ParticleTimeTracklistBox;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::GroupBox^  groupBox7;
	private: System::Windows::Forms::RadioButton^  InvradioButton;
	private: System::Windows::Forms::Button^  UVmodifybutton;
	public: System::Windows::Forms::TextBox^  TexFileNametextBox;
	private: System::Windows::Forms::Button^  TexFileOpenbutton;
	private: System::Windows::Forms::NumericUpDown^  AlphatestValuenumericUpDown;
	private: System::Windows::Forms::RadioButton^  BlendAddradioButton;
	private: System::Windows::Forms::RadioButton^  BlendOpaqueRadioButton;
	private: System::Windows::Forms::RadioButton^  BlendBlendradioButton;
	public: System::Windows::Forms::TabPage^  ModelanitabPage;
	public: System::Windows::Forms::CheckBox^  AniLoopcheckBox;
	private: System::Windows::Forms::TabPage^  EffectListtabPage;
	private: System::Windows::Forms::Button^  ReloadEffects;
	public: System::Windows::Forms::TreeView^  EffectListtreeView;
	private: System::Windows::Forms::TabPage^  TrailTabPage;
	private: System::Windows::Forms::GroupBox^  groupBox11;
	private: System::Windows::Forms::NumericUpDown^  TrailSegmentnumericUpDown;
	private: System::Windows::Forms::Label^  label44;
	private: System::Windows::Forms::CheckBox^  TrailAlignCheckBox;
	private: System::Windows::Forms::Label^  label43;
	private: System::Windows::Forms::Button^  TrailBackColorButton;
	private: System::Windows::Forms::Button^  TrailFrontColorButton;
	private: System::Windows::Forms::NumericUpDown^  TrailLifenumericUpDown;
	private: System::Windows::Forms::Label^  label41;
	private: System::Windows::Forms::NumericUpDown^  TrailWidthnumericUpDown;
	private: System::Windows::Forms::Label^  label42;
	private: System::Windows::Forms::GroupBox^  groupBox10;
	private: System::Windows::Forms::Button^  TrailTextureUVButton;
	private: System::Windows::Forms::RadioButton^  TrailAlphaBlendRadioButton;
	private: System::Windows::Forms::RadioButton^  TrailAddBlendRadioButton;
	private: System::Windows::Forms::Label^  label39;
	private: System::Windows::Forms::Button^  TrailTexFileOpenButton;
	public: System::Windows::Forms::TextBox^  trailTexture;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::NumericUpDown^  EmitterVelocityMaxUpDown;
	private: System::Windows::Forms::NumericUpDown^  EmitterVelocityMinUpDown;
	private: System::Windows::Forms::NumericUpDown^  ParticleTimeScaleYnumericUpDown;
	private: System::Windows::Forms::CheckBox^  ParticleScaleLock;
	private: System::Windows::Forms::NumericUpDown^  ParticleVariableYNumericUpDown;
	private: System::Windows::Forms::CheckBox^  FollowParentRotationCheckBox;
	public: System::Windows::Forms::Button^  MeshColorSelectButton;
	private: System::Windows::Forms::CheckBox^  EmitterDistortioncheckBox;
	private: System::Windows::Forms::Button^  EMITTER_DISTORTION_PROPERTY_BUTTON;
	public: System::Windows::Forms::TextBox^  TexSIFileNametextBox;
	private: System::Windows::Forms::Button^  SIDeletebutton;
	private: System::Windows::Forms::Button^  TexSIFileOpenbutton;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::CheckBox^  TrailLoopCheckBox;
	private: System::Windows::Forms::TextBox^  FindTextBox;
	private: System::Windows::Forms::CheckBox^  UseAlphaTestCheckBox;
	private: System::Windows::Forms::CheckBox^  RandomColorRangeCheckBox;
	public: System::Windows::Forms::RadioButton^  MeshRotateZRadioButton;
	public: System::Windows::Forms::RadioButton^  MeshRotateYRadioButton;
	public: System::Windows::Forms::RadioButton^  MeshRotateXRadioButton;
	public: System::Windows::Forms::NumericUpDown^  MeshRotateMaxNumericUpDown;
	public: System::Windows::Forms::NumericUpDown^  MeshRotateMinNumericUpDown;
	public: System::Windows::Forms::NumericUpDown^  MeshScalenumericUpDown;
	private: System::Windows::Forms::Button^  ACTOR_DISTORTION_PROPERTY_BUTTON;
	public: System::Windows::Forms::CheckBox^  MeshDistortioncheckBox;
	public: System::Windows::Forms::CheckBox^  MeshLoopcheckBox;
	public: System::Windows::Forms::RadioButton^  MeshAlignRadioButton;
	public: System::Windows::Forms::RadioButton^  MeshAlignXYRadioButton;
	public: System::Windows::Forms::RadioButton^  MeshNoOptionRadioButton;
	private: System::Windows::Forms::NumericUpDown^  EmitterStartTimeNumericUpDown;
	public: System::Windows::Forms::NumericUpDown^  MeshStartTimeNumericUpDown;
	private: System::Windows::Forms::TextBox^  ScaleTextBox;
	private: System::Windows::Forms::NumericUpDown^  spnSelfIlluminationScale;
	private: System::Windows::Forms::Label^  label18;
	private: System::Windows::Forms::CheckBox^  TrailDistortionCheckBox;
	private: System::Windows::Forms::Button^  TRAIL_DISTORTION_PROPERTY_BUTTON;
	private: System::Windows::Forms::ComboBox^  BillboardTypeComboBox;
	public: System::Windows::Forms::TreeView^  NodeListtreeView;
	public: System::Windows::Forms::TreeView^  AnitreeView;
	private: System::Windows::Forms::TextBox^  ActorNodeFindTextBox;
	private: System::Windows::Forms::TextBox^  ActorAniTreeViewFindTextBox;
	private: System::Windows::Forms::RadioButton^  TrailInverseBlendRadioButton;
	private: System::Windows::Forms::RadioButton^  TrailOpaqueBlendRadioButton;
	private: System::Windows::Forms::CheckBox^  TrailUseAlphaTestCheckBox;
	private: System::Windows::Forms::NumericUpDown^  TrailAlphatestValueNumericUpDown;
	private: System::Windows::Forms::CheckBox^  WorldVelocitycheckBox;
	private: System::Windows::Forms::NumericUpDown^  TrailSpnSelfIlluminationScale;
	private: System::Windows::Forms::Button^  TrailSIDeleteButton;
	public: System::Windows::Forms::TextBox^  TrailTexSIFileNameTextBox;
	private: System::Windows::Forms::Button^  TrailTexSIFileOpenButton;
	private: System::ComponentModel::IContainer^  components;
	public: 
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::ToolTip^  ToolTip;
			System::Windows::Forms::GroupBox^  groupBox1;
			System::Windows::Forms::Label^  label12;
			System::Windows::Forms::GroupBox^  groupBox3;
			System::Windows::Forms::Label^  label11;
			System::Windows::Forms::GroupBox^  groupBox12;
			System::Windows::Forms::Label^  label37;
			System::Windows::Forms::Label^  label6;
			System::Windows::Forms::Label^  label5;
			System::Windows::Forms::GroupBox^  groupBox9;
			System::Windows::Forms::GroupBox^  groupBox8;
			System::Windows::Forms::Label^  label26;
			System::Windows::Forms::TreeNode^  treeNode1 = (gcnew System::Windows::Forms::TreeNode(L"none"));
			System::Windows::Forms::Label^  label20;
			this->RandomColorRangeCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->MeshStartTimeNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->MeshLoopcheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->MeshColorSelectButton = (gcnew System::Windows::Forms::Button());
			this->ACTOR_DISTORTION_PROPERTY_BUTTON = (gcnew System::Windows::Forms::Button());
			this->MeshDistortioncheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->MeshNoOptionRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->MeshAlignXYRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->MeshAlignRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->MeshRotateZRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->MeshRotateYRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->MeshRotateXRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->MeshRotateMaxNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->MeshRotateMinNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->MeshScalenumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->textoffsetZ = (gcnew System::Windows::Forms::TextBox());
			this->textoffsetY = (gcnew System::Windows::Forms::TextBox());
			this->textoffsetX = (gcnew System::Windows::Forms::TextBox());
			this->ScaleTextBox = (gcnew System::Windows::Forms::TextBox());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->EffecttabPage = (gcnew System::Windows::Forms::TabPage());
			this->MeshtabPage = (gcnew System::Windows::Forms::TabPage());
			this->EmittertabPage = (gcnew System::Windows::Forms::TabPage());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->WorldVelocitycheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->EmitterStartTimeNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->EMITTER_DISTORTION_PROPERTY_BUTTON = (gcnew System::Windows::Forms::Button());
			this->EmitterDistortioncheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->FollowParentRotationCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->label40 = (gcnew System::Windows::Forms::Label());
			this->FixAmountcheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->withmecheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->label21 = (gcnew System::Windows::Forms::Label());
			this->RotationMaxnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->RotationMinnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->EmitterLifefnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->RandomVelcheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->EmitterDepSortcheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->RotationcheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->ExplosioncheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->GathercheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->RandomColorcheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->EmitterLoopcheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->BillboardTypeComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->EmitterVelocityMaxUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->EmitterVelocityMinUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label32 = (gcnew System::Windows::Forms::Label());
			this->label33 = (gcnew System::Windows::Forms::Label());
			this->label34 = (gcnew System::Windows::Forms::Label());
			this->label29 = (gcnew System::Windows::Forms::Label());
			this->label30 = (gcnew System::Windows::Forms::Label());
			this->label31 = (gcnew System::Windows::Forms::Label());
			this->EmitterTimeDelbutton = (gcnew System::Windows::Forms::Button());
			this->EmitterTimeAddbutton = (gcnew System::Windows::Forms::Button());
			this->EmitterTimeAnglenumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->EmitterTimeRangeZnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->EmitterTimeRangeYnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->EmitterTimeRangeXnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->EmitterTimePositionZnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->EmitterTimePositionYnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->EmitterTimePositionXnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->EmitterTimeAmountnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->EmitterTimePosnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->EmitterTimrTracklistBox = (gcnew System::Windows::Forms::ListBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->ParticletabPage = (gcnew System::Windows::Forms::TabPage());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->ParticleVariableYNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->ParticleScaleLock = (gcnew System::Windows::Forms::CheckBox());
			this->label38 = (gcnew System::Windows::Forms::Label());
			this->ParticleVariableNumericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->ParticleTimeRefineAddbutton = (gcnew System::Windows::Forms::Button());
			this->label36 = (gcnew System::Windows::Forms::Label());
			this->ParticleTimeScaleVarnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->VisibilitynumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label35 = (gcnew System::Windows::Forms::Label());
			this->label25 = (gcnew System::Windows::Forms::Label());
			this->label24 = (gcnew System::Windows::Forms::Label());
			this->label23 = (gcnew System::Windows::Forms::Label());
			this->ParticleTimeDelbutton = (gcnew System::Windows::Forms::Button());
			this->ParticleTimeVelMaxnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->ParticleLifemaxnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->ColorSelectbutton = (gcnew System::Windows::Forms::Button());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->ParticleLifeMinnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->ParticleTimeGRatenumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->ParticleTimeVelMinnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->ParticleTimeVelDirZnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->ParticleTimeVelDirYnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->ParticleTimeVelDirXnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->ParticleTimeScaleYnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->ParticleTimeScaleXnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->ParticleTimeAddbutton = (gcnew System::Windows::Forms::Button());
			this->ParticleTimePosnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->ParticleTimeTracklistBox = (gcnew System::Windows::Forms::ListBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->groupBox7 = (gcnew System::Windows::Forms::GroupBox());
			this->spnSelfIlluminationScale = (gcnew System::Windows::Forms::NumericUpDown());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->UseAlphaTestCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->SIDeletebutton = (gcnew System::Windows::Forms::Button());
			this->TexSIFileOpenbutton = (gcnew System::Windows::Forms::Button());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->TexSIFileNametextBox = (gcnew System::Windows::Forms::TextBox());
			this->InvradioButton = (gcnew System::Windows::Forms::RadioButton());
			this->UVmodifybutton = (gcnew System::Windows::Forms::Button());
			this->TexFileNametextBox = (gcnew System::Windows::Forms::TextBox());
			this->TexFileOpenbutton = (gcnew System::Windows::Forms::Button());
			this->AlphatestValuenumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->BlendAddradioButton = (gcnew System::Windows::Forms::RadioButton());
			this->BlendOpaqueRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->BlendBlendradioButton = (gcnew System::Windows::Forms::RadioButton());
			this->ModelanitabPage = (gcnew System::Windows::Forms::TabPage());
			this->ActorNodeFindTextBox = (gcnew System::Windows::Forms::TextBox());
			this->ActorAniTreeViewFindTextBox = (gcnew System::Windows::Forms::TextBox());
			this->NodeListtreeView = (gcnew System::Windows::Forms::TreeView());
			this->AnitreeView = (gcnew System::Windows::Forms::TreeView());
			this->AniLoopcheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->EffectListtabPage = (gcnew System::Windows::Forms::TabPage());
			this->FindTextBox = (gcnew System::Windows::Forms::TextBox());
			this->ReloadEffects = (gcnew System::Windows::Forms::Button());
			this->EffectListtreeView = (gcnew System::Windows::Forms::TreeView());
			this->TrailTabPage = (gcnew System::Windows::Forms::TabPage());
			this->groupBox11 = (gcnew System::Windows::Forms::GroupBox());
			this->TRAIL_DISTORTION_PROPERTY_BUTTON = (gcnew System::Windows::Forms::Button());
			this->TrailDistortionCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->TrailLoopCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->TrailSegmentnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label44 = (gcnew System::Windows::Forms::Label());
			this->TrailAlignCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->label43 = (gcnew System::Windows::Forms::Label());
			this->TrailBackColorButton = (gcnew System::Windows::Forms::Button());
			this->TrailFrontColorButton = (gcnew System::Windows::Forms::Button());
			this->TrailLifenumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label41 = (gcnew System::Windows::Forms::Label());
			this->TrailWidthnumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label42 = (gcnew System::Windows::Forms::Label());
			this->groupBox10 = (gcnew System::Windows::Forms::GroupBox());
			this->TrailSpnSelfIlluminationScale = (gcnew System::Windows::Forms::NumericUpDown());
			this->TrailSIDeleteButton = (gcnew System::Windows::Forms::Button());
			this->TrailTexSIFileOpenButton = (gcnew System::Windows::Forms::Button());
			this->TrailTexSIFileNameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->TrailAlphatestValueNumericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->TrailUseAlphaTestCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->TrailInverseBlendRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->TrailOpaqueBlendRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->TrailTextureUVButton = (gcnew System::Windows::Forms::Button());
			this->TrailAlphaBlendRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->TrailAddBlendRadioButton = (gcnew System::Windows::Forms::RadioButton());
			this->label39 = (gcnew System::Windows::Forms::Label());
			this->TrailTexFileOpenButton = (gcnew System::Windows::Forms::Button());
			this->trailTexture = (gcnew System::Windows::Forms::TextBox());
			ToolTip = (gcnew System::Windows::Forms::ToolTip(this->components));
			groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			label12 = (gcnew System::Windows::Forms::Label());
			groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			label11 = (gcnew System::Windows::Forms::Label());
			groupBox12 = (gcnew System::Windows::Forms::GroupBox());
			label37 = (gcnew System::Windows::Forms::Label());
			label6 = (gcnew System::Windows::Forms::Label());
			label5 = (gcnew System::Windows::Forms::Label());
			groupBox9 = (gcnew System::Windows::Forms::GroupBox());
			groupBox8 = (gcnew System::Windows::Forms::GroupBox());
			label26 = (gcnew System::Windows::Forms::Label());
			label20 = (gcnew System::Windows::Forms::Label());
			groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MeshStartTimeNumericUpDown))->BeginInit();
			groupBox3->SuspendLayout();
			groupBox12->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MeshRotateMaxNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MeshRotateMinNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MeshScalenumericUpDown))->BeginInit();
			groupBox9->SuspendLayout();
			groupBox8->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->EffecttabPage->SuspendLayout();
			this->MeshtabPage->SuspendLayout();
			this->EmittertabPage->SuspendLayout();
			this->groupBox2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterStartTimeNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->RotationMaxnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->RotationMinnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterLifefnumericUpDown))->BeginInit();
			this->groupBox6->SuspendLayout();
			this->groupBox5->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterVelocityMaxUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterVelocityMinUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimeAnglenumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimeRangeZnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimeRangeYnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimeRangeXnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimePositionZnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimePositionYnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimePositionXnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimeAmountnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimePosnumericUpDown))->BeginInit();
			this->ParticletabPage->SuspendLayout();
			this->groupBox4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleVariableYNumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleVariableNumericUpDown1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeScaleVarnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->VisibilitynumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeVelMaxnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleLifemaxnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleLifeMinnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeGRatenumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeVelMinnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeVelDirZnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeVelDirYnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeVelDirXnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeScaleYnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeScaleXnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimePosnumericUpDown))->BeginInit();
			this->groupBox7->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->spnSelfIlluminationScale))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AlphatestValuenumericUpDown))->BeginInit();
			this->ModelanitabPage->SuspendLayout();
			this->EffectListtabPage->SuspendLayout();
			this->TrailTabPage->SuspendLayout();
			this->groupBox11->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TrailSegmentnumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TrailLifenumericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TrailWidthnumericUpDown))->BeginInit();
			this->groupBox10->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TrailSpnSelfIlluminationScale))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TrailAlphatestValueNumericUpDown))->BeginInit();
			this->SuspendLayout();
			// 
			// ToolTip
			// 
			ToolTip->AutoPopDelay = 0;
			ToolTip->InitialDelay = 500;
			ToolTip->ReshowDelay = 100;
			// 
			// RandomColorRangeCheckBox
			// 
			this->RandomColorRangeCheckBox->AutoSize = true;
			this->RandomColorRangeCheckBox->Location = System::Drawing::Point(9, 113);
			this->RandomColorRangeCheckBox->Name = L"RandomColorRangeCheckBox";
			this->RandomColorRangeCheckBox->Size = System::Drawing::Size(137, 16);
			this->RandomColorRangeCheckBox->TabIndex = 117;
			this->RandomColorRangeCheckBox->Text = L"ColorRandomRange";
			ToolTip->SetToolTip(this->RandomColorRangeCheckBox, L"파티클 타임 트랙의 처음과 마지막 키의\r\n색상 범위 내에서 무작위로 색상이 결정\r\n됩니다.");
			this->RandomColorRangeCheckBox->UseVisualStyleBackColor = true;
			this->RandomColorRangeCheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::RandomColorRangeCheckBox_CheckedChanged);
			// 
			// groupBox1
			// 
			groupBox1->Controls->Add(label12);
			groupBox1->Controls->Add(this->MeshStartTimeNumericUpDown);
			groupBox1->Controls->Add(this->MeshLoopcheckBox);
			groupBox1->Controls->Add(this->MeshColorSelectButton);
			groupBox1->Location = System::Drawing::Point(3, 3);
			groupBox1->Name = L"groupBox1";
			groupBox1->Size = System::Drawing::Size(225, 111);
			groupBox1->TabIndex = 98;
			groupBox1->TabStop = false;
			// 
			// label12
			// 
			label12->AutoSize = true;
			label12->Location = System::Drawing::Point(6, 46);
			label12->Name = L"label12";
			label12->Size = System::Drawing::Size(67, 12);
			label12->TabIndex = 121;
			label12->Text = L"StartTime :";
			// 
			// MeshStartTimeNumericUpDown
			// 
			this->MeshStartTimeNumericUpDown->DecimalPlaces = 1;
			this->MeshStartTimeNumericUpDown->Location = System::Drawing::Point(83, 44);
			this->MeshStartTimeNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
			this->MeshStartTimeNumericUpDown->Name = L"MeshStartTimeNumericUpDown";
			this->MeshStartTimeNumericUpDown->Size = System::Drawing::Size(120, 21);
			this->MeshStartTimeNumericUpDown->TabIndex = 120;
			this->MeshStartTimeNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::MeshStartTimeNumericUpDown_ValueChanged);
			// 
			// MeshLoopcheckBox
			// 
			this->MeshLoopcheckBox->AutoSize = true;
			this->MeshLoopcheckBox->Location = System::Drawing::Point(6, 13);
			this->MeshLoopcheckBox->Name = L"MeshLoopcheckBox";
			this->MeshLoopcheckBox->Size = System::Drawing::Size(52, 16);
			this->MeshLoopcheckBox->TabIndex = 55;
			this->MeshLoopcheckBox->Text = L"Loop";
			this->MeshLoopcheckBox->UseVisualStyleBackColor = true;
			this->MeshLoopcheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::MeshLoopcheckBox_CheckedChanged);
			// 
			// MeshColorSelectButton
			// 
			this->MeshColorSelectButton->Location = System::Drawing::Point(6, 82);
			this->MeshColorSelectButton->Name = L"MeshColorSelectButton";
			this->MeshColorSelectButton->Size = System::Drawing::Size(75, 23);
			this->MeshColorSelectButton->TabIndex = 57;
			this->MeshColorSelectButton->Text = L"Color";
			this->MeshColorSelectButton->UseVisualStyleBackColor = true;
			this->MeshColorSelectButton->Click += gcnew System::EventHandler(this, &EffectTabForm::MeshColorSelectButton_Click);
			// 
			// groupBox3
			// 
			groupBox3->Controls->Add(this->ACTOR_DISTORTION_PROPERTY_BUTTON);
			groupBox3->Controls->Add(this->MeshDistortioncheckBox);
			groupBox3->Location = System::Drawing::Point(3, 269);
			groupBox3->Name = L"groupBox3";
			groupBox3->Size = System::Drawing::Size(224, 73);
			groupBox3->TabIndex = 100;
			groupBox3->TabStop = false;
			groupBox3->Text = L"Distortion";
			// 
			// ACTOR_DISTORTION_PROPERTY_BUTTON
			// 
			this->ACTOR_DISTORTION_PROPERTY_BUTTON->Location = System::Drawing::Point(6, 42);
			this->ACTOR_DISTORTION_PROPERTY_BUTTON->Name = L"ACTOR_DISTORTION_PROPERTY_BUTTON";
			this->ACTOR_DISTORTION_PROPERTY_BUTTON->Size = System::Drawing::Size(75, 23);
			this->ACTOR_DISTORTION_PROPERTY_BUTTON->TabIndex = 119;
			this->ACTOR_DISTORTION_PROPERTY_BUTTON->Text = L"속성";
			this->ACTOR_DISTORTION_PROPERTY_BUTTON->UseVisualStyleBackColor = true;
			this->ACTOR_DISTORTION_PROPERTY_BUTTON->Click += gcnew System::EventHandler(this, &EffectTabForm::ACTOR_DISTORTION_PROPERTY_BUTTON_Click);
			// 
			// MeshDistortioncheckBox
			// 
			this->MeshDistortioncheckBox->AutoSize = true;
			this->MeshDistortioncheckBox->Location = System::Drawing::Point(6, 20);
			this->MeshDistortioncheckBox->Name = L"MeshDistortioncheckBox";
			this->MeshDistortioncheckBox->Size = System::Drawing::Size(63, 16);
			this->MeshDistortioncheckBox->TabIndex = 118;
			this->MeshDistortioncheckBox->Text = L"Enable";
			this->MeshDistortioncheckBox->UseVisualStyleBackColor = true;
			this->MeshDistortioncheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::MeshDistortioncheckBox_CheckedChanged);
			// 
			// label11
			// 
			label11->AutoSize = true;
			label11->Location = System::Drawing::Point(6, 75);
			label11->Name = L"label11";
			label11->Size = System::Drawing::Size(59, 12);
			label11->TabIndex = 136;
			label11->Text = L"Min Max:";
			// 
			// groupBox12
			// 
			groupBox12->Controls->Add(this->MeshNoOptionRadioButton);
			groupBox12->Controls->Add(this->MeshAlignXYRadioButton);
			groupBox12->Controls->Add(this->MeshAlignRadioButton);
			groupBox12->Controls->Add(this->MeshRotateZRadioButton);
			groupBox12->Controls->Add(this->MeshRotateYRadioButton);
			groupBox12->Controls->Add(this->MeshRotateXRadioButton);
			groupBox12->Controls->Add(label11);
			groupBox12->Controls->Add(this->MeshRotateMaxNumericUpDown);
			groupBox12->Controls->Add(this->MeshRotateMinNumericUpDown);
			groupBox12->Controls->Add(label37);
			groupBox12->Controls->Add(this->MeshScalenumericUpDown);
			groupBox12->Location = System::Drawing::Point(3, 120);
			groupBox12->Name = L"groupBox12";
			groupBox12->Size = System::Drawing::Size(224, 143);
			groupBox12->TabIndex = 101;
			groupBox12->TabStop = false;
			groupBox12->Text = L"Transform Option";
			// 
			// MeshNoOptionRadioButton
			// 
			this->MeshNoOptionRadioButton->AutoSize = true;
			this->MeshNoOptionRadioButton->Location = System::Drawing::Point(6, 20);
			this->MeshNoOptionRadioButton->Name = L"MeshNoOptionRadioButton";
			this->MeshNoOptionRadioButton->Size = System::Drawing::Size(53, 16);
			this->MeshNoOptionRadioButton->TabIndex = 143;
			this->MeshNoOptionRadioButton->TabStop = true;
			this->MeshNoOptionRadioButton->Text = L"None";
			this->MeshNoOptionRadioButton->UseVisualStyleBackColor = true;
			this->MeshNoOptionRadioButton->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::MeshNoOptionRadioButton_CheckedChanged);
			// 
			// MeshAlignXYRadioButton
			// 
			this->MeshAlignXYRadioButton->AutoSize = true;
			this->MeshAlignXYRadioButton->Location = System::Drawing::Point(150, 20);
			this->MeshAlignXYRadioButton->Name = L"MeshAlignXYRadioButton";
			this->MeshAlignXYRadioButton->Size = System::Drawing::Size(71, 16);
			this->MeshAlignXYRadioButton->TabIndex = 142;
			this->MeshAlignXYRadioButton->TabStop = true;
			this->MeshAlignXYRadioButton->Text = L"Align XY";
			this->MeshAlignXYRadioButton->UseVisualStyleBackColor = true;
			this->MeshAlignXYRadioButton->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::MeshAlignXYRadioButton_CheckedChanged);
			// 
			// MeshAlignRadioButton
			// 
			this->MeshAlignRadioButton->AutoSize = true;
			this->MeshAlignRadioButton->Location = System::Drawing::Point(78, 20);
			this->MeshAlignRadioButton->Name = L"MeshAlignRadioButton";
			this->MeshAlignRadioButton->Size = System::Drawing::Size(51, 16);
			this->MeshAlignRadioButton->TabIndex = 141;
			this->MeshAlignRadioButton->TabStop = true;
			this->MeshAlignRadioButton->Text = L"Align";
			this->MeshAlignRadioButton->UseVisualStyleBackColor = true;
			this->MeshAlignRadioButton->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::MeshAlignRadioButton_CheckedChanged);
			// 
			// MeshRotateZRadioButton
			// 
			this->MeshRotateZRadioButton->AutoSize = true;
			this->MeshRotateZRadioButton->Location = System::Drawing::Point(150, 52);
			this->MeshRotateZRadioButton->Name = L"MeshRotateZRadioButton";
			this->MeshRotateZRadioButton->Size = System::Drawing::Size(66, 16);
			this->MeshRotateZRadioButton->TabIndex = 140;
			this->MeshRotateZRadioButton->TabStop = true;
			this->MeshRotateZRadioButton->Text = L"RotateZ";
			this->MeshRotateZRadioButton->UseVisualStyleBackColor = true;
			this->MeshRotateZRadioButton->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::MeshRotateZRadioButton_CheckedChanged);
			// 
			// MeshRotateYRadioButton
			// 
			this->MeshRotateYRadioButton->AutoSize = true;
			this->MeshRotateYRadioButton->Location = System::Drawing::Point(78, 52);
			this->MeshRotateYRadioButton->Name = L"MeshRotateYRadioButton";
			this->MeshRotateYRadioButton->Size = System::Drawing::Size(66, 16);
			this->MeshRotateYRadioButton->TabIndex = 139;
			this->MeshRotateYRadioButton->TabStop = true;
			this->MeshRotateYRadioButton->Text = L"RotateY";
			this->MeshRotateYRadioButton->UseVisualStyleBackColor = true;
			this->MeshRotateYRadioButton->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::MeshRotateYRadioButton_CheckedChanged);
			// 
			// MeshRotateXRadioButton
			// 
			this->MeshRotateXRadioButton->AutoSize = true;
			this->MeshRotateXRadioButton->Location = System::Drawing::Point(6, 52);
			this->MeshRotateXRadioButton->Name = L"MeshRotateXRadioButton";
			this->MeshRotateXRadioButton->Size = System::Drawing::Size(66, 16);
			this->MeshRotateXRadioButton->TabIndex = 138;
			this->MeshRotateXRadioButton->TabStop = true;
			this->MeshRotateXRadioButton->Text = L"RotateX";
			this->MeshRotateXRadioButton->UseVisualStyleBackColor = true;
			this->MeshRotateXRadioButton->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::MeshRotateXRadioButton_CheckedChanged);
			// 
			// MeshRotateMaxNumericUpDown
			// 
			this->MeshRotateMaxNumericUpDown->Location = System::Drawing::Point(124, 73);
			this->MeshRotateMaxNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, 0});
			this->MeshRotateMaxNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, System::Int32::MinValue});
			this->MeshRotateMaxNumericUpDown->Name = L"MeshRotateMaxNumericUpDown";
			this->MeshRotateMaxNumericUpDown->Size = System::Drawing::Size(49, 21);
			this->MeshRotateMaxNumericUpDown->TabIndex = 134;
			this->MeshRotateMaxNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::MeshRotateMaxNumericUpDown_ValueChanged);
			// 
			// MeshRotateMinNumericUpDown
			// 
			this->MeshRotateMinNumericUpDown->Location = System::Drawing::Point(71, 73);
			this->MeshRotateMinNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, 0});
			this->MeshRotateMinNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, System::Int32::MinValue});
			this->MeshRotateMinNumericUpDown->Name = L"MeshRotateMinNumericUpDown";
			this->MeshRotateMinNumericUpDown->Size = System::Drawing::Size(47, 21);
			this->MeshRotateMinNumericUpDown->TabIndex = 133;
			this->MeshRotateMinNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::MeshRotateMinNumericUpDown_ValueChanged);
			// 
			// label37
			// 
			label37->AutoSize = true;
			label37->Location = System::Drawing::Point(6, 115);
			label37->Name = L"label37";
			label37->Size = System::Drawing::Size(81, 12);
			label37->TabIndex = 129;
			label37->Text = L"Mesh Scale :";
			// 
			// MeshScalenumericUpDown
			// 
			this->MeshScalenumericUpDown->DecimalPlaces = 2;
			this->MeshScalenumericUpDown->ImeMode = System::Windows::Forms::ImeMode::NoControl;
			this->MeshScalenumericUpDown->Location = System::Drawing::Point(93, 113);
			this->MeshScalenumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
			this->MeshScalenumericUpDown->Name = L"MeshScalenumericUpDown";
			this->MeshScalenumericUpDown->Size = System::Drawing::Size(73, 21);
			this->MeshScalenumericUpDown->TabIndex = 128;
			this->MeshScalenumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->MeshScalenumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::MeshScalenumericUpDown_ValueChanged);
			// 
			// label6
			// 
			label6->AutoSize = true;
			label6->Location = System::Drawing::Point(7, 17);
			label6->Name = L"label6";
			label6->Size = System::Drawing::Size(63, 12);
			label6->TabIndex = 30;
			label6->Text = L"Life(/fra) :";
			// 
			// label5
			// 
			label5->AutoSize = true;
			label5->Location = System::Drawing::Point(7, 44);
			label5->Name = L"label5";
			label5->Size = System::Drawing::Size(67, 12);
			label5->TabIndex = 119;
			label5->Text = L"StartTime :";
			// 
			// groupBox9
			// 
			groupBox9->Controls->Add(this->textoffsetZ);
			groupBox9->Controls->Add(this->textoffsetY);
			groupBox9->Controls->Add(this->textoffsetX);
			groupBox9->Location = System::Drawing::Point(3, 3);
			groupBox9->Name = L"groupBox9";
			groupBox9->Size = System::Drawing::Size(223, 56);
			groupBox9->TabIndex = 57;
			groupBox9->TabStop = false;
			groupBox9->Text = L"Offset";
			// 
			// textoffsetZ
			// 
			this->textoffsetZ->Location = System::Drawing::Point(141, 20);
			this->textoffsetZ->Name = L"textoffsetZ";
			this->textoffsetZ->Size = System::Drawing::Size(54, 21);
			this->textoffsetZ->TabIndex = 2;
			this->textoffsetZ->Text = L"0";
			this->textoffsetZ->TextChanged += gcnew System::EventHandler(this, &EffectTabForm::textoffset_TextChanged);
			// 
			// textoffsetY
			// 
			this->textoffsetY->Location = System::Drawing::Point(81, 20);
			this->textoffsetY->Name = L"textoffsetY";
			this->textoffsetY->Size = System::Drawing::Size(54, 21);
			this->textoffsetY->TabIndex = 1;
			this->textoffsetY->Text = L"0";
			this->textoffsetY->TextChanged += gcnew System::EventHandler(this, &EffectTabForm::textoffset_TextChanged);
			// 
			// textoffsetX
			// 
			this->textoffsetX->Location = System::Drawing::Point(21, 20);
			this->textoffsetX->Name = L"textoffsetX";
			this->textoffsetX->Size = System::Drawing::Size(54, 21);
			this->textoffsetX->TabIndex = 0;
			this->textoffsetX->Text = L"0";
			this->textoffsetX->TextChanged += gcnew System::EventHandler(this, &EffectTabForm::textoffset_TextChanged);
			// 
			// groupBox8
			// 
			groupBox8->Controls->Add(this->ScaleTextBox);
			groupBox8->Location = System::Drawing::Point(3, 65);
			groupBox8->Name = L"groupBox8";
			groupBox8->Size = System::Drawing::Size(223, 56);
			groupBox8->TabIndex = 58;
			groupBox8->TabStop = false;
			groupBox8->Text = L"Scale";
			// 
			// ScaleTextBox
			// 
			this->ScaleTextBox->Location = System::Drawing::Point(21, 20);
			this->ScaleTextBox->MaxLength = 8;
			this->ScaleTextBox->Name = L"ScaleTextBox";
			this->ScaleTextBox->Size = System::Drawing::Size(54, 21);
			this->ScaleTextBox->TabIndex = 0;
			this->ScaleTextBox->Text = L"1";
			this->ScaleTextBox->TextChanged += gcnew System::EventHandler(this, &EffectTabForm::textoffset_TextChanged);
			// 
			// label26
			// 
			label26->AutoSize = true;
			label26->Location = System::Drawing::Point(9, 119);
			label26->Name = L"label26";
			label26->Size = System::Drawing::Size(102, 12);
			label26->TabIndex = 107;
			label26->Text = L"Self illumination :";
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->EffecttabPage);
			this->tabControl1->Controls->Add(this->MeshtabPage);
			this->tabControl1->Controls->Add(this->EmittertabPage);
			this->tabControl1->Controls->Add(this->ParticletabPage);
			this->tabControl1->Controls->Add(this->ModelanitabPage);
			this->tabControl1->Controls->Add(this->EffectListtabPage);
			this->tabControl1->Controls->Add(this->TrailTabPage);
			this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl1->Location = System::Drawing::Point(0, 0);
			this->tabControl1->Multiline = true;
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(280, 700);
			this->tabControl1->TabIndex = 1;
			// 
			// EffecttabPage
			// 
			this->EffecttabPage->Controls->Add(groupBox8);
			this->EffecttabPage->Controls->Add(groupBox9);
			this->EffecttabPage->Location = System::Drawing::Point(4, 40);
			this->EffecttabPage->Name = L"EffecttabPage";
			this->EffecttabPage->Size = System::Drawing::Size(272, 656);
			this->EffecttabPage->TabIndex = 4;
			this->EffecttabPage->Text = L"Effect";
			this->EffecttabPage->UseVisualStyleBackColor = true;
			// 
			// MeshtabPage
			// 
			this->MeshtabPage->Controls->Add(groupBox12);
			this->MeshtabPage->Controls->Add(groupBox3);
			this->MeshtabPage->Controls->Add(groupBox1);
			this->MeshtabPage->Location = System::Drawing::Point(4, 40);
			this->MeshtabPage->Name = L"MeshtabPage";
			this->MeshtabPage->Size = System::Drawing::Size(272, 656);
			this->MeshtabPage->TabIndex = 5;
			this->MeshtabPage->Text = L"Mesh";
			this->MeshtabPage->UseVisualStyleBackColor = true;
			// 
			// EmittertabPage
			// 
			this->EmittertabPage->AutoScroll = true;
			this->EmittertabPage->Controls->Add(this->groupBox2);
			this->EmittertabPage->Controls->Add(this->groupBox6);
			this->EmittertabPage->Controls->Add(this->groupBox5);
			this->EmittertabPage->Location = System::Drawing::Point(4, 40);
			this->EmittertabPage->Name = L"EmittertabPage";
			this->EmittertabPage->Padding = System::Windows::Forms::Padding(3);
			this->EmittertabPage->Size = System::Drawing::Size(272, 656);
			this->EmittertabPage->TabIndex = 1;
			this->EmittertabPage->Text = L"Emitter";
			this->EmittertabPage->UseVisualStyleBackColor = true;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->WorldVelocitycheckBox);
			this->groupBox2->Controls->Add(label5);
			this->groupBox2->Controls->Add(this->EmitterStartTimeNumericUpDown);
			this->groupBox2->Controls->Add(this->RandomColorRangeCheckBox);
			this->groupBox2->Controls->Add(this->EMITTER_DISTORTION_PROPERTY_BUTTON);
			this->groupBox2->Controls->Add(this->EmitterDistortioncheckBox);
			this->groupBox2->Controls->Add(this->FollowParentRotationCheckBox);
			this->groupBox2->Controls->Add(this->label40);
			this->groupBox2->Controls->Add(this->FixAmountcheckBox);
			this->groupBox2->Controls->Add(this->withmecheckBox);
			this->groupBox2->Controls->Add(this->label22);
			this->groupBox2->Controls->Add(this->label21);
			this->groupBox2->Controls->Add(this->RotationMaxnumericUpDown);
			this->groupBox2->Controls->Add(this->RotationMinnumericUpDown);
			this->groupBox2->Controls->Add(label6);
			this->groupBox2->Controls->Add(this->EmitterLifefnumericUpDown);
			this->groupBox2->Controls->Add(this->RandomVelcheckBox);
			this->groupBox2->Controls->Add(this->EmitterDepSortcheckBox);
			this->groupBox2->Controls->Add(this->RotationcheckBox);
			this->groupBox2->Controls->Add(this->ExplosioncheckBox);
			this->groupBox2->Controls->Add(this->GathercheckBox);
			this->groupBox2->Controls->Add(this->RandomColorcheckBox);
			this->groupBox2->Controls->Add(this->EmitterLoopcheckBox);
			this->groupBox2->Location = System::Drawing::Point(3, 3);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(225, 297);
			this->groupBox2->TabIndex = 48;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Emitter Attr";
			// 
			// WorldVelocitycheckBox
			// 
			this->WorldVelocitycheckBox->AutoSize = true;
			this->WorldVelocitycheckBox->Location = System::Drawing::Point(109, 178);
			this->WorldVelocitycheckBox->Name = L"WorldVelocitycheckBox";
			this->WorldVelocitycheckBox->Size = System::Drawing::Size(104, 16);
			this->WorldVelocitycheckBox->TabIndex = 120;
			this->WorldVelocitycheckBox->Text = L"World Velocity";
			this->WorldVelocitycheckBox->UseVisualStyleBackColor = true;
			this->WorldVelocitycheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::WorldVelocitycheckBox_CheckedChanged);
			// 
			// EmitterStartTimeNumericUpDown
			// 
			this->EmitterStartTimeNumericUpDown->DecimalPlaces = 1;
			this->EmitterStartTimeNumericUpDown->Location = System::Drawing::Point(84, 42);
			this->EmitterStartTimeNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
			this->EmitterStartTimeNumericUpDown->Name = L"EmitterStartTimeNumericUpDown";
			this->EmitterStartTimeNumericUpDown->Size = System::Drawing::Size(120, 21);
			this->EmitterStartTimeNumericUpDown->TabIndex = 118;
			this->EmitterStartTimeNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterStartTimeNumericUpDown_ValueChanged);
			// 
			// EMITTER_DISTORTION_PROPERTY_BUTTON
			// 
			this->EMITTER_DISTORTION_PROPERTY_BUTTON->Location = System::Drawing::Point(7, 266);
			this->EMITTER_DISTORTION_PROPERTY_BUTTON->Name = L"EMITTER_DISTORTION_PROPERTY_BUTTON";
			this->EMITTER_DISTORTION_PROPERTY_BUTTON->Size = System::Drawing::Size(108, 23);
			this->EMITTER_DISTORTION_PROPERTY_BUTTON->TabIndex = 116;
			this->EMITTER_DISTORTION_PROPERTY_BUTTON->Text = L"Distortion 속성";
			this->EMITTER_DISTORTION_PROPERTY_BUTTON->UseVisualStyleBackColor = true;
			this->EMITTER_DISTORTION_PROPERTY_BUTTON->Click += gcnew System::EventHandler(this, &EffectTabForm::EMITTER_DISTORTION_PROPERTY_BUTTON_Click);
			// 
			// EmitterDistortioncheckBox
			// 
			this->EmitterDistortioncheckBox->AutoSize = true;
			this->EmitterDistortioncheckBox->Location = System::Drawing::Point(9, 135);
			this->EmitterDistortioncheckBox->Name = L"EmitterDistortioncheckBox";
			this->EmitterDistortioncheckBox->Size = System::Drawing::Size(76, 16);
			this->EmitterDistortioncheckBox->TabIndex = 60;
			this->EmitterDistortioncheckBox->Text = L"Distortion";
			this->EmitterDistortioncheckBox->UseVisualStyleBackColor = true;
			this->EmitterDistortioncheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterDistortioncheckBox_CheckedChanged);
			// 
			// FollowParentRotationCheckBox
			// 
			this->FollowParentRotationCheckBox->AutoSize = true;
			this->FollowParentRotationCheckBox->Location = System::Drawing::Point(9, 199);
			this->FollowParentRotationCheckBox->Name = L"FollowParentRotationCheckBox";
			this->FollowParentRotationCheckBox->Size = System::Drawing::Size(106, 16);
			this->FollowParentRotationCheckBox->TabIndex = 59;
			this->FollowParentRotationCheckBox->Text = L"FollowRotation";
			this->FollowParentRotationCheckBox->UseVisualStyleBackColor = true;
			this->FollowParentRotationCheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::FollowParentRotationCheckBox_CheckedChanged);
			// 
			// label40
			// 
			this->label40->AutoSize = true;
			this->label40->Location = System::Drawing::Point(32, 241);
			this->label40->Name = L"label40";
			this->label40->Size = System::Drawing::Size(43, 12);
			this->label40->TabIndex = 58;
			this->label40->Text = L"/frame";
			// 
			// FixAmountcheckBox
			// 
			this->FixAmountcheckBox->AutoSize = true;
			this->FixAmountcheckBox->Location = System::Drawing::Point(109, 135);
			this->FixAmountcheckBox->Name = L"FixAmountcheckBox";
			this->FixAmountcheckBox->Size = System::Drawing::Size(84, 16);
			this->FixAmountcheckBox->TabIndex = 57;
			this->FixAmountcheckBox->Text = L"FixAmount";
			this->FixAmountcheckBox->UseVisualStyleBackColor = true;
			this->FixAmountcheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::FixAmountcheckBox_CheckedChanged);
			// 
			// withmecheckBox
			// 
			this->withmecheckBox->AutoSize = true;
			this->withmecheckBox->Location = System::Drawing::Point(9, 179);
			this->withmecheckBox->Name = L"withmecheckBox";
			this->withmecheckBox->Size = System::Drawing::Size(65, 16);
			this->withmecheckBox->TabIndex = 56;
			this->withmecheckBox->Text = L"WithMe";
			this->withmecheckBox->UseVisualStyleBackColor = true;
			this->withmecheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::withmecheckBox_CheckedChanged);
			// 
			// label22
			// 
			this->label22->AutoSize = true;
			this->label22->Location = System::Drawing::Point(110, 224);
			this->label22->Name = L"label22";
			this->label22->Size = System::Drawing::Size(30, 12);
			this->label22->TabIndex = 55;
			this->label22->Text = L"Min:";
			// 
			// label21
			// 
			this->label21->AutoSize = true;
			this->label21->Location = System::Drawing::Point(161, 224);
			this->label21->Name = L"label21";
			this->label21->Size = System::Drawing::Size(51, 12);
			this->label21->TabIndex = 54;
			this->label21->Text = L"~ Max :";
			// 
			// RotationMaxnumericUpDown
			// 
			this->RotationMaxnumericUpDown->Location = System::Drawing::Point(150, 239);
			this->RotationMaxnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, 0});
			this->RotationMaxnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, System::Int32::MinValue});
			this->RotationMaxnumericUpDown->Name = L"RotationMaxnumericUpDown";
			this->RotationMaxnumericUpDown->Size = System::Drawing::Size(62, 21);
			this->RotationMaxnumericUpDown->TabIndex = 10;
			this->RotationMaxnumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, 0});
			this->RotationMaxnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::RotationMaxnumericUpDown_ValueChanged);
			// 
			// RotationMinnumericUpDown
			// 
			this->RotationMinnumericUpDown->Location = System::Drawing::Point(79, 239);
			this->RotationMinnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, 0});
			this->RotationMinnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, System::Int32::MinValue});
			this->RotationMinnumericUpDown->Name = L"RotationMinnumericUpDown";
			this->RotationMinnumericUpDown->Size = System::Drawing::Size(65, 21);
			this->RotationMinnumericUpDown->TabIndex = 9;
			this->RotationMinnumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, System::Int32::MinValue});
			this->RotationMinnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::RotationMinnumericUpDown_ValueChanged);
			// 
			// EmitterLifefnumericUpDown
			// 
			this->EmitterLifefnumericUpDown->Location = System::Drawing::Point(84, 13);
			this->EmitterLifefnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
			this->EmitterLifefnumericUpDown->Name = L"EmitterLifefnumericUpDown";
			this->EmitterLifefnumericUpDown->Size = System::Drawing::Size(120, 21);
			this->EmitterLifefnumericUpDown->TabIndex = 1;
			this->EmitterLifefnumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {90, 0, 0, 0});
			this->EmitterLifefnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterLifefnumericUpDown_ValueChanged);
			// 
			// RandomVelcheckBox
			// 
			this->RandomVelcheckBox->AutoSize = true;
			this->RandomVelcheckBox->Location = System::Drawing::Point(109, 91);
			this->RandomVelcheckBox->Name = L"RandomVelcheckBox";
			this->RandomVelcheckBox->Size = System::Drawing::Size(116, 16);
			this->RandomVelcheckBox->TabIndex = 5;
			this->RandomVelcheckBox->Text = L"VelocityRandom";
			this->RandomVelcheckBox->UseVisualStyleBackColor = true;
			this->RandomVelcheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::RandomVelcheckBox_CheckedChanged);
			// 
			// EmitterDepSortcheckBox
			// 
			this->EmitterDepSortcheckBox->AutoSize = true;
			this->EmitterDepSortcheckBox->Location = System::Drawing::Point(109, 69);
			this->EmitterDepSortcheckBox->Name = L"EmitterDepSortcheckBox";
			this->EmitterDepSortcheckBox->Size = System::Drawing::Size(95, 16);
			this->EmitterDepSortcheckBox->TabIndex = 3;
			this->EmitterDepSortcheckBox->Text = L"DepthSorting";
			this->EmitterDepSortcheckBox->UseVisualStyleBackColor = true;
			this->EmitterDepSortcheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterDepSortcheckBox_CheckedChanged);
			// 
			// RotationcheckBox
			// 
			this->RotationcheckBox->AutoSize = true;
			this->RotationcheckBox->Location = System::Drawing::Point(9, 223);
			this->RotationcheckBox->Name = L"RotationcheckBox";
			this->RotationcheckBox->Size = System::Drawing::Size(69, 16);
			this->RotationcheckBox->TabIndex = 8;
			this->RotationcheckBox->Text = L"Rotation";
			this->RotationcheckBox->UseVisualStyleBackColor = true;
			this->RotationcheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::RotationcheckBox_CheckedChanged);
			// 
			// ExplosioncheckBox
			// 
			this->ExplosioncheckBox->AutoSize = true;
			this->ExplosioncheckBox->Location = System::Drawing::Point(109, 157);
			this->ExplosioncheckBox->Name = L"ExplosioncheckBox";
			this->ExplosioncheckBox->Size = System::Drawing::Size(80, 16);
			this->ExplosioncheckBox->TabIndex = 7;
			this->ExplosioncheckBox->Text = L"Explosion";
			this->ExplosioncheckBox->UseVisualStyleBackColor = true;
			this->ExplosioncheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::ExplosioncheckBox_CheckedChanged);
			// 
			// GathercheckBox
			// 
			this->GathercheckBox->AutoSize = true;
			this->GathercheckBox->Location = System::Drawing::Point(9, 157);
			this->GathercheckBox->Name = L"GathercheckBox";
			this->GathercheckBox->Size = System::Drawing::Size(61, 16);
			this->GathercheckBox->TabIndex = 6;
			this->GathercheckBox->Text = L"Gather";
			this->GathercheckBox->UseVisualStyleBackColor = true;
			this->GathercheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::GathercheckBox_CheckedChanged);
			// 
			// RandomColorcheckBox
			// 
			this->RandomColorcheckBox->AutoSize = true;
			this->RandomColorcheckBox->Location = System::Drawing::Point(9, 91);
			this->RandomColorcheckBox->Name = L"RandomColorcheckBox";
			this->RandomColorcheckBox->Size = System::Drawing::Size(101, 16);
			this->RandomColorcheckBox->TabIndex = 4;
			this->RandomColorcheckBox->Text = L"ColorRandom";
			this->RandomColorcheckBox->UseVisualStyleBackColor = true;
			this->RandomColorcheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::RandomColorcheckBox_CheckedChanged);
			// 
			// EmitterLoopcheckBox
			// 
			this->EmitterLoopcheckBox->AutoSize = true;
			this->EmitterLoopcheckBox->Location = System::Drawing::Point(9, 71);
			this->EmitterLoopcheckBox->Name = L"EmitterLoopcheckBox";
			this->EmitterLoopcheckBox->Size = System::Drawing::Size(52, 16);
			this->EmitterLoopcheckBox->TabIndex = 2;
			this->EmitterLoopcheckBox->Text = L"Loop";
			this->EmitterLoopcheckBox->UseVisualStyleBackColor = true;
			this->EmitterLoopcheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterLoopcheckBox_CheckedChanged);
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->BillboardTypeComboBox);
			this->groupBox6->Location = System::Drawing::Point(3, 306);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(224, 73);
			this->groupBox6->TabIndex = 49;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = L"Particle Billboard type";
			// 
			// BillboardTypeComboBox
			// 
			this->BillboardTypeComboBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->BillboardTypeComboBox->Items->AddRange(gcnew cli::array< System::Object^  >(10) {L"Billboard", L"Vertical", L"Billboard Align XY", 
				L"Billboard Align YZ", L"Billboard Align ZX", L"Spread", L"Spread Align XY", L"Spread Align YZ", L"Spread Align ZX", L"Emitter Align"});
			this->BillboardTypeComboBox->Location = System::Drawing::Point(10, 31);
			this->BillboardTypeComboBox->Name = L"BillboardTypeComboBox";
			this->BillboardTypeComboBox->Size = System::Drawing::Size(194, 20);
			this->BillboardTypeComboBox->TabIndex = 6;
			this->BillboardTypeComboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &EffectTabForm::BillboardTypeComboBox_SelectedIndexChanged);
			// 
			// groupBox5
			// 
			this->groupBox5->Controls->Add(this->EmitterVelocityMaxUpDown);
			this->groupBox5->Controls->Add(this->EmitterVelocityMinUpDown);
			this->groupBox5->Controls->Add(this->label3);
			this->groupBox5->Controls->Add(this->label32);
			this->groupBox5->Controls->Add(this->label33);
			this->groupBox5->Controls->Add(this->label34);
			this->groupBox5->Controls->Add(this->label29);
			this->groupBox5->Controls->Add(this->label30);
			this->groupBox5->Controls->Add(this->label31);
			this->groupBox5->Controls->Add(this->EmitterTimeDelbutton);
			this->groupBox5->Controls->Add(this->EmitterTimeAddbutton);
			this->groupBox5->Controls->Add(this->EmitterTimeAnglenumericUpDown);
			this->groupBox5->Controls->Add(this->EmitterTimeRangeZnumericUpDown);
			this->groupBox5->Controls->Add(this->EmitterTimeRangeYnumericUpDown);
			this->groupBox5->Controls->Add(this->EmitterTimeRangeXnumericUpDown);
			this->groupBox5->Controls->Add(this->EmitterTimePositionZnumericUpDown);
			this->groupBox5->Controls->Add(this->EmitterTimePositionYnumericUpDown);
			this->groupBox5->Controls->Add(this->EmitterTimePositionXnumericUpDown);
			this->groupBox5->Controls->Add(this->EmitterTimeAmountnumericUpDown);
			this->groupBox5->Controls->Add(this->EmitterTimePosnumericUpDown);
			this->groupBox5->Controls->Add(this->EmitterTimrTracklistBox);
			this->groupBox5->Controls->Add(this->label1);
			this->groupBox5->Controls->Add(this->label2);
			this->groupBox5->Controls->Add(this->label10);
			this->groupBox5->Controls->Add(this->label8);
			this->groupBox5->Controls->Add(this->label9);
			this->groupBox5->Location = System::Drawing::Point(3, 385);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(225, 263);
			this->groupBox5->TabIndex = 50;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = L"EmitterTimeTrack";
			// 
			// EmitterVelocityMaxUpDown
			// 
			this->EmitterVelocityMaxUpDown->DecimalPlaces = 1;
			this->EmitterVelocityMaxUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->EmitterVelocityMaxUpDown->Location = System::Drawing::Point(150, 235);
			this->EmitterVelocityMaxUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000000, 0, 0, 0});
			this->EmitterVelocityMaxUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000000, 0, 0, System::Int32::MinValue});
			this->EmitterVelocityMaxUpDown->Name = L"EmitterVelocityMaxUpDown";
			this->EmitterVelocityMaxUpDown->Size = System::Drawing::Size(67, 21);
			this->EmitterVelocityMaxUpDown->TabIndex = 115;
			this->EmitterVelocityMaxUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterVelocityMaxUpDown_ValueChanged);
			// 
			// EmitterVelocityMinUpDown
			// 
			this->EmitterVelocityMinUpDown->DecimalPlaces = 1;
			this->EmitterVelocityMinUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->EmitterVelocityMinUpDown->Location = System::Drawing::Point(78, 235);
			this->EmitterVelocityMinUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000000, 0, 0, 0});
			this->EmitterVelocityMinUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000000, 0, 0, System::Int32::MinValue});
			this->EmitterVelocityMinUpDown->Name = L"EmitterVelocityMinUpDown";
			this->EmitterVelocityMinUpDown->Size = System::Drawing::Size(66, 21);
			this->EmitterVelocityMinUpDown->TabIndex = 114;
			this->EmitterVelocityMinUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterVelocityMinUpDown_ValueChanged);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(6, 237);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(50, 12);
			this->label3->TabIndex = 113;
			this->label3->Text = L"Velocity";
			// 
			// label32
			// 
			this->label32->AutoSize = true;
			this->label32->Font = (gcnew System::Drawing::Font(L"굴림", 8));
			this->label32->Location = System::Drawing::Point(148, 191);
			this->label32->Name = L"label32";
			this->label32->Size = System::Drawing::Size(20, 11);
			this->label32->TabIndex = 112;
			this->label32->Text = L"Z :";
			// 
			// label33
			// 
			this->label33->AutoSize = true;
			this->label33->Font = (gcnew System::Drawing::Font(L"굴림", 8));
			this->label33->Location = System::Drawing::Point(75, 191);
			this->label33->Name = L"label33";
			this->label33->Size = System::Drawing::Size(20, 11);
			this->label33->TabIndex = 111;
			this->label33->Text = L"Y :";
			// 
			// label34
			// 
			this->label34->AutoSize = true;
			this->label34->Font = (gcnew System::Drawing::Font(L"굴림", 8));
			this->label34->Location = System::Drawing::Point(2, 191);
			this->label34->Name = L"label34";
			this->label34->Size = System::Drawing::Size(20, 11);
			this->label34->TabIndex = 110;
			this->label34->Text = L"X :";
			// 
			// label29
			// 
			this->label29->AutoSize = true;
			this->label29->Font = (gcnew System::Drawing::Font(L"굴림", 8));
			this->label29->Location = System::Drawing::Point(148, 141);
			this->label29->Name = L"label29";
			this->label29->Size = System::Drawing::Size(20, 11);
			this->label29->TabIndex = 109;
			this->label29->Text = L"Z :";
			// 
			// label30
			// 
			this->label30->AutoSize = true;
			this->label30->Font = (gcnew System::Drawing::Font(L"굴림", 8));
			this->label30->Location = System::Drawing::Point(75, 141);
			this->label30->Name = L"label30";
			this->label30->Size = System::Drawing::Size(20, 11);
			this->label30->TabIndex = 108;
			this->label30->Text = L"Y :";
			// 
			// label31
			// 
			this->label31->AutoSize = true;
			this->label31->Font = (gcnew System::Drawing::Font(L"굴림", 8));
			this->label31->Location = System::Drawing::Point(2, 141);
			this->label31->Name = L"label31";
			this->label31->Size = System::Drawing::Size(20, 11);
			this->label31->TabIndex = 107;
			this->label31->Text = L"X :";
			// 
			// EmitterTimeDelbutton
			// 
			this->EmitterTimeDelbutton->Location = System::Drawing::Point(62, 46);
			this->EmitterTimeDelbutton->Name = L"EmitterTimeDelbutton";
			this->EmitterTimeDelbutton->Size = System::Drawing::Size(47, 23);
			this->EmitterTimeDelbutton->TabIndex = 2;
			this->EmitterTimeDelbutton->Text = L"Del";
			this->EmitterTimeDelbutton->UseVisualStyleBackColor = true;
			this->EmitterTimeDelbutton->Click += gcnew System::EventHandler(this, &EffectTabForm::EmitterTimeDelbutton_Click);
			// 
			// EmitterTimeAddbutton
			// 
			this->EmitterTimeAddbutton->Location = System::Drawing::Point(62, 17);
			this->EmitterTimeAddbutton->Name = L"EmitterTimeAddbutton";
			this->EmitterTimeAddbutton->Size = System::Drawing::Size(47, 23);
			this->EmitterTimeAddbutton->TabIndex = 1;
			this->EmitterTimeAddbutton->Text = L"Add";
			this->EmitterTimeAddbutton->UseVisualStyleBackColor = true;
			this->EmitterTimeAddbutton->Click += gcnew System::EventHandler(this, &EffectTabForm::EmitterTimeAddbutton_Click);
			// 
			// EmitterTimeAnglenumericUpDown
			// 
			this->EmitterTimeAnglenumericUpDown->Location = System::Drawing::Point(133, 108);
			this->EmitterTimeAnglenumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {360, 0, 0, 0});
			this->EmitterTimeAnglenumericUpDown->Name = L"EmitterTimeAnglenumericUpDown";
			this->EmitterTimeAnglenumericUpDown->Size = System::Drawing::Size(71, 21);
			this->EmitterTimeAnglenumericUpDown->TabIndex = 5;
			this->EmitterTimeAnglenumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {360, 0, 0, 0});
			this->EmitterTimeAnglenumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterTimeAnglenumericUpDown_ValueChanged);
			// 
			// EmitterTimeRangeZnumericUpDown
			// 
			this->EmitterTimeRangeZnumericUpDown->DecimalPlaces = 2;
			this->EmitterTimeRangeZnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->EmitterTimeRangeZnumericUpDown->Location = System::Drawing::Point(150, 203);
			this->EmitterTimeRangeZnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200000000, 0, 0, 0});
			this->EmitterTimeRangeZnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200000000, 0, 0, System::Int32::MinValue});
			this->EmitterTimeRangeZnumericUpDown->Name = L"EmitterTimeRangeZnumericUpDown";
			this->EmitterTimeRangeZnumericUpDown->Size = System::Drawing::Size(67, 21);
			this->EmitterTimeRangeZnumericUpDown->TabIndex = 11;
			this->EmitterTimeRangeZnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterTimeRangeZnumericUpDown_ValueChanged);
			// 
			// EmitterTimeRangeYnumericUpDown
			// 
			this->EmitterTimeRangeYnumericUpDown->DecimalPlaces = 2;
			this->EmitterTimeRangeYnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->EmitterTimeRangeYnumericUpDown->Location = System::Drawing::Point(77, 203);
			this->EmitterTimeRangeYnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200000000, 0, 0, 0});
			this->EmitterTimeRangeYnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200000000, 0, 0, System::Int32::MinValue});
			this->EmitterTimeRangeYnumericUpDown->Name = L"EmitterTimeRangeYnumericUpDown";
			this->EmitterTimeRangeYnumericUpDown->Size = System::Drawing::Size(67, 21);
			this->EmitterTimeRangeYnumericUpDown->TabIndex = 10;
			this->EmitterTimeRangeYnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterTimeRangeYnumericUpDown_ValueChanged);
			// 
			// EmitterTimeRangeXnumericUpDown
			// 
			this->EmitterTimeRangeXnumericUpDown->DecimalPlaces = 2;
			this->EmitterTimeRangeXnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->EmitterTimeRangeXnumericUpDown->Location = System::Drawing::Point(4, 203);
			this->EmitterTimeRangeXnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200000000, 0, 0, 0});
			this->EmitterTimeRangeXnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200000000, 0, 0, System::Int32::MinValue});
			this->EmitterTimeRangeXnumericUpDown->Name = L"EmitterTimeRangeXnumericUpDown";
			this->EmitterTimeRangeXnumericUpDown->Size = System::Drawing::Size(67, 21);
			this->EmitterTimeRangeXnumericUpDown->TabIndex = 9;
			this->EmitterTimeRangeXnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterTimeRangeXnumericUpDown_ValueChanged);
			// 
			// EmitterTimePositionZnumericUpDown
			// 
			this->EmitterTimePositionZnumericUpDown->DecimalPlaces = 2;
			this->EmitterTimePositionZnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->EmitterTimePositionZnumericUpDown->Location = System::Drawing::Point(150, 155);
			this->EmitterTimePositionZnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200000000, 0, 0, 
				0});
			this->EmitterTimePositionZnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200000000, 0, 0, 
				System::Int32::MinValue});
			this->EmitterTimePositionZnumericUpDown->Name = L"EmitterTimePositionZnumericUpDown";
			this->EmitterTimePositionZnumericUpDown->Size = System::Drawing::Size(67, 21);
			this->EmitterTimePositionZnumericUpDown->TabIndex = 8;
			this->EmitterTimePositionZnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterTimePositionZnumericUpDown_ValueChanged);
			// 
			// EmitterTimePositionYnumericUpDown
			// 
			this->EmitterTimePositionYnumericUpDown->DecimalPlaces = 2;
			this->EmitterTimePositionYnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->EmitterTimePositionYnumericUpDown->Location = System::Drawing::Point(77, 155);
			this->EmitterTimePositionYnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200000000, 0, 0, 
				0});
			this->EmitterTimePositionYnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200000000, 0, 0, 
				System::Int32::MinValue});
			this->EmitterTimePositionYnumericUpDown->Name = L"EmitterTimePositionYnumericUpDown";
			this->EmitterTimePositionYnumericUpDown->Size = System::Drawing::Size(67, 21);
			this->EmitterTimePositionYnumericUpDown->TabIndex = 7;
			this->EmitterTimePositionYnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterTimePositionYnumericUpDown_ValueChanged);
			// 
			// EmitterTimePositionXnumericUpDown
			// 
			this->EmitterTimePositionXnumericUpDown->DecimalPlaces = 2;
			this->EmitterTimePositionXnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->EmitterTimePositionXnumericUpDown->Location = System::Drawing::Point(4, 155);
			this->EmitterTimePositionXnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200000000, 0, 0, 
				0});
			this->EmitterTimePositionXnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200000000, 0, 0, 
				System::Int32::MinValue});
			this->EmitterTimePositionXnumericUpDown->Name = L"EmitterTimePositionXnumericUpDown";
			this->EmitterTimePositionXnumericUpDown->Size = System::Drawing::Size(67, 21);
			this->EmitterTimePositionXnumericUpDown->TabIndex = 6;
			this->EmitterTimePositionXnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterTimePositionXnumericUpDown_ValueChanged);
			// 
			// EmitterTimeAmountnumericUpDown
			// 
			this->EmitterTimeAmountnumericUpDown->Location = System::Drawing::Point(133, 69);
			this->EmitterTimeAmountnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->EmitterTimeAmountnumericUpDown->Name = L"EmitterTimeAmountnumericUpDown";
			this->EmitterTimeAmountnumericUpDown->Size = System::Drawing::Size(71, 21);
			this->EmitterTimeAmountnumericUpDown->TabIndex = 4;
			this->EmitterTimeAmountnumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->EmitterTimeAmountnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterTimeAmountnumericUpDown_ValueChanged);
			// 
			// EmitterTimePosnumericUpDown
			// 
			this->EmitterTimePosnumericUpDown->DecimalPlaces = 2;
			this->EmitterTimePosnumericUpDown->Location = System::Drawing::Point(133, 31);
			this->EmitterTimePosnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
			this->EmitterTimePosnumericUpDown->Name = L"EmitterTimePosnumericUpDown";
			this->EmitterTimePosnumericUpDown->Size = System::Drawing::Size(71, 21);
			this->EmitterTimePosnumericUpDown->TabIndex = 3;
			this->EmitterTimePosnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterTimePosnumericUpDown_ValueChanged);
			// 
			// EmitterTimrTracklistBox
			// 
			this->EmitterTimrTracklistBox->FormattingEnabled = true;
			this->EmitterTimrTracklistBox->ItemHeight = 12;
			this->EmitterTimrTracklistBox->Location = System::Drawing::Point(6, 17);
			this->EmitterTimrTracklistBox->Name = L"EmitterTimrTracklistBox";
			this->EmitterTimrTracklistBox->SelectionMode = System::Windows::Forms::SelectionMode::MultiExtended;
			this->EmitterTimrTracklistBox->Size = System::Drawing::Size(54, 112);
			this->EmitterTimrTracklistBox->TabIndex = 0;
			this->EmitterTimrTracklistBox->SelectedIndexChanged += gcnew System::EventHandler(this, &EffectTabForm::EmitterTimrTracklistBox_SelectedIndexChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(113, 16);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(91, 12);
			this->label1->TabIndex = 22;
			this->label1->Text = L"Time Position :";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(116, 54);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(88, 12);
			this->label2->TabIndex = 23;
			this->label2->Text = L"Amount / Sec:";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(167, 93);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(37, 12);
			this->label10->TabIndex = 43;
			this->label10->Text = L"Angle";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(6, 131);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(58, 12);
			this->label8->TabIndex = 33;
			this->label8->Text = L"Position :";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(6, 179);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(84, 12);
			this->label9->TabIndex = 12;
			this->label9->Text = L"EmitterRange:";
			// 
			// ParticletabPage
			// 
			this->ParticletabPage->AutoScroll = true;
			this->ParticletabPage->Controls->Add(this->groupBox4);
			this->ParticletabPage->Controls->Add(this->groupBox7);
			this->ParticletabPage->Location = System::Drawing::Point(4, 40);
			this->ParticletabPage->Name = L"ParticletabPage";
			this->ParticletabPage->Padding = System::Windows::Forms::Padding(3);
			this->ParticletabPage->Size = System::Drawing::Size(272, 656);
			this->ParticletabPage->TabIndex = 2;
			this->ParticletabPage->Text = L"Particle";
			this->ParticletabPage->UseVisualStyleBackColor = true;
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->ParticleVariableYNumericUpDown);
			this->groupBox4->Controls->Add(this->ParticleScaleLock);
			this->groupBox4->Controls->Add(this->label38);
			this->groupBox4->Controls->Add(this->ParticleVariableNumericUpDown1);
			this->groupBox4->Controls->Add(this->ParticleTimeRefineAddbutton);
			this->groupBox4->Controls->Add(this->label36);
			this->groupBox4->Controls->Add(this->ParticleTimeScaleVarnumericUpDown);
			this->groupBox4->Controls->Add(this->VisibilitynumericUpDown);
			this->groupBox4->Controls->Add(this->label35);
			this->groupBox4->Controls->Add(this->label25);
			this->groupBox4->Controls->Add(this->label24);
			this->groupBox4->Controls->Add(this->label23);
			this->groupBox4->Controls->Add(this->ParticleTimeDelbutton);
			this->groupBox4->Controls->Add(this->ParticleTimeVelMaxnumericUpDown);
			this->groupBox4->Controls->Add(this->ParticleLifemaxnumericUpDown);
			this->groupBox4->Controls->Add(this->ColorSelectbutton);
			this->groupBox4->Controls->Add(this->label19);
			this->groupBox4->Controls->Add(this->ParticleLifeMinnumericUpDown);
			this->groupBox4->Controls->Add(this->label17);
			this->groupBox4->Controls->Add(this->ParticleTimeGRatenumericUpDown);
			this->groupBox4->Controls->Add(this->label16);
			this->groupBox4->Controls->Add(this->ParticleTimeVelMinnumericUpDown);
			this->groupBox4->Controls->Add(this->label14);
			this->groupBox4->Controls->Add(this->label15);
			this->groupBox4->Controls->Add(this->ParticleTimeVelDirZnumericUpDown);
			this->groupBox4->Controls->Add(this->ParticleTimeVelDirYnumericUpDown);
			this->groupBox4->Controls->Add(this->ParticleTimeVelDirXnumericUpDown);
			this->groupBox4->Controls->Add(this->ParticleTimeScaleYnumericUpDown);
			this->groupBox4->Controls->Add(this->ParticleTimeScaleXnumericUpDown);
			this->groupBox4->Controls->Add(this->label13);
			this->groupBox4->Controls->Add(this->ParticleTimeAddbutton);
			this->groupBox4->Controls->Add(this->ParticleTimePosnumericUpDown);
			this->groupBox4->Controls->Add(this->ParticleTimeTracklistBox);
			this->groupBox4->Controls->Add(this->label7);
			this->groupBox4->Location = System::Drawing::Point(3, 204);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(229, 389);
			this->groupBox4->TabIndex = 99;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Particle";
			// 
			// ParticleVariableYNumericUpDown
			// 
			this->ParticleVariableYNumericUpDown->DecimalPlaces = 1;
			this->ParticleVariableYNumericUpDown->Enabled = false;
			this->ParticleVariableYNumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->ParticleVariableYNumericUpDown->Location = System::Drawing::Point(74, 334);
			this->ParticleVariableYNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5000, 0, 0, 0});
			this->ParticleVariableYNumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5000, 0, 0, System::Int32::MinValue});
			this->ParticleVariableYNumericUpDown->Name = L"ParticleVariableYNumericUpDown";
			this->ParticleVariableYNumericUpDown->Size = System::Drawing::Size(67, 21);
			this->ParticleVariableYNumericUpDown->TabIndex = 117;
			this->ParticleVariableYNumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->ParticleVariableYNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleVariableYNumericUpDown_ValueChanged);
			// 
			// ParticleScaleLock
			// 
			this->ParticleScaleLock->AutoSize = true;
			this->ParticleScaleLock->Checked = true;
			this->ParticleScaleLock->CheckState = System::Windows::Forms::CheckState::Checked;
			this->ParticleScaleLock->Location = System::Drawing::Point(6, 361);
			this->ParticleScaleLock->Name = L"ParticleScaleLock";
			this->ParticleScaleLock->Size = System::Drawing::Size(51, 16);
			this->ParticleScaleLock->TabIndex = 116;
			this->ParticleScaleLock->Text = L"Lock";
			this->ParticleScaleLock->UseVisualStyleBackColor = true;
			this->ParticleScaleLock->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleScaleLock_CheckedChanged);
			// 
			// label38
			// 
			this->label38->AutoSize = true;
			this->label38->Location = System::Drawing::Point(72, 292);
			this->label38->Name = L"label38";
			this->label38->Size = System::Drawing::Size(79, 12);
			this->label38->TabIndex = 115;
			this->label38->Text = L"(0~Variation)";
			// 
			// ParticleVariableNumericUpDown1
			// 
			this->ParticleVariableNumericUpDown1->DecimalPlaces = 1;
			this->ParticleVariableNumericUpDown1->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->ParticleVariableNumericUpDown1->Location = System::Drawing::Point(74, 307);
			this->ParticleVariableNumericUpDown1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5000, 0, 0, 0});
			this->ParticleVariableNumericUpDown1->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5000, 0, 0, System::Int32::MinValue});
			this->ParticleVariableNumericUpDown1->Name = L"ParticleVariableNumericUpDown1";
			this->ParticleVariableNumericUpDown1->Size = System::Drawing::Size(67, 21);
			this->ParticleVariableNumericUpDown1->TabIndex = 114;
			this->ParticleVariableNumericUpDown1->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->ParticleVariableNumericUpDown1->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleVariableNumericUpDown1_ValueChanged);
			// 
			// ParticleTimeRefineAddbutton
			// 
			this->ParticleTimeRefineAddbutton->Location = System::Drawing::Point(59, 78);
			this->ParticleTimeRefineAddbutton->Name = L"ParticleTimeRefineAddbutton";
			this->ParticleTimeRefineAddbutton->Size = System::Drawing::Size(49, 23);
			this->ParticleTimeRefineAddbutton->TabIndex = 113;
			this->ParticleTimeRefineAddbutton->Text = L"reAdd";
			this->ParticleTimeRefineAddbutton->UseVisualStyleBackColor = true;
			this->ParticleTimeRefineAddbutton->Click += gcnew System::EventHandler(this, &EffectTabForm::ParticleTimeRefineAddbutton_Click);
			// 
			// label36
			// 
			this->label36->AutoSize = true;
			this->label36->Location = System::Drawing::Point(159, 292);
			this->label36->Name = L"label36";
			this->label36->Size = System::Drawing::Size(48, 12);
			this->label36->TabIndex = 112;
			this->label36->Text = L"(Noise)";
			// 
			// ParticleTimeScaleVarnumericUpDown
			// 
			this->ParticleTimeScaleVarnumericUpDown->DecimalPlaces = 1;
			this->ParticleTimeScaleVarnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->ParticleTimeScaleVarnumericUpDown->Location = System::Drawing::Point(157, 307);
			this->ParticleTimeScaleVarnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5000, 0, 0, 0});
			this->ParticleTimeScaleVarnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5000, 0, 0, System::Int32::MinValue});
			this->ParticleTimeScaleVarnumericUpDown->Name = L"ParticleTimeScaleVarnumericUpDown";
			this->ParticleTimeScaleVarnumericUpDown->Size = System::Drawing::Size(60, 21);
			this->ParticleTimeScaleVarnumericUpDown->TabIndex = 111;
			this->ParticleTimeScaleVarnumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->ParticleTimeScaleVarnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleTimeScaleVarnumericUpDown_ValueChanged);
			// 
			// VisibilitynumericUpDown
			// 
			this->VisibilitynumericUpDown->DecimalPlaces = 2;
			this->VisibilitynumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
			this->VisibilitynumericUpDown->Location = System::Drawing::Point(127, 147);
			this->VisibilitynumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->VisibilitynumericUpDown->Name = L"VisibilitynumericUpDown";
			this->VisibilitynumericUpDown->Size = System::Drawing::Size(96, 21);
			this->VisibilitynumericUpDown->TabIndex = 9;
			this->VisibilitynumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->VisibilitynumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::VisibilitynumericUpDown_ValueChanged);
			// 
			// label35
			// 
			this->label35->AutoSize = true;
			this->label35->Location = System::Drawing::Point(126, 132);
			this->label35->Name = L"label35";
			this->label35->Size = System::Drawing::Size(58, 12);
			this->label35->TabIndex = 110;
			this->label35->Text = L"visibility :";
			// 
			// label25
			// 
			this->label25->AutoSize = true;
			this->label25->Font = (gcnew System::Drawing::Font(L"굴림", 8));
			this->label25->Location = System::Drawing::Point(145, 222);
			this->label25->Name = L"label25";
			this->label25->Size = System::Drawing::Size(20, 11);
			this->label25->TabIndex = 106;
			this->label25->Text = L"Z :";
			// 
			// label24
			// 
			this->label24->AutoSize = true;
			this->label24->Font = (gcnew System::Drawing::Font(L"굴림", 8));
			this->label24->Location = System::Drawing::Point(72, 222);
			this->label24->Name = L"label24";
			this->label24->Size = System::Drawing::Size(20, 11);
			this->label24->TabIndex = 105;
			this->label24->Text = L"Y :";
			// 
			// label23
			// 
			this->label23->AutoSize = true;
			this->label23->Font = (gcnew System::Drawing::Font(L"굴림", 8));
			this->label23->Location = System::Drawing::Point(1, 222);
			this->label23->Name = L"label23";
			this->label23->Size = System::Drawing::Size(20, 11);
			this->label23->TabIndex = 104;
			this->label23->Text = L"X :";
			// 
			// ParticleTimeDelbutton
			// 
			this->ParticleTimeDelbutton->Location = System::Drawing::Point(59, 105);
			this->ParticleTimeDelbutton->Name = L"ParticleTimeDelbutton";
			this->ParticleTimeDelbutton->Size = System::Drawing::Size(49, 23);
			this->ParticleTimeDelbutton->TabIndex = 5;
			this->ParticleTimeDelbutton->Text = L"Del";
			this->ParticleTimeDelbutton->UseVisualStyleBackColor = true;
			this->ParticleTimeDelbutton->Click += gcnew System::EventHandler(this, &EffectTabForm::ParticleTimeDelbutton_Click);
			// 
			// ParticleTimeVelMaxnumericUpDown
			// 
			this->ParticleTimeVelMaxnumericUpDown->DecimalPlaces = 1;
			this->ParticleTimeVelMaxnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->ParticleTimeVelMaxnumericUpDown->Location = System::Drawing::Point(164, 174);
			this->ParticleTimeVelMaxnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
			this->ParticleTimeVelMaxnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, System::Int32::MinValue});
			this->ParticleTimeVelMaxnumericUpDown->Name = L"ParticleTimeVelMaxnumericUpDown";
			this->ParticleTimeVelMaxnumericUpDown->Size = System::Drawing::Size(59, 21);
			this->ParticleTimeVelMaxnumericUpDown->TabIndex = 11;
			this->ParticleTimeVelMaxnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleTimeVelMaxnumericUpDown_ValueChanged);
			// 
			// ParticleLifemaxnumericUpDown
			// 
			this->ParticleLifemaxnumericUpDown->Location = System::Drawing::Point(157, 20);
			this->ParticleLifemaxnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {3000000, 0, 0, 0});
			this->ParticleLifemaxnumericUpDown->Name = L"ParticleLifemaxnumericUpDown";
			this->ParticleLifemaxnumericUpDown->Size = System::Drawing::Size(60, 21);
			this->ParticleLifemaxnumericUpDown->TabIndex = 2;
			this->ParticleLifemaxnumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
			this->ParticleLifemaxnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleLifemaxnumericUpDown_ValueChanged);
			// 
			// ColorSelectbutton
			// 
			this->ColorSelectbutton->Location = System::Drawing::Point(164, 100);
			this->ColorSelectbutton->Name = L"ColorSelectbutton";
			this->ColorSelectbutton->Size = System::Drawing::Size(59, 23);
			this->ColorSelectbutton->TabIndex = 7;
			this->ColorSelectbutton->Text = L"Color";
			this->ColorSelectbutton->UseVisualStyleBackColor = true;
			this->ColorSelectbutton->Click += gcnew System::EventHandler(this, &EffectTabForm::ColorSelectbutton_Click);
			// 
			// label19
			// 
			this->label19->AutoSize = true;
			this->label19->Location = System::Drawing::Point(4, 22);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(79, 12);
			this->label19->TabIndex = 98;
			this->label19->Text = L"Particle Life :";
			// 
			// ParticleLifeMinnumericUpDown
			// 
			this->ParticleLifeMinnumericUpDown->Location = System::Drawing::Point(95, 20);
			this->ParticleLifeMinnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
			this->ParticleLifeMinnumericUpDown->Name = L"ParticleLifeMinnumericUpDown";
			this->ParticleLifeMinnumericUpDown->Size = System::Drawing::Size(60, 21);
			this->ParticleLifeMinnumericUpDown->TabIndex = 1;
			this->ParticleLifeMinnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleLifeMinnumericUpDown_ValueChanged);
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Location = System::Drawing::Point(117, 105);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(43, 12);
			this->label17->TabIndex = 96;
			this->label17->Text = L"Color :";
			// 
			// ParticleTimeGRatenumericUpDown
			// 
			this->ParticleTimeGRatenumericUpDown->DecimalPlaces = 2;
			this->ParticleTimeGRatenumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->ParticleTimeGRatenumericUpDown->Location = System::Drawing::Point(147, 263);
			this->ParticleTimeGRatenumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
			this->ParticleTimeGRatenumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, System::Int32::MinValue});
			this->ParticleTimeGRatenumericUpDown->Name = L"ParticleTimeGRatenumericUpDown";
			this->ParticleTimeGRatenumericUpDown->Size = System::Drawing::Size(76, 21);
			this->ParticleTimeGRatenumericUpDown->TabIndex = 17;
			this->ParticleTimeGRatenumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {983, 0, 0, 131072});
			this->ParticleTimeGRatenumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleTimeGRatenumericUpDown_ValueChanged);
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Location = System::Drawing::Point(88, 265);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(53, 12);
			this->label16->TabIndex = 93;
			this->label16->Text = L"G-Rate :";
			// 
			// ParticleTimeVelMinnumericUpDown
			// 
			this->ParticleTimeVelMinnumericUpDown->DecimalPlaces = 1;
			this->ParticleTimeVelMinnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->ParticleTimeVelMinnumericUpDown->Location = System::Drawing::Point(101, 174);
			this->ParticleTimeVelMinnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
			this->ParticleTimeVelMinnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, System::Int32::MinValue});
			this->ParticleTimeVelMinnumericUpDown->Name = L"ParticleTimeVelMinnumericUpDown";
			this->ParticleTimeVelMinnumericUpDown->Size = System::Drawing::Size(59, 21);
			this->ParticleTimeVelMinnumericUpDown->TabIndex = 10;
			this->ParticleTimeVelMinnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleTimeVelMinnumericUpDown_ValueChanged);
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Location = System::Drawing::Point(6, 176);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(100, 12);
			this->label14->TabIndex = 91;
			this->label14->Text = L"Local Force세기:";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Location = System::Drawing::Point(1, 207);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(98, 12);
			this->label15->TabIndex = 12;
			this->label15->Text = L"Force Direction :";
			// 
			// ParticleTimeVelDirZnumericUpDown
			// 
			this->ParticleTimeVelDirZnumericUpDown->DecimalPlaces = 2;
			this->ParticleTimeVelDirZnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->ParticleTimeVelDirZnumericUpDown->Location = System::Drawing::Point(155, 236);
			this->ParticleTimeVelDirZnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->ParticleTimeVelDirZnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, System::Int32::MinValue});
			this->ParticleTimeVelDirZnumericUpDown->Name = L"ParticleTimeVelDirZnumericUpDown";
			this->ParticleTimeVelDirZnumericUpDown->Size = System::Drawing::Size(67, 21);
			this->ParticleTimeVelDirZnumericUpDown->TabIndex = 16;
			this->ParticleTimeVelDirZnumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->ParticleTimeVelDirZnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleTimeVelDirZnumericUpDown_ValueChanged);
			// 
			// ParticleTimeVelDirYnumericUpDown
			// 
			this->ParticleTimeVelDirYnumericUpDown->DecimalPlaces = 2;
			this->ParticleTimeVelDirYnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->ParticleTimeVelDirYnumericUpDown->Location = System::Drawing::Point(78, 236);
			this->ParticleTimeVelDirYnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->ParticleTimeVelDirYnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, System::Int32::MinValue});
			this->ParticleTimeVelDirYnumericUpDown->Name = L"ParticleTimeVelDirYnumericUpDown";
			this->ParticleTimeVelDirYnumericUpDown->Size = System::Drawing::Size(67, 21);
			this->ParticleTimeVelDirYnumericUpDown->TabIndex = 14;
			this->ParticleTimeVelDirYnumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->ParticleTimeVelDirYnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleTimeVelDirYnumericUpDown_ValueChanged);
			// 
			// ParticleTimeVelDirXnumericUpDown
			// 
			this->ParticleTimeVelDirXnumericUpDown->DecimalPlaces = 2;
			this->ParticleTimeVelDirXnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->ParticleTimeVelDirXnumericUpDown->Location = System::Drawing::Point(1, 236);
			this->ParticleTimeVelDirXnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->ParticleTimeVelDirXnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, System::Int32::MinValue});
			this->ParticleTimeVelDirXnumericUpDown->Name = L"ParticleTimeVelDirXnumericUpDown";
			this->ParticleTimeVelDirXnumericUpDown->Size = System::Drawing::Size(67, 21);
			this->ParticleTimeVelDirXnumericUpDown->TabIndex = 13;
			this->ParticleTimeVelDirXnumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->ParticleTimeVelDirXnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleTimeVelDirXnumericUpDown_ValueChanged);
			// 
			// ParticleTimeScaleYnumericUpDown
			// 
			this->ParticleTimeScaleYnumericUpDown->DecimalPlaces = 1;
			this->ParticleTimeScaleYnumericUpDown->Enabled = false;
			this->ParticleTimeScaleYnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->ParticleTimeScaleYnumericUpDown->Location = System::Drawing::Point(1, 334);
			this->ParticleTimeScaleYnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5000, 0, 0, 0});
			this->ParticleTimeScaleYnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5000, 0, 0, System::Int32::MinValue});
			this->ParticleTimeScaleYnumericUpDown->Name = L"ParticleTimeScaleYnumericUpDown";
			this->ParticleTimeScaleYnumericUpDown->Size = System::Drawing::Size(61, 21);
			this->ParticleTimeScaleYnumericUpDown->TabIndex = 18;
			this->ParticleTimeScaleYnumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->ParticleTimeScaleYnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleTimeScaleYnumericUpDown_ValueChanged);
			// 
			// ParticleTimeScaleXnumericUpDown
			// 
			this->ParticleTimeScaleXnumericUpDown->DecimalPlaces = 1;
			this->ParticleTimeScaleXnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->ParticleTimeScaleXnumericUpDown->Location = System::Drawing::Point(0, 307);
			this->ParticleTimeScaleXnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5000, 0, 0, 0});
			this->ParticleTimeScaleXnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5000, 0, 0, System::Int32::MinValue});
			this->ParticleTimeScaleXnumericUpDown->Name = L"ParticleTimeScaleXnumericUpDown";
			this->ParticleTimeScaleXnumericUpDown->Size = System::Drawing::Size(62, 21);
			this->ParticleTimeScaleXnumericUpDown->TabIndex = 18;
			this->ParticleTimeScaleXnumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->ParticleTimeScaleXnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleTimeScalenumericUpDown_ValueChanged);
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(1, 292);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(45, 12);
			this->label13->TabIndex = 85;
			this->label13->Text = L"Scale :";
			// 
			// ParticleTimeAddbutton
			// 
			this->ParticleTimeAddbutton->Location = System::Drawing::Point(59, 51);
			this->ParticleTimeAddbutton->Name = L"ParticleTimeAddbutton";
			this->ParticleTimeAddbutton->Size = System::Drawing::Size(49, 23);
			this->ParticleTimeAddbutton->TabIndex = 4;
			this->ParticleTimeAddbutton->Text = L"Add";
			this->ParticleTimeAddbutton->UseVisualStyleBackColor = true;
			this->ParticleTimeAddbutton->Click += gcnew System::EventHandler(this, &EffectTabForm::ParticleTimeAddbutton_Click);
			// 
			// ParticleTimePosnumericUpDown
			// 
			this->ParticleTimePosnumericUpDown->Location = System::Drawing::Point(127, 76);
			this->ParticleTimePosnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
			this->ParticleTimePosnumericUpDown->Name = L"ParticleTimePosnumericUpDown";
			this->ParticleTimePosnumericUpDown->Size = System::Drawing::Size(96, 21);
			this->ParticleTimePosnumericUpDown->TabIndex = 6;
			this->ParticleTimePosnumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->ParticleTimePosnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleTimePosnumericUpDown_ValueChanged);
			// 
			// ParticleTimeTracklistBox
			// 
			this->ParticleTimeTracklistBox->FormattingEnabled = true;
			this->ParticleTimeTracklistBox->ItemHeight = 12;
			this->ParticleTimeTracklistBox->Location = System::Drawing::Point(6, 51);
			this->ParticleTimeTracklistBox->Name = L"ParticleTimeTracklistBox";
			this->ParticleTimeTracklistBox->SelectionMode = System::Windows::Forms::SelectionMode::MultiExtended;
			this->ParticleTimeTracklistBox->Size = System::Drawing::Size(51, 112);
			this->ParticleTimeTracklistBox->TabIndex = 3;
			this->ParticleTimeTracklistBox->SelectedIndexChanged += gcnew System::EventHandler(this, &EffectTabForm::ParticleTimeTracklistBox_SelectedIndexChanged);
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(126, 61);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(91, 12);
			this->label7->TabIndex = 71;
			this->label7->Text = L"Time Position :";
			// 
			// groupBox7
			// 
			this->groupBox7->Controls->Add(this->spnSelfIlluminationScale);
			this->groupBox7->Controls->Add(this->label18);
			this->groupBox7->Controls->Add(this->UseAlphaTestCheckBox);
			this->groupBox7->Controls->Add(this->SIDeletebutton);
			this->groupBox7->Controls->Add(this->TexSIFileOpenbutton);
			this->groupBox7->Controls->Add(this->label4);
			this->groupBox7->Controls->Add(this->TexSIFileNametextBox);
			this->groupBox7->Controls->Add(this->InvradioButton);
			this->groupBox7->Controls->Add(this->UVmodifybutton);
			this->groupBox7->Controls->Add(this->TexFileNametextBox);
			this->groupBox7->Controls->Add(this->TexFileOpenbutton);
			this->groupBox7->Controls->Add(this->AlphatestValuenumericUpDown);
			this->groupBox7->Controls->Add(this->BlendAddradioButton);
			this->groupBox7->Controls->Add(this->BlendOpaqueRadioButton);
			this->groupBox7->Controls->Add(this->BlendBlendradioButton);
			this->groupBox7->Location = System::Drawing::Point(3, 6);
			this->groupBox7->Name = L"groupBox7";
			this->groupBox7->Size = System::Drawing::Size(229, 186);
			this->groupBox7->TabIndex = 0;
			this->groupBox7->TabStop = false;
			this->groupBox7->Text = L"Material";
			// 
			// spnSelfIlluminationScale
			// 
			this->spnSelfIlluminationScale->DecimalPlaces = 2;
			this->spnSelfIlluminationScale->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->spnSelfIlluminationScale->Location = System::Drawing::Point(156, 157);
			this->spnSelfIlluminationScale->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4, 0, 0, 0});
			this->spnSelfIlluminationScale->Name = L"spnSelfIlluminationScale";
			this->spnSelfIlluminationScale->Size = System::Drawing::Size(61, 21);
			this->spnSelfIlluminationScale->TabIndex = 105;
			this->spnSelfIlluminationScale->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->spnSelfIlluminationScale->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::spnSelfIlluminationScale_ValueChanged);
			// 
			// label18
			// 
			this->label18->AutoSize = true;
			this->label18->Location = System::Drawing::Point(8, 162);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(134, 12);
			this->label18->TabIndex = 104;
			this->label18->Text = L"SelfIllumination Scale :";
			// 
			// UseAlphaTestCheckBox
			// 
			this->UseAlphaTestCheckBox->AutoSize = true;
			this->UseAlphaTestCheckBox->Location = System::Drawing::Point(7, 91);
			this->UseAlphaTestCheckBox->Name = L"UseAlphaTestCheckBox";
			this->UseAlphaTestCheckBox->Size = System::Drawing::Size(81, 16);
			this->UseAlphaTestCheckBox->TabIndex = 103;
			this->UseAlphaTestCheckBox->Text = L"AlphaTest";
			this->UseAlphaTestCheckBox->UseVisualStyleBackColor = true;
			this->UseAlphaTestCheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::UseAlphaTestCheckBox_CheckedChanged);
			// 
			// SIDeletebutton
			// 
			this->SIDeletebutton->AutoSize = true;
			this->SIDeletebutton->Location = System::Drawing::Point(185, 128);
			this->SIDeletebutton->Name = L"SIDeletebutton";
			this->SIDeletebutton->Size = System::Drawing::Size(33, 23);
			this->SIDeletebutton->TabIndex = 102;
			this->SIDeletebutton->Text = L"Del";
			this->SIDeletebutton->UseVisualStyleBackColor = true;
			this->SIDeletebutton->Click += gcnew System::EventHandler(this, &EffectTabForm::SIDeletebutton_Click);
			// 
			// TexSIFileOpenbutton
			// 
			this->TexSIFileOpenbutton->AutoSize = true;
			this->TexSIFileOpenbutton->Location = System::Drawing::Point(155, 128);
			this->TexSIFileOpenbutton->Name = L"TexSIFileOpenbutton";
			this->TexSIFileOpenbutton->Size = System::Drawing::Size(27, 23);
			this->TexSIFileOpenbutton->TabIndex = 101;
			this->TexSIFileOpenbutton->Text = L"...";
			this->TexSIFileOpenbutton->UseVisualStyleBackColor = true;
			this->TexSIFileOpenbutton->Click += gcnew System::EventHandler(this, &EffectTabForm::TexSIFileOpenbutton_Click);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(6, 115);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(102, 12);
			this->label4->TabIndex = 100;
			this->label4->Text = L"Self illumination :";
			// 
			// TexSIFileNametextBox
			// 
			this->TexSIFileNametextBox->Location = System::Drawing::Point(6, 130);
			this->TexSIFileNametextBox->Name = L"TexSIFileNametextBox";
			this->TexSIFileNametextBox->ReadOnly = true;
			this->TexSIFileNametextBox->Size = System::Drawing::Size(145, 21);
			this->TexSIFileNametextBox->TabIndex = 99;
			// 
			// InvradioButton
			// 
			this->InvradioButton->AutoSize = true;
			this->InvradioButton->Location = System::Drawing::Point(7, 69);
			this->InvradioButton->Name = L"InvradioButton";
			this->InvradioButton->Size = System::Drawing::Size(64, 16);
			this->InvradioButton->TabIndex = 98;
			this->InvradioButton->TabStop = true;
			this->InvradioButton->Text = L"Inverse";
			this->InvradioButton->UseVisualStyleBackColor = true;
			this->InvradioButton->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::InvradioButton_CheckedChanged);
			// 
			// UVmodifybutton
			// 
			this->UVmodifybutton->AutoSize = true;
			this->UVmodifybutton->Location = System::Drawing::Point(166, 66);
			this->UVmodifybutton->Name = L"UVmodifybutton";
			this->UVmodifybutton->Size = System::Drawing::Size(52, 23);
			this->UVmodifybutton->TabIndex = 7;
			this->UVmodifybutton->Text = L"UV";
			this->UVmodifybutton->UseVisualStyleBackColor = true;
			this->UVmodifybutton->Click += gcnew System::EventHandler(this, &EffectTabForm::UVmodifybutton_Click);
			// 
			// TexFileNametextBox
			// 
			this->TexFileNametextBox->Location = System::Drawing::Point(6, 18);
			this->TexFileNametextBox->Name = L"TexFileNametextBox";
			this->TexFileNametextBox->ReadOnly = true;
			this->TexFileNametextBox->Size = System::Drawing::Size(172, 21);
			this->TexFileNametextBox->TabIndex = 0;
			// 
			// TexFileOpenbutton
			// 
			this->TexFileOpenbutton->AutoSize = true;
			this->TexFileOpenbutton->Location = System::Drawing::Point(185, 18);
			this->TexFileOpenbutton->Name = L"TexFileOpenbutton";
			this->TexFileOpenbutton->Size = System::Drawing::Size(27, 23);
			this->TexFileOpenbutton->TabIndex = 6;
			this->TexFileOpenbutton->Text = L"...";
			this->TexFileOpenbutton->UseVisualStyleBackColor = true;
			this->TexFileOpenbutton->Click += gcnew System::EventHandler(this, &EffectTabForm::TexFileOpenbutton_Click);
			// 
			// AlphatestValuenumericUpDown
			// 
			this->AlphatestValuenumericUpDown->Location = System::Drawing::Point(94, 86);
			this->AlphatestValuenumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->AlphatestValuenumericUpDown->Name = L"AlphatestValuenumericUpDown";
			this->AlphatestValuenumericUpDown->Size = System::Drawing::Size(51, 21);
			this->AlphatestValuenumericUpDown->TabIndex = 5;
			this->AlphatestValuenumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {80, 0, 0, 0});
			this->AlphatestValuenumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::AlphatestValuenumericUpDown_ValueChanged);
			// 
			// BlendAddradioButton
			// 
			this->BlendAddradioButton->AutoSize = true;
			this->BlendAddradioButton->Location = System::Drawing::Point(7, 47);
			this->BlendAddradioButton->Name = L"BlendAddradioButton";
			this->BlendAddradioButton->Size = System::Drawing::Size(45, 16);
			this->BlendAddradioButton->TabIndex = 1;
			this->BlendAddradioButton->TabStop = true;
			this->BlendAddradioButton->Text = L"Add";
			this->BlendAddradioButton->UseVisualStyleBackColor = true;
			this->BlendAddradioButton->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::BlendAddradioButton_CheckedChanged);
			// 
			// BlendOpaqueRadioButton
			// 
			this->BlendOpaqueRadioButton->AutoSize = true;
			this->BlendOpaqueRadioButton->Location = System::Drawing::Point(119, 47);
			this->BlendOpaqueRadioButton->Name = L"BlendOpaqueRadioButton";
			this->BlendOpaqueRadioButton->Size = System::Drawing::Size(67, 16);
			this->BlendOpaqueRadioButton->TabIndex = 4;
			this->BlendOpaqueRadioButton->TabStop = true;
			this->BlendOpaqueRadioButton->Text = L"Opaque";
			this->BlendOpaqueRadioButton->UseVisualStyleBackColor = true;
			this->BlendOpaqueRadioButton->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::BlendOpaqueRadioButton_CheckedChanged);
			// 
			// BlendBlendradioButton
			// 
			this->BlendBlendradioButton->AutoSize = true;
			this->BlendBlendradioButton->Location = System::Drawing::Point(58, 47);
			this->BlendBlendradioButton->Name = L"BlendBlendradioButton";
			this->BlendBlendradioButton->Size = System::Drawing::Size(55, 16);
			this->BlendBlendradioButton->TabIndex = 2;
			this->BlendBlendradioButton->TabStop = true;
			this->BlendBlendradioButton->Text = L"Blend";
			this->BlendBlendradioButton->UseVisualStyleBackColor = true;
			this->BlendBlendradioButton->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::BlendBlendradioButton_CheckedChanged);
			// 
			// ModelanitabPage
			// 
			this->ModelanitabPage->AutoScroll = true;
			this->ModelanitabPage->Controls->Add(this->ActorNodeFindTextBox);
			this->ModelanitabPage->Controls->Add(this->ActorAniTreeViewFindTextBox);
			this->ModelanitabPage->Controls->Add(this->NodeListtreeView);
			this->ModelanitabPage->Controls->Add(this->AnitreeView);
			this->ModelanitabPage->Controls->Add(this->AniLoopcheckBox);
			this->ModelanitabPage->Location = System::Drawing::Point(4, 40);
			this->ModelanitabPage->Name = L"ModelanitabPage";
			this->ModelanitabPage->Size = System::Drawing::Size(272, 656);
			this->ModelanitabPage->TabIndex = 3;
			this->ModelanitabPage->Text = L"Animation";
			this->ModelanitabPage->UseVisualStyleBackColor = true;
			// 
			// ActorNodeFindTextBox
			// 
			this->ActorNodeFindTextBox->Location = System::Drawing::Point(8, 293);
			this->ActorNodeFindTextBox->Name = L"ActorNodeFindTextBox";
			this->ActorNodeFindTextBox->Size = System::Drawing::Size(141, 21);
			this->ActorNodeFindTextBox->TabIndex = 9;
			this->ActorNodeFindTextBox->TextChanged += gcnew System::EventHandler(this, &EffectTabForm::ActorNodeFindTextBox_TextChanged);
			// 
			// ActorAniTreeViewFindTextBox
			// 
			this->ActorAniTreeViewFindTextBox->Location = System::Drawing::Point(8, 22);
			this->ActorAniTreeViewFindTextBox->Name = L"ActorAniTreeViewFindTextBox";
			this->ActorAniTreeViewFindTextBox->Size = System::Drawing::Size(141, 21);
			this->ActorAniTreeViewFindTextBox->TabIndex = 8;
			this->ActorAniTreeViewFindTextBox->TextChanged += gcnew System::EventHandler(this, &EffectTabForm::ActorAniTreeViewFindTextBox_TextChanged);
			// 
			// NodeListtreeView
			// 
			this->NodeListtreeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->NodeListtreeView->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->NodeListtreeView->HideSelection = false;
			this->NodeListtreeView->Location = System::Drawing::Point(8, 320);
			this->NodeListtreeView->Name = L"NodeListtreeView";
			this->NodeListtreeView->Size = System::Drawing::Size(256, 238);
			this->NodeListtreeView->TabIndex = 7;
			this->NodeListtreeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &EffectTabForm::NodeListtreeView_AfterSelect);
			// 
			// AnitreeView
			// 
			this->AnitreeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->AnitreeView->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->AnitreeView->HideSelection = false;
			this->AnitreeView->Location = System::Drawing::Point(8, 49);
			this->AnitreeView->Name = L"AnitreeView";
			treeNode1->Name = L"none";
			treeNode1->Text = L"none";
			this->AnitreeView->Nodes->AddRange(gcnew cli::array< System::Windows::Forms::TreeNode^  >(1) {treeNode1});
			this->AnitreeView->Size = System::Drawing::Size(256, 238);
			this->AnitreeView->TabIndex = 5;
			this->AnitreeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &EffectTabForm::AnitreeView_AfterSelect);
			// 
			// AniLoopcheckBox
			// 
			this->AniLoopcheckBox->AutoSize = true;
			this->AniLoopcheckBox->Location = System::Drawing::Point(8, 3);
			this->AniLoopcheckBox->Name = L"AniLoopcheckBox";
			this->AniLoopcheckBox->Size = System::Drawing::Size(52, 16);
			this->AniLoopcheckBox->TabIndex = 6;
			this->AniLoopcheckBox->Text = L"Loop";
			this->AniLoopcheckBox->UseVisualStyleBackColor = true;
			this->AniLoopcheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::AniLoopcheckBox_CheckedChanged);
			// 
			// EffectListtabPage
			// 
			this->EffectListtabPage->Controls->Add(this->FindTextBox);
			this->EffectListtabPage->Controls->Add(this->ReloadEffects);
			this->EffectListtabPage->Controls->Add(this->EffectListtreeView);
			this->EffectListtabPage->Location = System::Drawing::Point(4, 40);
			this->EffectListtabPage->Name = L"EffectListtabPage";
			this->EffectListtabPage->Padding = System::Windows::Forms::Padding(3);
			this->EffectListtabPage->Size = System::Drawing::Size(272, 656);
			this->EffectListtabPage->TabIndex = 0;
			this->EffectListtabPage->Text = L"EffectList";
			this->EffectListtabPage->UseVisualStyleBackColor = true;
			// 
			// FindTextBox
			// 
			this->FindTextBox->Location = System::Drawing::Point(2, 8);
			this->FindTextBox->Name = L"FindTextBox";
			this->FindTextBox->Size = System::Drawing::Size(103, 21);
			this->FindTextBox->TabIndex = 2;
			this->FindTextBox->TextChanged += gcnew System::EventHandler(this, &EffectTabForm::FindTextBox_TextChanged);
			// 
			// ReloadEffects
			// 
			this->ReloadEffects->Location = System::Drawing::Point(144, 6);
			this->ReloadEffects->Name = L"ReloadEffects";
			this->ReloadEffects->Size = System::Drawing::Size(81, 23);
			this->ReloadEffects->TabIndex = 0;
			this->ReloadEffects->Text = L"ReloadList";
			this->ReloadEffects->UseVisualStyleBackColor = true;
			this->ReloadEffects->Click += gcnew System::EventHandler(this, &EffectTabForm::ReloadEffects_Click);
			// 
			// EffectListtreeView
			// 
			this->EffectListtreeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->EffectListtreeView->HideSelection = false;
			this->EffectListtreeView->Location = System::Drawing::Point(2, 35);
			this->EffectListtreeView->Name = L"EffectListtreeView";
			this->EffectListtreeView->Size = System::Drawing::Size(267, 569);
			this->EffectListtreeView->TabIndex = 0;
			this->EffectListtreeView->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &EffectTabForm::EffectListtreeView_AfterSelect);
			this->EffectListtreeView->BeforeSelect += gcnew System::Windows::Forms::TreeViewCancelEventHandler(this, &EffectTabForm::EffectListtreeView_BeforeSelect);
			// 
			// TrailTabPage
			// 
			this->TrailTabPage->Controls->Add(this->groupBox11);
			this->TrailTabPage->Controls->Add(this->groupBox10);
			this->TrailTabPage->Location = System::Drawing::Point(4, 40);
			this->TrailTabPage->Name = L"TrailTabPage";
			this->TrailTabPage->Padding = System::Windows::Forms::Padding(3);
			this->TrailTabPage->Size = System::Drawing::Size(272, 656);
			this->TrailTabPage->TabIndex = 6;
			this->TrailTabPage->Text = L"Trail";
			this->TrailTabPage->UseVisualStyleBackColor = true;
			// 
			// groupBox11
			// 
			this->groupBox11->Controls->Add(this->TRAIL_DISTORTION_PROPERTY_BUTTON);
			this->groupBox11->Controls->Add(this->TrailDistortionCheckBox);
			this->groupBox11->Controls->Add(this->TrailLoopCheckBox);
			this->groupBox11->Controls->Add(this->TrailSegmentnumericUpDown);
			this->groupBox11->Controls->Add(this->label44);
			this->groupBox11->Controls->Add(this->TrailAlignCheckBox);
			this->groupBox11->Controls->Add(this->label43);
			this->groupBox11->Controls->Add(this->TrailBackColorButton);
			this->groupBox11->Controls->Add(this->TrailFrontColorButton);
			this->groupBox11->Controls->Add(this->TrailLifenumericUpDown);
			this->groupBox11->Controls->Add(this->label41);
			this->groupBox11->Controls->Add(this->TrailWidthnumericUpDown);
			this->groupBox11->Controls->Add(this->label42);
			this->groupBox11->Location = System::Drawing::Point(3, 203);
			this->groupBox11->Name = L"groupBox11";
			this->groupBox11->Size = System::Drawing::Size(223, 286);
			this->groupBox11->TabIndex = 1;
			this->groupBox11->TabStop = false;
			this->groupBox11->Text = L"Trail";
			// 
			// TRAIL_DISTORTION_PROPERTY_BUTTON
			// 
			this->TRAIL_DISTORTION_PROPERTY_BUTTON->Location = System::Drawing::Point(11, 217);
			this->TRAIL_DISTORTION_PROPERTY_BUTTON->Name = L"TRAIL_DISTORTION_PROPERTY_BUTTON";
			this->TRAIL_DISTORTION_PROPERTY_BUTTON->Size = System::Drawing::Size(108, 23);
			this->TRAIL_DISTORTION_PROPERTY_BUTTON->TabIndex = 12;
			this->TRAIL_DISTORTION_PROPERTY_BUTTON->Text = L"Distortion 속성";
			this->TRAIL_DISTORTION_PROPERTY_BUTTON->UseVisualStyleBackColor = true;
			this->TRAIL_DISTORTION_PROPERTY_BUTTON->Click += gcnew System::EventHandler(this, &EffectTabForm::TRAIL_DISTORTION_PROPERTY_BUTTON_Click);
			// 
			// TrailDistortionCheckBox
			// 
			this->TrailDistortionCheckBox->AutoSize = true;
			this->TrailDistortionCheckBox->Location = System::Drawing::Point(11, 195);
			this->TrailDistortionCheckBox->Name = L"TrailDistortionCheckBox";
			this->TrailDistortionCheckBox->Size = System::Drawing::Size(76, 16);
			this->TrailDistortionCheckBox->TabIndex = 11;
			this->TrailDistortionCheckBox->Text = L"Distortion";
			this->TrailDistortionCheckBox->UseVisualStyleBackColor = true;
			this->TrailDistortionCheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::TrailDistortionCheckBox_CheckedChanged);
			// 
			// TrailLoopCheckBox
			// 
			this->TrailLoopCheckBox->AutoSize = true;
			this->TrailLoopCheckBox->Location = System::Drawing::Point(82, 20);
			this->TrailLoopCheckBox->Name = L"TrailLoopCheckBox";
			this->TrailLoopCheckBox->Size = System::Drawing::Size(52, 16);
			this->TrailLoopCheckBox->TabIndex = 1;
			this->TrailLoopCheckBox->Text = L"Loop";
			this->TrailLoopCheckBox->UseVisualStyleBackColor = true;
			this->TrailLoopCheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::TrailLoopCheckBox_CheckedChanged);
			// 
			// TrailSegmentnumericUpDown
			// 
			this->TrailSegmentnumericUpDown->DecimalPlaces = 1;
			this->TrailSegmentnumericUpDown->Location = System::Drawing::Point(65, 106);
			this->TrailSegmentnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->TrailSegmentnumericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->TrailSegmentnumericUpDown->Name = L"TrailSegmentnumericUpDown";
			this->TrailSegmentnumericUpDown->Size = System::Drawing::Size(139, 21);
			this->TrailSegmentnumericUpDown->TabIndex = 7;
			this->TrailSegmentnumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->TrailSegmentnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::TrailSegmentnumericUpDown_ValueChanged);
			// 
			// label44
			// 
			this->label44->AutoSize = true;
			this->label44->Location = System::Drawing::Point(2, 108);
			this->label44->Name = L"label44";
			this->label44->Size = System::Drawing::Size(55, 12);
			this->label44->TabIndex = 6;
			this->label44->Text = L"Segment";
			// 
			// TrailAlignCheckBox
			// 
			this->TrailAlignCheckBox->AutoSize = true;
			this->TrailAlignCheckBox->Location = System::Drawing::Point(17, 20);
			this->TrailAlignCheckBox->Name = L"TrailAlignCheckBox";
			this->TrailAlignCheckBox->Size = System::Drawing::Size(52, 16);
			this->TrailAlignCheckBox->TabIndex = 0;
			this->TrailAlignCheckBox->Text = L"Align";
			this->TrailAlignCheckBox->UseVisualStyleBackColor = true;
			this->TrailAlignCheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::TrailAlignCheckBox_CheckedChanged);
			// 
			// label43
			// 
			this->label43->AutoSize = true;
			this->label43->Location = System::Drawing::Point(13, 143);
			this->label43->Name = L"label43";
			this->label43->Size = System::Drawing::Size(35, 12);
			this->label43->TabIndex = 8;
			this->label43->Text = L"Color";
			// 
			// TrailBackColorButton
			// 
			this->TrailBackColorButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
			this->TrailBackColorButton->Location = System::Drawing::Point(149, 138);
			this->TrailBackColorButton->Name = L"TrailBackColorButton";
			this->TrailBackColorButton->Size = System::Drawing::Size(55, 23);
			this->TrailBackColorButton->TabIndex = 10;
			this->TrailBackColorButton->Text = L"Back";
			this->TrailBackColorButton->UseVisualStyleBackColor = true;
			this->TrailBackColorButton->Click += gcnew System::EventHandler(this, &EffectTabForm::TrailBackColorButton_Click);
			// 
			// TrailFrontColorButton
			// 
			this->TrailFrontColorButton->BackColor = System::Drawing::Color::Transparent;
			this->TrailFrontColorButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
			this->TrailFrontColorButton->Location = System::Drawing::Point(81, 138);
			this->TrailFrontColorButton->Name = L"TrailFrontColorButton";
			this->TrailFrontColorButton->Size = System::Drawing::Size(55, 23);
			this->TrailFrontColorButton->TabIndex = 9;
			this->TrailFrontColorButton->Text = L"Front";
			this->TrailFrontColorButton->UseVisualStyleBackColor = false;
			this->TrailFrontColorButton->Click += gcnew System::EventHandler(this, &EffectTabForm::TrailFrontColorButton_Click);
			// 
			// TrailLifenumericUpDown
			// 
			this->TrailLifenumericUpDown->DecimalPlaces = 2;
			this->TrailLifenumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
			this->TrailLifenumericUpDown->Location = System::Drawing::Point(65, 52);
			this->TrailLifenumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {300, 0, 0, 0});
			this->TrailLifenumericUpDown->Name = L"TrailLifenumericUpDown";
			this->TrailLifenumericUpDown->Size = System::Drawing::Size(139, 21);
			this->TrailLifenumericUpDown->TabIndex = 3;
			this->TrailLifenumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::TrailLifenumericUpDown_ValueChanged);
			// 
			// label41
			// 
			this->label41->AutoSize = true;
			this->label41->Location = System::Drawing::Point(36, 54);
			this->label41->Name = L"label41";
			this->label41->Size = System::Drawing::Size(21, 12);
			this->label41->TabIndex = 2;
			this->label41->Text = L"life";
			// 
			// TrailWidthnumericUpDown
			// 
			this->TrailWidthnumericUpDown->DecimalPlaces = 1;
			this->TrailWidthnumericUpDown->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->TrailWidthnumericUpDown->Location = System::Drawing::Point(65, 79);
			this->TrailWidthnumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {400, 0, 0, 0});
			this->TrailWidthnumericUpDown->Name = L"TrailWidthnumericUpDown";
			this->TrailWidthnumericUpDown->Size = System::Drawing::Size(139, 21);
			this->TrailWidthnumericUpDown->TabIndex = 5;
			this->TrailWidthnumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::TrailWidthnumericUpDown_ValueChanged);
			// 
			// label42
			// 
			this->label42->AutoSize = true;
			this->label42->Location = System::Drawing::Point(22, 81);
			this->label42->Name = L"label42";
			this->label42->Size = System::Drawing::Size(35, 12);
			this->label42->TabIndex = 4;
			this->label42->Text = L"width";
			// 
			// groupBox10
			// 
			this->groupBox10->Controls->Add(this->TrailSpnSelfIlluminationScale);
			this->groupBox10->Controls->Add(label20);
			this->groupBox10->Controls->Add(this->TrailSIDeleteButton);
			this->groupBox10->Controls->Add(this->TrailTexSIFileOpenButton);
			this->groupBox10->Controls->Add(label26);
			this->groupBox10->Controls->Add(this->TrailTexSIFileNameTextBox);
			this->groupBox10->Controls->Add(this->TrailAlphatestValueNumericUpDown);
			this->groupBox10->Controls->Add(this->TrailUseAlphaTestCheckBox);
			this->groupBox10->Controls->Add(this->TrailInverseBlendRadioButton);
			this->groupBox10->Controls->Add(this->TrailOpaqueBlendRadioButton);
			this->groupBox10->Controls->Add(this->TrailTextureUVButton);
			this->groupBox10->Controls->Add(this->TrailAlphaBlendRadioButton);
			this->groupBox10->Controls->Add(this->TrailAddBlendRadioButton);
			this->groupBox10->Controls->Add(this->label39);
			this->groupBox10->Controls->Add(this->TrailTexFileOpenButton);
			this->groupBox10->Controls->Add(this->trailTexture);
			this->groupBox10->Location = System::Drawing::Point(3, 3);
			this->groupBox10->Name = L"groupBox10";
			this->groupBox10->Size = System::Drawing::Size(223, 194);
			this->groupBox10->TabIndex = 0;
			this->groupBox10->TabStop = false;
			this->groupBox10->Text = L"Material";
			// 
			// TrailSpnSelfIlluminationScale
			// 
			this->TrailSpnSelfIlluminationScale->DecimalPlaces = 2;
			this->TrailSpnSelfIlluminationScale->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->TrailSpnSelfIlluminationScale->Location = System::Drawing::Point(159, 161);
			this->TrailSpnSelfIlluminationScale->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4, 0, 0, 0});
			this->TrailSpnSelfIlluminationScale->Name = L"TrailSpnSelfIlluminationScale";
			this->TrailSpnSelfIlluminationScale->Size = System::Drawing::Size(61, 21);
			this->TrailSpnSelfIlluminationScale->TabIndex = 111;
			this->TrailSpnSelfIlluminationScale->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->TrailSpnSelfIlluminationScale->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::TrailSpnSelfIlluminationScale_ValueChanged);
			// 
			// label20
			// 
			label20->AutoSize = true;
			label20->Location = System::Drawing::Point(11, 166);
			label20->Name = L"label20";
			label20->Size = System::Drawing::Size(134, 12);
			label20->TabIndex = 110;
			label20->Text = L"SelfIllumination Scale :";
			// 
			// TrailSIDeleteButton
			// 
			this->TrailSIDeleteButton->AutoSize = true;
			this->TrailSIDeleteButton->Location = System::Drawing::Point(188, 132);
			this->TrailSIDeleteButton->Name = L"TrailSIDeleteButton";
			this->TrailSIDeleteButton->Size = System::Drawing::Size(33, 23);
			this->TrailSIDeleteButton->TabIndex = 109;
			this->TrailSIDeleteButton->Text = L"Del";
			this->TrailSIDeleteButton->UseVisualStyleBackColor = true;
			this->TrailSIDeleteButton->Click += gcnew System::EventHandler(this, &EffectTabForm::TrailSIDeleteButton_Click);
			// 
			// TrailTexSIFileOpenButton
			// 
			this->TrailTexSIFileOpenButton->AutoSize = true;
			this->TrailTexSIFileOpenButton->Location = System::Drawing::Point(158, 132);
			this->TrailTexSIFileOpenButton->Name = L"TrailTexSIFileOpenButton";
			this->TrailTexSIFileOpenButton->Size = System::Drawing::Size(27, 23);
			this->TrailTexSIFileOpenButton->TabIndex = 108;
			this->TrailTexSIFileOpenButton->Text = L"...";
			this->TrailTexSIFileOpenButton->UseVisualStyleBackColor = true;
			this->TrailTexSIFileOpenButton->Click += gcnew System::EventHandler(this, &EffectTabForm::TrailTexSIFileOpenButton_Click);
			// 
			// TrailTexSIFileNameTextBox
			// 
			this->TrailTexSIFileNameTextBox->Location = System::Drawing::Point(9, 134);
			this->TrailTexSIFileNameTextBox->Name = L"TrailTexSIFileNameTextBox";
			this->TrailTexSIFileNameTextBox->ReadOnly = true;
			this->TrailTexSIFileNameTextBox->Size = System::Drawing::Size(145, 21);
			this->TrailTexSIFileNameTextBox->TabIndex = 106;
			// 
			// TrailAlphatestValueNumericUpDown
			// 
			this->TrailAlphatestValueNumericUpDown->Location = System::Drawing::Point(103, 85);
			this->TrailAlphatestValueNumericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->TrailAlphatestValueNumericUpDown->Name = L"TrailAlphatestValueNumericUpDown";
			this->TrailAlphatestValueNumericUpDown->Size = System::Drawing::Size(51, 21);
			this->TrailAlphatestValueNumericUpDown->TabIndex = 9;
			this->TrailAlphatestValueNumericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {80, 0, 0, 0});
			this->TrailAlphatestValueNumericUpDown->ValueChanged += gcnew System::EventHandler(this, &EffectTabForm::TrailAlphatestValueNumericUpDown_ValueChanged);
			// 
			// TrailUseAlphaTestCheckBox
			// 
			this->TrailUseAlphaTestCheckBox->AutoSize = true;
			this->TrailUseAlphaTestCheckBox->Location = System::Drawing::Point(17, 90);
			this->TrailUseAlphaTestCheckBox->Name = L"TrailUseAlphaTestCheckBox";
			this->TrailUseAlphaTestCheckBox->Size = System::Drawing::Size(81, 16);
			this->TrailUseAlphaTestCheckBox->TabIndex = 8;
			this->TrailUseAlphaTestCheckBox->Text = L"AlphaTest";
			this->TrailUseAlphaTestCheckBox->UseVisualStyleBackColor = true;
			this->TrailUseAlphaTestCheckBox->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::TrailUseAlphaTestCheckBox_CheckedChanged);
			// 
			// TrailInverseBlendRadioButton
			// 
			this->TrailInverseBlendRadioButton->AutoSize = true;
			this->TrailInverseBlendRadioButton->Location = System::Drawing::Point(90, 63);
			this->TrailInverseBlendRadioButton->Name = L"TrailInverseBlendRadioButton";
			this->TrailInverseBlendRadioButton->Size = System::Drawing::Size(64, 16);
			this->TrailInverseBlendRadioButton->TabIndex = 7;
			this->TrailInverseBlendRadioButton->TabStop = true;
			this->TrailInverseBlendRadioButton->Text = L"Inverse";
			this->TrailInverseBlendRadioButton->UseVisualStyleBackColor = true;
			this->TrailInverseBlendRadioButton->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::TrailInverseBlendRadioButton_CheckedChanged);
			// 
			// TrailOpaqueBlendRadioButton
			// 
			this->TrailOpaqueBlendRadioButton->AutoSize = true;
			this->TrailOpaqueBlendRadioButton->Location = System::Drawing::Point(17, 63);
			this->TrailOpaqueBlendRadioButton->Name = L"TrailOpaqueBlendRadioButton";
			this->TrailOpaqueBlendRadioButton->Size = System::Drawing::Size(67, 16);
			this->TrailOpaqueBlendRadioButton->TabIndex = 6;
			this->TrailOpaqueBlendRadioButton->TabStop = true;
			this->TrailOpaqueBlendRadioButton->Text = L"Opaque";
			this->TrailOpaqueBlendRadioButton->UseVisualStyleBackColor = true;
			this->TrailOpaqueBlendRadioButton->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::TrailOpaqueBlendRadioButton_CheckedChanged);
			// 
			// TrailTextureUVButton
			// 
			this->TrailTextureUVButton->Location = System::Drawing::Point(155, 40);
			this->TrailTextureUVButton->Name = L"TrailTextureUVButton";
			this->TrailTextureUVButton->Size = System::Drawing::Size(62, 23);
			this->TrailTextureUVButton->TabIndex = 5;
			this->TrailTextureUVButton->Text = L"UV";
			this->TrailTextureUVButton->UseVisualStyleBackColor = true;
			this->TrailTextureUVButton->Click += gcnew System::EventHandler(this, &EffectTabForm::TrailTextureUVButton_Click);
			// 
			// TrailAlphaBlendRadioButton
			// 
			this->TrailAlphaBlendRadioButton->AutoSize = true;
			this->TrailAlphaBlendRadioButton->Location = System::Drawing::Point(90, 41);
			this->TrailAlphaBlendRadioButton->Name = L"TrailAlphaBlendRadioButton";
			this->TrailAlphaBlendRadioButton->Size = System::Drawing::Size(55, 16);
			this->TrailAlphaBlendRadioButton->TabIndex = 4;
			this->TrailAlphaBlendRadioButton->TabStop = true;
			this->TrailAlphaBlendRadioButton->Text = L"Alpha";
			this->TrailAlphaBlendRadioButton->UseVisualStyleBackColor = true;
			this->TrailAlphaBlendRadioButton->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::TrailAlphaBlendRadioButton_CheckedChanged);
			// 
			// TrailAddBlendRadioButton
			// 
			this->TrailAddBlendRadioButton->AutoSize = true;
			this->TrailAddBlendRadioButton->Location = System::Drawing::Point(17, 41);
			this->TrailAddBlendRadioButton->Name = L"TrailAddBlendRadioButton";
			this->TrailAddBlendRadioButton->Size = System::Drawing::Size(45, 16);
			this->TrailAddBlendRadioButton->TabIndex = 3;
			this->TrailAddBlendRadioButton->TabStop = true;
			this->TrailAddBlendRadioButton->Text = L"Add";
			this->TrailAddBlendRadioButton->UseVisualStyleBackColor = true;
			this->TrailAddBlendRadioButton->CheckedChanged += gcnew System::EventHandler(this, &EffectTabForm::TrailAddRadioButton_CheckedChanged);
			// 
			// label39
			// 
			this->label39->AutoSize = true;
			this->label39->Location = System::Drawing::Point(9, 17);
			this->label39->Name = L"label39";
			this->label39->Size = System::Drawing::Size(48, 12);
			this->label39->TabIndex = 0;
			this->label39->Text = L"Texture";
			// 
			// TrailTexFileOpenButton
			// 
			this->TrailTexFileOpenButton->Location = System::Drawing::Point(185, 13);
			this->TrailTexFileOpenButton->Name = L"TrailTexFileOpenButton";
			this->TrailTexFileOpenButton->Size = System::Drawing::Size(32, 21);
			this->TrailTexFileOpenButton->TabIndex = 2;
			this->TrailTexFileOpenButton->Text = L"...";
			this->TrailTexFileOpenButton->UseVisualStyleBackColor = true;
			this->TrailTexFileOpenButton->Click += gcnew System::EventHandler(this, &EffectTabForm::TrailTexFileOpenButton_Click);
			// 
			// trailTexture
			// 
			this->trailTexture->Location = System::Drawing::Point(65, 14);
			this->trailTexture->Name = L"trailTexture";
			this->trailTexture->ReadOnly = true;
			this->trailTexture->Size = System::Drawing::Size(113, 21);
			this->trailTexture->TabIndex = 1;
			this->trailTexture->UseWaitCursor = true;
			// 
			// EffectTabForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(280, 700);
			this->Controls->Add(this->tabControl1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"EffectTabForm";
			this->Text = L"EffectTabForm";
			groupBox1->ResumeLayout(false);
			groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MeshStartTimeNumericUpDown))->EndInit();
			groupBox3->ResumeLayout(false);
			groupBox3->PerformLayout();
			groupBox12->ResumeLayout(false);
			groupBox12->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MeshRotateMaxNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MeshRotateMinNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MeshScalenumericUpDown))->EndInit();
			groupBox9->ResumeLayout(false);
			groupBox9->PerformLayout();
			groupBox8->ResumeLayout(false);
			groupBox8->PerformLayout();
			this->tabControl1->ResumeLayout(false);
			this->EffecttabPage->ResumeLayout(false);
			this->MeshtabPage->ResumeLayout(false);
			this->EmittertabPage->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterStartTimeNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->RotationMaxnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->RotationMinnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterLifefnumericUpDown))->EndInit();
			this->groupBox6->ResumeLayout(false);
			this->groupBox5->ResumeLayout(false);
			this->groupBox5->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterVelocityMaxUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterVelocityMinUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimeAnglenumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimeRangeZnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimeRangeYnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimeRangeXnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimePositionZnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimePositionYnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimePositionXnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimeAmountnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->EmitterTimePosnumericUpDown))->EndInit();
			this->ParticletabPage->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleVariableYNumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleVariableNumericUpDown1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeScaleVarnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->VisibilitynumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeVelMaxnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleLifemaxnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleLifeMinnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeGRatenumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeVelMinnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeVelDirZnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeVelDirYnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeVelDirXnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeScaleYnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimeScaleXnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ParticleTimePosnumericUpDown))->EndInit();
			this->groupBox7->ResumeLayout(false);
			this->groupBox7->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->spnSelfIlluminationScale))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->AlphatestValuenumericUpDown))->EndInit();
			this->ModelanitabPage->ResumeLayout(false);
			this->ModelanitabPage->PerformLayout();
			this->EffectListtabPage->ResumeLayout(false);
			this->EffectListtabPage->PerformLayout();
			this->TrailTabPage->ResumeLayout(false);
			this->groupBox11->ResumeLayout(false);
			this->groupBox11->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TrailSegmentnumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TrailLifenumericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TrailWidthnumericUpDown))->EndInit();
			this->groupBox10->ResumeLayout(false);
			this->groupBox10->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TrailSpnSelfIlluminationScale))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TrailAlphatestValueNumericUpDown))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
		// 컨트롤 활성/비활성 함수 모음
	public: System::Void DisableAllControls()
			{
				EffecttabPage->Enabled = false;
				EmittertabPage->Enabled = false;
				ParticletabPage->Enabled = false;
				MeshtabPage->Enabled = false;
				TrailTabPage->Enabled = false;
			}
	public: System::Void ActiveEffectControls()
			{
				DisableAllControls();
				EffecttabPage->Enabled = true;
				tabControl1->SelectTab(EffecttabPage);
			}
	public: System::Void ActiveEmitterControls()
			{
				DisableAllControls();
				EmittertabPage->Enabled = true;
				ParticletabPage->Enabled = true;

				if( tabControl1->SelectedTab == EmittertabPage )
					return;

				if( tabControl1->SelectedTab == ParticletabPage )
					return;

				if( tabControl1->SelectedTab == EffecttabPage )
					return;

				tabControl1->SelectTab(EmittertabPage);
			}
	public: System::Void ActiveActorControls()
			{
				DisableAllControls();
				MeshtabPage->Enabled = true;

				if( tabControl1->SelectedTab == MeshtabPage )
					return;

				if( tabControl1->SelectedTab == EffecttabPage )
					return;

				tabControl1->SelectTab(MeshtabPage);
			}
	public: System::Void ActiveTrailControls()
			{
				DisableAllControls();
				TrailTabPage->Enabled = true;
				tabControl1->SelectTab(TrailTabPage);
			}
	public: System::Void SetEffectValue(RSceneNode* pSceneNode)
			{
				RVector pos = pSceneNode->GetPosition();
				float scale = pSceneNode->GetScale().x;
				textoffsetX->Text = Decimal(pos.x).ToString();
				textoffsetY->Text = Decimal(pos.y).ToString();
				textoffsetZ->Text = Decimal(pos.z).ToString();
				ScaleTextBox->Text = Decimal(scale).ToString();
			}
			// 클래스 값에 대한 Get/Set 함수들
	public: System::Void SetTrailValue(RTrail *pTrail)
			{
				bool bOldModified = g_bModified;

				trailTexture->Text = gcnew String(pTrail->GetTextureName().c_str());
				TrailTexSIFileNameTextBox->Text = gcnew String(pTrail->GetSITextureName().c_str());

				TrailLifenumericUpDown->Value = Decimal(pTrail->GetLife());
				TrailWidthnumericUpDown->Value = Decimal(pTrail->GetWidth());
				TrailSegmentnumericUpDown->Value = Decimal(pTrail->GetSegment());

				TrailFrontColorButton->BackColor = *MHelp::MVector4ToColor(pTrail->GetFrontColor());
				TrailBackColorButton->BackColor = *MHelp::MVector4ToColor(pTrail->GetBackColor());
				TrailAlignCheckBox->Checked = pTrail->GetAlign();
				TrailLoopCheckBox->Checked = pTrail->IsLooping();
				TrailDistortionCheckBox->Checked = pTrail->IsDistortion();

				TrailAddBlendRadioButton->Checked = pTrail->IsBlendModeAdd();
				TrailAlphaBlendRadioButton->Checked = pTrail->IsBlendModeAlpha();
				TrailOpaqueBlendRadioButton->Checked = pTrail->IsBlendModeOpaque();
				TrailInverseBlendRadioButton->Checked = pTrail->IsBlendModeInverse();
				TrailUseAlphaTestCheckBox->Checked = pTrail->IsUseAlphaTest();
				TrailAlphatestValueNumericUpDown->Value = Decimal(pTrail->GetAlphaRef());

				TrailSpnSelfIlluminationScale->Value = Decimal(pTrail->GetSelfIlluminationScale());

				g_bModified = bOldModified;
			}

	public: System::Void SetEmitterTimeVariables(REmitter *pEmitter, const std::vector<REmitterVariable> &rVar)
			{
				EmitterTimrTracklistBox->Items->Clear();
				for(size_t i=0;i< rVar.size();i++)
				{
					EmitterTimrTracklistBox->Items->Add( String::Format("{0}",
						rVar[i].fEventTime*((int)EmitterLifefnumericUpDown->Value)));
				}

				SetEmitterTimeVariables(pEmitter, pEmitter->GetEmitterTrack(), 0);

				// 이미터 키 리스트를 초기화 후 첫번째 키를 선택해준다.
				if( EmitterTimrTracklistBox->Items->Count > 0 )
					EmitterTimrTracklistBox->SelectedIndex = 0;
			}

	public: System::Void SetEmitterTimeVariables(REmitter *pEmitter, const std::vector<REmitterVariable> &rVar, int iSel)
			{
				if ( rVar.empty())
					return;
				if ( static_cast<int>(rVar.size()) <= iSel )
					return;

				EmitterTimePosnumericUpDown->Value		= Decimal(rVar[iSel].fEventTime*pEmitter->GetEmitLife()*30);
				EmitterTimeAmountnumericUpDown->Value	= rVar[iSel].uAmount;
				EmitterTimeAnglenumericUpDown->Value	= Decimal(rVar[iSel].fAngle);
				EmitterTimeRangeXnumericUpDown->Value	= Decimal(rVar[iSel].vRange.x);
				EmitterTimeRangeYnumericUpDown->Value	= Decimal(rVar[iSel].vRange.y);
				EmitterTimeRangeZnumericUpDown->Value	= Decimal(rVar[iSel].vRange.z);
				EmitterTimePositionXnumericUpDown->Value = Decimal(rVar[iSel].vPos.x);
				EmitterTimePositionYnumericUpDown->Value = Decimal(rVar[iSel].vPos.y);
				EmitterTimePositionZnumericUpDown->Value = Decimal(rVar[iSel].vPos.z);
				EmitterVelocityMinUpDown->Value = Decimal(rVar[iSel].fMinSpeed);
				EmitterVelocityMaxUpDown->Value = Decimal(rVar[iSel].fMaxSpeed);
			}

	public: System::Void SetEmitterRangeZEnable(EMITTER_TYPE _type)
			{
				if( EMITTER_RING == _type )
					EmitterTimeRangeZnumericUpDown->Enabled = false;
				else
					EmitterTimeRangeZnumericUpDown->Enabled = true;
			}

	public: System::Void SetEmitterValue(REmitter *pEmitter)
			{
				bool bOldModified = g_bModified;

				// MainForm의 TypeComboBox 선택
				//int type = pEmitter->GetEmitterType();
				//EmitterTypecomboBox->SelectedIndex	= type;

				EmitterLifefnumericUpDown->Value	= Decimal(pEmitter->GetEmitLife()*30);
				EmitterStartTimeNumericUpDown->Value= Decimal(pEmitter->GetStartTime()*30);
				EmitterLoopcheckBox->Checked		= pEmitter->IsLooping();
				EmitterDepSortcheckBox->Checked		= pEmitter->IsSorting();
				RandomColorcheckBox->Checked		= pEmitter->IsRandomColor();
				RandomColorRangeCheckBox->Checked	= pEmitter->IsRandomColorRange();
				RandomVelcheckBox->Checked			= pEmitter->IsRandomVel();
				GathercheckBox->Checked				= pEmitter->IsGather();
				ExplosioncheckBox->Checked			= pEmitter->IsExplosion();
				withmecheckBox->Checked				= pEmitter->IsWithMe();
				FollowParentRotationCheckBox->Checked=pEmitter->IsFollowParentRotation();
				OnWithMeChanged();
				FixAmountcheckBox->Checked			= pEmitter->IsFixAmount();
				RotationcheckBox->Checked			= pEmitter->IsRotate();
				EmitterDistortioncheckBox->Checked	= pEmitter->IsDistortion();
				WorldVelocitycheckBox->Checked		= pEmitter->IsWorldVelocity();

				//SetEmitterRangeZEnable( pEmitter->GetEmitterType() );				

				RotationMinnumericUpDown->Maximum = 180;
				RotationMaxnumericUpDown->Minimum = -180;

				float fmin = RadianToDegree(pEmitter->GetRotationRadian().m_Min);
				float fmax = RadianToDegree(pEmitter->GetRotationRadian().m_Max);

				if( fmin > 180 ) fmin = 180;
				if( fmin < -180) fmin = -180;
				if( fmax > 180 ) fmax = 180;
				if( fmax < -180) fmax = -180;

				RotationMinnumericUpDown->Value		= Decimal(fmin);
				RotationMaxnumericUpDown->Value		= Decimal(fmax);

				BillboardTypeComboBox->SelectedIndex = pEmitter->GetBillboardType();

				SetEmitterTimeVariables(pEmitter, pEmitter->GetEmitterTrack());
				g_bModified = bOldModified;				
			}

	public: System::Void UpdateParticleTimeVariables(REmitter *pEmitter, const std::vector<RParticleVariable> &rVar, int iSel)
			{
				bool bOldModified = g_bModified;
				if ( rVar.empty())
					return;

				ParticleTimePosnumericUpDown->Value		= Decimal(rVar[iSel].fEventTime*g_pMainApp->GetSelectedEmitter()->GetParticleLife().m_Max*30);
				ParticleTimePosnumericUpDown->Maximum	= Decimal(g_pMainApp->GetSelectedEmitter()->GetParticleLife().m_Max*30);

				// MainForm의 TimeTrackBar 세팅
				//TimeTrackBar->Value						= Decimal::ToInt16(ParticleTimePosnumericUpDown->Value);

				//color
				ColorSelectbutton->BackColor			= Color::FromArgb((int)(rVar[iSel].vColor.r*255), (int)(rVar[iSel].vColor.g*255), (int)(rVar[iSel].vColor.b*255));

				VisibilitynumericUpDown->Value	= Decimal(rVar[iSel].vColor.a);
				ParticleTimeVelMinnumericUpDown->Value	= Decimal(rVar[iSel].fMinSpeed);//*10);
				ParticleTimeVelMaxnumericUpDown->Value	= Decimal(rVar[iSel].fMaxSpeed);//*10);
				ParticleTimeVelDirXnumericUpDown->Value = Decimal(rVar[iSel].vDirection.x);
				ParticleTimeVelDirYnumericUpDown->Value = Decimal(rVar[iSel].vDirection.y);
				ParticleTimeVelDirZnumericUpDown->Value = Decimal(rVar[iSel].vDirection.z);
				ParticleTimeGRatenumericUpDown->Value	= Decimal(rVar[iSel].fGRate);

				// Value Property를 바꿀때 이벤트 수신을 끔
				g_bIgnoreValueChangedMessage = true;
				ParticleTimeScaleVarnumericUpDown->Value	= Decimal(rVar[iSel].fNoise);
				ParticleTimeScaleXnumericUpDown->Value		= Decimal(rVar[iSel].fScaleX);
				ParticleTimeScaleYnumericUpDown->Value		= Decimal(rVar[iSel].fScaleY);
				ParticleVariableNumericUpDown1->Value		= Decimal(rVar[iSel].fInitScaleX);
				ParticleVariableYNumericUpDown->Value		= Decimal(rVar[iSel].fInitScaleY);
				g_bIgnoreValueChangedMessage = false;

				g_bModified = bOldModified;
			}

	public: System::Void UpdateParticleTimeVariables(REmitter *pEmitter, const std::vector<RParticleVariable> &rVar)
			{
				ParticleTimeTracklistBox->Items->Clear();
				for(size_t i=0;i< rVar.size();i++)
				{
					bool bLockState = false;
					if( rVar[i].fScaleX == rVar[i].fScaleY &&
						rVar[i].fInitScaleX == rVar[i].fInitScaleY )
					{
						bLockState = true;
					}

					ParticleTimeTracklistBox->Items->Add( gcnew ParticleItemObject(String::Format("{0}",
						rVar[i].fEventTime * pEmitter->GetParticleLife().m_Max*30), bLockState));
				}

				ParticleScaleLock->Checked = true;
				UpdateParticleTimeVariables(pEmitter, pEmitter->GetParticleTrack(), 0);

				// 파티클 키를 초기화 후 첫번째 키를 선택해준다.
				if( ParticleTimeTracklistBox->Items->Count > 0 )
					ParticleTimeTracklistBox->SelectedIndex = 0;
			}

	public: System::Void SetParticleValue(REmitter *pEmitter)
			{
				bool bOldModified = g_bModified;

				TexFileNametextBox->Text = gcnew String(pEmitter->GetTextureName().c_str());
				TexSIFileNametextBox->Text = gcnew String(pEmitter->GetSITextureName().c_str());

				BlendAddradioButton->Checked = pEmitter->IsBlendModeAdd();
				BlendBlendradioButton->Checked = pEmitter->IsBlendModeAlpha();
				BlendOpaqueRadioButton->Checked = pEmitter->IsBlendModeOpaque();
				InvradioButton->Checked = pEmitter->IsBlendModeInverse();
				UseAlphaTestCheckBox->Checked = pEmitter->IsUseAlphaTest();
				AlphatestValuenumericUpDown->Value = Decimal(pEmitter->GetAlphaRef());

				Decimal particleLifeMin( pEmitter->GetParticleLife().m_Min*30 );
				Decimal particleLifeMax( pEmitter->GetParticleLife().m_Max*30 );
				Decimal  SIScale(pEmitter->GetSelfIlluminationScale());
				
				ParticleLifeMinnumericUpDown->Value	= particleLifeMin;
				ParticleLifemaxnumericUpDown->Value = particleLifeMax;
				spnSelfIlluminationScale->Value = SIScale;

				UpdateParticleTimeVariables(pEmitter, pEmitter->GetParticleTrack());
				g_bModified = bOldModified;
			}
			 // mesh - tab
	private: System::Void MeshLoopcheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
			 {
 				 if (g_bIgnoreValueChangedMessage)
					 return;

				 g_pMainApp->GetSelectedEffectActor()->SetLooping( MeshLoopcheckBox->Checked ? true : false );
				 g_bModified = true;
			 }
	private: System::Void MeshStartTimeNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if( REffectActor* pEffectActor = g_pMainApp->GetSelectedEffectActor() )
				 {
					 float fStartTime = ((float)MeshStartTimeNumericUpDown->Value)/30;
					 pEffectActor->SetStartTime(fStartTime);
					 g_bModified = true;
				 }
			 }
	private: System::Void MeshNoOptionRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 MeshTransformOption_Changed();
				 g_bModified = true;
			 }
	private: System::Void MeshAlignRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 MeshTransformOption_Changed();
				 g_bModified = true;
			 }
	private: System::Void MeshAlignXYRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 MeshTransformOption_Changed();
				 g_bModified = true;
			 }
	private: System::Void MeshRotateXRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 MeshTransformOption_Changed();
				 g_bModified = true;
			 }
	private: System::Void MeshRotateYRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 MeshTransformOption_Changed();
				 g_bModified = true;
			 }
	private: System::Void MeshRotateZRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 MeshTransformOption_Changed();
				 g_bModified = true;
			 }
	private: System::Void MeshRotateMinNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if( MeshRotateMinNumericUpDown->Value > MeshRotateMaxNumericUpDown->Value )
					 MeshRotateMaxNumericUpDown->Value = MeshRotateMinNumericUpDown->Value;

				 MeshTransformOption_Changed();
				 g_bModified = true;
			 }
	private: System::Void MeshRotateMaxNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if( MeshRotateMaxNumericUpDown->Value < MeshRotateMinNumericUpDown->Value )
					 MeshRotateMinNumericUpDown->Value = MeshRotateMaxNumericUpDown->Value;

				 MeshTransformOption_Changed();
				 g_bModified = true;
			 }
	private: System::Void MeshTransformOption_Changed()
			 {
				 REffectActor* pEffectActor = g_pMainApp->GetSelectedEffectActor();
				 if( pEffectActor )
				 {
					 float fRotateMin = DegreeToRadian( (float)MeshRotateMinNumericUpDown->Value );
					 float fRotateMax = DegreeToRadian( (float)MeshRotateMaxNumericUpDown->Value );

					 if( MeshNoOptionRadioButton->Checked )
					 {
						 pEffectActor->RemoveTransformController();
						 pEffectActor->SetTransform( RMatrix::IDENTITY );
					 }
					 else
					 if( MeshAlignRadioButton->Checked )
						 pEffectActor->SetAlignController();
					 else
					 if( MeshAlignXYRadioButton->Checked )
						 pEffectActor->SetAlignZFixedController();
					 else
					 if( MeshRotateXRadioButton->Checked )
						 pEffectActor->SetRotateXController( fRotateMin, fRotateMax );
					 else
					 if( MeshRotateYRadioButton->Checked )
						 pEffectActor->SetRotateYController( fRotateMin, fRotateMax );
					 else
					 if( MeshRotateZRadioButton->Checked )
						 pEffectActor->SetRotateZController( fRotateMin, fRotateMax );

					 if( MeshRotateXRadioButton->Checked || MeshRotateYRadioButton->Checked || MeshRotateZRadioButton->Checked )
					 {
						 MeshRotateMinNumericUpDown->Enabled = true;
						 MeshRotateMaxNumericUpDown->Enabled = true;
					 }
					 else
					 {
						 MeshRotateMinNumericUpDown->Enabled = false;
						 MeshRotateMaxNumericUpDown->Enabled = false;
					 }
				 }
			 }
	private: System::Void MeshScalenumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
			 {
 				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if (g_pMainApp->GetSelectedEffectActor())
				 {
					 g_pMainApp->GetSelectedEffectActor()->SetScale(RVector(1.0f, 1.0f, 1.0f) * (float)MeshScalenumericUpDown->Value);					 
					 g_bModified = true;
				 }
			 }
			 // emitter - tab
	private: System::Void EmitterLifefnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
 				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
				 {
					 pEmitter->Stop();
					 pEmitter->SetEmitLife(((float)EmitterLifefnumericUpDown->Value) / 30);
					 GlobalObjects::g_pTrackBar->SetRange(0, (int)EmitterLifefnumericUpDown->Value);

					 SetEmitterTimeVariables(pEmitter, pEmitter->GetEmitterTrack());
					 pEmitter->Start();

					 g_bModified = true;
				 }
			 }
	private: System::Void EmitterStartTimeNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
				 {
					 float fStartTime = ((float)EmitterStartTimeNumericUpDown->Value)/30;
					 pEmitter->SetStartTime(fStartTime);
					 g_bModified = true;
				 }
			}
	private: System::Void EmitterLoopcheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
 				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
				 {
					 pEmitter->SetLooping(EmitterLoopcheckBox->Checked);
					 g_bModified = true;
				 }
			 }
	private: System::Void EmitterDepSortcheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
				 {
					 pEmitter->SetSorting(EmitterDepSortcheckBox->Checked);
					 g_bModified = true;
				 }
			 }
	private: System::Void RandomColorcheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
				 {
					 pEmitter->SetRandomColor(RandomColorcheckBox->Checked);

					 if ( RandomColorcheckBox->Checked )
						 RandomColorRangeCheckBox->Checked = false;

					 g_bModified = true;
				 }
			 }
	private: System::Void RandomColorRangeCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
				 {
					 pEmitter->SetRandomColorRange(RandomColorRangeCheckBox->Checked);

					 if ( RandomColorRangeCheckBox->Checked )
						 RandomColorcheckBox->Checked = false;

					 g_bModified = true;
				 }
			 }
	private: System::Void RandomVelcheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
				 {
					 pEmitter->SetRandomVel(RandomVelcheckBox->Checked);
					 g_bModified = true;
				 }
			 }
	private: System::Void GathercheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
				 {
					 pEmitter->SetGather(GathercheckBox->Checked);
					 g_bModified = true;

					 if( GathercheckBox->Checked && ExplosioncheckBox->Checked )
						ExplosioncheckBox->Checked = false;
				 }
			 }
	private: System::Void ExplosioncheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
				 {
					 pEmitter->SetExplosion(ExplosioncheckBox->Checked);
					 g_bModified = true;

					 if( ExplosioncheckBox->Checked && GathercheckBox->Checked )
						GathercheckBox->Checked = false;
				 }
			 }
	private: System::Void withmecheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
				 {
					 pEmitter->SetWithMe(withmecheckBox->Checked);
					 g_bModified = true;

					 OnWithMeChanged();
				 }
			 }
	private: System::Void OnWithMeChanged() {
					 if( withmecheckBox->Checked )
						 FollowParentRotationCheckBox->Enabled = true;
					 else
						 FollowParentRotationCheckBox->Enabled = false;
			 }
	private: System::Void FixAmountcheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
				 {
					 pEmitter->SetFixAmount(FixAmountcheckBox->Checked);
					 g_bModified = true;
				 }
			 }
	private: System::Void FollowParentRotationCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
				 {
					 pEmitter->SetFollowParentRotation(FollowParentRotationCheckBox->Checked);
					 g_bModified = true;
				 }
			 }
	private: System::Void RotationcheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if( g_pMainApp->GetSelectedEmitter() == NULL) 
					 return;

				 g_pMainApp->GetSelectedEmitter()->SetRotate(RotationcheckBox->Checked);
				 g_bModified = true;
			 }
	private: System::Void RotationMinnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if( g_pMainApp->GetSelectedEmitter() == NULL) return;

				 if( RotationMinnumericUpDown->Value > RotationMaxnumericUpDown->Value)
					 RotationMaxnumericUpDown->Value = RotationMinnumericUpDown->Value;

				 g_pMainApp->GetSelectedEmitter()->SetRotationRadian(CMinMax<float>(DegreeToRadian((float)RotationMinnumericUpDown->Value), g_pMainApp->GetSelectedEmitter()->GetRotationRadian().m_Max));

				 g_bModified = true;
			 }
	private: System::Void RotationMaxnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if( g_pMainApp->GetSelectedEmitter() == NULL) return;

				 if( RotationMinnumericUpDown->Value > RotationMaxnumericUpDown->Value)
					 RotationMinnumericUpDown->Value = RotationMaxnumericUpDown->Value;

				 g_pMainApp->GetSelectedEmitter()->SetRotationRadian(CMinMax<float>(g_pMainApp->GetSelectedEmitter()->GetRotationRadian().m_Min, DegreeToRadian((float)RotationMaxnumericUpDown->Value)));
				 g_bModified = true;
			 }

	private: System::Void EmitterTimrTracklistBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
				 if( EmitterTimrTracklistBox->SelectedIndices->Count > 1)
				 {
					 // [10/2/2007 madduck] 드래그 하면서 첨 선택된 인덱스의 값이 다른 인덱스들의 값으로 카피되는 걸 막기위해서.
				 }
				 else if( EmitterTimrTracklistBox->SelectedIndex > -1)
				 {
					 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
					 _ASSERT(pEmitter);
					 SetEmitterTimeVariables(pEmitter, pEmitter->GetEmitterTrack(), EmitterTimrTracklistBox->SelectedIndex);
				 }
			 }

	private: System::Void EmitterTimeAddbutton_Click(System::Object^  sender, System::EventArgs^  e) {
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 if ( pEmitter == NULL )
					 return;

				 if( g_pMainApp->GetSelectedEmitter()->GetEmitLife() == 0.0f )
					 return;

				 float fEventTime = (float)GlobalObjects::g_fTrackBarValue/(g_pMainApp->GetSelectedEmitter()->GetEmitLife()*30);

				 vector<REmitterVariable> &rVar = pEmitter->GetEmitterTrack();
				 for ( UINT ui = 0; ui<rVar.size(); ++ui )
				 {
					 if ( rVar[ui].fEventTime == fEventTime )
					 {
						 System::Windows::Forms::MessageBox::Show(L"동일한 시간은 추가할 수 없습니다.");
						 return;
					 }
				 }

				 REmitterVariable ptVar;
				 ptVar.fEventTime= (float)GlobalObjects::g_fTrackBarValue/(g_pMainApp->GetSelectedEmitter()->GetEmitLife()*30);
				 ptVar.uAmount	= (int)EmitterTimeAmountnumericUpDown->Value;
				 ptVar.fAngle	= (float)EmitterTimeAnglenumericUpDown->Value;
				 ptVar.vRange	= RVector((float)EmitterTimeRangeXnumericUpDown->Value,
					 (float)EmitterTimeRangeYnumericUpDown->Value, (float)EmitterTimeRangeZnumericUpDown->Value);	//
				 ptVar.vPos		= RVector((float)EmitterTimePositionXnumericUpDown->Value,
					 (float)EmitterTimePositionYnumericUpDown->Value, (float)EmitterTimePositionZnumericUpDown->Value);

				 pEmitter->GetEmitterTrack().push_back(ptVar);
				 pEmitter->UpdateEmitterVar();

				 SetEmitterTimeVariables(pEmitter, pEmitter->GetEmitterTrack());
				 EmitterTimrTracklistBox->SelectedIndex = pEmitter->GetEmittertimetrackNum(ptVar.fEventTime);

				 g_bModified = true;
			 }
	private: System::Void EmitterTimeDelbutton_Click(System::Object^  sender, System::EventArgs^  e) {
				 if( EmitterTimrTracklistBox->SelectedIndex > -1)
				 {
					 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();

					 int iSel = EmitterTimrTracklistBox->SelectedIndex;
					 int size = pEmitter->GetEmitterTrack().size();

					 if ( iSel < 1 || (iSel >= size -1))
					 {
						 System::Windows::Forms::MessageBox::Show(L"처음과 끝은 삭제할 수 없습니다.", "에러");
						 return;
					 }

					 std::vector<REmitterVariable> &rVar = pEmitter->GetEmitterTrack();

					 pEmitter->Stop();
					 pEmitter->DeleteAllParticles();

					 rVar.erase( rVar.begin() + iSel);
					 pEmitter->UpdateEmitterVar();

					 SetEmitterTimeVariables(pEmitter, pEmitter->GetEmitterTrack());

					 g_bModified = true;
				 }
			 }
	private: System::Void EmitterTimeAmountnumericUpDown_ValueChanged(System::Object^ sender, System::EventArgs^ e) {
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 System::Diagnostics::Debug::Assert(pEmitter!=NULL);

				 for( int i = 0; i < EmitterTimrTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = EmitterTimrTracklistBox->SelectedIndices[i];
					 pEmitter->GetEmitterTrack()[index].uAmount	= (UINT)EmitterTimeAmountnumericUpDown->Value;
					 g_bModified = true;
				 }
			 }
	private: System::Void EmitterTimePosnumericUpDown_ValueChanged(System::Object^ sender, System::EventArgs^ e) {

				 int posValue = Decimal::ToInt32(EmitterTimePosnumericUpDown->Value);
				 if( posValue > GlobalObjects::g_pTrackBar->Maximum )
				 {
					 EmitterTimePosnumericUpDown->Value = GlobalObjects::g_pTrackBar->Maximum;
					 posValue = GlobalObjects::g_pTrackBar->Maximum;
				 }
				 else
				 if( posValue < GlobalObjects::g_pTrackBar->Minimum )
				 {
					 EmitterTimePosnumericUpDown->Value = GlobalObjects::g_pTrackBar->Minimum;
					 posValue = GlobalObjects::g_pTrackBar->Minimum;
				 }
				 GlobalObjects::g_pTrackBar->Value	= posValue;

				 GlobalObjects::g_fTrackBarValue	= Decimal::ToSingle(EmitterTimePosnumericUpDown->Value);
			 }
	private: System::Void EmitterTimeAnglenumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 System::Diagnostics::Debug::Assert(pEmitter!=NULL);

				 for( int i = 0; i < EmitterTimrTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = EmitterTimrTracklistBox->SelectedIndices[i];
					 pEmitter->GetEmitterTrack()[index].fAngle = (float)EmitterTimeAnglenumericUpDown->Value;
					 g_bModified = true;
				 }
			 }
	private: System::Void EmitterTimePositionXnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 for( int i = 0; i < EmitterTimrTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = EmitterTimrTracklistBox->SelectedIndices[i];
					 pEmitter->GetEmitterTrack()[index].vPos.x = (float)EmitterTimePositionXnumericUpDown->Value;
					 pEmitter->UpdateEmitterVar();
					 g_bModified = true;
				 }
			 }
	private: System::Void EmitterTimePositionYnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 for( int i = 0; i < EmitterTimrTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = EmitterTimrTracklistBox->SelectedIndices[i];
					 pEmitter->GetEmitterTrack()[index].vPos.y = (float)EmitterTimePositionYnumericUpDown->Value;
					 pEmitter->UpdateEmitterVar();
					 g_bModified = true;
				 }
			 }
	private: System::Void EmitterTimePositionZnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 for( int i = 0; i < EmitterTimrTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = EmitterTimrTracklistBox->SelectedIndices[i];
					 pEmitter->GetEmitterTrack()[index].vPos.z = (float)EmitterTimePositionZnumericUpDown->Value;
					 pEmitter->UpdateEmitterVar();
					 g_bModified = true;
				 }
			 }
			 // End : Position XYZ 업데이트 부분

	private: System::Void EmitterTimeRangeXnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 for( int i = 0; i < EmitterTimrTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = EmitterTimrTracklistBox->SelectedIndices[i];
					 pEmitter->GetEmitterTrack()[index].vRange.x = (float)EmitterTimeRangeXnumericUpDown->Value;
					 g_bModified = true;
				 }
			 }
	private: System::Void EmitterTimeRangeYnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 for( int i = 0; i < EmitterTimrTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = EmitterTimrTracklistBox->SelectedIndices[i];
					 pEmitter->GetEmitterTrack()[index].vRange.y = (float)EmitterTimeRangeYnumericUpDown->Value;
					 g_bModified = true;
				 }
			 }
	private: System::Void EmitterTimeRangeZnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 for( int i = 0; i < EmitterTimrTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = EmitterTimrTracklistBox->SelectedIndices[i];
					 pEmitter->GetEmitterTrack()[index].vRange.z = (float)EmitterTimeRangeZnumericUpDown->Value;
					 g_bModified = true;
				 }
			 }
	private: System::Void TexFileOpenbutton_Click(System::Object^  sender, System::EventArgs^  e) {
				 if( g_pMainApp->GetSelectedEmitter() == NULL) 
					 return;

				 OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;
				 openFileDialog1->InitialDirectory = gcnew String( CToolConfig::GetInstance().m_strEffectTexturePath.c_str() );
				 openFileDialog1->Filter = "Image Files(bmp, tga, jpg, png, dds)|*.bmp;*.tga;*.jpg;*.png;*.dds|bmp Files (*.bmp)|*.bmp|tga files (*.tga)|*.tga|jpg Files (*.jpg)|*.jpg|png files (*.png)|*.png|dds files (*.dds)|*.dds|All files (*.*)|*.*";
				 openFileDialog1->FilterIndex = 0;
				 openFileDialog1->RestoreDirectory = true;

				 if ( openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				 {
					 String^ FileName = openFileDialog1->FileName;	//path까지 다 들어온다.
					 g_pMainApp->SetEmitterTexture(FileName);

					 UVmodifybutton_Click(sender, e);
				 }
			 }
	private: System::Void BlendAddradioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
  				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if( g_pMainApp->GetSelectedEmitter() == NULL) 
					 return;

				 if(BlendAddradioButton->Checked)
				 {
					 g_pMainApp->GetSelectedEmitter()->SetBlendModeAdd();
					 g_bModified = true;
				 }
			 }
	private: System::Void BlendBlendradioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
 				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if( g_pMainApp->GetSelectedEmitter() == NULL) 
					 return;

				 if(BlendBlendradioButton->Checked)
				 {
					 g_pMainApp->GetSelectedEmitter()->SetBlendModeAlpha();
					 g_bModified = true;
				 }
			 }
	private: System::Void InvradioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
 				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if( g_pMainApp->GetSelectedEmitter() == NULL) 
					 return;

				 if(InvradioButton->Checked)
				 {
					 g_pMainApp->GetSelectedEmitter()->SetBlendModeInverse();
					 g_bModified = true;
				 }
			 }
	private: System::Void BlendOpaqueRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
 				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if( g_pMainApp->GetSelectedEmitter() == NULL) 
					 return;

				 if(BlendOpaqueRadioButton->Checked)
				 {
					 g_pMainApp->GetSelectedEmitter()->SetBlendModeOpaque();
					 g_bModified = true;
				 }
			 }
	private: System::Void UseAlphaTestCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
 				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if( g_pMainApp->GetSelectedEmitter() == NULL) 
					 return;

				 g_pMainApp->GetSelectedEmitter()->SetUseAlphaTest(UseAlphaTestCheckBox->Checked);
				 g_bModified = true;
			 }
	private: System::Void AlphatestValuenumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
 				 if (g_bIgnoreValueChangedMessage)
					 return;

				 if( g_pMainApp->GetSelectedEmitter() == NULL) 
					 return;

				 g_pMainApp->GetSelectedEmitter()->SetAlphaRef((float)AlphatestValuenumericUpDown->Value);
				 g_bModified = true;
			 }

	private: System::Void ParticleTimeAddbutton_Click(System::Object^  sender, System::EventArgs^  e) {
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 if (pEmitter == NULL)
					 return;

				 float fEventTime= (float)GlobalObjects::g_fTrackBarValue/(pEmitter->GetParticleLife().m_Max*30);
				 std::vector<RParticleVariable> &rVar = pEmitter->GetParticleTrack();

				 for ( UINT ui = 0; ui<rVar.size(); ++ui )
				 {
					 if ( rVar[ui].fEventTime == fEventTime )
					 {
						 System::Windows::Forms::MessageBox::Show(L"동일한 시간은 추가할 수 없습니다.");
						 return;
					 }
				 }

				 RParticleVariable ptVar;
				 ptVar.fEventTime	= GlobalObjects::g_fTrackBarValue/(pEmitter->GetParticleLife().m_Max*30);
				 ptVar.vColor.r		= (float)ColorSelectbutton->BackColor.R/255.f;		//
				 ptVar.vColor.g		= (float)ColorSelectbutton->BackColor.G/255.f;		//
				 ptVar.vColor.b		= (float)ColorSelectbutton->BackColor.B/255.f;		//
				 ptVar.vColor.a		= 1.0f;
				 ptVar.fMinSpeed		= (float)ParticleTimeVelMinnumericUpDown->Value/10;
				 ptVar.fMaxSpeed		= (float)ParticleTimeVelMaxnumericUpDown->Value/10;
				 ptVar.vDirection.x		= (float)ParticleTimeVelDirXnumericUpDown->Value;	//
				 ptVar.vDirection.y		= (float)ParticleTimeVelDirYnumericUpDown->Value;	//
				 ptVar.vDirection.z		= (float)ParticleTimeVelDirZnumericUpDown->Value;	//
				 ptVar.fGRate		= (float)ParticleTimeGRatenumericUpDown->Value;		//

				 ptVar.fScaleX	= (float)ParticleTimeScaleXnumericUpDown->Value;
				 ptVar.fScaleY	= (float)ParticleTimeScaleYnumericUpDown->Value;

				 ptVar.fNoise	= (float)ParticleTimeScaleVarnumericUpDown->Value;

				 pEmitter->GetParticleTrack().push_back(ptVar);

				 pEmitter->UpdateParticleVar();
				 SetParticleValue(pEmitter);
				 ParticleTimeTracklistBox->SelectedIndex = pEmitter->GetParticletimetrackNum(ptVar.fEventTime);

				 // RParticleVariable가 늘어났지만 m_vecInitScale가 추가가 안되서 서로 길이가 동기화 되지 않는다. - 090603, OZ
				 // m_vecInitScale도 값으 넣어주면 되지요. 문제는 어케 넣느냐...
				 // m_vecInitScale은 파티클마다 소유한다. ParticleVariable은 이미터(엄밀히는 디스크립션)가 소유한다.
				 // 이미터의 모든 파티클에 m_vecInitScale를 조절해주면되나
				 // m_vecInitScale은 재 조정은 RParticle::Init에서 수행된다. 이를 호출하도록 하는게 깔끔할 듯
				 pEmitter->InitAllParticle();

				 g_bModified = true;
			 }

	private: System::Void ParticleTimeRefineAddbutton_Click(System::Object^  sender, System::EventArgs^  e) {
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 if (pEmitter == NULL)
					 return;

				 Int32 nTrackCount = ParticleTimeTracklistBox->SelectedIndices->Count;
				 if ( nTrackCount < 2)
				 {
					 ::MessageBox(NULL, "2개 이상의 track을 선택하세요", "ERROR", MB_ICONERROR);
					 return;
				 }

				 std::vector<RParticleVariable> &rVar = pEmitter->GetParticleTrack();
				 int nFirstSelectedIndex = ParticleTimeTracklistBox->SelectedIndices[0]+1;

				 for ( Int32 i = 0; i<nTrackCount-1; ++i )
				 {
					 int nIndex1 = ParticleTimeTracklistBox->SelectedIndices[i];
					 int nIndex2 = ParticleTimeTracklistBox->SelectedIndices[i+1];
					 const RParticleVariable &ptCurrentVar = rVar[nIndex1];
					 const RParticleVariable &ptNextVar = rVar[nIndex2];

					 RParticleVariable ptVar;
					 ptVar.fEventTime	= (ptCurrentVar.fEventTime + ptNextVar.fEventTime)/2;
					 ptVar.vColor.r		= (ptCurrentVar.vColor.r + ptNextVar.vColor.r)/2;
					 ptVar.vColor.g		= (ptCurrentVar.vColor.g + ptNextVar.vColor.g)/2;
					 ptVar.vColor.b		= (ptCurrentVar.vColor.b + ptNextVar.vColor.b)/2;
					 ptVar.vColor.a		= (ptCurrentVar.vColor.a + ptNextVar.vColor.a)/2;
					 ptVar.fMinSpeed		= (ptCurrentVar.fMinSpeed + ptNextVar.fMinSpeed)/2;
					 ptVar.fMaxSpeed		= (ptCurrentVar.fMaxSpeed + ptNextVar.fMaxSpeed)/2;
					 ptVar.vDirection = (ptCurrentVar.vDirection + ptNextVar.vDirection)/2;
					 ptVar.fGRate		= (ptCurrentVar.fGRate + ptNextVar.fGRate)/2;

					 ptVar.fScaleX	= (ptCurrentVar.fScaleX + ptNextVar.fScaleX)/2;
					 ptVar.fScaleY	= (ptCurrentVar.fScaleY + ptNextVar.fScaleY)/2;

					 // init scale
					 ptVar.fInitScaleX	= (ptCurrentVar.fInitScaleX + ptNextVar.fInitScaleX)/2;
					 ptVar.fInitScaleY	= (ptCurrentVar.fInitScaleY + ptNextVar.fInitScaleY)/2;

					 pEmitter->GetParticleTrack().push_back(ptVar);
				 }
				 pEmitter->UpdateParticleVar();
				 SetParticleValue(pEmitter);
				 ParticleTimeTracklistBox->SelectedIndex = nFirstSelectedIndex;

				 pEmitter->InitAllParticle();

				 g_bModified = true;
			 }

	private: System::Void ParticleTimeDelbutton_Click(System::Object^  sender, System::EventArgs^  e) {

				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 if (pEmitter == NULL)
					 return;

				 if( ParticleTimeTracklistBox->SelectedIndex > -1)
				 {
					 int iSel = ParticleTimeTracklistBox->SelectedIndex;
					 int size = pEmitter->GetParticleTrack().size();
					 if ( iSel < 1 || (iSel >= size -1))
					 {
						 System::Windows::Forms::MessageBox::Show(L"처음과 끝은 삭제할 수 없습니다.", "에러");
						 return;
					 }

					 std::vector<RParticleVariable> &rVar = pEmitter->GetParticleTrack();

					 pEmitter->Stop();
					 pEmitter->DeleteAllParticles();

					 rVar.erase( rVar.begin() + iSel);
					 pEmitter->UpdateParticleVar();

					 UpdateParticleTimeVariables(pEmitter, pEmitter->GetParticleTrack());
					 if (pEmitter->IsLooping())
						 g_pMainApp->StartButton();

					 g_bModified = true;
				 }
			 }

	private: System::Void ParticleTimePosnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 GlobalObjects::g_pTrackBar->Value	= Decimal::ToInt32(ParticleTimePosnumericUpDown->Value);
				 GlobalObjects::g_fTrackBarValue = Decimal::ToSingle(ParticleTimePosnumericUpDown->Value);
			 }

	private: System::Void ColorSelectbutton_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
 				 System::Drawing::Color^ color = gcnew System::Drawing::Color();
				 *color = ColorSelectbutton->BackColor;

				 if( MHelp::DoColorDialog(color))
				 {
					 ColorSelectbutton->BackColor = *color;

					 MVector4 vColor = MHelp::ColorToMVector4(color);
					 vColor.a = 1.0f;

					 for( int i = 0; i < ParticleTimeTracklistBox->SelectedIndices->Count; ++i)
					 {
						 int index = ParticleTimeTracklistBox->SelectedIndices[i];
						 RParticleVariable &ptVar = g_pMainApp->GetSelectedEmitter()->GetParticleTrack()[index];
						 ptVar.vColor.r	= vColor.r;
						 ptVar.vColor.g	= vColor.g;
						 ptVar.vColor.b	= vColor.b;
					 }
					 g_bModified = true;
				 }
			 }
	private: System::Void ParticleTimeAlphanumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 }

			 // Update Local AABB
	private: System::Void ParticleTimeVelMinnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 for( int i = 0; i < ParticleTimeTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = ParticleTimeTracklistBox->SelectedIndices[i];
					 g_pMainApp->GetSelectedEmitter()->GetParticleTrack()[index].fMinSpeed = (float)ParticleTimeVelMinnumericUpDown->Value;///10;
					 g_bModified = true;
				 }
			 }
	private: System::Void ParticleTimeVelMaxnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 for( int i = 0; i < ParticleTimeTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = ParticleTimeTracklistBox->SelectedIndices[i];
					 g_pMainApp->GetSelectedEmitter()->GetParticleTrack()[index].fMaxSpeed = (float)ParticleTimeVelMaxnumericUpDown->Value;///10;
					 g_bModified = true;
				 }
			 }

	private: System::Void ParticleTimeVelDirXnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 for( int i = 0; i < ParticleTimeTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = ParticleTimeTracklistBox->SelectedIndices[i];
					 g_pMainApp->GetSelectedEmitter()->GetParticleTrack()[index].vDirection.x = (float)ParticleTimeVelDirXnumericUpDown->Value;
					 g_bModified = true;
				 }
			 }
	private: System::Void ParticleTimeVelDirYnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 for( int i = 0; i < ParticleTimeTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = ParticleTimeTracklistBox->SelectedIndices[i];
					 g_pMainApp->GetSelectedEmitter()->GetParticleTrack()[index].vDirection.y = (float)ParticleTimeVelDirYnumericUpDown->Value;
					 g_bModified = true;
				 }
			 }
	private: System::Void ParticleTimeVelDirZnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 for( int i = 0; i < ParticleTimeTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = ParticleTimeTracklistBox->SelectedIndices[i];
					 g_pMainApp->GetSelectedEmitter()->GetParticleTrack()[index].vDirection.z = (float)ParticleTimeVelDirZnumericUpDown->Value;
					 g_bModified = true;
				 }
			 }
	private: System::Void ParticleTimeGRatenumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 for( int i = 0; i < ParticleTimeTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = ParticleTimeTracklistBox->SelectedIndices[i];
					 float fValue = (float)ParticleTimeGRatenumericUpDown->Value;
					 g_pMainApp->GetSelectedEmitter()->GetParticleTrack()[index].fGRate = fValue;
					 g_bModified = true;
				 }
			 }

	private: System::Void ParticleTimeScalenumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

 				 g_bIgnoreValueChangedMessage = true;
 				 for( int i = 0; i < ParticleTimeTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = ParticleTimeTracklistBox->SelectedIndices[i];
					 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
					 RParticleVariable &ptVar = pEmitter->GetParticleTrack()[index];

					 ptVar.fScaleX = (float)ParticleTimeScaleXnumericUpDown->Value;

					 if ( ParticleScaleLock->Checked )
					 {
						 ParticleTimeScaleYnumericUpDown->Value = ParticleTimeScaleXnumericUpDown->Value;
						 ptVar.fScaleY = (float)ParticleTimeScaleYnumericUpDown->Value;
					 }
				 }
				 g_bIgnoreValueChangedMessage = false;
				 g_bModified = true;
			 }
	private: System::Void ParticleTimeScaleVarnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (g_bIgnoreValueChangedMessage)
					 return;

				 g_bIgnoreValueChangedMessage = true;
				 for( int i = 0; i < ParticleTimeTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = ParticleTimeTracklistBox->SelectedIndices[i];
					 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
					 RParticleVariable &ptVar = pEmitter->GetParticleTrack()[index];

					 ptVar.fNoise = (float)ParticleTimeScaleVarnumericUpDown->Value;
				 }
				 g_bIgnoreValueChangedMessage = false;
				 g_bModified = true;
			 }

	private: System::Void ParticleLifeMinnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 if (pEmitter == NULL)
					 return;

				 pEmitter->Stop();
				 pEmitter->SetParticleLife(CMinMax<float>((float)ParticleLifeMinnumericUpDown->Value/30,
					 pEmitter->GetParticleLife().m_Max));
				 g_bModified = true;

				 if( ParticleLifemaxnumericUpDown->Value < ParticleLifeMinnumericUpDown->Value )
					 ParticleLifemaxnumericUpDown->Value = ParticleLifeMinnumericUpDown->Value;

				 GlobalObjects::g_pTrackBar->SetRange(0, (int)ParticleLifemaxnumericUpDown->Value);
				 UpdateParticleTimeVariables(pEmitter, pEmitter->GetParticleTrack());

				 pEmitter->Start();
			 }
	private: System::Void ParticleLifemaxnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 if (pEmitter == NULL)
					 return;

				 pEmitter->Stop();
				 pEmitter->SetParticleLife(CMinMax<float>(pEmitter->GetParticleLife().m_Min,
					 (float)ParticleLifemaxnumericUpDown->Value/30));
				 g_bModified = true;

				 if( ParticleLifemaxnumericUpDown->Value < ParticleLifeMinnumericUpDown->Value )
					 ParticleLifeMinnumericUpDown->Value = ParticleLifemaxnumericUpDown->Value;

				 GlobalObjects::g_pTrackBar->SetRange(0, (int)ParticleLifemaxnumericUpDown->Value);
				 UpdateParticleTimeVariables(pEmitter, pEmitter->GetParticleTrack());

				 pEmitter->Start();
			 }
	private: System::Void UVmodifybutton_Click(System::Object^  sender, System::EventArgs^  e) {
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 if ( pEmitter == NULL )
					 return;

				 if ( pEmitter->GetTexture() == NULL )
					 return;

				 PicForm ^pEdit = gcnew PicForm(pEmitter);
				 pEdit->ShowDialog();

				 g_bModified = true;
				 System::Windows::Forms::DialogResult res = pEdit->DialogResult;
				 if (res == System::Windows::Forms::DialogResult::Cancel)
					 pEdit->Hide();

				 PointF ^pivot = pEdit->GetTexturePivot();
				 RectangleF ^rect = pEdit->GetTextureRect();

				 pEmitter->SetTextUVPivot(RVector2(pivot->X, pivot->Y));
				 pEmitter->SetTexUV(RVector4(rect->Left, rect->Top, rect->Right, rect->Bottom));

				 pEdit = nullptr;
			 }
private: System::Void ParticleTimeTracklistBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 if( ParticleTimeTracklistBox->SelectedIndices->Count > 1)
			 {
				 // [10/2/2007 madduck] 드래그 하면서 첨 선택된 인덱스의 값이 다른 인덱스들의 값으로 카피되는 걸 막기위해서.
			 }
			 else if( ParticleTimeTracklistBox->SelectedIndex > -1)
			 {
				 UpdateParticleTimeVariables(g_pMainApp->GetSelectedEmitter(), g_pMainApp->GetSelectedEmitter()->GetParticleTrack(), ParticleTimeTracklistBox->SelectedIndex);

				 ParticleItemObject^ itemObject = safe_cast<ParticleItemObject^>(ParticleTimeTracklistBox->SelectedItem);
				 ParticleScaleLock->Checked = ( itemObject->lockState );
			 }
		 }
private: System::Void VisibilitynumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 //add와 blend 를 구분하지말구 알파값을 세팅하도록
			 // ui 이름은 visibility
			 for( int i = 0; i < ParticleTimeTracklistBox->SelectedIndices->Count; ++i)
			 {
				 int index = ParticleTimeTracklistBox->SelectedIndices[i];
				 g_pMainApp->GetSelectedEmitter()->GetParticleTrack()[index].vColor.a = (float)VisibilitynumericUpDown->Value;
				 g_bModified = true;
			 }
		 }

private: System::Void EffectListtreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) {
			 if(!g_pMainApp->ConfirmSave())
			 {
				 return;
			 }

			 // 다이얼로그에서 취소가 눌리었으면 아예 이곳에 오질 않으므로 그냥 진행 - 090603, OZ
			 if( e->Node->Parent != nullptr )
			 {
				 String^ parentName(e->Node->Parent->Text);
				 return;
			 }
			 String^ nodeName(e->Node->Text);
			 String^ FileName = String::Format("{0}", nodeName);

			 //effectmgr에서 로드 하도록
			 bool bOldModifiedInfo = g_bModified;
			 g_pMainApp->LoadEffectFile(FileName);
			 g_bModified = bOldModifiedInfo;	// 이때 뭔가 편집이 바뀌는것은 아니다.
		 }

private: System::Void EffectListtreeView_BeforeSelect(System::Object^  sender, System::Windows::Forms::TreeViewCancelEventArgs^  e) {	// 090603, OZ
			// 저장 확인 : 취소 누르면 이벤트 자체를 취소해야 하기에 비포 셀렉트에서 다이얼로그 처리 - 090603, OZ
			//if(!g_pMainApp->ConfirmSave())
			//{
			//	// 이벤트 취소
			//	e->Cancel = true;
			//}
		 }

private: System::Void AniLoopcheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->OnLoopAnimation();
		 }
private: System::Void AnitreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) {
			 if(g_pMainApp->GetActor() == NULL) return;

			 g_pMainApp->SetAnimation_Play(e->Node->Text);
		 }
private: System::Void NodeListtreeView_AfterSelect(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) {
			 g_pMainApp->SelectedNode(e->Node->Text);
		 }
private: System::Void ReloadEffects_Click(System::Object^  sender, System::EventArgs^  e) {
			 // 저장확인 : 취소 추가 - 090602, OZ
			 if( g_pMainApp->ConfirmSave())
			 {
				g_pMainApp->ReloadEffectList();
			 }
		 }
private: System::Void textoffset_TextChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (g_bIgnoreValueChangedMessage)
				 return;

			 RSceneNode* pSceneNode = g_pMainApp->GetSelectedChild();
			 if ( MDynamicCast( REffectSceneNode, pSceneNode ) )
			 {
				 RVector vOffset(0, 0, 0);
				 float fScale = 1.f;

				 try {
					 vOffset.x = (float)(::Convert::ToSingle(textoffsetX->Text));
					 vOffset.y = (float)(::Convert::ToSingle(textoffsetY->Text));
					 vOffset.z = (float)(::Convert::ToSingle(textoffsetZ->Text));
					 fScale = (float)(::Convert::ToSingle(ScaleTextBox->Text));
				 } 
				 catch (...)
				 {
				 }

				 RMatrix mat;
				 mat.MakeIdentity();
				 mat.SetTranslation(vOffset);
				 mat.SetScale(fScale);
				 pSceneNode->SetTransform( mat );
			 }
		 }
private: System::Void TrailTexFileOpenButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 RTrail* pTrail = g_pMainApp->GetSelectedTrail();
			 if (pTrail == NULL)
				 return;

			 OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;
			 openFileDialog1->InitialDirectory = gcnew String( CToolConfig::GetInstance().m_strEffectTexturePath.c_str() );
			 openFileDialog1->Filter = "Image Files(bmp, tga, jpg, png, dds)|*.bmp;*.tga;*.jpg;*.png;*.dds|bmp Files (*.bmp)|*.bmp|tga files (*.tga)|*.tga|jpg Files (*.jpg)|*.jpg|png files (*.png)|*.png|dds files (*.dds)|*.dds|All files (*.*)|*.*";
			 openFileDialog1->FilterIndex = 0;
			 openFileDialog1->RestoreDirectory = false;

			 if ( openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			 {
				 String^ PathName = openFileDialog1->FileName;
				 g_pMainApp->SetTrailTexture( PathName );

				 TrailTextureUVButton_Click( sender, e );
			 }
		 }
private: System::Void TrailAddRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if ( RTrail* pTrail = g_pMainApp->GetSelectedTrail())
			 {
				 pTrail->SetBlendModeAdd();
				 g_bModified = true;
			 }
		 }
private: System::Void TrailAlphaBlendRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if ( RTrail* pTrail = g_pMainApp->GetSelectedTrail())
			 {
				 pTrail->SetBlendModeAlpha();
				 g_bModified = true;
			 }
		 }
private: System::Void TrailOpaqueBlendRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if ( RTrail* pTrail = g_pMainApp->GetSelectedTrail())
			 {
				 pTrail->SetBlendModeOpaque();
				 g_bModified = true;
			 }
		 }
private: System::Void TrailInverseBlendRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if ( RTrail* pTrail = g_pMainApp->GetSelectedTrail())
			 {
				 pTrail->SetBlendModeInverse();
				 g_bModified = true;
			 }
		 }
private: System::Void TrailUseAlphaTestCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (g_bIgnoreValueChangedMessage)
				 return;

			 if ( RTrail* pTrail = g_pMainApp->GetSelectedTrail())
			 {
				 pTrail->SetUseAlphaTest(TrailUseAlphaTestCheckBox->Checked);
				 g_bModified = true;
			 }
		 }
private: System::Void TrailAlphatestValueNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (g_bIgnoreValueChangedMessage)
				 return;

			 if ( RTrail* pTrail = g_pMainApp->GetSelectedTrail())
			 {
				 pTrail->SetAlphaRef((float)TrailAlphatestValueNumericUpDown->Value);
				 g_bModified = true;
			 }
		 }
private: System::Void TrailAlignCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if ( RTrail* pTrail = g_pMainApp->GetSelectedTrail())
			 {
				 pTrail->SetAlign(TrailAlignCheckBox->Checked);
				 g_bModified = true;
			 }
		 }
private: System::Void TrailLoopCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if ( RTrail* pTrail = g_pMainApp->GetSelectedTrail())
			 {
				 pTrail->SetLooping(TrailLoopCheckBox->Checked);
				 g_bModified = true;
			 }
		 }
private: System::Void TrailLifenumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 RTrail* pTrail = g_pMainApp->GetSelectedTrail();
			 if (pTrail == NULL)
				 return;
			 try
			 {
				 pTrail->SetLife((float)TrailLifenumericUpDown->Value);
				 g_bModified = true;
			 }
			 catch (...)
			 {
				 pTrail->SetLife(0.001f);
			 }
		 }
private: System::Void TrailWidthnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 RTrail* pTrail = g_pMainApp->GetSelectedTrail();
			 if (pTrail == NULL)
				 return;

			 try
			 {
				 pTrail->SetWidth((float)TrailWidthnumericUpDown->Value);
				 g_bModified = true;
			 }
			 catch (...)
			 {
				 pTrail->SetWidth(0.001f);
			 }
		 }
private: System::Void TrailSegmentnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 RTrail* pTrail = g_pMainApp->GetSelectedTrail();
			 if (pTrail == NULL)
				 return;
			 try
			 {
				 pTrail->SetSegment((int)TrailSegmentnumericUpDown->Value);
				 g_bModified = true;
			 }
			 catch (...)
			 {
				 pTrail->SetSegment(1);
			 }
		 }
private: System::Void TrailFrontColorButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 if ( RTrail* pTrail = g_pMainApp->GetSelectedTrail())
			 {
				 System::Drawing::Color^ color = gcnew System::Drawing::Color();
				 *color = TrailFrontColorButton->BackColor;

				 if ( MHelp::DoColorDialog(color))
				 {
					 TrailFrontColorButton->BackColor = *color;
					 MVector4 vColor = MHelp::ColorToMVector4(color);
					 vColor.a = 1.0f;

					 pTrail->SetFrontColor(vColor);
					 g_bModified = true;
				 }
			 }
		 }
private: System::Void TrailBackColorButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 if ( RTrail* pTrail = g_pMainApp->GetSelectedTrail())
			 {
				 System::Drawing::Color^ color = gcnew System::Drawing::Color();
				 *color = TrailBackColorButton->BackColor;

				 if ( MHelp::DoColorDialog(color))
				 {
					 TrailBackColorButton->BackColor = *color;
					 MVector4 vColor = MHelp::ColorToMVector4(color);
					 vColor.a = 1.0f;

					 pTrail->SetBackColor(vColor);
					 g_bModified = true;
				 }
			 }
		 }
private: System::Void TrailTextureUVButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 RTrail *pTrail = g_pMainApp->GetSelectedTrail();
			 if ( pTrail == NULL )
				 return;

			 if ( pTrail->GetTexture() == NULL )
				 return;

			 PicForm ^pEdit = gcnew PicForm(pTrail);
			 pEdit->ShowDialog();

			 g_bModified = true;
			 System::Windows::Forms::DialogResult res = pEdit->DialogResult;
			 if (res == System::Windows::Forms::DialogResult::Cancel)
				 pEdit->Hide();

			 RectangleF ^rect = pEdit->GetTextureRect();
			 pTrail->SetTexUV(RVector4(rect->Left, rect->Top, rect->Right, rect->Bottom));
			 pEdit = nullptr;
		 }
private: System::Void ParticleVariableNumericUpDown1_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (g_bIgnoreValueChangedMessage)
				 return;
			 
			 g_bIgnoreValueChangedMessage = true;
			 if ( ParticleScaleLock->Checked )
				 ParticleVariableYNumericUpDown->Value = ParticleVariableNumericUpDown1->Value;

			 ParticleVariableNumericChange();

			 g_bIgnoreValueChangedMessage = false;
		 }

private: System::Void ParticleVariableYNumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (g_bIgnoreValueChangedMessage)
				 return;

 			 g_bIgnoreValueChangedMessage = true;
			 ParticleVariableNumericChange();
			 g_bIgnoreValueChangedMessage = false;
		 }

private: System::Void ParticleVariableNumericChange() {

 			 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
			 if ( pEmitter == NULL )
				 return;

 			 for( int i = 0; i < ParticleTimeTracklistBox->SelectedIndices->Count; ++i)
			 {
				 int index = ParticleTimeTracklistBox->SelectedIndices[i];
				 pEmitter->GetParticleTrack()[index].fInitScaleX = (float)ParticleVariableNumericUpDown1->Value;
				 pEmitter->GetParticleTrack()[index].fInitScaleY = (float)ParticleVariableYNumericUpDown->Value;
				 g_bModified = true;
			 }
		 }

private: System::Void EmitterVelocityMinUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter())
			 {
				 for( int i = 0; i < EmitterTimrTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = EmitterTimrTracklistBox->SelectedIndices[i];
					 pEmitter->GetEmitterTrack()[index].fMinSpeed = (float)EmitterVelocityMinUpDown->Value;
				 }
				 g_bModified = true;
			 }
		 }

private: System::Void EmitterVelocityMaxUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter())
			 {
				 for( int i = 0; i < EmitterTimrTracklistBox->SelectedIndices->Count; ++i)
				 {
					 int index = EmitterTimrTracklistBox->SelectedIndices[i];
					 pEmitter->GetEmitterTrack()[index].fMaxSpeed = (float)EmitterVelocityMaxUpDown->Value;
				 }
				 g_bModified = true;
			 }
		 }
private: System::Void ParticleTimeScaleYnumericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (g_bIgnoreValueChangedMessage)
				 return;

			 g_bIgnoreValueChangedMessage = true;
			 for( int i = 0; i < ParticleTimeTracklistBox->SelectedIndices->Count; ++i)
			 {
				 int index = ParticleTimeTracklistBox->SelectedIndices[i];
				 REmitter *pEmitter = g_pMainApp->GetSelectedEmitter();
				 RParticleVariable &ptVar = pEmitter->GetParticleTrack()[index];

				 ptVar.fScaleY = (float)ParticleTimeScaleYnumericUpDown->Value;
			 }
			 g_bIgnoreValueChangedMessage = false;
			 g_bModified = true;
		 }
private: System::Void ParticleScaleLock_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 bool bChecked = ( ParticleScaleLock->Checked  ? true : false );
			 if ( bChecked )
			 {
				 ParticleTimeScaleYnumericUpDown->Enabled = false;
				 ParticleTimeScaleYnumericUpDown->Value = ParticleTimeScaleXnumericUpDown->Value;

				 ParticleVariableYNumericUpDown->Enabled = false;
				 ParticleVariableYNumericUpDown->Value = ParticleVariableNumericUpDown1->Value;
			 }
			 else
			 {
				 ParticleTimeScaleYnumericUpDown->Enabled = true;
				 ParticleVariableYNumericUpDown->Enabled = true;
			 }

			 ListBox::SelectedObjectCollection ^ObjectItems = ParticleTimeTracklistBox->SelectedItems;
			 for ( int i = 0; i<ObjectItems->Count; ++i )
			 {
				 ParticleItemObject^ itemObject = safe_cast<ParticleItemObject^>(ObjectItems[i]);
				 itemObject->lockState = bChecked;
			 }
		 }
private: System::Void MeshColorSelectButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 if (g_bIgnoreValueChangedMessage)
				 return;

			 if( g_pMainApp->GetSelectedEffectActor() )
			 {
 				 System::Drawing::Color^ color = gcnew System::Drawing::Color();
				 *color = MeshColorSelectButton->BackColor;

				 if( MHelp::DoColorDialog(color))
				 {
					 MeshColorSelectButton->BackColor = *color;

					 MVector4 vColor = MHelp::ColorToMVector4(color);
					 vColor.a = 1.0f;
					 g_pMainApp->GetSelectedEffectActor()->SetMultiplyColor(vColor);
					 g_bModified = true;
				 }
			 }
		 }
private: System::Void EmitterDistortioncheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (g_bIgnoreValueChangedMessage)
				 return;

			 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
			 {
				 pEmitter->SetDistortion(EmitterDistortioncheckBox->Checked);
				 g_bModified = true;
			 }
		 }
private: System::Void MeshDistortioncheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (g_bIgnoreValueChangedMessage)
				 return;

			 g_pMainApp->GetSelectedEffectActor()->SetDistortion(MeshDistortioncheckBox->Checked);

			 g_bModified = true;
		 }

private: System::Void EMITTER_DISTORTION_PROPERTY_BUTTON_Click(System::Object^  sender, System::EventArgs^  e) {
			 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
			 {
				if( pEmitter->IsDistortion() )
				{
					ReflectivePropertyForm^ reflectivePropertyForm = gcnew ReflectivePropertyForm;	//폼생성
					reflectivePropertyForm->InitializeComponent();									//폼초기화시작

					static CDistortionPropertyRef reflectiveDistortion;
					reflectiveDistortion.SetParam(pEmitter->GetDistortionParam());
					reflectivePropertyForm->setText("Emitter Distortion Property");	
					reflectivePropertyForm->setBaseObject((void*)&reflectiveDistortion);					//폼에 리플렉션 클래스연결

					reflectivePropertyForm->addAllReflection();										//연결된 리플렉션클래스 동기화

					reflectivePropertyForm->Show();													//폼보이기
					reflectivePropertyForm->CompleteToInitializeComponent();

					g_bModified = true;
				 }
			 }
		 }
private: System::Void ACTOR_DISTORTION_PROPERTY_BUTTON_Click(System::Object^  sender, System::EventArgs^  e) {
			 if ( REffectActor *pEffectActor = g_pMainApp->GetSelectedEffectActor() )
			 {
				 if( pEffectActor->IsDistortion() )
				 {
					 ReflectivePropertyForm^ reflectivePropertyForm = gcnew ReflectivePropertyForm;
					 reflectivePropertyForm->InitializeComponent();

					 static CDistortionPropertyRef reflectiveDistortion;
					 reflectiveDistortion.SetParam(pEffectActor->GetDistortionParam());
					 reflectivePropertyForm->setText("EffectActor Distortion Property");	
					 reflectivePropertyForm->setBaseObject((void*)&reflectiveDistortion);

					 reflectivePropertyForm->addAllReflection();

					 reflectivePropertyForm->Show();
					 reflectivePropertyForm->CompleteToInitializeComponent();

					 g_bModified = true;
				 }
			 }
		 }



private: System::Void TexSIFileOpenbutton_Click(System::Object^  sender, System::EventArgs^  e) {

			 OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;
			 openFileDialog1->InitialDirectory = gcnew String( CToolConfig::GetInstance().m_strEffectTexturePath.c_str() );
			 openFileDialog1->Filter = "Image Files(bmp, tga, jpg, png, dds)|*.bmp;*.tga;*.jpg;*.png;*.dds|bmp Files (*.bmp)|*.bmp|tga files (*.tga)|*.tga|jpg Files (*.jpg)|*.jpg|png files (*.png)|*.png|dds files (*.dds)|*.dds|All files (*.*)|*.*";
			 openFileDialog1->FilterIndex = 0;
			 openFileDialog1->RestoreDirectory = true;

			 if ( openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			 {
				 String^ FileName = openFileDialog1->FileName;	//path까지 다 들어온다.
				 g_pMainApp->SetEmitterSITexture(FileName);
				 g_bModified = true;
			 }
		 }
private: System::Void SIDeletebutton_Click(System::Object^  sender, System::EventArgs^  e) {
			 String^ FileName = "";
			 g_pMainApp->SetEmitterSITexture(FileName);
			 g_bModified = true;
		 }
private: System::Void FindTextBox_TextChanged(System::Object^  sender, System::EventArgs^  e) {
			 g_pMainApp->SetEffectListTreeViewFilter( FindTextBox->Text );
			 g_pMainApp->SetEffectListTreeView();
		 }
private: System::Void spnSelfIlluminationScale_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if (g_bIgnoreValueChangedMessage)
				 return;

			 if( g_pMainApp->GetSelectedEmitter() == NULL) 
				 return;

			 g_pMainApp->GetSelectedEmitter()->SetSelfIlluminationScale((float)spnSelfIlluminationScale->Value);
			 g_bModified = true;
		 }
private: System::Void TrailDistortionCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if ( RTrail* pTrail = g_pMainApp->GetSelectedTrail() )
			 {
				 pTrail->SetDistortion(TrailDistortionCheckBox->Checked);
				 g_bModified = true;
			 }
		 }
private: System::Void TRAIL_DISTORTION_PROPERTY_BUTTON_Click(System::Object^  sender, System::EventArgs^  e) {
			 if ( RTrail* pTrail = g_pMainApp->GetSelectedTrail() )
			 {
				 if( pTrail->IsDistortion() )
				 {
					 ReflectivePropertyForm^ reflectivePropertyForm = gcnew ReflectivePropertyForm;
					 reflectivePropertyForm->InitializeComponent();

					 static CDistortionPropertyRef reflectiveDistortion;
					 reflectiveDistortion.SetParam(pTrail->GetDistortionParam());
					 reflectivePropertyForm->setText("EffectActor Distortion Property");	
					 reflectivePropertyForm->setBaseObject((void*)&reflectiveDistortion);

					 reflectivePropertyForm->addAllReflection();

					 reflectivePropertyForm->Show();
					 reflectivePropertyForm->CompleteToInitializeComponent();

					 g_bModified = true;
				 }
			 }
		 }
private: System::Void BillboardTypeComboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (g_bIgnoreValueChangedMessage)
				 return;

			 if( g_pMainApp->GetSelectedEmitter() == NULL) return;

			 int selectedIndex = BillboardTypeComboBox->SelectedIndex;
			 if( selectedIndex != -1 )
			 {
				 g_pMainApp->GetSelectedEmitter()->SetBillboardType((BILLBOARD_TYPE)selectedIndex);
				 g_bModified = true;
			 }
		 }
private: System::Void WorldVelocitycheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (g_bIgnoreValueChangedMessage)
				 return;

			 if ( REmitter *pEmitter = g_pMainApp->GetSelectedEmitter() )
			 {
				 pEmitter->SetWorldVelocity(WorldVelocitycheckBox->Checked);
				 g_bModified = true;
			 }
		 }
private: System::Void ActorAniTreeViewFindTextBox_TextChanged(System::Object^  sender, System::EventArgs^  e)
		 {
			 g_pMainApp->SetAnitreeViewFilter( ActorAniTreeViewFindTextBox->Text );
			 g_pMainApp->SetAnitreeView();
		 }
private: System::Void ActorNodeFindTextBox_TextChanged(System::Object^  sender, System::EventArgs^  e)
		 {
			 g_pMainApp->SetNodeListTreeViewFilter( ActorNodeFindTextBox->Text );
			 g_pMainApp->SetNodeListTreeView();
		 }
private: System::Void TrailTexSIFileOpenButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 RTrail* pTrail = g_pMainApp->GetSelectedTrail();
			 if (pTrail == NULL)
				 return;

			 OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;
			 openFileDialog1->InitialDirectory = gcnew String( CToolConfig::GetInstance().m_strEffectTexturePath.c_str() );
			 openFileDialog1->Filter = "Image Files(bmp, tga, jpg, png, dds)|*.bmp;*.tga;*.jpg;*.png;*.dds|bmp Files (*.bmp)|*.bmp|tga files (*.tga)|*.tga|jpg Files (*.jpg)|*.jpg|png files (*.png)|*.png|dds files (*.dds)|*.dds|All files (*.*)|*.*";
			 openFileDialog1->FilterIndex = 0;
			 openFileDialog1->RestoreDirectory = false;

			 if ( openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			 {
				 String^ PathName = openFileDialog1->FileName;
				 g_pMainApp->SetTrailSITexture( PathName );
				 g_bModified = true;
			 }
		 }
private: System::Void TrailSIDeleteButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 String^ FileName = "";
			 g_pMainApp->SetTrailSITexture(FileName);
			 g_bModified = true;
		 }
private: System::Void TrailSpnSelfIlluminationScale_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (g_bIgnoreValueChangedMessage)
				 return;

			 RTrail* pTrail = g_pMainApp->GetSelectedTrail();
			 if( pTrail == NULL) 
				 return;

			 pTrail->SetSelfIlluminationScale((float)TrailSpnSelfIlluminationScale->Value);
			 g_bModified = true;
		 }
};
} // end of EffectTool_NET namespace 
