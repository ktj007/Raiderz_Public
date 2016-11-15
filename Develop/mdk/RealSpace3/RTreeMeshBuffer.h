#pragma once

#include "RDevice.h"
#include "RTypes.h"
#include <vector>

namespace rs3 {


enum EBufferType
{
	BT_INDEXEDSTRIP = 0,
	BT_TRIANGLELIST,
	BT_INDEXEDTRIANGLELIST,
};




class RTreeInstancingBuffer
{
private:
	HVERTEXBUFFER				m_hInstancingBuffer;
	RDevice*					m_pDevice;
	int							m_nCnt;

public:
	RTreeInstancingBuffer() :
	m_hInstancingBuffer(R_NONE), m_pDevice(NULL), m_nCnt(0)
	{
	}
	~RTreeInstancingBuffer()
	{
		Destroy();
	}
					
public:
	void	Destroy()
	{
		if( m_hInstancingBuffer != R_NONE && m_pDevice != NULL)
		{
			m_pDevice->DeleteVertexBuffer(m_hInstancingBuffer);
			m_hInstancingBuffer = R_NONE;
			m_pDevice = NULL;
		}
	}

	void	Create( RDevice* pDevice, int nCnt)
	{
		m_nCnt = nCnt;
		m_pDevice = pDevice;
		if(!m_hInstancingBuffer)
		{
			int nInstancingCnt = m_nCnt;
			int nStructSize = sizeof(float);	// 인덱스만을 가짐.
			int bufferSize = nInstancingCnt * nStructSize;
			// 데이터 채움. 오로지 순차적인 인덱스만 있으면 됨.
			float* pData = new float[ nInstancingCnt];
			for( int i=0; i<nInstancingCnt; i++)
				pData[i] = (float)i;
			// 생성시 인덱스로만 한번만 채워주고 그 후로는 바뀔 일이 없으므로 다이나믹으로 하지 않는다.
			m_hInstancingBuffer = m_pDevice->CreateVertexBuffer( bufferSize, nStructSize, pData);
			// 데이터 지워줌,
			SAFE_DELETE_ARRAY( pData);
		}
	}

	HVERTEXBUFFER	GetBuffer()					{ return m_hInstancingBuffer;	}
	int				GetMaxSize()				{ return m_nCnt;	}
};





template<class VType>
class RTreeMeshBuffer									// 트리에 포함된 메시 한개의 모든 LOD를 한개의 VB에 저장한다.
{
		typedef vector<int>		tmIndexBuffer;
		typedef vector<VType>	tmVertexBuffer;

		struct SMeshInfo
		{
			int					m_nVertexStartOffset;
			int					m_nVertexBufferSize;
			int					m_nIndexStartOffset;
			int					m_nIndexBufferSize;

			SMeshInfo() : m_nVertexStartOffset(0), m_nVertexBufferSize(0), m_nIndexStartOffset(0), m_nIndexBufferSize(0){}
			SMeshInfo(int a, int b, int c, int d) : m_nVertexStartOffset(a), m_nVertexBufferSize(b), m_nIndexStartOffset(c), m_nIndexBufferSize(d) {}
		};

public:
		RDevice*				m_pDevice;

		HVERTEXBUFFER			m_hVertexBuffer;		
		HINDEXBUFFER			m_hIndexBuffer;

		int						m_nVBSize;
		int						m_nIBSize;

		tmVertexBuffer			m_vb;
		tmIndexBuffer			m_ib;
		int						m_nVertexSize;

		vector<SMeshInfo>		m_vMeshInfo;

		EBufferType				m_BufferType;


		RTreeMeshBuffer(RDevice* dev, EBufferType t) : 
		m_pDevice(dev), m_hIndexBuffer(R_NONE), m_hVertexBuffer(R_NONE), m_BufferType(t), m_nVertexSize(sizeof(VType)), m_nVBSize(0), m_nIBSize(0)
		{
		}

		~RTreeMeshBuffer() { Destroy(); }

