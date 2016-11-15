#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace EffectTool_NET {

	/// <summary>
	/// PropertyForm에 대한 요약입니다.
	///
	/// 경고: 이 클래스의 이름을 변경하면 이 클래스가 의존하는
	///          모든 .resx 파일과 관련된 관리되는 리소스 컴파일러 도구의
	///          '리소스 파일 이름' 속성도 변경해야 합니다. 그렇지 않은 경우
	///          디자이너는 이 폼과 관련된 지역화된 리소스와
	///          올바르게 상호 작용할 수 없습니다.
	/// </summary>
	public ref class PropertyForm : public System::Windows::Forms::Form
	{
	public:
		PropertyForm(void)
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
		~PropertyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::PropertyGrid^  sceneNodePropertyGrid;
	protected: 

	protected: 

	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		void InitializeComponent(void)
		{
			this->sceneNodePropertyGrid = (gcnew System::Windows::Forms::PropertyGrid());
			this->SuspendLayout();
			// 
			// sceneNodePropertyGrid
			// 
			this->sceneNodePropertyGrid->Dock = System::Windows::Forms::DockStyle::Fill;
			this->sceneNodePropertyGrid->Location = System::Drawing::Point(0, 0);
			this->sceneNodePropertyGrid->Name = L"sceneNodePropertyGrid";
			this->sceneNodePropertyGrid->Size = System::Drawing::Size(292, 273);
			this->sceneNodePropertyGrid->TabIndex = 0;
			// 
			// PropertyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(292, 273);
			this->Controls->Add(this->sceneNodePropertyGrid);
			this->Name = L"PropertyForm";
			this->Text = L"PropertyForm";
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}
