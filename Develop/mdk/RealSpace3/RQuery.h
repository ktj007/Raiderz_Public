#pragma once

#include "RTypes.h"

namespace rs3 
{
	class RQuery
	{
	public:
		virtual	void	BeginIssue(void) = 0;
		virtual	void	EndIssue(void) = 0;
		virtual	bool	Data(int* pData) = 0;

		virtual	bool	Create(void* pDevice) = 0;
		virtual	void	Release(void) = 0;
	};

	class ROcclusionQuery : public RQuery
	{
	public:
		virtual	void	BeginIssue(void) = 0;
		virtual	void	EndIssue(void) = 0;
		virtual	bool	Data(int* pData) = 0;

		virtual	bool	Create(void* pDevice) = 0;
		virtual	void	Release(void) = 0;
	};

	class REventQuery : public RQuery
	{
	public:
		virtual	void	BeginIssue(void) = 0;
		virtual	void	EndIssue(void) = 0;
		virtual	bool	Data(int* pData) = 0;

		virtual	bool	Create(void* pDevice) = 0;
		virtual	void	Release(void) = 0;
	};


}