		void					SetVertexBuffer(const vector<VType>& vb)							// Indexed 모드에서 사용
		{
			if (m_BufferType != BT_INDEXEDSTRIP && m_BufferType != BT_INDEXEDTRIANGLELIST)
			{
				_ASSERT(!"Not Vaild Type!");
				return;
			}

			m_vb = vb;
		}


		void					AddIndex(const vector<int>& ib)										// Indexed 모드에서 사용
		{
			if (m_BufferType != BT_INDEXEDSTRIP && m_BufferType != BT_INDEXEDTRIANGLELIST)
			{
				_ASSERT(!"Not Vaild Type!");
				return;
			}

			if (ib.empty()) return;

			int IndexStartOffset;

			size_t miSize = m_vMeshInfo.size();
			if (miSize == 0)
			{
				IndexStartOffset = 0;
			}
			else
			{
				IndexStartOffset = m_vMeshInfo[miSize-1].m_nIndexStartOffset + m_vMeshInfo[miSize-1].m_nIndexBufferSize;
			}
			m_ib.insert(m_ib.end(), ib.begin(), ib.end());

			m_vMeshInfo.push_back(SMeshInfo(0, 0, IndexStartOffset, ib.size()));
		}


		void					AddTriangleList(const vector<VType>& vb)							// Triangle List 모드에서 사용
		{
			if (m_BufferType != BT_TRIANGLELIST)
			{
				_ASSERT(!"Not Vaild Type!");
				return;
			}

			if (vb.empty()) return;

			int VertexStartOffset;

			size_t miSize = m_vMeshInfo.size();
			if (miSize == 0)
				VertexStartOffset = 0;
			else
				VertexStartOffset = m_vMeshInfo[miSize-1].m_nVertexStartOffset + m_vMeshInfo[miSize-1].m_nVertexBufferSize;

			m_vb.insert(m_vb.end(), vb.begin(), vb.end());

			m_vMeshInfo.push_back(SMeshInfo(VertexStartOffset, vb.size(), 0, 0));
		}


		void					CreateDXBuffers(bool useDynamicVB = false)								// 가지고 있는 데이터로 DX VB/IB 생성
		{
			if (!m_vb.empty())
			{
				if (useDynamicVB)
					m_hVertexBuffer = m_pDevice->CreateVertexBuffer(m_nVertexSize * m_vb.size(), m_nVertexSize, &(m_vb[0]), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY);
				else
					m_hVertexBuffer = m_pDevice->CreateVertexBuffer(m_nVertexSize * m_vb.size(), m_nVertexSize, &(m_vb[0]));
				m_nVBSize = m_vb.size();
			}
			if (!m_ib.empty())
			{
				m_hIndexBuffer = m_pDevice->CreateIndexBuffer(m_ib.size(), RFMT_INDEX32, &(m_ib[0]));
				m_nIBSize = m_ib.size();
			}
		}

		void					ReplaceDXBuffers()
		{
			void* buf = m_pDevice->LockVertexBuffer(m_hVertexBuffer);
			memcpy(buf, &(m_vb[0]), m_nVertexSize * m_vb.size());
			m_pDevice->UnlockVertexBuffer(m_hVertexBuffer);
		}

		void					DestroyTempBuffer()
		{
			m_nVBSize = m_nIBSize = 0;
			m_vMeshInfo.clear();
			m_vb.clear();
			m_ib.clear();
		}

		void					Destroy()															// 데이터 모두 파괴
		{
			if (m_hVertexBuffer != R_NONE)
			{
				m_pDevice->DeleteVertexBuffer(m_hVertexBuffer);
				m_hVertexBuffer = R_NONE;
			}
			if (m_hIndexBuffer != R_NONE)
			{
				m_pDevice->DeleteIndexBuffer(m_hIndexBuffer);
				m_hIndexBuffer = R_NONE;
			}
			DestroyTempBuffer();
		}


