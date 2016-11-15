#include "StdAfx.h"
#include "ReActionAnimationTable.h"
#include "ReActionAnimationInputForm.h"
#include "TClipboard.h"

enum COLUMN_INDEX
{
	CI_SOURCE_ANI,
	CI_REACTION_ANI,
	CI_WEIGHT,
	CI_SPEED,
	CI_WEAPON_BONENAME,
	CI_DISABLE,

	CI_MAX
};

System::Void Character_NET::ReActionAnimationTable::ReActionAnimationTable_Load( System::Object^ sender, System::EventArgs^ e )
{
	Load_AnimationList();


	// 이미 선택된 애니메이션이 있다면...
	if(m_strSelectAniName != nullptr)
	{
		int nIndex = FindAnimationItemIndexAnimation(m_strSelectAniName);
		if(nIndex != -1)
		{
			ReActionAniamtion_dataGridView->CurrentCell = ReActionAniamtion_dataGridView->Rows[nIndex]->Cells[CI_SOURCE_ANI];
			ReActionAniamtion_dataGridView->FirstDisplayedCell = ReActionAniamtion_dataGridView->CurrentCell;
		}
	}
}

void Character_NET::ReActionAnimationTable::Load_AnimationList()
{
	ReActionAniamtion_dataGridView->Rows->Clear();

	if( g_pMainApp->GetActor() == NULL || g_pMainApp->GetActor()->m_pMesh == NULL)
		return;

	RMeshAnimationMgr* pAMgr = &g_pMainApp->GetActor()->m_pMesh->m_AniMgr;

	vector<string> strErrorMessage;

	for(RAnimationHashList_Iter it = pAMgr->m_NodeHashList.begin(); it != pAMgr->m_NodeHashList.end();it++)
	{
		RAnimation* pAnimation = *it;
		if(pAnimation)
		{
			string strAniName = pAnimation->GetAliasName();

			//파일이 존재하는냐?
			RAnimation* pAnimation = g_pMainApp->m_TabAnimation.GetAnimation(strAniName.c_str());
			if( pAnimation == NULL
				|| pAnimation->GetID() < 0
				|| pAnimation->GetResource() == NULL
				|| !ExistFile(pAnimation->GetResource()->GetName().c_str()))
			{
				continue;
			}

			String^ Name = gcnew String(strAniName.c_str() );

			array<String^>^arrayData = gcnew array<String^>(CI_MAX);
			GetReActionDataToArray(Name, arrayData);

			ReActionAniamtion_dataGridView->Rows->Add(arrayData);

			// 애니메이션 콤보박스 아이템 추가
			Column2->Items->Add(Name);
		}
	}
}

System::Void Character_NET::ReActionAnimationTable::Finish_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if(m_bSave)
	{
		for(int i = 0; i < ReActionAniamtion_dataGridView->Rows->Count; ++i)
		{
			ReAction_DATA tempData;
			GetSelectRowData(i, tempData);

			String^ strAni = GetSelectCellData(i, CI_SOURCE_ANI);
			ReAction_DATA * pReActionData = g_pMainApp->GetActor()->GetAnimationInfoMgr()->m_toolReActionAnimationInfo.Get(MStringToCharPointer(strAni));

			if(pReActionData)
			{
				bool bSet = CheckReActionDataChange(*pReActionData, tempData);
				if(bSet)
				{
					if(tempData.strReActionName.empty())
					{
						// 삭제
						g_pMainApp->GetActor()->GetAnimationInfoMgr()->m_toolReActionAnimationInfo.DelReActionAnimationInfo(MStringToCharPointer(strAni));
					}
					else 
						pReActionData->Set(tempData);
				}
			}
			else
			{
				ReAction_DATA checkData;

				bool bNew = CheckReActionDataChange(checkData, tempData);

				if(bNew)
				{
					string strSourceAni = MStringToCharPointer(strAni);
					g_pMainApp->GetActor()->GetAnimationInfoMgr()->m_toolReActionAnimationInfo.Set(strSourceAni, tempData);
				}
			}
		}


		g_pMainApp->GetActor()->GetAnimationInfoMgr()->SaveXml();
	}

	this->DialogResult = System::Windows::Forms::DialogResult::OK;
	this->Close();
}

