#pragma once

#include <vector>
#include <list>
#include <set>

#include "MCAnimationKey.h"
#include "RMaterial.h"
#include "RTTGroup/IRTTGroup.h"

using namespace std;

class Mtl;
class INode;
class RMaxMaterialKeeper;
class Matrix3;

class MCplug2;

class TiXmlElement;
typedef TiXmlElement		MXmlElement;

class IPropertyContainer;

namespace rs3 {
class RMaterial;
}

using namespace rs3;

class RMaxMaterial
{
	friend RMaxMaterialKeeper;

	Mtl		*m_pMtl;
	bool	m_bUsed;
	int		m_nIndex;
	
	int		m_nSubMtrlID;

	RMaxMaterial	*m_pParent;
	RMaxMaterial	*m_pBaseMaterial;
	vector<RMaxMaterial*>	m_children;

	RMaterial	*m_pRMaterial;

	// u,v animation
	bool	m_bOffsetAni;
	bool	m_bScaleAni;
	bool	m_bRotateAni;

	// u,v animation
	MCKeyTrack<MCVec3Key> m_trackOffset;
	MCKeyTrack<MCVec3Key> m_trackScale;
	MCKeyTrack<MCVec3Key> m_trackRotate;

	int		m_nUsingUVChannelNum;	// 사용하는 UV채널 개수

public:
	RMaxMaterial(void);
	~RMaxMaterial(void);

	RMaxMaterial* GetMaterialByID(int nID);
	size_t GetChildMaterialCount();

	int	GetIndex() { return m_nIndex; }

	RMaxMaterial* GetBaseMaterial() { return m_pBaseMaterial; }
	RMaterial* GetRMaterial() { return m_pRMaterial; }

	void UpdateUVChannelNum( int nUVChannelIndex )
	{
		if( m_nUsingUVChannelNum < nUVChannelIndex )
			m_nUsingUVChannelNum = nUVChannelIndex;
	}
	int GetUsingUVChannelNum() { return m_nUsingUVChannelNum; }

	Matrix3*	m_uvTransform;	// diffuse의 uv transform, 컴파일안되는문제때문에 포인터로 선언 -_-
};

inline size_t RMaxMaterial::GetChildMaterialCount() { return m_children.size(); }

class RMaxMaterialKeeper
{
	struct LightMap
	{
		string strLightMapName;
		string strUserProperty;
		IRTTGroup*	pRTTGroup;
	};

	vector<RMaxMaterial*>	m_materials;
	vector<RMaxMaterial*>	m_clearedMaterials; // 사용하지 않거나 빈 마테리얼이라서 제거된 마테리얼들. 소멸자에서 완전히 지운다.
	vector<LightMap>		m_defaultLightMaps;

	bool CheckForAnimation(Texmap *tex, bool &bOffset, bool &bScale, bool &bRotate);
	RMaxMaterial* AddMtl(Mtl *mtl,RMaxMaterial *pParent = NULL, int nSubMtrlID = -1);

	void GetMaterialListRecurse(INode* node, MCplug2* pExporter);
	void GetSubMaterial(RMaxMaterial *material, bool bSetForceUsed = false);
	void GetTextureMap(RMtrlTexInfo* pTexInfo, Texmap* tex, int subNo);
	
	TimeValue m_timeStart;
	TimeValue m_timeEnd;
	TimeValue m_timeDelta;

public:
	RMaxMaterialKeeper(void);
	~RMaxMaterialKeeper(void);

	void GetMaterialList(INode*	node, MCplug2* pExporter );
	void AddSubMtl(Mtl *mtl,RMaxMaterial *pParent = NULL, bool bSetForceUsed = false);	

	bool CompareTexture(Mtl *pMtl1, Mtl *pMtl2, int nID);
	bool Compare(RMaxMaterial *pMaxMtl1, RMaxMaterial *pMaxMtl2);
	RMaxMaterial* GetExistMaterial(RMaxMaterial *pMaxMaterial);
	RMaxMaterial* GetMaterial(Mtl *mtl);

	bool IsExistMaterial(RMaxMaterial *pMaxMaterial);

	void ClearVoidMaterial();

	/// dds 로 변환
	bool ExportTextures(/*BOOL bUsePartsColor,*/ BOOL bUseDDS, BOOL bExportTexture, 
						D3DFORMAT plainFormat, D3DFORMAT alphaFormat, const string& targetPath );
	void ExportXML(MXmlElement *pElement);

	int GetCount() { return (int)m_materials.size(); }

	void SetStartTime(TimeValue t);
	void SetEndTime(TimeValue t);
	void SetDeltaTime(TimeValue t);

	int AddLightMap(const string& lightMapName, const string& strProperty, IRTTGroup* pRTTGroup);

	RMaxMaterial* GetMaterial(int i) { return m_materials[i]; }

private:
	const TCHAR* GetTextureName(Texmap* tex);
	void GatherTexture(RMaxMaterial *material, vector<Texmap*>& Textures, vector<int>& TexturesIDs, vector<Texmap*>& CompositeTextures, vector<int>& CompositeTextureIDs);
	void AddCompositeMap(rs3::RMaterial* m, vector<Texmap*>& CompositeTextures, vector<int>& CompositeTextureIDs);
	bool GetCompositeLayerInfo(rs3::RMaterial* m, Texmap* tex, RTextureType type, vector<COMPOSITE_LAYER_INFO>& OutLayer);
	RTextureType ConvertTextureTyepFromMaxToRS3(int MaxType);
	bool GetPropertyParamBlock(IPropertyContainer* IProperties, const TCHAR* name, IParamBlock2*& pOutParamBlock, ParamID& Outid);
};

//////////////////////////////////////////////////////////////////////////

inline void RMaxMaterialKeeper::SetStartTime(TimeValue t)
{
	m_timeStart = t;
}

inline void RMaxMaterialKeeper::SetEndTime(TimeValue t)
{
	m_timeEnd = t;
}

inline void RMaxMaterialKeeper::SetDeltaTime(TimeValue t)
{
	m_timeDelta = t;
}