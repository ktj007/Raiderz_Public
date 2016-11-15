#include "StdAfx.h"
#include "EffectSelectForm.h"


System::Void Character_NET::EffectSelectForm::EffectSelectForm_Load( System::Object^ sender, System::EventArgs^ e )
{
	m_loadProgressBar.Show();

	vector<string> vecFileName;
	vector<int> vecBuffID;
	g_pMainApp->m_pEffectManager->GetEffectPureFileNameList(vecFileName);
	g_pMainApp->m_BuffInfo.GetEnchantBuffIDList(vecBuffID);

	m_loadProgressBar.Init(vecFileName.size() + vecBuffID.size());
	int i = 0;

	for(vector<string>::iterator it = vecFileName.begin(); it != vecFileName.end(); ++it)
	{
		effectSelect_comboBox->Items->Add(gcnew String(it->c_str()));
		effectSelect_comboBox1->Items->Add(gcnew String(it->c_str()));
		effectSelect_comboBox2->Items->Add(gcnew String(it->c_str()));
		effectSelect_comboBox3->Items->Add(gcnew String(it->c_str()));
		effectSelect_comboBox4->Items->Add(gcnew String(it->c_str()));
		effectSelect_comboBox5->Items->Add(gcnew String(it->c_str()));

		m_loadProgressBar.SetProGressBarData(i++);
	}


	for(vector<int>::iterator itbuff = vecBuffID.begin(); itbuff != vecBuffID.end(); ++itbuff)
	{
		EnchantBuffEffectID_comboBox->Items->Add(*itbuff);
		m_loadProgressBar.SetProGressBarData(i++);
	}

	EquipType_comboBox->Items->Add(gcnew String("none"));
	EquipType_comboBox->Items->Add(gcnew String("1hs"));
	EquipType_comboBox->Items->Add(gcnew String("1hb"));
	EquipType_comboBox->Items->Add(gcnew String("2hd"));
	EquipType_comboBox->Items->Add(gcnew String("sta"));
	EquipType_comboBox->Items->Add(gcnew String("arc"));
	EquipType_comboBox->Items->Add(gcnew String("2hb"));
	EquipType_comboBox->Items->Add(gcnew String("dwd"));
	EquipType_comboBox->Items->Add(gcnew String("dwp"));
	EquipType_comboBox->Items->Add(gcnew String("head"));
	EquipType_comboBox->Items->Add(gcnew String("body"));
	EquipType_comboBox->Items->Add(gcnew String("hands"));
	EquipType_comboBox->Items->Add(gcnew String("leg"));
	EquipType_comboBox->Items->Add(gcnew String("feet"));

	m_loadProgressBar.SetProGressBarValue(100);
}

String^ Character_NET::EffectSelectForm::GetSelectEffectName(int nIndex)
{
	if(nIndex == 0)
		return effectSelect_comboBox->Text;
	else if(nIndex == 1)
		return effectSelect_comboBox1->Text;
	else if(nIndex == 2)
		return effectSelect_comboBox2->Text;
	else if(nIndex == 3)
		return effectSelect_comboBox3->Text;
	else if(nIndex == 4)
		return effectSelect_comboBox4->Text;

	return effectSelect_comboBox5->Text;
}

int Character_NET::EffectSelectForm::GetSelectEnchantBuffEffectID()
{
	String^ strBuffID = EnchantBuffEffectID_comboBox->Text;

	return  atoi(MStringToCharPointer(strBuffID));
}

System::Void Character_NET::EffectSelectForm::EnchantBuffEffectID_comboBox_SelectedIndexChanged( System::Object^ sender, System::EventArgs^ e )
{
	XEnchantBuffEffectData* pEnchantBuffEffectData = g_pMainApp->m_BuffInfo.GetEnchantBuffEffectData(GetSelectEnchantBuffEffectID());
	if(pEnchantBuffEffectData == NULL)
		return;

	effectSelect_comboBox->Text = "";
	effectSelect_comboBox1->Text = "";
	effectSelect_comboBox2->Text = "";
	effectSelect_comboBox3->Text = "";
	effectSelect_comboBox4->Text = "";
	effectSelect_comboBox5->Text = "";
}

void Character_NET::EffectSelectForm::SetEffectName( String^ strEffectName, int nIndex )
{
	if(nIndex == 0)
		effectSelect_comboBox->Text = strEffectName;
	else if(nIndex == 1)
		effectSelect_comboBox1->Text = strEffectName;
	else if(nIndex == 2)
		effectSelect_comboBox2->Text = strEffectName;
	else if(nIndex == 3)
		effectSelect_comboBox3->Text = strEffectName;
	else if(nIndex == 4)
		effectSelect_comboBox4->Text = strEffectName;
	else
		effectSelect_comboBox5->Text = strEffectName;
}

void Character_NET::EffectSelectForm::EffectSelectComboBox()
{
	EnchantBuffEffectID_comboBox->Text = "";
}

void Character_NET::EffectSelectForm::SetEffectBuffID( int nBuffID )
{
	char chBuff[64] = {0,};
	sprintf_s(chBuff, "%d", nBuffID);
	EnchantBuffEffectID_comboBox->Text = gcnew String(chBuff);
}

String^ Character_NET::EffectSelectForm::GetSelectEnchantEquipType()
{
	return  EquipType_comboBox->Text;
}

void Character_NET::EffectSelectForm::SetEffectEquipType( String^ strEquipType )
{
	EquipType_comboBox->Text = strEquipType;
}
