#include "stdafx.h"
#include "RSceneNode.h"
#include "RSceneManager.h"
#include "MXml.h"
#include "RS3UnitTestUtility.h"

using namespace rs3;

TEST( TestSceneNodeUserProperty, PushRemove )
{
	RSceneNode testNode;

	// 추가
	testNode.PushUserProperty("TestProperty1", "Value1");
	testNode.PushUserProperty("TestProperty2", "Value2");
	testNode.PushUserProperty("TestProperty3", "Value3");

	EXPECT_EQ( 3, testNode.GetUserPropertyCount() );
	EXPECT_STREQ( "TestProperty3", testNode.GetUserPropertyName(2).c_str() );
	EXPECT_STREQ( "TestProperty3", testNode.GetUserPropertyName(2).c_str() );

	// 삭제
	testNode.RemoveUserProperty(0);

	EXPECT_EQ( 2, testNode.GetUserPropertyCount() );
	EXPECT_STREQ( "TestProperty3", testNode.GetUserPropertyName(1).c_str() );
}


TEST( TestSceneNodeUserProperty, SetUserPropertyValue )
{
	RSceneNode testNode;

	// 추가
	testNode.PushUserProperty("TestProperty1", "Value1");

	// 실행
	testNode.SetUserPropertyValue( 0, "ValueChanged");

	// 검증
	EXPECT_STREQ( "ValueChanged", testNode.GetUserPropertyValue(0).c_str() );
}

TEST( TestSceneNodeUserProperty, SaveLoad )
{
	// 설치
	NullDeviceEngineCreater useNullDevice;

	RSceneNode testNode;
	testNode.PushUserProperty("TestProperty1", "Value1");
	testNode.PushUserProperty("TestProperty2", "Value2");


	// 실행
	RSceneManager* pTestSceneManager = REngine::GetSceneManagerPtr();

	MXmlElement xmlElement("SCENENODE");
	pTestSceneManager->SaveXML( &testNode, &xmlElement );


	RSceneNode testNodeLoaded;
	MXmlElement* pNodeElement = xmlElement.FirstChildElement( testNode.GetNodeIDString() );
	pTestSceneManager->LoadXML( &testNodeLoaded, *pNodeElement );

	// 검증
	EXPECT_EQ( 2, testNodeLoaded.GetUserPropertyCount() );
	EXPECT_STREQ( "TestProperty1",	testNodeLoaded.GetUserPropertyName(0).c_str() );
	EXPECT_STREQ( "Value1",			testNodeLoaded.GetUserPropertyValue(0).c_str() );
	EXPECT_STREQ( "TestProperty2",	testNodeLoaded.GetUserPropertyName(1).c_str() );
	EXPECT_STREQ( "Value2",			testNodeLoaded.GetUserPropertyValue(1).c_str() );
}