#include "stdafx.h"
#include "C_XMLConfig.h"
#include "C_XMLResourceList.h"

#include "MXml.h"
#include "RSceneNode.h"

#include "MainFrm.h"

#define DEFAULT_PATH_DATA	"../../Data/"
#define PATH_TILE			"Texture/Tile/"
#define PATH_TREE			"Model/SpeedTree/"
#define PATH_TEXTURE		"Texture/SpeedTree/;Texture/Map/;Texture/Character/;Texture/SFX/;Texture/Sky/;Texture/Weapon/"
#define PATH_EFFECT			"SFX/"
#define PATH_ELU			"Model"
#define PATH_MAPOBJECT		"Model/MapObject/"
#define PATH_SKY			"Model/Sky/"
#define PATH_EFT			"fx/"
#define PATH_SHADER			"Shader/"
#define PATH_SHADER_DEV		"ShaderDev/"
#define PATH_SOUND			"Sound/"
#define PATH_BGM			"Sound/BGM/"
#define PATH_EFFECTSOUND	"Sound/Effect/"

#define DEFAULT_PATH_SERVER	"../GameServer/"
#define PATH_SCRIPT			"script/"
#define PATH_FIELD			"field/"
#define PATH_SCRIPT_NPC		"npc/"
#define PATH_SCRIPT_FIELD	"field/"
#define PATH_ZONE			"zone/"

#define PATH_CAM_ANI		"camera/"

using namespace rs3;

bool CXMLConfig::LoadXML(const char *filename)
{
	// 기본 리소스 경로 설정, config.xml 파일로 오버라이딩할수 있다
	SetPathesFromDataPath_();

	MXml kXml;
	if ( kXml.LoadFile(filename))
	{
		MXmlElement *pRoot = kXml.Doc()->RootElement();
		MXmlElement *pChildElement = pRoot->FirstChildElement();

		while ( pChildElement != NULL )
		{
			const char *pValue = pChildElement->Value();

			// Tool에 필요한 값
			if ( _stricmp(pValue,"MaxFarZ")==0 )
				_Contents(&fMaxFarZ, pChildElement);
			else if ( _stricmp(pValue,"MinFarZ")==0 )
				_Contents(&fMinFarZ, pChildElement);
			else if ( _stricmp(pValue,"WheelZoomFactor")==0 )
				_Contents(&fWheelZoomFactor, pChildElement);
			else if ( _stricmp(pValue,"KeyZoomFactor")==0 )
				_Contents(&fKeyZoomFactor, pChildElement);
			else if ( _stricmp(pValue,"WASD_Factor")==0 )
				_Contents(&fVelocity, pChildElement);
			else if ( _stricmp(pValue,"RotateFactor")==0 )
				_Contents(&fRotateStep, pChildElement);
			else if ( _stricmp(pValue,"PanFactor")==0 )
				_Contents(&fPanStep, pChildElement);
			//////////////////////////////////////////////////////////////////////////
			// 리소스 경로 추가 설정
			else if ( _stricmp(pValue,"TexturePath")==0 )
				_Contents( strTexturePath, pChildElement );
			else if ( _stricmp(pValue,"MeshPath")==0 )
				_Contents( pcEluPath, pChildElement );

			// dump path
			else if ( _stricmp(pValue,"DumpPath")==0 )
			{
				std::string& dumpPath = CMainFrame::GetDumpPathString();
				_Contents( dumpPath, pChildElement );
			}

			pChildElement = pChildElement->NextSiblingElement();
		}
	}

	//init ResourceList after set path from data path
	CXMLResourceList::GetReference().InitResourcePath();

	return true;
}

void CXMLConfig::SaveXML(const char *filename)
{
	SetPathesFromDataPath_();

	MXml xml;

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "euc-kr", "");
	xml.Doc()->LinkEndChild(pDec);
	
	MXmlElement* pElement1 = new MXmlElement( "Config" );
	xml.Doc()->LinkEndChild(pElement1);

	MXmlElement* pElement = new MXmlElement( "MaxFarZ" );
	pElement1->LinkEndChild(pElement);
	_SetContents(pElement, fMaxFarZ);

	pElement = new MXmlElement( "MinFarZ" );
	pElement1->LinkEndChild(pElement);
	_SetContents(pElement, fMinFarZ);

	pElement = new MXmlElement( "WheelZoomFactor" );
	pElement1->LinkEndChild(pElement);
	_SetContents(pElement, fWheelZoomFactor);

	pElement = new MXmlElement( "KeyZoomFactor" );
	pElement1->LinkEndChild(pElement);
	_SetContents(pElement, fKeyZoomFactor);

	pElement = new MXmlElement( "RotateFactor" );
	pElement1->LinkEndChild(pElement);
	_SetContents(pElement, fRotateStep);

	pElement = new MXmlElement( "PanFactor" );
	pElement1->LinkEndChild(pElement);
	_SetContents(pElement, fPanStep);

	//datapath 넣는것이 필요할까?
	/*pElement = new MXmlElement( "datapath" );
	pElement1->LinkEndChild(pElement);
	_SetContents(pElement, pcDataPath);*/
	
	////////
	xml.Doc()->SaveFile(filename);
}