System::Void Character_NET::ReActionAnimationTable::ReActionAniamtion_dataGridView_CellValueChanged( System::Object^ sender, System::Windows::Forms::DataGridViewCellEventArgs^ e )
{
	if(e->RowIndex == -1 ||
		e->ColumnIndex == -1)
		return;

	String^ strAni = GetSelectCellData(e->RowIndex, CI_SOURCE_ANI);
	if(strAni == nullptr)
		return;

	ReAction_DATA tempData;
	GetSelectRowData(e->RowIndex, tempData);
	CheckEffectiveReActionData(e->RowIndex, tempData);

	ReAction_DATA * pReActionData = g_pMainApp->GetActor()->GetAnimationInfoMgr()->m_toolReActionAnimationInfo.Get(MStringToCharPointer(strAni));

	if(pReActionData)
	{
		m_bSave |= CheckReActionDataChange(*pReActionData, tempData);

		//pReActionData->Set(tempData);
	}
	else
	{
		ReAction_DATA checkData;

		m_bSave |= CheckReActionDataChange(checkData, tempData);

		//if(m_bSave)
		//{
		//	string strSourceAni = MStringToCharPointer(strAni);
		//	g_pMainApp->GetActor()->GetAnimationInfoMgr()->m_toolReActionAnimationInfo.Set(strSourceAni, tempData);
		//}
	}
}

int Character_NET::ReActionAnimationTable::GetSelectSourceAniList( vector<string>& vecSelectAni )
{
	int nCheckIndex = -1;
	int nResultColumnIndex = -1;

	DataGridViewSelectedCellCollection^ selectCells = ReActionAniamtion_dataGridView->SelectedCells;
	
	for(int i = 0; i < selectCells->Count; ++i)
	{
		int nRow = selectCells[i]->RowIndex;

		String^ strAni = GetSelectCellData(nRow, CI_SOURCE_ANI);
		vecSelectAni.push_back(MStringToCharPointer(strAni));

		int nColumn = selectCells[i]->ColumnIndex;
		if(nCheckIndex == -1)
			nCheckIndex = nColumn;
		else if(nCheckIndex == nColumn)
			nResultColumnIndex = nCheckIndex;
		else
			nResultColumnIndex = -1;
	}

	return nCheckIndex;
}

String^ Character_NET::ReActionAnimationTable::GetSelectCellData( int nRow, int nColumn )
{
	if(nRow == -1 ||
		nColumn == -1)
		return nullptr;

	if(ReActionAniamtion_dataGridView->Rows->Count <= nRow || ReActionAniamtion_dataGridView->Rows[nRow]->Cells->Count <= nColumn ||
		ReActionAniamtion_dataGridView->Rows[nRow]->Cells[nColumn]->Value == nullptr)
		return nullptr;

	return ReActionAniamtion_dataGridView->Rows[nRow]->Cells[nColumn]->Value->ToString();
}

void Character_NET::ReActionAnimationTable::GetSelectRowData( int nRow, ReAction_DATA& reAction_Data )
{
	String^ strDisable = GetSelectCellData(nRow, CI_DISABLE);

	if(strDisable == gcnew String("True"))
	{
		reAction_Data.init();
		reAction_Data.strReActionName.clear();
		reAction_Data.strWeaponBoneName.clear();
		reAction_Data.bDisable			= true;
		return;
	}

	String^ strAni = GetSelectCellData(nRow, CI_REACTION_ANI);
	//if(strAni == nullptr)
	//	return;

	String^ strWeight = GetSelectCellData(nRow, CI_WEIGHT);
	String^ strSpeed = GetSelectCellData(nRow, CI_SPEED);
	String^ strWeaponBoneName = GetSelectCellData(nRow, CI_WEAPON_BONENAME);

	if(strAni != nullptr)
		reAction_Data.strReActionName	= MStringToCharPointer(strAni);

	if(strWeight != nullptr)
		reAction_Data.fWeight			= atof(MStringToCharPointer(strWeight));
	if(strSpeed != nullptr)
		reAction_Data.fSpeed			= atof(MStringToCharPointer(strSpeed));
	if(strWeaponBoneName != nullptr)
		reAction_Data.strWeaponBoneName	= MStringToCharPointer(strWeaponBoneName);
}

