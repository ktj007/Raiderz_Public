#include "StdAfx.h"
#include "SearchCustomEffectInfoName.h"

vector<string> vecEffectList;

System::Void Character_NET::SearchCustomEffectInfoName::SearchCustomEffectInfoName_Load( System::Object^ sender, System::EventArgs^ e )
{
	SearchEffectList();
}

void Character_NET::SearchCustomEffectInfoName::Init()
{
	vecEffectList.clear();
	g_pMainApp->m_TabAnimation.GetCustomEffectList(vecEffectList);
}

void Character_NET::SearchCustomEffectInfoName::SearchEffectList( string& strSearchText, vector<string>& vecResult )
{
	// °Ë»ö...
	vector<string> vecSearchList;
	TStrings::ParseFindString(gcnew String(strSearchText.c_str()), vecSearchList);
	
	for(vector<string>::iterator it = vecEffectList.begin(); it != vecEffectList.end(); ++it)
	{
		for(vector<string>::iterator itSearchText = vecSearchList.begin(); itSearchText != vecSearchList.end(); ++itSearchText)
		{
			if(CompareEffectName(*it, *itSearchText))
			{
				vecResult.push_back(*it);
			}
		}
	}
}

void Character_NET::SearchCustomEffectInfoName::SearchEffectList()
{
	vector<string> vecResult;
	string strSource = MStringToCharPointer(search_textBox->Text);
	SearchEffectList(strSource, vecResult);

	search_listBox->Items->Clear();
	for(vector<string>::iterator it = vecResult.begin(); it != vecResult.end(); ++it)
	{
		search_listBox->Items->Add(gcnew String(it->c_str()));
	}
}

bool Character_NET::SearchCustomEffectInfoName::CompareEffectName( string& strSource, string& strTarget )
{
	String^ tnstr1 = gcnew String(strSource.c_str());
	String^ tnstr2 = gcnew String(strTarget.c_str());

	if( tnstr1->StartsWith(tnstr2) || tnstr1->EndsWith(tnstr2) || IsEqualString(tnstr1, tnstr2))
		return true;

	return false;
}

System::Void Character_NET::SearchCustomEffectInfoName::search_listBox_DoubleClick( System::Object^ sender, System::EventArgs^ e )
{
	m_strSelectCustoEffectInfoName = search_listBox->SelectedItem->ToString();
	this->Close();
}
