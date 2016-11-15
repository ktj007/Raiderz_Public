#include "stdafx.h"
#include "C_XMLXConst.h"
//#include "RTreeSystem.h"
//#include "RTreeResourceMgr.h"
#include "RTreeDef.h"

// 상수 기본값 정의
int		XCONST::CAMERA_FOV				= 90;
float	XCONST::CAMERA_NEAR_Z			= 10.0f;
float	XCONST::CAMERA_FAR_Z			= 200000.0f;
float	XCONST::GRASS_LOD_NEAR			= 2000.0f;
float	XCONST::GRASS_LOD_FAR			= 3000.0f;
float	XCONST::TREE_LOD_NEAR			= RTREE_DEFAULT_LOD_NEAR;
float	XCONST::TREE_LOD_FAR			= RTREE_DEFAULT_LOD_FAR;

int		XCONST::GAME_CAMERA_FOV			= 60;
float	XCONST::GAME_CAMERA_NEAR_Z		= 10.0f;
float	XCONST::GAME_CAMERA_FAR_Z		= 50000.0f;
float	XCONST::GAME_GRASS_LOD_NEAR		= 2000.0f;
float	XCONST::GAME_GRASS_LOD_FAR		= 3000.0f;
float	XCONST::GAME_TREE_LOD_NEAR		= 3000.0f;
float	XCONST::GAME_TREE_LOD_FAR		= 15000.0f;


#define CONST_XML_TAG_CONST			"CONST"
#define CONST_XML_TAG_CONST_GAME	"CONST_GAME"
#define CONST_XML_TAG_MAIET			"maiet"

#define _SET_XMLVALUE_NUM(_variable, _tagname, _parentElement)						\
{																					\
	MXmlElement* _pDataElement = _parentElement->FirstChildElement(_tagname);		\
	if (_pDataElement) _Attribute(&_variable, _pDataElement, "value");				\
}	

bool XCONST::Load(const char* szFileName)
{
	MXml xml;

	if (!xml.LoadFile(szFileName)) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(CONST_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), CONST_XML_TAG_CONST))
		{
			_SET_XMLVALUE_NUM(XCONST::CAMERA_FOV,				"CAMERA_FOV",				pElement);
			_SET_XMLVALUE_NUM(XCONST::CAMERA_NEAR_Z,			"CAMERA_NEAR_Z",			pElement);
			_SET_XMLVALUE_NUM(XCONST::CAMERA_FAR_Z,				"CAMERA_FAR_Z",				pElement);
			_SET_XMLVALUE_NUM(XCONST::GRASS_LOD_NEAR,			"GRASS_LOD_NEAR",			pElement);
			_SET_XMLVALUE_NUM(XCONST::GRASS_LOD_FAR,			"GRASS_LOD_FAR",			pElement);
			_SET_XMLVALUE_NUM(XCONST::TREE_LOD_NEAR,			"TREE_LOD_NEAR",			pElement);
			_SET_XMLVALUE_NUM(XCONST::TREE_LOD_FAR,				"TREE_LOD_FAR",				pElement);
		}
		else if (!_stricmp(pElement->Value(), CONST_XML_TAG_CONST_GAME))
		{
			_SET_XMLVALUE_NUM(XCONST::GAME_CAMERA_FOV,			"CAMERA_FOV",				pElement);
			_SET_XMLVALUE_NUM(XCONST::GAME_CAMERA_NEAR_Z,		"CAMERA_NEAR_Z",			pElement);
			_SET_XMLVALUE_NUM(XCONST::GAME_CAMERA_FAR_Z,		"CAMERA_FAR_Z",				pElement);
			_SET_XMLVALUE_NUM(XCONST::GAME_GRASS_LOD_NEAR,		"GRASS_LOD_NEAR",			pElement);
			_SET_XMLVALUE_NUM(XCONST::GAME_GRASS_LOD_FAR,		"GRASS_LOD_FAR",			pElement);
			_SET_XMLVALUE_NUM(XCONST::GAME_TREE_LOD_NEAR,		"TREE_LOD_NEAR",			pElement);
			_SET_XMLVALUE_NUM(XCONST::GAME_TREE_LOD_FAR,		"TREE_LOD_FAR",				pElement);
		}
	}

	return true;
}