bool Character_NET::ReActionAnimationTable::CheckReActionDataChange( ReAction_DATA& sourceData, ReAction_DATA& targetData )
{
	bool bChange = false;
	
	bChange |= (sourceData.strReActionName != targetData.strReActionName);
	bChange |= (sourceData.fWeight != targetData.fWeight);
	bChange |= (sourceData.fSpeed != targetData.fSpeed);
	bChange |= (sourceData.bDisable != targetData.bDisable);
	bChange |= (sourceData.strWeaponBoneName != targetData.strWeaponBoneName);

	return bChange;
}

void Character_NET::ReActionAnimationTable::GetReActionDataToArray( String^ strAni, array<String^>^ arrayData )
{
	arrayData[CI_SOURCE_ANI] = strAni;

	ReAction_DATA * pReActionData = g_pMainApp->GetActor()->GetAnimationInfoMgr()->m_toolReActionAnimationInfo.Get(MStringToCharPointer(strAni));
	if(pReActionData)
	{
		if(pReActionData->bDisable)
		{
			arrayData[CI_REACTION_ANI]	= nullptr;
			arrayData[CI_WEIGHT]		= nullptr;
			arrayData[CI_SPEED]			= nullptr;
			arrayData[CI_WEAPON_BONENAME] = nullptr;
			arrayData[CI_DISABLE]		= gcnew String("True");

			return;
		}

		char chTemp[256] = {0,};
		sprintf(chTemp, "%s", pReActionData->strReActionName.c_str());
		arrayData[CI_REACTION_ANI]	= gcnew String(chTemp);

		sprintf(chTemp, "%f", pReActionData->fWeight);
		arrayData[CI_WEIGHT]		= gcnew String(chTemp);

		sprintf(chTemp, "%f", pReActionData->fSpeed);
		arrayData[CI_SPEED]			= gcnew String(chTemp);

		sprintf(chTemp, "%s", pReActionData->strWeaponBoneName.c_str());
		arrayData[CI_WEAPON_BONENAME]	= gcnew String(chTemp);

		arrayData[CI_DISABLE]		= gcnew String("False");
	}
}

void Character_NET::ReActionAnimationTable::CheckEffectiveReActionData( int nRow, ReAction_DATA& checkData )
{
	if(checkData.fWeight > 1.0f ||
		checkData.fWeight < 0.0f)
	{
		System::Windows::Forms::MessageBox::Show(gcnew String("Weight 값은 0 ~ 1 사이어야 합니다."), "에러");
	}

	if(checkData.bDisable)
	{
		SetSelectCellData(nRow, CI_REACTION_ANI, nullptr);
		SetSelectCellData(nRow, CI_WEIGHT, nullptr);
		SetSelectCellData(nRow, CI_SPEED, nullptr);
		SetSelectCellData(nRow, CI_WEAPON_BONENAME, nullptr);
	}
}

