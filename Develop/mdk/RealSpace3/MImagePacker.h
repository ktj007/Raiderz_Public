#pragma once

#include "MemPool.h"

//////////////////////////////////////////////////////////////////////////
// MImagePackerNode
// 2D 이미지를 하나로 합치기 위한 클래스
struct MImagePackerNode
{
public:
	// 가상의 이미지. 크기만 가지고있다
	struct MImage
	{
		int nID;
		int nWidth;
		int nHeight;

		bool Fit(const MRect& targetRc) const
		{
			if(targetRc.GetWidth()<nWidth) return false;
			if(targetRc.GetHeight()<nHeight) return false;
			return true;
		}
		bool FitPerfectly(const MRect& targetRc) const
		{
			if(targetRc.GetWidth()==nWidth &&
				targetRc.GetHeight()==nHeight) return true;
			return false;
		}
	};

	// 순회 방문자
	struct MImagePackerNodeVisitor
	{
		virtual void Visit( MImage& _Image, const MRect& _ImageRange ) = 0;
	};

protected:
	MRect				rc;
	MImage				m_img;	// m_img.nID 가 0 이면 비어있는 공간이다
	int					m_nAvailableSpace;	///< 사용가능한 픽셀 수
	int					m_nMaxWidth;		///< 사용가능한 블럭들 중 최대 너비
	int					m_nMaxHeight;		///< 사용가능한 블럭들 중 최대 높이
	
public:
	MImagePackerNode(const MRect& _rc);
	virtual ~MImagePackerNode();

	MImagePackerNode*	parent;
	MImagePackerNode*	child[2];

	int					GetWidth() const { return rc.GetWidth(); }
	int					GetHeight() const { return rc.GetHeight(); }

	int					GetTop() const { return rc.top; }
	int					GetLeft() const { return rc.left; }
	int					GetRight() const { return rc.right; }
	int					GetBottom() const { return rc.bottom; }

	int					GetImageID() const { return m_img.nID; }

	void				UpdateAvailable();
	int					GetAvailableSpace() { return m_nAvailableSpace; }

	/// 이미지를 삽입한다, 공간이 없으면 NULL 을 리턴한다.
	MImagePackerNode*	Insert(const MImage& img);
	
	/// 이미지를 삭제한다. 넣은 역순으로 삭제할때만 동작을 보장한다.
	void				Remove(MImagePackerNode* pNode);

	/// 자식 노드들을 순회한다.
	void				Traverse( MImagePackerNodeVisitor* pVisitor );
};

//////////////////////////////////////////////////////////////////////////
// 간단한 사용법
/*

	// 전체 노드를 하나 만든다
	MImagePackerNode aRootNode(MRect(0,0,256,256));

	for(int i=0;i< nImages ; i++)
	{
		MImagePackerNode::MImage img;
		img.nID = i+1;	// 0 은 안됨
		img.nWidth = nWidth;	// i번째 이미지 너비
		img.nHeight = nHeight;	// i번째 이미지 높이

		MImagePackerNode* pNode = aRootNode.Insert(img);
		if(!pNode) {
			// 실패 : 용량이 모자람
			return false;
		}
	}

	// 성공. 트리로 구성되어있다. 적당히 가공하여 저장한다
*/