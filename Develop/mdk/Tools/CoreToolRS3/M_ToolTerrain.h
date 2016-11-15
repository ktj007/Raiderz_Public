
/**
 * RealSpace3의 RTerrain을 상속받아, 툴에서 사용할 수 있도록 만든 클래스
 */
	
#pragma once

#include "RTerrain.h"
#include "RDirectionalLightSceneNode.h"
#include "RD3DVertexUtil.h"

#include "C_ControlSceneProperty.h"

namespace rs3 {

class RTreeInstance;
class RFont;
class CORETOOL_API RToolTerrain : public RTerrain
{
	MDeclareRTTI;
protected:
	// 그리드를 그릴 때 사용하는 텍스쳐
	RTexture* m_hGridTexture;
	RTexture* m_hMarkingTexture;
	RTexture* m_hNoneTexture;

	 // Alpha - Mask 맵의 상태 정보를 저장하는 구조체
	struct LayerInfoAttend
	{
		bool bMapChanged;

		LayerInfoAttend()
			: bMapChanged(true)
		{
		}
	};
	
	// Displacement-Map에 대한 논리적 변경에 대한 Flag
	bool m_bDisplacementMapChanged;

	// Alpha Map 정보를 담고 있는 배열
	LayerInfoAttend *m_pAttendInfo;
	int m_nTotalAlphaTextureNum;

	HVERTEXBUFFER m_hGuideVertex;
	RTexture *m_hMaterialMap;
	DWORD *m_pMaterialMap;

	bool m_bObjectLight;

	/**
	 * LightMap 정보(m_pLightMap)을 Alpha-Mask 맵에 합친다.
	 *
	 * @param	nIndex	Alpha-Mask 맵 레이어 인덱스
	 */
	template< typename T_ContainerToPick >
	bool NodeContainerPicking(T_ContainerToPick &nodeContainer, RVector &orign, RVector &to, RPICKINFO *pInfo);

	/**
	 * Grid를 그릴 때 사용되는 Texture를 생성한다.
	 *
	 * @return  성공하면 true, 실패하면 false
	 */
	bool CreateUtlityTexture();
	/**
	 * 해당 UV 영역에 Alpha Pixel(RGBA 32bit format)이 있는지 검사한다.
	 * @return	한 픽셀이라도 ref보다 작은 값이면 false, 그렇지 않으면 true
	 */
	bool IsAlphaPixel(int nIndex, float left, float top, float right, float bottom, long ref = 0xff);
	/**
	 * 해당 UV 영역에서 가장 비중을 많이 차지하는 Alpha Mask의 ID
	 * @return	Mask의 ID, 없으면 -1이 된다.
	 */
	int	ActuallyAlphaMask(float left, float top, float right, float bottom);

public:
	void SetBaseMapName();

protected:
	//영역에 Terrain 변경작업 수행
	void ApplyTerrainVertex_(RECT *pRect = NULL);

	/**
	 * AlphaMaskMap을 Resize하는 부분. index와 Resize 배율을 입력받는다.
	 * @param	nIndex	Alpha MaskMap의 인덱스
	 * @param	nPower	배율
	 * @param	bMultiply	true면 nPower만큼 곱하고, false면 nPower만큼 나눈다.
	 * @return	Resize 조건이 맞아서 하게되면 true, 아무 것도 하지 않으면 false를 리턴한다.
	 */
	bool ResizeLayerAlpha(int nIndex ,int nPower, bool bMultiply = true);
	bool FloodFillMapValidMaker(int nX, int nY, float fSlopeDot);
	void UpdateMaterialMap();

	///	DisplacementMap, AlphaMaskMap, LightMap, GrassFile 을 저장한다.
	class CreateName
	{
	public:
		static std::string DisplacementMapFile(RResource *pRes);
		static std::string BaseMapFile(RResource *pRes);
		static std::string GrassFile(RResource *pRes);
		static std::string AlphaLayerFile(RResource *pRes, int nIndex);
		static std::string LightMapFile(RResource *pRes, int nIndex);
	};

	void SaveDataFiles();
	
	bool AddLayer(RTexture *pTileTexture, RTexture* pNormalTexture, bool bFillAlpha);
public:
	static DWORD GRID_TEXTURE_COLOR;
	static DWORD MARKING_TEXTURE_COLOR;

	/// Seed Position에 해당하지 않는 곳은 무효화 처리한다.
	void UpdateDescriptMapMakedFromSeed(const MVector3 seedPosition, float fSlopeDot);
	void UpdateDescriptMapMakedAll(bool bFlag);
	void UpdateDescriptMapMakedBySlope(float fSlopeDot);
	/// Splatting mask-map(모든 layer)을 참조해서 displacement map - desc 배열에 마크한다.
	/// Alpha Splatting Mask의 데이터를 참조해서 Material ID를 기록한다.
	void UpdateDescriptMapAndMaterial(RECT rect);
	/// 지형을 검사하여, 물밑인지 아닌지 검사한다.
	void UpdateDescriptMapUnderWater();

