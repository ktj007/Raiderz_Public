#pragma once

#include <vector>
#include "MVector2.h"
#include "MSphere.h"

/// 기본적인 높이 필드 속성을 담고 있는 클래스
class RHeightField
{
public:
	enum TERRAIN_DESC_BOOLEAN_ATTRIB	// 32bit signed
	{
		NONE = 0x00,
		VALID_POS	= 0x20000000,
		UNDER_WATER = 0x40000000,
		INVALID_POS	= 0x80000000		// 사용할 수 없는 위치값
	};

private:
	// 높이 정보
	float *m_pHeightMap;
	// 기타 정보 (flag 같은 것)
	struct SDesc
	{
		DWORD dwAttribute;
		DWORD dwMaterial;
	};

	SDesc *m_pHeightMapDesc;

	int m_nHeightMapArraySize;
	float m_fMinZ, m_fMaxZ;

	struct SDescHeader
	{
		char pcHeader[4];
		DWORD dwVersion;
	};

	struct SGridInfo
	{
		MBox m_aabb;
		RECT m_kRect;
	};
	/// 전처리를 위한 GridMap ( AABB로 구성)
	SGridInfo *m_pGridMap;

	/// GridMap 의 가로, 세로 칸 수
	int m_nGridWidth;
	int m_nGridHeight;
	/// GridMap의 한칸은 몇칸의 HeightFieldMap으로 되어있는가 
	int m_nGridSize;

	/// HeightMap에 대한 정보 ( Pitch는 각 라인 크기, Map은 칸(점이 아닌) 개수
	int m_nHeightMapPitch;
	int m_nWidthMap;
	int m_nHeightMap;

	/// 한 칸이 차지하는 x, y 크기
	MVector2 m_vScale;
	/// 전체 AABB 상자
	MBox m_aabb;

	void UpdateHeightField();
	void GetGridAABB(MBox *pAABB, const RECT *pRect);

	/// param nGridSize	그리드 한 칸의 크기
	void CreateGridInfo(int nGridSize);
	
	void CreateDesc();

	/// return vP에서 피킹된 지점까지의 거리를 구한다. pIndex*은 피킹된 삼각형의 인덱스
	float GetPickLineRect(MVector3 *pOut, const MVector3 &vP, const MVector3 &vQ, const MVector3 &vDir, const RECT *pRect
		, MVector3 *pTriangle, TERRAIN_DESC_BOOLEAN_ATTRIB NonPickingAttrib);

	/// 최적화 용도, 투영직선에 해당하는 Grid를 추출해낸다. P1, P2는 반드시 클리핑 되어 있어야 한다.
	void GetLineIntersectGrid(std::vector<std::pair<int, int> > &vecArray, const MVector2 &P1, const MVector2 &P2, float fXScale, float fYScale);

	static std::vector<std::pair<int, int> > m_vecGridArray;
	static std::vector<std::pair<int, int> > m_vecTriangleArray;

	static int GRID_SIZE;
	static int DESC_VERSION;

	/// dds fp32 포맷 읽는 부분 ( 높이맵 전용 포맷 )
	float*	LoadR32FDDSFromFile(const char *filename, int *outWidth, int *outHeight, float fZScale, float fZOffset);

protected:
public:
	RHeightField();
	virtual ~RHeightField();

	bool LoadHeightFieldDescFile(const char *filename);
	void SaveHeightFieldDescFile(const char *filename);

	bool LoadHeightFieldmapFromR32F(const char *filename, const MVector2 &vSize);
	void SaveHeightFieldmapToR32F(const char *filename);

	bool Create(int nImageWidth, int nImageHeight, MVector2 &vSize);

	void GetPositionAtIndexXY(MVector3 *pOut, int w, int h);
	bool GetHeightAt(float *height, MVector3 *pNormal, float x, float y);
	bool GetHeightAtWithHole(float *height, MVector3 *pNormal, float x, float y); // Hole인 부분의 지역도 높이 값을 가져온다.

	// 여기서 x, y는 displacement map의 x,y 인덱스이다. ( x : 0 ~ GetWidthMap(), y : 0 ~ GetHeightMap() )
	float GetHeightAtXYIndex(int x, int y);

	/**
	 @param		x	x 좌표[0-GetWidthSize)
	 @param		y	y 좌표[0-GetHeightSize)
	 @param		param	속성(bit flag의 위치)
	 @param		bFlag	속성을 true로 할 것인지, falsg로 할 것인지
	*/
	void SetAttribute(int nx, int ny, TERRAIN_DESC_BOOLEAN_ATTRIB attrib, bool bFlag);
	void SetMaterial(int nx, int ny, DWORD dwID);
	/**
	@param		x	x 좌표[0-GetWidthSize)
	@param		y	y 좌표[0-GetHeightSize)
	@param		param	속성(bit flag의 위치)
	@return		속성값이 true인지, false인지
	*/
	bool GetAttribute(float x, float y, TERRAIN_DESC_BOOLEAN_ATTRIB attrib);
	bool GetAttribute(int x, int y, TERRAIN_DESC_BOOLEAN_ATTRIB attrib);

	/**
	 	사용자 재질
	 */
	DWORD GetMaterial(int x, int y);
	DWORD GetMaterial(float x, float y);

	void ClearDescriptMap();

	void UpdateAABB();
	bool GetPickLine(MVector3 *pOut, MVector3 *pOutNormal, float *pPickDistance, const MVector3 &vP, const MVector3 &vQ, TERRAIN_DESC_BOOLEAN_ATTRIB NonPickingAttrib = INVALID_POS);

	/**
	@brief		AABB 와 충돌 검사
	@param		bbox	AABB 구조체
	@return		충돌 했으면 true, 그렇지 않으면 false를 리턴한다
	*/
	bool IsIntersect(const MBox &bbox);

	/// 직접 HeightField 포인터를 가져온다. 변경후에는 UpdateAABB 반드시 호출 해 줄 것
	inline float* GetPtr();

	/// GetScale().x, GetScale().y 와 동일
	inline float GetXScale();
	inline float GetYScale();

	/// GetAABB의 vmin.z, vmax.z 와 동일
	inline float GetMinZ();
	inline float GetMaxZ();

	/// 가로, 세로 실제 차지하는 크기
	inline float GetWidthSize();
	inline float GetHeightSize();

	inline int GetWidthMap();
	inline int GetHeightMap();
	inline int GetHeightMapPitch();

	inline const MBox& GetAABB();
	inline const MVector2& GetScale();
};

float* RHeightField::GetPtr()
{
	return m_pHeightMap;
}

float RHeightField::GetXScale()
{
	return m_vScale.x;
}

float RHeightField::GetYScale()
{
	return m_vScale.y;
}

float RHeightField::GetMinZ()
{
	return m_fMinZ;
}

float RHeightField::GetMaxZ()
{
	return m_fMaxZ;
}

int RHeightField::GetWidthMap()
{
	return m_nWidthMap;
}

int RHeightField::GetHeightMap()
{
	return m_nHeightMap;
}

int RHeightField::GetHeightMapPitch()
{
	return m_nHeightMapPitch;
}

const MBox& RHeightField::GetAABB()
{
	return m_aabb;
}

const MVector2& RHeightField::GetScale()
{
	return m_vScale;
}

float RHeightField::GetWidthSize()
{
	return ( m_vScale.x * m_nWidthMap );
}

float RHeightField::GetHeightSize()
{
	return ( m_vScale.y * m_nHeightMap );
}
