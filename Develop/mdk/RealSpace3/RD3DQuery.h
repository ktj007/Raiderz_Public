#pragma once

#include "RTypes.h"
#include "RQuery.h"
#include <set>

namespace rs3 
{
	class RDeviceD3D;

	class RS_API RD3DOcclusionQuery  : public ROcclusionQuery
	{
	public:
		RD3DOcclusionQuery(void);
		virtual	~RD3DOcclusionQuery(void);

	public:
		virtual	void	BeginIssue(void);
		virtual	void	EndIssue(void);
		virtual	bool	Data(int* pData);

		virtual	bool	Create(void* pDevice);
		virtual	void	Release(void);

	protected:
		LPDIRECT3DQUERY9			m_pD3DQuery;
	};

	class RS_API RD3DEventQuery  : public REventQuery
	{
	public:
		RD3DEventQuery(void);
		virtual	~RD3DEventQuery(void);

	public:
		virtual	void	BeginIssue(void)	{}
		virtual	void	EndIssue(void);
		virtual	bool	Data(int* pData)	{	EmptyCommandBuffer(); return true;}

		virtual	bool	Create(void* pDevice);
		virtual	void	Release(void);

		void	EmptyCommandBuffer();

		

	protected:
		LPDIRECT3DQUERY9			m_pD3DQuery;
	};

	enum QUERY_TYPE
	{
		QUERY_OCCLUSION = 0,
		QUERY_EVENT,
		QUERY_END,
	};

	class RS_API RD3DQueryManager
	{
		friend RDeviceD3D;
	public:
		RD3DQueryManager(void);
		virtual ~RD3DQueryManager(void);

	public:
		RQuery*		CreateQuery(QUERY_TYPE query_type);
		void		ReleaseQuery(RQuery* pQuery);

		void		Release(void);

		void		LostDevice(void);
		void		RestoreDevice(void);

	protected:
		static LPDIRECT3DDEVICE9		m_pD3DDevice;
		
		typedef std::set<RQuery*> QUERY_SET;
		QUERY_SET		m_setQuery;
	};
}