System::Void Character_NET::ReActionAnimationTable::modify_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	vector<string> vecSelAni;
	int nSmartColumnIndex = GetSelectSourceAniList(vecSelAni);

	DataGridViewSelectedCellCollection^ selectCells = ReActionAniamtion_dataGridView->SelectedCells;

	vector<string> vecAni;
	GetSourceAniList(vecAni);

	//////////////////////////////////////////////////////////////////////////
	ReActionAnimationInputForm reActionAnimation_input;
	
	bool bOnlyOne = false;
	ReAction_DATA only_one;
	if(vecSelAni.size() == 1)
	{
		GetSelectRowData(selectCells[0]->RowIndex, only_one);
		bOnlyOne = true;
	}

	//////////////////////////////////////////////////////////////////////////
	if(nSmartColumnIndex == CI_SOURCE_ANI)
	{
		// 전체
		reActionAnimation_input.SetReActionAnimationList(vecAni);
	}
	else if(nSmartColumnIndex == CI_WEIGHT)
	{
		// Weight만...
		reActionAnimation_input.m_bUsableWeight = true;
	}
	else if(nSmartColumnIndex == CI_SPEED)
	{
		// Speed만
		reActionAnimation_input.m_bUsableSpeed = true;
	}
	else if(nSmartColumnIndex == CI_DISABLE)
	{
		reActionAnimation_input.m_bUsableDisable = true;
	}

	// 한개뿐이면...
	if(bOnlyOne)
	{
		char chTemp[256] = {0,};


		if(nSmartColumnIndex == CI_SOURCE_ANI)
			reActionAnimation_input.reAction_comboBox->Text = gcnew String(only_one.strReActionName.c_str());
		if(nSmartColumnIndex == CI_SOURCE_ANI || nSmartColumnIndex == CI_WEIGHT)
		{
			sprintf(chTemp, "%f", only_one.fWeight);
			reActionAnimation_input.weight_textBox->Text = gcnew String(chTemp);
		}
		if(nSmartColumnIndex == CI_SOURCE_ANI || nSmartColumnIndex == CI_SPEED)
		{
			sprintf(chTemp, "%f", only_one.fSpeed);
			reActionAnimation_input.speed_textBox->Text = gcnew String(chTemp);
		}
		if(nSmartColumnIndex == CI_SOURCE_ANI || nSmartColumnIndex == CI_WEAPON_BONENAME)
		{
			reActionAnimation_input.weaponBoneName_comboBox->Text = gcnew String(only_one.strWeaponBoneName.c_str());
		}
		if(nSmartColumnIndex == CI_DISABLE)
		{
			if(only_one.bDisable)
			{
				reActionAnimation_input.Disable_checkBox->Checked = true;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	reActionAnimation_input.ShowDialog();
	System::Windows::Forms::DialogResult res = reActionAnimation_input.DialogResult;
	if (res == System::Windows::Forms::DialogResult::OK)
	{
		for(int i = 0; i < selectCells->Count; ++i)
		{
			int nRow = selectCells[i]->RowIndex;

			if(nSmartColumnIndex == CI_SOURCE_ANI)
				SetSelectCellData(nRow, CI_REACTION_ANI, reActionAnimation_input.reAction_comboBox->Text);
			if(nSmartColumnIndex == CI_SOURCE_ANI || nSmartColumnIndex == CI_WEIGHT)
				SetSelectCellData(nRow, CI_WEIGHT, reActionAnimation_input.weight_textBox->Text);
			if(nSmartColumnIndex == CI_SOURCE_ANI || nSmartColumnIndex == CI_SPEED)
				SetSelectCellData(nRow, CI_SPEED, reActionAnimation_input.speed_textBox->Text);
			if(nSmartColumnIndex == CI_SOURCE_ANI || nSmartColumnIndex == CI_WEAPON_BONENAME)
				SetSelectCellData(nRow, CI_WEAPON_BONENAME, reActionAnimation_input.weaponBoneName_comboBox->Text);
			if(nSmartColumnIndex == CI_DISABLE)
			{
				if(reActionAnimation_input.Disable_checkBox->Checked)
					SetSelectCellData(nRow, CI_DISABLE, gcnew String("True"));
				else
					SetSelectCellData(nRow, CI_DISABLE, gcnew String("False"));
			}
		}
	}
}

void Character_NET::ReActionAnimationTable::GetSourceAniList( vector<string>& vecAni )
{
	for(int i = 0; i < ReActionAniamtion_dataGridView->Rows->Count; ++i)
	{
		String^ strAni = GetSelectCellData(i, CI_SOURCE_ANI);

		if(strAni != nullptr)
			vecAni.push_back(MStringToCharPointer(strAni));
	}
}

void Character_NET::ReActionAnimationTable::SetSelectCellData( int nRow, int nColumn, String^ strData )
{
	if(nRow == -1 ||
		nColumn == -1)
		return;

	if(ReActionAniamtion_dataGridView->Rows->Count <= nRow || ReActionAniamtion_dataGridView->Rows[nRow]->Cells->Count <= nColumn)
		return;

	ReActionAniamtion_dataGridView->Rows[nRow]->Cells[nColumn]->Value = strData;
}

void Character_NET::ReActionAnimationTable::InitSetAnimationInfo( String^ strAni )
{
	m_strSelectAniName = strAni;
}

int Character_NET::ReActionAnimationTable::FindAnimationItemIndexAnimation( String^ strAnimation )
{
	for(int i = 0; i < ReActionAniamtion_dataGridView->Rows->Count; ++i)
	{
		if(GetSelectCellData(i, CI_SOURCE_ANI) == strAnimation)
		{
			return i;
		}
	}

	return -1;
}

System::Void Character_NET::ReActionAnimationTable::Del_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	DelReActionData();
}

void Character_NET::ReActionAnimationTable::DelReActionData()
{
	vector<string> vecSelAni;
	int nSmartColumnIndex = GetSelectSourceAniList(vecSelAni);

	DataGridViewSelectedCellCollection^ selectCells = ReActionAniamtion_dataGridView->SelectedCells;

	ReAction_DATA default_Data;
	char chDefaultWeight[256] = {0,};
	char chDefaultSpeed[256] = {0,};
	sprintf(chDefaultWeight, "%f", default_Data.fWeight);
	sprintf(chDefaultSpeed, "%f", default_Data.fSpeed);
	//////////////////////////////////////////////////////////////////////////
	for(int i = 0; i < selectCells->Count; ++i)
	{
		int nRow = selectCells[i]->RowIndex;

		if(nSmartColumnIndex == CI_SOURCE_ANI)
		{
			// 전체
			SetSelectCellData(nRow, CI_REACTION_ANI, nullptr);
			SetSelectCellData(nRow, CI_WEIGHT, nullptr);
			SetSelectCellData(nRow, CI_SPEED, nullptr);
		}
		else if(nSmartColumnIndex == CI_REACTION_ANI)
		{
			SetSelectCellData(nRow, CI_REACTION_ANI, nullptr);
		}
		else if(nSmartColumnIndex == CI_WEIGHT)
		{
			// Weight만...
			SetSelectCellData(nRow, CI_WEIGHT, nullptr);

			if(GetSelectCellData(nRow, CI_REACTION_ANI) != nullptr)
				SetSelectCellData(nRow, CI_WEIGHT, gcnew String(chDefaultWeight));
		}
		else if(nSmartColumnIndex == CI_SPEED)
		{
			// Speed만
			SetSelectCellData(nRow, CI_SPEED, nullptr);

			if(GetSelectCellData(nRow, CI_REACTION_ANI) != nullptr)
				SetSelectCellData(nRow, CI_SPEED, gcnew String(chDefaultSpeed));
		}
		else if(nSmartColumnIndex == CI_WEAPON_BONENAME)
		{
			SetSelectCellData(nRow, CI_WEAPON_BONENAME, nullptr);
		}
		else if(nSmartColumnIndex == CI_DISABLE)
		{
			SetSelectCellData(nRow, CI_DISABLE, gcnew String("False"));
		}
	}
}

System::Void Character_NET::ReActionAnimationTable::ReActionAniamtion_dataGridView_KeyDown( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
{
	if(e->KeyData == Keys::Delete)
	{
		DelReActionData();
	}

	if(e->Control)
	{
		if(e->KeyCode == Keys::V)
		{
			// 복사
			PasteReActionData();
		}
	}
}

System::Void Character_NET::ReActionAnimationTable::ReActionAniamtion_dataGridView_EditingControlShowing( System::Object^ sender, System::Windows::Forms::DataGridViewEditingControlShowingEventArgs^ e )
{
	if(ReActionAniamtion_dataGridView->CurrentCell->ColumnIndex == CI_REACTION_ANI)
	{
		ComboBox^ cb = static_cast<ComboBox^>(e->Control);
		if( cb != nullptr)
		{
			cb->DropDownStyle = ComboBoxStyle::DropDown;
			
		}
	}
}

System::Void Character_NET::ReActionAnimationTable::ReActionAniamtion_dataGridView_CellValidating( System::Object^ sender, System::Windows::Forms::DataGridViewCellValidatingEventArgs^ e )
{
	if(ReActionAniamtion_dataGridView->CurrentCell->ColumnIndex == CI_REACTION_ANI)
	{
		if(String::IsNullOrEmpty(e->FormattedValue->ToString()) == false &&
			Column2->Items->Contains(e->FormattedValue) == false)
		{
			System::Windows::Forms::MessageBox::Show("애니메이션 리스트에 존재하지 않습니다.", "에러",MessageBoxButtons::OK);
		}
		else
		{
			ReActionAniamtion_dataGridView->CurrentCell->Value = e->FormattedValue;
		}
	}
}

void Character_NET::ReActionAnimationTable::CopyReActionData()
{
	DataGridViewSelectedCellCollection^ selectCells = ReActionAniamtion_dataGridView->SelectedCells;
	if(selectCells->Count < CI_MAX)
		return;

	ReAction_DATA reactionData;
	GetSelectRowData(ReActionAniamtion_dataGridView->CurrentRow->Index, reactionData);

	Character_NET::TClipboardReActionRowData^ clipboardReActionRowData = gcnew Character_NET::TClipboardReActionRowData();
	clipboardReActionRowData->m_pReactionData = new ReAction_DATA();
	clipboardReActionRowData->m_pReactionData->Set(reactionData);

	TClipboardHelper::CopyRowData(clipboardReActionRowData);
}

void Character_NET::ReActionAnimationTable::PasteReActionData()
{
	TClipboardReActionRowData^ clipboardReActionRowData = TClipboardHelper::PasteRowData();

	if (clipboardReActionRowData == nullptr ||
		clipboardReActionRowData->m_pReactionData == NULL) 
		return;

	int nRow = ReActionAniamtion_dataGridView->CurrentRow->Index;

	// 전체
	char chTemp[256] = {0,};
	SetSelectCellData(nRow, CI_REACTION_ANI, gcnew String(clipboardReActionRowData->m_pReactionData->strReActionName.c_str()));
	sprintf(chTemp, "%f", clipboardReActionRowData->m_pReactionData->fWeight);
	SetSelectCellData(nRow, CI_WEIGHT, gcnew String(chTemp));
	sprintf(chTemp, "%f", clipboardReActionRowData->m_pReactionData->fSpeed);
	SetSelectCellData(nRow, CI_SPEED, gcnew String(chTemp));
	SetSelectCellData(nRow, CI_WEAPON_BONENAME, gcnew String(clipboardReActionRowData->m_pReactionData->strWeaponBoneName.c_str()));
	
	if(clipboardReActionRowData->m_pReactionData->bDisable)
		SetSelectCellData(nRow, CI_DISABLE, gcnew String("True"));
	else
		SetSelectCellData(nRow, CI_DISABLE, gcnew String("False"));
}

System::Void Character_NET::ReActionAnimationTable::ReActionAniamtion_dataGridView_KeyUp( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
{
	if(e->Control)
	{
		if(e->KeyCode == Keys::C)
		{
			CopyReActionData();
			// 카피
		}
	}
}
