#pragma once

using namespace rs3;

namespace rs3{
	class RRotationController;
	class RRotationController;
	class RMaterialResource;
};

class TMtrl
{
protected:
	int				m_nItemID;		// xitem.xml 에서 참조할 ID

	vector<RMaterialResource*> m_MaterialTable;

	bool			AddSubMtrl_NPC(MXmlElement* pNPCElement, int nNPCID);
	bool			AddSubMtrl_Item(MXmlElement* pItemElement, int nItemID);

public:
	TMtrl(void);
	~TMtrl(void);

	void			Init();

	bool			ExportMtrlSet();

	bool			Export_NPC();
	bool			Export_Item();

	void			SetItemDataSubMtrl( int nItemID );
	void			SetNPCInfoSubMtrl( int NPCID );

	bool			SetMtrlList();

	bool			SetMeshTreeView();
	bool			SetMeshTreeView_Item();
	bool			SetMeshTreeView_NPC();

	bool			ChangeMaterialSet();
	bool			ChangeMaterialSet_Item();
	bool			ChangeMaterialSet_NPC();

	void			SetCurrentMaterialList(int nItemID);
	void 			SetCurrentMaterialList_Item(int nItemID);
	void 			SetCurrentMaterialList_NPC(int nNPCID);
};
