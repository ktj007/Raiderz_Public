#pragma once
#include "wx/panel.h"
#include "wx/treectrl.h"

namespace rs3
{
	class RSceneNode;
}
using namespace rs3;

namespace EffectTool
{

class SceneNodeTreeCtrl : public wxTreeCtrl
{
	DECLARE_DYNAMIC_CLASS(SceneNodeTreeCtrl)
	wxDECLARE_NO_COPY_CLASS(SceneNodeTreeCtrl);

	class SceneNodeTreeItemData : public wxTreeItemData
	{
	public:
		SceneNodeTreeItemData(RSceneNode* pSceneNode) : m_pSceneNode(pSceneNode) {}
		RSceneNode* GetSceneNode() { return m_pSceneNode; }
	private:
		RSceneNode* m_pSceneNode;
	};

public:
	SceneNodeTreeCtrl();
	SceneNodeTreeCtrl(wxWindow *parent, wxWindowID id);

	void SetEffectSceneNode(RSceneNode* pSceneNode);

private:
	void InitIcons();
	wxImageList* CreateImageList(wxIcon* pIcons, size_t count);

	void AppendChildSceneNodesToTree(wxTreeItemId parentTreeItem);

	wxTreeItemId AppendOneSceneNodeToTree(wxTreeItemId parentTreeItem, RSceneNode* pSceneNode);

	wxTreeItemId FindChildTreeItem(wxTreeItemId parentTreeItem, const RSceneNode* pSceneNode);

	bool IsEqualSceneNodeItem(wxTreeItemId item, const RSceneNode* pSceneNode);

	RSceneNode* GetSceneNodeFromTreeItem(wxTreeItemId treeItem);

	void UpdateTreeItem(wxTreeItemId item);
	void SetItemChecked(wxTreeItemId item, bool bChecked);

	int GetImageIndex(RSceneNode* pSceneNode);
	int GetSelectedImageIndex(RSceneNode* pSceneNode);

	SceneNodeTreeItemData* NewSceneNodeTreeItemData(RSceneNode* pSceneNode);
};

class EffectScenePanel : public wxPanel
{
	enum eID
	{
		ID_EFFECTSCENE_TREECTRL = wxID_HIGHEST+1,
	};

public:
	EffectScenePanel(wxWindow* parent);
	~EffectScenePanel();

	void Init();

	void SetEffectSceneNode(RSceneNode* pSceneNode);

private:
	void AppendChildSceneNodesToTree(wxTreeCtrl* pTreeCtrl, wxTreeItemId parentTreeItem);

	wxTreeItemId AppendSceneNodeToTree(wxTreeCtrl* pTreeCtrl, wxTreeItemId parentTreeItem, RSceneNode* pSceneNode);

	wxTreeItemId FindTreeItem(wxTreeCtrl* pTreeCtrl, wxTreeItemId parentItemID, const RSceneNode* pSceneNode);

	bool IsEqualItem(wxTreeCtrl* pTreeCtrl, wxTreeItemId id, const RSceneNode* pSceneNode);

	DECLARE_EVENT_TABLE()
};

}