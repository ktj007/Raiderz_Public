#pragma once

namespace rs3 
{
	
	class RHeapAlloc
	{
	public:
		RHeapAlloc(void);
		~RHeapAlloc(void);

		static void*	allocate(size_t size);
		static void		deallocate(void* p);

	public:
		static void		InitHeap(void);
		static void		ReleaseHeap(void);
	private:
		static HANDLE               s_heap;
	};

		
}