	/// 상속. 초기화 처리 문제 때문에 ...
	virtual bool CreateFromXML(MXmlElement &element, bool bBackgroundCreation);

	// 불필요한 Alpha mask 의 pixel을 삭제한다.
	int RebuildAlphaMask();

	bool AddLayer(const char *pTileName, bool bFillAlpha = false);
	void DeleteLayer(int nIndex);
	bool DeleteNormalMap(int nIndex);
	bool ChangeTileMap(int nIndex, const char *pTileName);
	bool ChangeNormalMap(int nIndex, const char *pTileName);
	bool SwapLayer(int nIndex1, int nIndex2);
	void CreateAlphaTexture(int nIndex, bool bFillAlpha = false, DWORD dwWidth = 0, DWORD dwHeight = 0);
	void UpdateAlphaTexture(int nIndex);
	void CountAlphaTexture();

	bool ResizeLayerAlphaMul2X(int nIndex);
	bool ResizeLayerAlphaDiv2X(int nIndex);

	void ReloadTexture();

	void SetTiling(int nIndex, rs3::RVector &uvw);



	virtual bool LoadDisplacementmapFromR32File(bool _bMainThreadCall, const char *filename, int nPatchSize, int nLODLevel, float fXSize, float fYSize
		, float fZScale /* = 1.0f */, float fZOffset);

	virtual void CreateLayer(int nNum, int nPatchNum);

	bool CreateNewTerrain(const char *pPath, const char *pResourceName, const char *pBaseTileName, int nWidth, int nHeight, float fXSize, float fYSize);
	bool UpdateNormal(int nWidth, int nHeight, float fXSize, float fYSize);
	bool CreateDisplacementmapPlane(int nWidth, int nHeight, float fXSize, float fYSize);
	bool SaveDisplacementmapToFile();
	bool SaveAlphaMapToFile();
	bool RecreateAndSaveBakedMap(); // baked map 새로 생성하고 저장

	bool CreateGrass();

	void CreateGuideVertex();

	int RenderGuideMap();
	int RenderPatchInfo(RFont* pFont);
	
	virtual void RenderMStage( RCameraSceneNode* pCurrentCamera,RRENDERINGMETHOD renderMethod) override;
	virtual void OnUpdate() override;

	RToolTerrain(RDevice *pDevice);
	virtual ~RToolTerrain();

	// .obj 파일 Export/import
	bool ExportObjFile(int nLevel, int nWidthPartition, int nHeightPartition);
	bool ImportObjFile(const char *filename, std::string *pErrMessage);

	virtual void SaveToXML(MXmlElement &element);

	void AllChangedFlag(bool bFlag);

	// baseMap
	bool SaveBaseMapToFile();

	// grassMap
	void UpdateGrassOnTerrain();
	bool SaveGrassToFile();

	void ApplyCircleActionWithFuncPtr (
		const rs3::RVector* pVecCenterPos,					//편집할 Terrain Circle Area의 중심점
		float fSize, float fScale, float fFactor,			//Circle 영역 크기, 행동의 세기, 행동의 팩터
		float (*funcPtrAction)(float, float, float, float),	//지형 편집 행동의 종류(함수 포인터)
		bool bTerrainVertexUpdate = true);					//Terrain Vertex Update 여부

	void ApplyRelaxAction(float _fStrength,				//세기
		float _fSizeFactor = 0.0f, RECT* pRect = NULL);	//rect영역 사이즈의 팩터 및 rect 영역. NULL 이면 전체

	// TOOL 에서 사용하는 Picking
	bool GetPickLine(RVector &rOut, RVector *pNormal, const RVector &p, const RVector &q);

	// static serialize function
	static bool GetAllWaterPlane(RSceneNode *pSceneNode);
};

template< typename T_ContainerToPick >
inline bool RToolTerrain::NodeContainerPicking(T_ContainerToPick &nodeContainer, RVector &orign, RVector &to, RPICKINFO *pInfo)
{
	pInfo->inPickOrigin = orign;
	pInfo->inPickTo = to;	// by dubble. pick 호출 방법이 바뀌었습니다.

	for ( T_ContainerToPick::iterator it = nodeContainer.begin(); it != nodeContainer.end(); ++it )
	{
		RSceneNode *pNode = (RSceneNode*)(*it);
		if ( pNode->Pick(*pInfo))
			return true;
	}
	return false;
}

}