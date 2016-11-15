#pragma once

#include <map>
#include <string>

#include "RParticlePool.h"
#include "RVType.h"

namespace rs3 {

class REffectSceneNode;

class REffectSharedBuffer
{
private:
	HVERTEXBUFFER		m_hVertexBuffer;
	unsigned int		m_nVertexCount;
	PVERTEXFORMAT*		m_pVertexArray;
	int					m_nVertexWriteCount;
	int					m_nVertexBufferSize;
	int					m_nNextVertexData;		//D3DLOCK_NOOVERWRITE 사용시에 gpu가 사용하고 있지 않는 버퍼의 메모리영역 시작주소
	int					m_nCurrentVertexData;	//현재 락을 걸어서 사용한 버퍼의 주소. 렌더링시에 사용됨


	HINDEXBUFFER		m_hIndexBuffer;
	unsigned int		m_nIndexCount;
	WORD*				m_pIndexArray;
	int					m_nIndexWriteCount;

public:
	REffectSharedBuffer( int nVertexCount, int nIndexCount );
	~REffectSharedBuffer();

	PVERTEXFORMAT*		GetVertexArray( unsigned int nCount );
	WORD*				GetIndexArray( unsigned int nCount );

	bool	WriteVertex( unsigned int nCount );
	bool	WriteIndex( unsigned int nCount );

	int		DrawPrimitiveTriangleList();
	int		DrawIndexedPrimitiveTriangleStrip();

private:
	void	ReCreateVertexBuffer();
	void	ReCreateIndexBuffer();

	void	DeleteVertexBuffer();
	void	DeleteIndexBuffer();
};

class RS_API REffectManager
{
	typedef std::pair<int, REffectSceneNode*> REF_EFFECT_SRC;
	typedef std::map<std::string, REF_EFFECT_SRC> EFFECT_SRC_MAP;
	EFFECT_SRC_MAP					m_mapEffectSources;
	static RParticlePool*			m_pParticlePool;
	static REffectSharedBuffer*		m_pSharedBuffer;

	static float					m_fCreateParticleRange; ///< 카메라 위치 기준 파티클 생성 범위

public:
	REffectManager();
	~REffectManager();

	void				Destroy();

	// 미리 캐싱해두기 위해..
	REffectSceneNode*	CreateEffectSource(const char* szFileName, bool bBackgroundCreation, bool bAddInfiniteEffectSource = true);
	int					AddRefEffectSource(const std::string& _strName, REffectSceneNode* _pEffectSceneNode);
	void				RemoveRefEffectSource(const std::string& _strName, REffectSceneNode* _pNotDeleteLastSrc_If_EqualToThis = NULL);

	// 파일이 바껴서 게임도중에 새로 읽기 위해서 캐시된 이펙트를 지워주는 용도로 사용한다.
	void				ClearEffectSource();

	/// 새 REffectSceneNode 를 생성한다. 생성된 노드는 호출한 쪽에서 delete 해주어야 한다
	REffectSceneNode*	CreateEffect(const char* szFileName, bool bBackgroundCreation = false);
	REffectSceneNode*	GetEffect(const char* szFileName);

	/// 이펙트를 복사한다.
	RSceneNode*			CloneEffect(RSceneNode* pSource);

	static void			SetCreateParticleRange( float fRange )	{ m_fCreateParticleRange = fRange; }
	static float		GetCreateParticleRange()				{ return m_fCreateParticleRange; }

	/// 파티클 Pool 인터페이스
	static RParticle*	GetParticle(unsigned int nHandle)		{ return m_pParticlePool->Get(nHandle); }
	static unsigned int	GetNewParticle()						{ return m_pParticlePool->GetNew(); }
	static void			RemoveParticle(unsigned int nHandle)	{ m_pParticlePool->Remove(nHandle); }
	static unsigned int	GetParticleCount()						{ return m_pParticlePool->GetCount(); }
	static unsigned int	GetParticleCapacity()					{ return m_pParticlePool->GetCapacity(); }

	// 공용 버퍼 인터페이스
	static PVERTEXFORMAT*		GetVertexArray(unsigned int nCount)		{ return m_pSharedBuffer->GetVertexArray(nCount); }
	static WORD*				GetIndexArray(unsigned int nCount)		{ return m_pSharedBuffer->GetIndexArray(nCount); }
	static bool					WriteVertexBuffer(unsigned int nCount)	{ return m_pSharedBuffer->WriteVertex(nCount); }
	static bool					WriteIndexBuffer(unsigned int nCount)	{ return m_pSharedBuffer->WriteIndex(nCount); }
	static int					DrawPrimitiveTriangleList()				{ return m_pSharedBuffer->DrawPrimitiveTriangleList(); }
	static int					DrawIndexedPrimitiveTriangleStrip()		{ return m_pSharedBuffer->DrawIndexedPrimitiveTriangleStrip(); }

protected:
	void				CloneControllers(RSceneNode* pDestNode, RSceneNode* pSource);
	void				CloneChildren(RSceneNode* pDestNode, RSceneNode* pSource);
};

}
