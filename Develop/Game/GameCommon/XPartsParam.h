#pragma once

#include "XPartsSlotType.h"

class XPartsParam
{
protected:
	void Clear()
	{
		nPartsID = 0;
		strMeshName = _T("");
		pItemData = NULL;
		vecParts.clear();
	}
public:
	int nPartsID;
	tstring strMeshName;
	CSItemData* pItemData;

	struct Part
	{
		tstring	strNodeName;
		tstring	strParentName;
		tstring	strActorNodeName;
		DWORD	dwPartsColor;
		Part() : dwPartsColor(0xFFFFFFFF) {}
	};
	vector<Part>	vecParts;

	XPartsParam() 
	{
		Clear();
	}

	void AddPart(tstring& nodeName, tstring& parentName, tstring& actorNodeName, unsigned int nColor=0xFFFFFFFF)
	{
		Part part;
		part.strNodeName = nodeName;
		part.strParentName = parentName;
		part.strActorNodeName = actorNodeName;
		part.dwPartsColor = nColor;

		vecParts.push_back(part);
	}

	void Make( int partsID, const TCHAR* meshName, CSItemData* itemData	)
	{
		Clear();
		nPartsID = partsID;
		strMeshName = meshName ? meshName : _T("");
		pItemData = itemData;
	}

	void Make( int partsID, const TCHAR* meshName, CSItemData* itemData, const TCHAR* nodeName, const TCHAR* parentName=NULL, const TCHAR* actorNodeName=NULL, const DWORD color=0xFFFFFFFF )
	{
		vector<Part> parts;

		Part part;

		if (nodeName)
		{
			part.strNodeName = nodeName;
			if (parentName)
				part.strParentName = parentName;
			if (actorNodeName)
				part.strActorNodeName = actorNodeName;

			part.dwPartsColor = color;
			parts.push_back(part);
		}

		Make(partsID,
			meshName,
			itemData,
			parts);
	}

	void Make( int partsID, const TCHAR* meshName, CSItemData* itemData, vector<Part>& parts )
	{
		Clear();
		nPartsID = partsID;
		strMeshName = meshName ? meshName : _T("");
		pItemData = itemData;

		vecParts = parts;
	}

	void MakeForDefault(int partsID, const TCHAR* meshName, vector<Part>& parts)
	{
		Clear();
		nPartsID = partsID;
		strMeshName = meshName ? meshName : _T("");
		vecParts = parts;
	}

};

class XPlayerPartsParam : public XPartsParam
{
private:
public:
	XPlayerPartsParam() : XPartsParam() {}

	void MakeForHair(const TCHAR* meshName, const TCHAR* nodeName, DWORD color)
	{
		Clear();
		nPartsID = PLAYER_PARTS_SLOT_HAIR;
		strMeshName = meshName ? meshName : _T("");

		if (nodeName)
		{
			Part part;
			part.strNodeName = nodeName;
			part.dwPartsColor = color;
			vecParts.push_back(part);
		}
	}
	void MakeForFace(const TCHAR* meshName, const TCHAR* nodeName, DWORD color)
	{
		Clear();
		nPartsID = PLAYER_PARTS_SLOT_FACE;
		strMeshName = meshName ? meshName : _T("");

		if (nodeName)
		{
			Part part;
			part.strNodeName = nodeName;
			part.dwPartsColor = color;
			vecParts.push_back(part);
		}
	}
	void MakeForQuiver(const TCHAR* meshName, const TCHAR* nodeName, const TCHAR* parentName)
	{
		Clear();
		nPartsID = PLAYER_PARTS_SLOT_QUIVER;
		strMeshName = meshName ? meshName : _T("");

		if (nodeName)
		{
			Part part;
			part.strNodeName = nodeName;

			if (parentName)
				part.strNodeName = parentName;

			vecParts.push_back(part);
		}
	}
};
