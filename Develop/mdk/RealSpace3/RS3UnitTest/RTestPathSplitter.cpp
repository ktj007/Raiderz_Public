#include "stdafx.h"
#include "RPathSplitter.h"

using namespace rs3;

TEST( TestPathSplitter, GetPath )
{
	RPathSplitter ps;

	ps.SetPath( ".;test;testb;testc/" );
	EXPECT_EQ( 4, ps.GetPathCount() );

	EXPECT_STREQ( ps.GetPath(0).c_str(), "./" );
	EXPECT_STREQ( ps.GetPath(1).c_str(), "test/" );
	EXPECT_STREQ( ps.GetPath(3).c_str(), "testc/" );

	// 전체 재처리된 경로가 맞는지
	EXPECT_STREQ( ps.GetPath().c_str(),"./;test/;testb/;testc/" );
}
