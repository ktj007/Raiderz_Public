#include "wx/wxprec.h"
#include "wx/treectrl.h"

#include "EffectScenePanel.h"

#include "RSceneNode.h"
#include "RActorNode.h"
#include "RActorNodeConstraintController.h"
#include "REffectActor.h"
#include "REffectSceneNode.h"
#include "RTrail.h"
#include "REmitter.h"
#include "RPointLightSceneNode.h"
#include "RSpotLightSceneNode.h"

#include "res/effectscenenode.xpm"
#include "res/actor.xpm"
#include "res/actornode.xpm"
#include "res/trail.xpm"
#include "res/pointlight.xpm"
#include "res/spotlight.xpm"
#include "res/emitter_sphere.xpm"
#include "res/emitter_box.xpm"
#include "res/emitter_ring.xpm"

#include "res/unchecked.xpm"
#include "res/checked.xpm"

namespace EffectTool
{

///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(SceneNodeTreeCtrl, wxTreeCtrl)

SceneNodeTreeCtrl::SceneNodeTreeCtrl()
: wxTreeCtrl()
{
}

SceneNodeTreeCtrl::SceneNodeTreeCtrl(wxWindow *parent, wxWindowID id) :
 wxTreeCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_EDIT_LABELS | wxTR_ROW_LINES | wxTR_MULTIPLE)
{
	InitIcons();
}

void SceneNodeTreeCtrl::InitIcons()
{
	{
		wxIcon icons[9];
		icons[0] = wxIcon(effectscenenode_xpm);
		icons[1] = wxIcon(actor_xpm);
		icons[2] = wxIcon(actornode_xpm);
		icons[3] = wxIcon(trail_xpm);
		icons[4] = wxIcon(pointlight_xpm);
		icons[5] = wxIcon(spotlight_xpm);
		icons[6] = wxIcon(emitter_sphere_xpm);
		icons[7] = wxIcon(emitter_box_xpm);
		icons[8] = wxIcon(emitter_ring_xpm);
		wxImageList* pImageList = CreateImageList(icons, _countof(icons));
		AssignImageList(pImageList);
	}
	{
		wxIcon icons[2];
		icons[0] = wxIcon(unchecked_xpm);
		icons[1] = wxIcon(checked_xpm);
		wxImageList* pImageList = CreateImageList(icons, _countof(icons));
		AssignStateImageList(pImageList);
	}
}

wxImageList* SceneNodeTreeCtrl::CreateImageList(wxIcon* pIcons, size_t count)
{
	_ASSERT(pIcons);
	_ASSERT(count > 0);

	int width = pIcons[0].GetWidth();
	int height = pIcons[0].GetHeight();
	wxImageList* pImageList = new wxImageList(width, height, true);
	for( size_t i = 0; i < count; ++i )
	{
		_ASSERT(pIcons[i].IsOk());
		pImageList->Add(pIcons[i]);
	}

	return pImageList;
}

void SceneNodeTreeCtrl::SetEffectSceneNode(RSceneNode* pSceneNode)
{
	_ASSERT(pSceneNode);
	REffectSceneNode* pEffectSceneNode = MDynamicCast(REffectSceneNode, pSceneNode);
	_ASSERT(pEffectSceneNode);

	DeleteAllItems();

	wxString effectNodeName = pSceneNode->GetNodeName();
	wxTreeItemId rootItem = AddRoot(effectNodeName,
		GetImageIndex(pEffectSceneNode),
		GetSelectedImageIndex(pEffectSceneNode),
		NewSceneNodeTreeItemData(pEffectSceneNode));
	_ASSERT(rootItem);

	UpdateTreeItem(rootItem);

	AppendChildSceneNodesToTree(rootItem);

	ExpandAll();
}

void SceneNodeTreeCtrl::AppendChildSceneNodesToTree(wxTreeItemId parentTreeItem)
{
	_ASSERT(parentTreeItem);

	RSceneNode* pParentSceneNode = GetSceneNodeFromTreeItem(parentTreeItem);
	_ASSERT(pParentSceneNode);

	const RSceneNodeList& childList = pParentSceneNode->GetChildren();
	for(RSceneNodeList::const_iterator it = childList.begin(); it != childList.end(); ++it)
	{
		RSceneNode* pSceneNode = *it;
		wxTreeItemId item = AppendOneSceneNodeToTree(parentTreeItem, pSceneNode);
		AppendChildSceneNodesToTree(item);
	}
}

wxTreeItemId SceneNodeTreeCtrl::AppendOneSceneNodeToTree(wxTreeItemId parentTreeItem, RSceneNode* pSceneNode)
{
	_ASSERT(parentTreeItem);
	_ASSERT(pSceneNode);

	wxTreeItemId item;

	// actornode constraint 가 붙은 경우에는 actornode 의 자식처럼 보여준다
	RActorNodeConstraintController* pConstraint = GetFirstActorNodeConstraintController(pSceneNode);
	if(pConstraint)
	{
		const RActorNode* pActorNode = pConstraint->GetTargetActorNode();
		item = FindChildTreeItem(parentTreeItem, pActorNode);
	}
	else
	{
		item = parentTreeItem;
	}
	_ASSERT(item);

	wxTreeItemId childItem = AppendItem( item,
		pSceneNode->GetNodeName(),
		GetImageIndex(pSceneNode),
		GetSelectedImageIndex(pSceneNode),
		NewSceneNodeTreeItemData(pSceneNode));
	_ASSERT(childItem);

	UpdateTreeItem(childItem);

	return childItem;
}

