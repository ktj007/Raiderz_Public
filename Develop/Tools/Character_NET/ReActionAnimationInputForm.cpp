#include "StdAfx.h"
#include "ReActionAnimationInputForm.h"


System::Void Character_NET::ReActionAnimationInputForm::ReActionAnimationInputForm_Load( System::Object^ sender, System::EventArgs^ e )
{
	Disable_checkBox->Enabled = false;
	label4->Enabled = false;

	if(m_bUsableWeight)
	{
		reAction_comboBox->Enabled = false;
		label1->Enabled = false;

		speed_textBox->Enabled = false;
		label3->Enabled = false;
	}
	else if(m_bUsableSpeed)
	{
		reAction_comboBox->Enabled = false;
		label1->Enabled = false;
		
		weight_textBox->Enabled = false;
		label2->Enabled = false;
	}
	else if(m_bUsableDisable)
	{
		reAction_comboBox->Enabled = false;
		label1->Enabled = false;

		weight_textBox->Enabled = false;
		label2->Enabled = false;

		speed_textBox->Enabled = false;
		label3->Enabled = false;

		Disable_checkBox->Enabled = true;
		label4->Enabled = true;
	}
}

void Character_NET::ReActionAnimationInputForm::SetReActionAnimationList( vector<string>& vecReActionAnimationList )
{
	for(vector<string>::iterator it = vecReActionAnimationList.begin(); it != vecReActionAnimationList.end(); ++it)
	{
		reAction_comboBox->Items->Add(gcnew String(it->c_str()));
	}
}

System::Void Character_NET::ReActionAnimationInputForm::weight_textBox_KeyPress( System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e )
{
	if(CheckInputForNumber(sender, e) == false)
		return;

	float fWeight = 0.0f;
	if(weight_textBox->Text != nullptr)
		fWeight	= atof(MStringToCharPointer(weight_textBox->Text));

	if(fWeight > 1.0f || fWeight < 0.0f)
	{
		System::Windows::Forms::MessageBox::Show(gcnew String("Weight 값은 0 ~ 1 사이어야 합니다."), "에러");
	}
}

System::Void Character_NET::ReActionAnimationInputForm::speed_textBox_KeyPress( System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e )
{
	CheckInputForNumber(sender, e);
}

bool Character_NET::ReActionAnimationInputForm::CheckInputForNumber( System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e )
{
	if( !(Char::IsDigit(e->KeyChar) || e->KeyChar == Convert::ToChar(Keys::Back) || e->KeyChar == Convert::ToChar(gcnew String("."))) )
	{
		e->Handled = true;

		return false;
	}

	return true;
}

bool Character_NET::ReActionAnimationInputForm::CheckInputForEnterOrEsc( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
{
	if(e->KeyData == Keys::Enter)
	{
		this->DialogResult = System::Windows::Forms::DialogResult::OK;
		return true;
	}
	else if(e->KeyData == Keys::Escape)
	{
		return true;
	}

	return false;
}