		int						Render(int MeshIdx)													// 렌더링~
		{
			RPFC_THISFUNC;
			if ((MeshIdx < 0 ) || (MeshIdx >= (int)m_vMeshInfo.size())) return 0;
			const SMeshInfo meshInfo = m_vMeshInfo[MeshIdx];
			if (m_BufferType == BT_INDEXEDSTRIP)
			{
				m_pDevice->DrawIndexedPrimitive(RPT_TRIANGLESTRIP, 0, 0, (int)m_vb.size(), meshInfo.m_nIndexStartOffset, meshInfo.m_nIndexBufferSize - 2);
				return meshInfo.m_nIndexBufferSize - 2;
			}
			else if (m_BufferType == BT_TRIANGLELIST)
			{
				m_pDevice->DrawPrimitive(RPT_TRIANGLELIST, meshInfo.m_nVertexStartOffset, meshInfo.m_nVertexBufferSize / 3);
				return meshInfo.m_nVertexBufferSize / 3;
			}
			else if (m_BufferType == BT_INDEXEDTRIANGLELIST)
			{
				m_pDevice->DrawIndexedPrimitive(RPT_TRIANGLELIST, 0, 0, (int)m_vb.size(), meshInfo.m_nIndexStartOffset, meshInfo.m_nIndexBufferSize / 3);
				return meshInfo.m_nIndexBufferSize / 3;
			}
			return 0;
		}

		int Render(  RTreeInstancingBuffer* pTreeInstancingBuffer, int nCnt)		// 인스턴싱을 사용하여 랜더링
		{
			// 항상 LOD 0?
			const SMeshInfo meshInfo = m_vMeshInfo[0];
			if (m_BufferType == BT_INDEXEDSTRIP)
			{
				m_pDevice->DrawIndexedPrimitive(RPT_TRIANGLESTRIP, 0, 0, (int)m_vb.size(), meshInfo.m_nIndexStartOffset, meshInfo.m_nIndexBufferSize - 2);
				return meshInfo.m_nIndexBufferSize - 2;
			}
			else if (m_BufferType == BT_TRIANGLELIST)
			{
				m_pDevice->DrawPrimitive(RPT_TRIANGLELIST, meshInfo.m_nVertexStartOffset, meshInfo.m_nVertexBufferSize / 3);
				return meshInfo.m_nVertexBufferSize / 3;
			}
			else if (m_BufferType == BT_INDEXEDTRIANGLELIST)
			{
				m_pDevice->DrawIndexedPrimitive(RPT_TRIANGLELIST, 0, 0, (int)m_vb.size(), meshInfo.m_nIndexStartOffset, meshInfo.m_nIndexBufferSize / 3);
				return meshInfo.m_nIndexBufferSize / 3;
			}
			return 0;
		}


};	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Dynamic VertexBuffer for billboard

template <class T>
class RDynamicVertexBuffer
{
public:

		RDynamicVertexBuffer() : vb(R_NONE), vbSize(0) {}

		bool					Create(int vsz, int vs = DefaultSize)
		{
			vb = REngine::GetDevice().CreateVertexBuffer(vs * vsz, vsz, NULL, D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY);
			vbSize = vs;
			vertexSize = vsz;
			return vb != R_NONE;
		}


		void					Destroy()
		{
			REngine::GetDevice().DeleteVertexBuffer(vb);
		}

		void					AddVertices(T* data, int itemNum)
		{
			VertexData.insert(VertexData.end(), data, data+itemNum);
		}

		void					ClearVertices()
		{
			VertexData.erase(VertexData.begin(), VertexData.end());
		}

		void					ApplyData()
		{
			RPFC_THISFUNC;
			if (vbSize < (int)VertexData.size())
			{
				Destroy();
				Create(vertexSize, VertexData.size() * 2);
			}
			void* dat = REngine::GetDevice().LockVertexBuffer(vb, D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD);
			memcpy(dat, &(VertexData[0]), sizeof(T)*VertexData.size());
			REngine::GetDevice().UnlockVertexBuffer(vb);
		}

		void					SetVertexBufferToDevice()
		{
			RPFC_THISFUNC;
			REngine::GetDevice().SetVertexBuffer(vb, 0, 0);
		}

		HVERTEXBUFFER			vb;
		int						vbSize;
		int						vertexSize;
		vector<T>				VertexData;
		static	const int		DefaultSize = 5000;
};

}