CXMLConfig::CXMLConfig()
: fMaxFarZ(10000.0f)
, fMinFarZ(100000.0f)
, fWheelZoomFactor(2.0f)
, fKeyZoomFactor(100.0f)
, fVelocity(200.0f)
, fRotateStep(0.4f)
, fPanStep(10.0f)
, strTexturePath("")
, strServerPath(DEFAULT_PATH_SERVER)
{
	pcTilepath[0] = '\0';
	//pcBaseMesh[0] = '\0';
	pcSkyPath[0] = '\0';

	pcTreepath[0] = '\0';
	pcEluPath[0] = '\0';
	pcMapObjectPath[0] = '\0';
	pcEftpath[0] = '\0';
	pcShaderpath[0] = '\0';

	pcBGMPath[0] = '\0';
	pcEffectSoundPath[0] = '\0';
	pcSoundPath[0] = '\0';

	strcpy(pcDataPath, DEFAULT_PATH_DATA);
}

CXMLConfig::~CXMLConfig()
{
}

void CXMLConfig::SetPathesFromDataPath_()
{
	sprintf(pcTilepath, "%s%s", pcDataPath, PATH_TILE);
	sprintf(pcTreepath, "%s%s", pcDataPath, PATH_TREE);

	if(REngine::IsDevelopVersion())
		sprintf(pcShaderpath, "%s%s;%s%scomponents/", pcDataPath, PATH_SHADER_DEV, pcDataPath, PATH_SHADER_DEV);
	else
		sprintf(pcShaderpath, "%s%s;%s%scomponents/", pcDataPath, PATH_SHADER, pcDataPath, PATH_SHADER);

	sprintf(pcEluPath, "%s%s", pcDataPath, PATH_ELU);
	sprintf(pcMapObjectPath, "%s%s", pcDataPath, PATH_MAPOBJECT);
	sprintf(pcEftpath, "%s%s", pcDataPath, PATH_EFT);
	sprintf(pcEffectpath, "%s%s", pcDataPath, PATH_EFFECT);
	sprintf(pcSkyPath, "%s%s", pcDataPath, PATH_SKY);

	//사운드패스
	sprintf(pcSoundPath, "%s%s", pcDataPath, PATH_SOUND);
	sprintf(pcBGMPath,"%s%s", pcDataPath, PATH_BGM);
	sprintf(pcEffectSoundPath, "%s%s", pcDataPath, PATH_EFFECTSOUND);

	// 텍스쳐 패쓰 정리
	strTexturePath = PATH_TEXTURE;

	vector<string>		PathList;
	string::size_type i = 0;
	string::size_type lastSemicolon = 0;
	string path;
	while((i=strTexturePath.find(";",i))!=-1)
	{
		path = strTexturePath.substr(lastSemicolon,i-lastSemicolon);
		PathList.push_back(path);
		i++;
		lastSemicolon = i;
	}

	path = strTexturePath.substr(lastSemicolon);
	if(!path.empty())
		PathList.push_back(path);

	for (int i = 0; i < (int)PathList.size(); i++)
	{
		PathList[i] = pcDataPath + PathList[i];
	}

	strTexturePath = "";
	for (int i = 0; i < (int)PathList.size(); i++)
	{
		strTexturePath = strTexturePath + PathList[i] + string(";");
	}

	//Server
	strServerScriptPath = strServerPath + PATH_SCRIPT;
	strServerFieldPath = strServerPath + PATH_FIELD;
	strServerZonePath = strServerPath + PATH_ZONE;

	char szfull[256]= "";
	_fullpath( szfull, strServerScriptPath.c_str(), 256 );
	strServerScriptFullPath = szfull;
	//strServerFieldScriptFullPath = strServerScriptFullPath + PATH_SCRIPT_FIELD;
	strServerFieldScriptFullPath = strServerPath + PATH_SCRIPT + PATH_FIELD;
	strServerZoneFullPath = strServerPath + PATH_ZONE;
	//strServerNPCScriptFullPath = strServerScriptFullPath + PATH_SCRIPT_NPC;
	strServerNPCScriptFullPath = strServerPath + PATH_SCRIPT_NPC;

	_fullpath( szfull, strServerFieldPath.c_str(), 256 );
	strServerFieldFullPath = szfull;

	strCameraAniPath = std::string(pcEffectpath) + PATH_CAM_ANI;

	_fullpath( szfull, pcDataPath, 256 );
	strDataFullPath = szfull;
}
