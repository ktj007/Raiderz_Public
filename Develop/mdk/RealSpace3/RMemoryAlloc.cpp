#include "stdafx.h"
#include "RMemoryAlloc.h"

namespace rs3
{
	HANDLE RHeapAlloc::s_heap = NULL;

	RHeapAlloc::RHeapAlloc( void )
	{
	}

	RHeapAlloc::~RHeapAlloc( void )
	{
	}

	void* RHeapAlloc::allocate(size_t size)
	{
		return HeapAlloc(s_heap, 0, size);
	}

	void RHeapAlloc::deallocate( void* p )
	{
		 HeapFree(s_heap, 0, p);
	}

#define HEAP_LFH
	void RHeapAlloc::InitHeap( void )
	{
		s_heap = HeapCreate(0, 0, 0);
	#ifdef HEAP_LFH
		ULONG heapFragValue = 2;
		HeapSetInformation(
			s_heap,
			HeapCompatibilityInformation,
			&heapFragValue,
			sizeof(heapFragValue)
			);
	#endif
	}

	void RHeapAlloc::ReleaseHeap( void )
	{
		HeapDestroy(s_heap);
		s_heap = NULL;
	}
}