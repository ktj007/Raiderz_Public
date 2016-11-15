#ifndef _LIGHTLIST_H
#define _LIGHTLIST_H

#include <string>
#include <list>

#include "RTypes.h"
#include "MXml.h"

using namespace std;

namespace rs3 {

// light 플래그들
#define RL_FLAG_CASTSHADOW		0x0010

class MCLIGHT
{
public:
	LightType lightType;
	string	ParentName;
	string	Name;
	RVector Color;
	RVector Position;
	RVector Direction;
	RVector Up;
	float	fIntensity;
	float	fAttnStart,fAttnEnd;
	DWORD	dwFlags;
	bool	bStatic;
	float	fFalloff;

	list<string>	m_userPropertyString;	// Light 에도 Property 가 필요하다.,

	void CalcName2Type(INode* node);
	void ExportUserProperty(MXmlElement *pElement);
};

class LightList : public list<MCLIGHT*> {
public:
	virtual ~LightList();

	bool Save(MXmlElement *pElement, bool bStaticMesh);
};

}


#endif
