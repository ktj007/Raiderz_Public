#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <list>
#include <string>

#include "MXml.h"
#include "MMath.h"

using namespace std;

class RMaxMeshNode;

struct EFFECTDESC
{
	RMaxMeshNode* pParentMaxMeshNode;
	string effectFileName;
	string parentNodeName;
	MVector3 position;
	MVector3 direction;
	MVector3 scale;
	bool bStatic;
};

class EffectList : public list< EFFECTDESC* >
{
	
public:
	EffectList(void);
	virtual ~EffectList(void);

	void Add( RMaxMeshNode* pParentMaxMeshNode, const string& strNodeName, const string& strEffectName, bool bStatic, float fScale = 1.0f);	// node 에 붙일때
//	void Add(const string& strEffectName, const MVector3& pos, const MVector3& dir); // 특정 위치에 달릴때
	void Save(MXmlElement* pElement, bool bStatic);

protected:
	string RemoveExtension(const string& strEffectName);
};
