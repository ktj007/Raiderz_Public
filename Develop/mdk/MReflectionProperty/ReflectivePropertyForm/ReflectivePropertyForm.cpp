#include "ReflectivePropertyForm.h"

namespace ReflectivePropertyFormNameSpace
{
	ReflectivePropertyGrid::ReflectivePropertyGrid()
	{
		m_pBaseObject = NULL;

		m_accessors = gcnew Hashtable();
		m_settings = gcnew CustomPropGrid::Settings();
		m_eventHandler = gcnew System::EventHandler( this, &ReflectivePropertyGrid::updateReflect );

		this->Name = L"ReflectivePropertyGrid";
	}

	ReflectivePropertyGrid::~ReflectivePropertyGrid()
	{
	}

	void ReflectivePropertyGrid::InitializeComponent()
	{
		this->SuspendLayout();

		this->Location = System::Drawing::Point(200, 10);
		this->Size = System::Drawing::Size(300, 400);
	}

	void ReflectivePropertyGrid::CompleteToInitializeComponent()
	{
		Settings = m_settings;
		this->ResumeLayout(false);
	}

	void ReflectivePropertyGrid::addAllReflection()
	{
		_ASSERT( NULL != getBaseObject() );
		addAllReflection(getBaseObject());
	}

	void ReflectivePropertyGrid::addAllReflection( CBaseObject* pBaseObject )
	{
		vector<CAbstractProperty*> Properties;
		pBaseObject->GetRTTI()->EnumProperties( Properties );

		for ( unsigned int i = 0; i < Properties.size(); i++ )
		{
			switch ( Properties[i]->GetType() )
			{
			case eptFLOAT:
				{
					CTypedProperty<float>* pTypedProperty = (CTypedProperty<float>*)Properties[i];
					addReflection(pTypedProperty,pBaseObject);
				}break;

			case eptINT:
				{
					CTypedProperty<int>* pTypedProperty = (CTypedProperty<int>*)Properties[i];
					addReflection(pTypedProperty,pBaseObject);
				}break;

			case eptBOOL:
				{
					CTypedProperty<bool>* pTypedProperty = (CTypedProperty<bool>*)Properties[i];
					addReflection(pTypedProperty,pBaseObject);
				}break;

			case eptSTRING:
				{
					CTypedProperty<char*>* pTypedProperty = (CTypedProperty<char*>*)Properties[i];
					addReflection(pTypedProperty,pBaseObject);
				}break;
			case eptCOLOR:
				{
					CTypedProperty<DWORD>* pTypedProperty = (CTypedProperty<DWORD>*)Properties[i];
					addReflection(pTypedProperty,pBaseObject);
				}
				break;
			}
		}
	}

	System::Void ReflectivePropertyGrid::updateReflect( System::Object^ sender, System::EventArgs^ evt )
	{
		PropertyValueChangedEventArgs^ e = (PropertyValueChangedEventArgs^)evt;

		Accesser^ accesser = (Accesser^)m_accessors[e->ChangedItem->Label];
		CBaseObject* pObject = (CBaseObject*) accesser->m_pBaseObject;
		vector<CAbstractProperty*> Properties;
		pObject->GetRTTI()->EnumProperties( Properties );

		for ( unsigned int i = 0; i < Properties.size(); i++ )
		{
			System::String^ name = gcnew System::String(Properties[i]->GetName());
			if ( name == e->ChangedItem->Label)
			{
				switch ( Properties[i]->GetType() )
				{
				case eptFLOAT :
					{
						CTypedProperty<float>* pTypedProperty = (CTypedProperty<float>*)Properties[i];
						System::Single^ single = (System::Single^)e->ChangedItem->Value;
						float f = Convert::ToSingle(single);
						pTypedProperty->SetValue( pObject, f);
					}break;

				case eptINT:
					{
						CTypedProperty<int>* pTypedProperty = (CTypedProperty<int>*)Properties[i];
						System::Int32^ int32 = (System::Int32^)e->ChangedItem->Value;
						int i = Convert::ToInt32(int32);
						pTypedProperty->SetValue( pObject, i);
					}break;

				case eptBOOL:
					{
						CTypedProperty<bool>* pTypedProperty = (CTypedProperty<bool>*)Properties[i];
						System::Boolean^ boolean = (System::Boolean^)e->ChangedItem->Value;
						bool b = Convert::ToBoolean(boolean);
						pTypedProperty->SetValue( pObject, b);
					}break;

				case eptSTRING:
					{
						CTypedProperty<char*>* pTypedProperty = (CTypedProperty<char*>*)Properties[i];
						System::String^ str = (System::String^)e->ChangedItem->Value;
						char* szStr = (char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(str).ToPointer();
						pTypedProperty->SetValue( pObject, szStr);						
					}break;
				case eptCOLOR:
					{
						CTypedProperty<DWORD>* pTypedProperty = (CTypedProperty<DWORD>*)Properties[i];
						System::Drawing::Color color = (System::Drawing::Color)e->ChangedItem->Value;

						DWORD dwColor = System::Drawing::ColorTranslator::ToWin32(color);
						pTypedProperty->SetValue( pObject, dwColor);						
					}break;
				}
			}
		}
	}//updateReflect

	///////////////////////////////////////////////////////////////////////////

	ReflectivePropertyForm::ReflectivePropertyForm()
	{
		m_szText = NULL;
	}

	ReflectivePropertyForm::~ReflectivePropertyForm()
	{
	}

	void ReflectivePropertyForm::InitializeComponent()
	{
		this->propertyGrid1 = gcnew ReflectivePropertyGrid;
		this->propertyGrid1->InitializeComponent();

		this->SuspendLayout();

		// propertyGrid1
		this->propertyGrid1->Location = System::Drawing::Point(10, 10);
		this->propertyGrid1->Name = L"propertyGrid1";
		this->propertyGrid1->Size = System::Drawing::Size(300, 400);
		this->propertyGrid1->TabIndex = 0;

		// ReflectivePropertyForm
		this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->ClientSize = System::Drawing::Size(350, 500);
		this->Controls->Add(this->propertyGrid1);
		this->TopMost = true;
	}

	void ReflectivePropertyForm::CompleteToInitializeComponent()
	{
		this->propertyGrid1->CompleteToInitializeComponent();
		this->ResumeLayout(false);
	}

	void ReflectivePropertyForm::setText( const char* szText )
	{
		_ASSERT( NULL != szText	);
		this->Text = gcnew System::String(szText);
	}

	void ReflectivePropertyForm::addAllReflection(void)
	{
		_ASSERT( NULL != getBaseObject() );
		propertyGrid1->addAllReflection( getBaseObject() );
	}
}


