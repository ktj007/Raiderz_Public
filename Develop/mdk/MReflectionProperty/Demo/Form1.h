#pragma once

#include "car.h"

namespace Demo {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace ReflectivePropertyFormNameSpace;

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
		Form1(void)
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
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  button1;
	protected: 

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
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(40, 32);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(204, 64);
			this->button1->TabIndex = 0;
			this->button1->Text = L"ReflectivePropertyGrid를 뜨게하는 버튼";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(292, 266);
			this->Controls->Add(this->button1);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) 
			 {//여기가 ReflectivePropertyGrid버튼 눌렀을때 호출되는 곳이다 ~

				ReflectivePropertyForm^ reflectivePropertyForm = gcnew ReflectivePropertyForm;	//폼생성
				reflectivePropertyForm->InitializeComponent();									//폼초기화시작
	
				static ReCar reflectiveCar;															//리플렉션 클래스 객체 생성		
				reflectivePropertyForm->setText("윈도우창위에 보이는 텍스트");	
				reflectivePropertyForm->setBaseObject((void*)&reflectiveCar);					//폼에 리플렉션 클래스연결
				
				reflectivePropertyForm->addAllReflection();										//연결된 리플렉션클래스 동기화

				reflectivePropertyForm->Show();													//폼보이기
				reflectivePropertyForm->CompleteToInitializeComponent();						//폼초기화 끝
			 }
	};
}

