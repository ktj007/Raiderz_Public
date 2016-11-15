#include "stdafx.h"
#include "RTypes.h"
#include "RS3UnitTestUtility.h"
#include "RDevice.h" 
#include "RWorldSceneNode.h"


using namespace rs3;

TEST(TestWorldSceneNode,AutoUpdateList)
{
	class SceneNodeToTestAutoUpdate : public RSceneNode
	{
	public:
		using RSceneNode::SetAutoUpdate; 

	};

	RWorldSceneNode WorldSceneNode;

	SceneNodeToTestAutoUpdate		AutoUpdateTrueSceneNode;
	AutoUpdateTrueSceneNode.SetAutoUpdate(true);

	SceneNodeToTestAutoUpdate		AutoUpdateFalseSceneNode;
	AutoUpdateFalseSceneNode.SetAutoUpdate(false);

	WorldSceneNode.AddChild(&AutoUpdateFalseSceneNode);
	EXPECT_EQ( false , WorldSceneNode.ExistSceneNodeInAutoUpdateList(&AutoUpdateFalseSceneNode) );
	WorldSceneNode.RemoveChild(&AutoUpdateFalseSceneNode);

	WorldSceneNode.AddChild(&AutoUpdateTrueSceneNode);
	EXPECT_EQ( true , WorldSceneNode.ExistSceneNodeInAutoUpdateList(&AutoUpdateTrueSceneNode) );
	WorldSceneNode.RemoveChild(&AutoUpdateTrueSceneNode);
}

TEST(TestWorldSceneNode,AutoUpdate)
{
	class UpdateTransformSceneNode : public RSceneNode
	{
	public:
		UpdateTransformSceneNode(void)
			:m_bOnUpdateTransformWorked(false)
		{
			SetAutoUpdate(true);
		}

		virtual void				OnUpdateTransform()
		{
			m_bOnUpdateTransformWorked = true;
		}

		bool	IsOnUpdateTransformWorked(void)	{	return m_bOnUpdateTransformWorked; }
	private:
		bool	m_bOnUpdateTransformWorked;
	};

	NullDeviceEngineCreater NullDeviceEngineCreater;

	RWorldSceneNode				WorldSceneNode;
	UpdateTransformSceneNode	SceneNode;
	WorldSceneNode.AddChild(&SceneNode);
	WorldSceneNode.UpdateAutoUpdateSceneNode();
	WorldSceneNode.RemoveChild(&SceneNode);

	EXPECT_TRUE( SceneNode.IsOnUpdateTransformWorked() );
}