#pragma once

namespace RS3U
{
	/* XML의 종류들
	   1.  xxx.zone.xml 			: RWorldSceneNode를 구성하는 정보를 담고 있는 XML파일
	   2.  xxx.scene.xml 			: RSceneNodeInstance를 구성하는 정보를 담고 있는 XML파일
	   3.  xxx.Effect.scene.xml 	: 
	*/
	class RUSceneXML
	{
	public:
		RUSceneXML(void);
		~RUSceneXML(void);

	public:
		//static void Load
	};

	class RUSceneNodeCreator
	{
	public:
		static void CreateSceneNodeFromFile(char* szFileName){};
	};
}