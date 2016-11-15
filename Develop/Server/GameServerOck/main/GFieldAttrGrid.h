#pragma once

class GFieldInfo;

class GFieldAttrGrid
{
public:
	struct Cell
	{
		bool bPvPEnable;
		uint8 nPvPFaction1;
		uint8 nPvPFaction2;

		Cell()
			:bPvPEnable(false)
			,nPvPFaction1(0) 
			,nPvPFaction2(0)
		{}
	};
public:
	GFieldAttrGrid(GFieldInfo* pFieldInfo);
	~GFieldAttrGrid(void);

	// 초기화처리자
	void Init();
	// 소멸처리자
	void Fini();

	// 필드좌표를 그리드좌표로 변경
	MPoint	ConvertToGridPos(float fPosX, float fPosY);
	// 속성셀 반환, 필드 좌표를 통해서
	Cell*	FindCell(float fPosX, float fPosY);
	// 속성셀 반환, 그리드 좌표를 통해서
	Cell*	FindCellViaGridPos(int rx, int ry);
	// 유효한 그리드좌표인지 확인
	bool	IsValidGridPos(int rx, int ry);
	// 특정 부분값을 설정
	bool	SetCellAttr(MRect rect);
	// 사각형 범위에 맞는 속성셀들 반환
	bool	GetCellsViaRect( MRect rect, vector<Cell*>& outCells );
private:
	Cell* m_pCells;
	GFieldInfo*					m_pFieldInfo;
	int							m_nGridXSize;
	int							m_nGridYSize;
};