#include "stdafx.h"
#include "RTypes.h"
#include "RTreeResource.h"

using namespace rs3;

TEST( TreeResource, GetTextureLayerName )
{
	class TestTreeResource : public RTreeResource
	{
	public:
		using RTreeResource::GetTextureLayerName;
	};

	EXPECT_STREQ( TestTreeResource::GetTextureLayerName(TestTreeResource::SPT_TEXTURE_NUM), "_end" );
}