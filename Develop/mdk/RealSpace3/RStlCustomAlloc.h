/*
http://www.josuttis.com/cppcode/myalloc.hpp.html
The following code example is taken from the book
The C++ Standard Library - A Tutorial and Reference
by Nicolai M. Josuttis, Addison-Wesley, 1999
Copyright Nicolai M. Josuttis 1999
*/

//windows.h에 정의된 max매크로와 numeric_limits::max()함수가 충돌나서 잠시 max매크로 정의 끔
#undef max	

#include <limits>
#include "RMemoryAlloc.h"

namespace rs3 
{
	template <class T>
	class RStlCustomAlloc
	{
	public:
		// type definitions
		typedef T        value_type;
		typedef T*       pointer;
		typedef const T* const_pointer;
		typedef T&       reference;
		typedef const T& const_reference;
		typedef std::size_t    size_type;
		typedef std::ptrdiff_t difference_type;

		// rebind allocator to type U
		template <class U>
		struct rebind 
		{
			typedef RStlCustomAlloc<U> other;
		};

		// return address of values
		pointer address (reference value) const {	return &value;	}
		const_pointer address (const_reference value) const {	return &value;	}

		/* constructors and destructor
		* - nothing to do because the allocator has no state
		*/
		RStlCustomAlloc() throw() {}
		RStlCustomAlloc(const RStlCustomAlloc&) throw() {}
		template <class U>	RStlCustomAlloc (const RStlCustomAlloc<U>&) throw() {}
		~RStlCustomAlloc() throw() {}

		// return maximum number of elements that can be allocated
		inline size_type max_size () const throw() 
		{
			//return std::numeric_limits<std::size_t>::max() / sizeof(T);
			return std::numeric_limits<std::size_t>::max() / sizeof(T);
		}

		// initialize elements of allocated storage p with value value
		void construct (pointer p, const T& value) 
		{
			// initialize memory with placement new
			new((void*)p)T(value);
		}
		
		// destroy elements of initialized storage p
		void destroy (pointer p) 
		{
			// destroy objects by calling their destructor
			p->~T();
		}

		// allocate but don't initialize num elements of type T
		pointer allocate (size_type num, const void* = 0) 
		{
			//pointer ret = (pointer)(operator new(num*sizeof(T)));
			pointer ret = (pointer)RHeapAlloc::allocate(num*sizeof(T));
			return ret;
		}

		// deallocate storage p of deleted elements
		void deallocate (pointer p, size_type num) 
		{
			//operator delete((void*)p);
			RHeapAlloc::deallocate(p);
		}
	};

	// return that all specializations of this allocator are interchangeable
	template <class T1, class T2>
	bool operator== (const RStlCustomAlloc<T1>&,
		const RStlCustomAlloc<T2>&) throw() {
			return true;
	}
	template <class T1, class T2>
	bool operator!= (const RStlCustomAlloc<T1>&,
		const RStlCustomAlloc<T2>&) throw() {
			return false;
	}
}

//windows.h에 정의된 max매크로와 numeric_limits::max()함수가 충돌나서 잠시 max매크로 정의 끈걸, 다시 복귀시킴
#define max(a,b)    (((a) > (b)) ? (a) : (b))


