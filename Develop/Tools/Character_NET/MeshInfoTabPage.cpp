#include "StdAfx.h"
#include "MeshInfoTabPage.h"
#include "Xmlseral.h"
#include "AnimationSelectEditForm.h"

void Character_NET::MeshInfoTabPage::SetMeshInfo( System::String^ ModelName )
{
	ModelName_label->Text = ModelName;

	SetMeshInfoData(ModelName);
}

void Character_NET::MeshInfoTabPage::SetMeshInfoData( System::String^ ModelName )
{
	MeshInfo_propertyGrid->SelectedObject = nullptr;
	string strModelName = MStringToCharPointer(ModelName);
	CSMeshInfo * pMeshInfo = TMeshInfoMgr::GetInstance().GetMeshInfo(strModelName);
	if(pMeshInfo == NULL)
		return;

	XmlSerializable::MESHINFO^ xinfo = gcnew XmlSerializable::MESHINFO;

	// bparts
	RActor* pActor = g_pMainApp->GetActor();
	Character_NET::Form1^ pMainForm = GlobalObjects::g_pMainForm;
	if (pActor != NULL && pMainForm != nullptr)
	{
		bool bReadOnly[4] = {true, true, true, true};
		for (int nBPartID=1; nBPartID<=MAX_BREAKABLE_PARTS; nBPartID++)
		{
			char chNodeName[257] = {0,};
			sprintf(chNodeName, "%s%02d", "b_parts_", nBPartID);
			RActorNode* pBreakablePartsNode = pActor->GetActorNode(chNodeName);

			sprintf(chNodeName, "파괴음_%02d", nBPartID);
			String^ strStart = gcnew String(chNodeName);

			sprintf(chNodeName, "지면충돌음_%02d", nBPartID);
			String^ strCollision = gcnew String(chNodeName);

			using namespace System::ComponentModel;
			using namespace System::Reflection;

			if (pBreakablePartsNode)
				bReadOnly[nBPartID-1] = false;

			System::ComponentModel::PropertyDescriptor^ descriptor_start = System::ComponentModel::TypeDescriptor::GetProperties(xinfo->GetType())[strStart];
			ReadOnlyAttribute^ attrib_start = (ReadOnlyAttribute^)descriptor_start->Attributes[ ReadOnlyAttribute::typeid ];
			FieldInfo^ isReadOnly_start = attrib_start->GetType()->GetField("isReadOnly", BindingFlags::NonPublic | BindingFlags::Instance);
			isReadOnly_start->SetValue(attrib_start, bReadOnly[nBPartID-1]);

			System::ComponentModel::PropertyDescriptor^ descriptor_collision = System::ComponentModel::TypeDescriptor::GetProperties(xinfo->GetType())[strCollision];
			ReadOnlyAttribute^ attrib_collision = (ReadOnlyAttribute^)descriptor_collision->Attributes[ ReadOnlyAttribute::typeid ];
			FieldInfo^ isReadOnly_collision = attrib_collision->GetType()->GetField("isReadOnly", BindingFlags::NonPublic | BindingFlags::Instance);
			isReadOnly_collision->SetValue(attrib_collision, bReadOnly[nBPartID-1]);
		}

		#define _SET_BPARTS_CONTENTS( idx ) \
		if(bReadOnly[idx-1]) {\
			xinfo->파괴음_0##idx = gcnew String("<파츠가 없어서 편집할수 없음>"); \
			xinfo->지면충돌음_0##idx = gcnew String("<파츠가 없어서 편집할수 없음>"); \
			pMainForm->bp##idx->Checked = false; pMainForm->bp##idx->Enabled = false; \
		}else{\
			xinfo->파괴음_0##idx = gcnew String(pMeshInfo->m_strBPartsStartSound[idx-1].c_str()); \
			xinfo->지면충돌음_0##idx = gcnew String(pMeshInfo->m_strBPartsCollisionSound[idx-1].c_str()); \
			pMainForm->bp##idx->Checked = true; pMainForm->bp##idx->Enabled = true; \
		}

		_SET_BPARTS_CONTENTS(1);
		_SET_BPARTS_CONTENTS(2);
		_SET_BPARTS_CONTENTS(3);
		_SET_BPARTS_CONTENTS(4);

	}

	// set xinfo property to selectedObject
	MeshInfo_propertyGrid->SelectedObject = xinfo;
}

System::Void Character_NET::MeshInfoTabPage::MeshInfo_propertyGrid_PropertyValueChanged( System::Object^ s, System::Windows::Forms::PropertyValueChangedEventArgs^ e )
{
	if(MeshInfo_propertyGrid->SelectedObject == nullptr)
		return;

	string strModelName = MStringToCharPointer(ModelName_label->Text);
	CSMeshInfo * pMeshInfo = TMeshInfoMgr::GetInstance().GetMeshInfo(strModelName);
	if(pMeshInfo == NULL)
		return;

	XmlSerializable::MESHINFO^ xinfo = (XmlSerializable::MESHINFO^)(MeshInfo_propertyGrid->SelectedObject);


#define _B_PARTS_START_SOUND_SETTER( idx ) {\
	if (xinfo->파괴음_0##idx != nullptr) {\
		System::String^ strLabelName = "파괴음_0"; strLabelName += #idx; \
		if (e->ChangedItem->Label == strLabelName ) { \
			pMeshInfo->m_strBPartsStartSound[idx-1] = MStringToCharPointer(xinfo->파괴음_0##idx); \
			GlobalObjects::g_SaveNotifier.SetSaveMeshInfo(true); \
		} \
	}\
}

#define _B_PARTS_COLLISION_SOUND_SETTER( idx ) {\
	if (xinfo->지면충돌음_0##idx != nullptr) {\
		System::String^ strLabelName = "지면충돌음_0"; strLabelName += #idx; \
		if (e->ChangedItem->Label == strLabelName ) { \
			pMeshInfo->m_strBPartsCollisionSound[idx-1] = MStringToCharPointer(xinfo->지면충돌음_0##idx); \
			GlobalObjects::g_SaveNotifier.SetSaveMeshInfo(true); \
		} \
	}\
}

	_B_PARTS_START_SOUND_SETTER(1);
	_B_PARTS_START_SOUND_SETTER(2);
	_B_PARTS_START_SOUND_SETTER(3);
	_B_PARTS_START_SOUND_SETTER(4);

	_B_PARTS_COLLISION_SOUND_SETTER(1);
	_B_PARTS_COLLISION_SOUND_SETTER(2);
	_B_PARTS_COLLISION_SOUND_SETTER(3);
	_B_PARTS_COLLISION_SOUND_SETTER(4);

}