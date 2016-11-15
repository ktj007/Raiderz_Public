#pragma once

#include "RClothActorNode.h"

using namespace ReflectivePropertyFormNameSpace;

namespace Character_NET 
{
	public ref class ClothReflectivePropertyForm : public ReflectivePropertyForm
	{
	public:
		ClothReflectivePropertyForm(const char* szFormName,RClothInfo* pClothInfo,CBaseObject *pBaseObject);
	public:
		static ClothReflectivePropertyForm^ create(const char* szFormName,RClothInfo* pClothInfo,CBaseObject *pBaseObject,const char* szSaveFileName);

		void setClothInfo(RClothInfo* pClothInfo);
		void InitializeComponent(void);
	private:
		Void Save_Parameters_Click(System::Object^  sender, System::EventArgs^  e);
		Void Load_Parameters_Click(System::Object^  sender, System::EventArgs^  e);
		
	private:
		System::Windows::Forms::Button^				saveButton;
		//System::Windows::Forms::Button^				loadButton;

		RClothInfo* m_pClothInfo;	//save,load기능때문에 필요
		System::String^	m_saveFileName;
	};
}//namespace Character_NET 