wxTreeItemId SceneNodeTreeCtrl::FindChildTreeItem(wxTreeItemId parentTreeItem, const RSceneNode* pSceneNode)
{
	_ASSERT(parentTreeItem);
	_ASSERT(pSceneNode);

	if(false == parentTreeItem.IsOk())
		return wxTreeItemId();

	wxTreeItemIdValue cookie;
	wxTreeItemId item = GetFirstChild(parentTreeItem, cookie);
	while(item)
	{
		// debug
		wxString itemText = GetItemText(item);

		if(IsEqualSceneNodeItem(item, pSceneNode))
			return item;

		wxTreeItemId childItem = FindChildTreeItem(item, pSceneNode);
		if(childItem)
			return childItem;

		item = GetNextChild(parentTreeItem, cookie);
	}

	return item;
}

bool SceneNodeTreeCtrl::IsEqualSceneNodeItem(wxTreeItemId item, const RSceneNode* pSceneNode)
{
	return ( GetSceneNodeFromTreeItem(item) == pSceneNode );
}

RSceneNode* SceneNodeTreeCtrl::GetSceneNodeFromTreeItem(wxTreeItemId treeItem)
{
	SceneNodeTreeItemData* pItemData = static_cast<SceneNodeTreeItemData*>(GetItemData(treeItem));
	_ASSERT(pItemData);
	RSceneNode* pSceneNode = pItemData->GetSceneNode();
	_ASSERT(pSceneNode);

	return pSceneNode;
}

void SceneNodeTreeCtrl::UpdateTreeItem(wxTreeItemId item)
{
	_ASSERT(item);
	RSceneNode* pSceneNode = GetSceneNodeFromTreeItem(item);
	_ASSERT(pSceneNode);

	if(REffectActor* pEffectActor = MDynamicCast(REffectActor, pSceneNode))
	{
		SetItemChecked(item, pEffectActor->GetVisibleSwitch());
	}
	else
	if(REffectBase* pEffectBase = MDynamicCast(REffectBase, pSceneNode))
	{
		SetItemChecked(item, pSceneNode->GetVisible());
	}

	// 딸린 노드들은 수정불가
	if(pSceneNode->QueryAttribute(RSNA_DO_NOT_SAVE))
		SetItemTextColour(item, *wxLIGHT_GREY);
}

void SceneNodeTreeCtrl::SetItemChecked(wxTreeItemId item, bool bChecked)
{
	_ASSERT(item);

	if(bChecked)
		SetItemState(item, 1);
	else
		SetItemState(item, 0);
}

int SceneNodeTreeCtrl::GetImageIndex(RSceneNode* pSceneNode)
{
	_ASSERT(pSceneNode);

	if( MDynamicCast(REffectSceneNode, pSceneNode) )
		return 0;

	if( MDynamicCast(REffectActor, pSceneNode) )
		return 1;

	if( MDynamicCast(RActorNode, pSceneNode) )
		return 2;

	if( MDynamicCast(RTrail, pSceneNode) )
		return 3;

	if( MDynamicCast(RPointLightSceneNode, pSceneNode) )
		return 4;

	if( MDynamicCast(RSpotLightSceneNode, pSceneNode) )
		return 5;

	REmitter* pEmitter;
	if( pEmitter = MDynamicCast(REmitter, pSceneNode) )
	{
		const REmitterDescription* pDesc = pEmitter->GetDesc();

		switch( pDesc->eEmitterType )
		{
		case EMITTER_SPHERE: return 6;
		case EMITTER_BOX: return 7;
		case EMITTER_RING: return 8;
		}
	}

	return -1;
}

int SceneNodeTreeCtrl::GetSelectedImageIndex(RSceneNode* pSceneNode)
{
	return -1;
}

SceneNodeTreeCtrl::SceneNodeTreeItemData* SceneNodeTreeCtrl::NewSceneNodeTreeItemData(RSceneNode* pSceneNode)
{
	_ASSERT(pSceneNode);
	return new SceneNodeTreeItemData(pSceneNode);
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(EffectScenePanel, wxPanel)
END_EVENT_TABLE()

EffectScenePanel::EffectScenePanel(wxWindow* parent) :
 wxPanel(parent)
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	SceneNodeTreeCtrl* sceneTreeCtrl = new SceneNodeTreeCtrl(this, ID_EFFECTSCENE_TREECTRL);
	sizer->Add(sceneTreeCtrl, wxSizerFlags(1).Center().Border().Expand());

	SetSizerAndFit(sizer);
}

EffectScenePanel::~EffectScenePanel()
{
}

void EffectScenePanel::Init()
{

}

void EffectScenePanel::SetEffectSceneNode(RSceneNode* pSceneNode)
{
	SceneNodeTreeCtrl* treeCtrl = wxDynamicCast(FindWindow(ID_EFFECTSCENE_TREECTRL), SceneNodeTreeCtrl);
	_ASSERT(treeCtrl);
	treeCtrl->SetEffectSceneNode(pSceneNode);
}

///////////////////////////////////////////////////////////////////////////////

}
