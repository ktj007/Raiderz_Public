#include "stdafx.h"
#include "ClothReflectivePropertyForm.h"

namespace Character_NET
{
	ClothReflectivePropertyForm^ ClothReflectivePropertyForm::create(const char* szFormName,RClothInfo* pClothInfo,CBaseObject *pBaseObject ,const char* szSaveFileName)
	{
		ClothReflectivePropertyForm^ clothReflectiveProperty = gcnew ClothReflectivePropertyForm(szFormName,pClothInfo,pBaseObject);
		
		clothReflectiveProperty->Show();

		clothReflectiveProperty->addAllReflection();

		clothReflectiveProperty->CompleteToInitializeComponent();

		clothReflectiveProperty->m_saveFileName = gcnew System::String(szSaveFileName);

		return clothReflectiveProperty;
	}

	ClothReflectivePropertyForm::ClothReflectivePropertyForm(const char* szFormName,RClothInfo* pClothInfo ,CBaseObject *pBaseObject)
	{
		m_pClothInfo = NULL;

		InitializeComponent();

		setBaseObject((void*)pBaseObject);
		setText(szFormName);
		setClothInfo(pClothInfo);
	}

	void ClothReflectivePropertyForm::InitializeComponent( void )
	{
		ReflectivePropertyForm::InitializeComponent();

		this->saveButton	= gcnew System::Windows::Forms::Button();
		//this->loadButton	= gcnew System::Windows::Forms::Button();

		//save button
		this->saveButton->Location	= System::Drawing::Point(50, 450);
		this->saveButton->Text		= L"Save Parameters";
		this->saveButton->Click	   += gcnew System::EventHandler(this,&ClothReflectivePropertyForm::Save_Parameters_Click);

		//open button
		//this->loadButton->Location	= System::Drawing::Point(200, 450);
		//this->loadButton->Text		= L"Load Parameters";
		//this->loadButton->Click	   += gcnew System::EventHandler(this,&ClothReflectivePropertyForm::Load_Parameters_Click);

		this->Controls->Add(this->saveButton);
		//this->Controls->Add(this->loadButton);
	}

	void ClothReflectivePropertyForm::setClothInfo(RClothInfo* pClothInfo)
	{
		_ASSERT( NULL != pClothInfo	);
		m_pClothInfo = pClothInfo;
	}

	System::Void ClothReflectivePropertyForm::Save_Parameters_Click( System::Object^ sender, System::EventArgs^ e )
	{
		_ASSERT( NULL != m_pClothInfo);
		const char* szFileName = (const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(m_saveFileName).ToPointer();

		if( false == m_pClothInfo->save(szFileName) )	
			System::Windows::Forms::MessageBox::Show("파일 저장에 실패했습니다.","경고");
		else
		{
			// 모델 리로드...
			g_pMainApp->ReloadCharacter();
		}
	}

	System::Void ClothReflectivePropertyForm::Load_Parameters_Click( System::Object^ sender, System::EventArgs^ e )
	{
		_ASSERT( NULL != m_pClothInfo);

		System::Windows::Forms::OpenFileDialog^ openFileDialog= gcnew System::Windows::Forms::OpenFileDialog();
		openFileDialog->Title	= L"Cloth Parameter Load Dialog";
		openFileDialog->Filter	= "cloth files|*.clt";
		openFileDialog->FilterIndex = 1;
		openFileDialog->RestoreDirectory = true;
		if( openFileDialog->ShowDialog() != System::Windows::Forms::DialogResult::OK ) return;

		System::String^ fileName = openFileDialog->FileName;
		const char* szFileName = (const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(fileName).ToPointer();

		if( false == m_pClothInfo->load(szFileName) )	System::Windows::Forms::MessageBox::Show("파일 로드에 실패했습니다.","경고");
			
		//propertyGrid1->Refresh();
	}
}//namespace Character_NET