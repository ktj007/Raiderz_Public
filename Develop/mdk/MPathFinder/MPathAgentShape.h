#pragma once

#include <vector>
#include "../../../sdk/pathengine/i_pathengine.h"

// iShape를 랩핑한 클래스

/**
	Any shapes not deleted get cleaned up automatically when PathEngine is shut down. 
*/
enum MAGENTSHAPE
{
	MSHAPE_SMALL = 0,
	MSHAPE_MEDIUM,
	MSHAPE_LARGE,
	MSHAPE_UNKNOWN
};

struct SIntVector2
{
	int x,y;

	SIntVector2() : x(0), y(0) {}
	SIntVector2(int _x, int _y) : x(_x), y(_y) {}
	SIntVector2(float _x, float _y) : x((int)_x), y((int)_y) {}
};


class MPathAgentShape
{
public:
		iShape*						m_pShape;
		int							m_nSize;

		// 한변의 길이가 nSize인 정사각형 Shape를 만든다.
		void						CreateSquareShape(iPathEngine* pEngine, int nSize);
		void						Destroy() { if (m_pShape != NULL) delete m_pShape; m_pShape = NULL; }

		MPathAgentShape() : m_pShape(NULL), m_nSize(0) {}
		~MPathAgentShape() {}

		static	iShape* CreateSquareShape(iPathEngine* pEngine, int nWidth, int nHeight);
};


class MPathAgentShapeSet
{
public:
		vector<MPathAgentShape>		m_vShapes;
		vector<string>				m_vName;

		void						AddShape(const MPathAgentShape& shape, const string& strName);
		void						CreateDefaultShapes(iPathEngine* pEngine);

		MPathAgentShape*			GetShape(const string& strName);
		MPathAgentShape*			GetDefaultShape();

		void						Clear();
};
