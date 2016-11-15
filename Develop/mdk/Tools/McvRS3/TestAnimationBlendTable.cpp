#include "stdafx.h"

#include "UnitTest++.h"

#include "RD3DVertexUtil.h"
#include "RSkeleton.h"
#include "RMeshAnimationMgr.h"
#include "RAnimationBlendTable.h"

SUITE(Animation)
{
	TEST(TestAnimationBlendTable)
	{
		// 애니메이션 매니저 초기화
		RMeshAnimationMgr aniMgr;

		// add( alias name, filename ) 이며 filename 은 유일해야한다
		aniMgr.Add( "ani_01", "ani_01.ani" );
		aniMgr.Add( "ani_02", "ani_02.ani" );
		aniMgr.Add( "ani_03", "ani_03.ani" );
		aniMgr.Add( "ani_04", "ani_04.ani" );

		CHECK_EQUAL( aniMgr.m_NodeHashList[0]->GetAliasName(), "ani_01" );
		CHECK_EQUAL( aniMgr.m_NodeHashList[1]->GetAliasName(), "ani_02" );
		CHECK_EQUAL( aniMgr.m_NodeHashList[2]->GetAliasName(), "ani_03" );
		CHECK_EQUAL( aniMgr.m_NodeHashList[3]->GetAliasName(), "ani_04" );

		// 애니메이션 블렌드 테이블 테스트
		{
			RAnimationBlendTable aniBlendTable;
			CHECK( !aniBlendTable.SetAnimationTransData( 3, 3, BLENDDATA::DEFAULT_BLEND_TYPE, 100 ) );
			CHECK( !aniBlendTable.SetAnimationTransData( 3, 1, BLENDDATA::DEFAULT_BLEND_TYPE, 100 ) );
			CHECK( !aniBlendTable.SetAnimationTransData( "ttt", "ttt", BLENDDATA::DEFAULT_BLEND_TYPE, 100 ) );
			CHECK( !aniBlendTable.SetAnimationTransData( "ttt", "rrr", BLENDDATA::DEFAULT_BLEND_TYPE, 100 ) );

			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( 0, 1 ).eType, BLENDDATA::DEFAULT_BLEND_TYPE );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( 5, 8 ).eType, BLENDDATA::DEFAULT_BLEND_TYPE );

			aniBlendTable.Reset( &aniMgr );
			CHECK( !aniBlendTable.SetAnimationTransData( 1, 1, RAT_CROSS_FADE, 100 ) );
			CHECK( !aniBlendTable.SetAnimationTransData( "ani_04", "ani_04", RAT_CROSS_FADE, 100 ) );

			CHECK( aniBlendTable.SetAnimationTransData( 0, 1, RAT_CROSS_FADE, 100 ) );
			CHECK( aniBlendTable.SetAnimationTransData( "ani_04", "ani_03", RAT_NONE, 100 ) );

			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( 0, 1 ).eType, RAT_CROSS_FADE );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( "ani_01", "ani_02" ).eType, RAT_CROSS_FADE );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( 3, 2 ).eType, RAT_NONE );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( "ani_04", "ani_03" ).eType, RAT_NONE );

			// delete
			CHECK( aniBlendTable.DeleteAnimationTransType( "ani_01", "ani_02" ) );
			CHECK( !aniBlendTable.DeleteAnimationTransType( 5, 8 ) );
			CHECK( !aniBlendTable.DeleteAnimationTransType( "eee", "aaa" ) );

			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( 0, 1 ).eType, BLENDDATA::DEFAULT_BLEND_TYPE );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( 3, 2 ).eType, RAT_NONE );

			// duration
			CHECK( aniBlendTable.SetAnimationTransData( "ani_02", "ani_03", RAT_IMMEDIATE_BLEND, 200 ) );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( 1, 2 ).nDuration, 200);

		}

		// 애니메이션 매니저 데이터 저장, 읽기 테스트
		{
			MXml xml;
			MXmlDeclaration* dec = xml.CreateDeclaration( "1.0", "UTF-8", "" );
			xml.Doc()->LinkEndChild( dec );
			
			MXmlElement* rootElement = new MXmlElement( "XML" );
			xml.Doc()->LinkEndChild( rootElement );

			RAnimationBlendTable aniBlendTable;
			aniBlendTable.Reset( &aniMgr );
			aniBlendTable.SetAnimationTransData( 0, 1, RAT_DELAYED_BLEND, 100 );
			aniBlendTable.SetAnimationTransData( 2, 3, RAT_CROSS_FADE, 100 );
			aniBlendTable.SetAnimationTransData( 3, 1, RAT_DELAYED_BLEND, 300 );

			// 추가로 애니메이션 파일을 추가한 경우
			aniMgr.Add( "ani_05", "ani_05.ani" );
			aniBlendTable.Reset( &aniMgr );
			aniBlendTable.SetAnimationTransData( 4, 1, RAT_CROSS_FADE, 100 );
			aniBlendTable.SetAnimationTransData( 3, 4, RAT_NONE, 100 );

			aniBlendTable.SaveXml( rootElement );

			MXmlElement* pAniTransElements = rootElement->FirstChildElement( "AnimationTransitions" );
			aniBlendTable.Clear();
			aniBlendTable.LoadXml( &aniMgr, pAniTransElements );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( 0, 1 ).eType, RAT_DELAYED_BLEND );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( 2, 3 ).eType, RAT_CROSS_FADE );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( 3, 1 ).eType, RAT_DELAYED_BLEND );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( 3, 1 ).nDuration, 300);
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( 5, 9 ).eType, RAT_IMMEDIATE_BLEND );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( 4, 1 ).eType, RAT_CROSS_FADE );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( 3, 4 ).eType, RAT_NONE );

			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( "ani_01", "ani_02" ).eType, RAT_DELAYED_BLEND );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( "ani_03", "ani_04" ).eType, RAT_CROSS_FADE );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( "ani_04", "ani_02" ).eType, RAT_DELAYED_BLEND );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( "ani_06", "ani_10" ).eType, RAT_IMMEDIATE_BLEND );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( "ani_05", "ani_02" ).eType, RAT_CROSS_FADE );
			CHECK_EQUAL( aniBlendTable.GetAnimationTransData( "ani_04", "ani_05" ).eType, RAT_NONE );
		}
	}
}
