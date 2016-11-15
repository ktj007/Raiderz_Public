#pragma once

namespace rs3 
{
	class RRule
	{
	public:
		class Animation
		{
		public:
			static std::string	GetAnimationSetXMLFileNameByEluFileName(std::string EluFileName)
			{
				return std::string ( EluFileName + ".animation.xml" );
			}

			static std::string GetAnimationEventXMLFileByEluFileName(std::string EluFileName)
			{
				return std::string ( EluFileName + ".animationevent.xml" );
			}

			static std::string GetAutoAnimationNameByEluFileName(std::string EluFileName)
			{
				return MGetPureFileName(EluFileName);
			}

			static std::string GetAutoAnimationFileNameByEluFileName(std::string EluFileName)
			{
				return std::string ( EluFileName + ".ani" );
			}
		};

		class Terrain
		{
		public:
			static std::string GetTerrainDescFileNameByTerrainHeightmapFileName(std::string fileName)
			{
				return std::string ( MGetPureFileName( fileName ) + ".desc" );
			}
		};

		class GrassSystem
		{
		public:
			static std::string GetGrassTextureFileNameByGrassFileName(std::string fileName)
			{
				return std::string ( MGetPureFileName( fileName ) + ".dds" );
			}
		};
	};

}