#pragma once

namespace rs3 {

struct RSCID
{
	RSCID()
	{
		v[0] = 0;
		v[1] = 0;
	}

	RSCID( DWORD64 hi, DWORD64 low )
	{
		v[0] = hi;
		v[1] = low;
	}

	RSCID( unsigned int order )
	{
		_ASSERT( order < MAX_BITS );

		const DWORD64 ONE = 1;

		if( order < ELEMENT_BIT_SIZE )
		{
			v[0] = 0;
			v[1] = ONE<<order;
		}
		else
		{
			v[0] = ONE<<(order-ELEMENT_BIT_SIZE);
			v[1] = 0;
		}
	}

	RSCID& operator = ( const RSCID& other ) { v[0] = other.v[0]; v[1] = other.v[1]; return *this; }

	RSCID operator | ( const RSCID& other ) const { return RSCID( v[0] | other.v[0], v[1] | other.v[1] ); }
	RSCID& operator |= ( const RSCID& other ) { v[0] |= other.v[0]; v[1] |= other.v[1]; return *this; }

	RSCID operator & ( const RSCID& other ) const { return RSCID( v[0] & other.v[0], v[1] & other.v[1] ); }
	RSCID& operator &= ( const RSCID& other ) { v[0] &= other.v[0]; v[1] &= other.v[1]; return *this; }

	RSCID operator ~ () const { return RSCID( ~v[0], ~v[1] ); }

	bool operator == ( const RSCID& other ) const { return( v[0] == other.v[0] && v[1] == other.v[1] ); }
	bool operator != ( const RSCID& other ) const { return( v[0] != other.v[0] || v[1] != other.v[1] ); }

	bool operator <= ( const RSCID& other ) const
	{
		if( *this == other ) return true;
		if( *this < other ) return true;

		return false;
	}

	bool operator <  ( const RSCID& other ) const
	{
		if( v[0] < other.v[0] ) return true;
		if( v[0] > other.v[0] ) return false;

		if( v[1] < other.v[1] ) return true;
		
		return false;
	}

	bool operator >= ( const RSCID& other ) const
	{
		if( *this == other ) return true;
		if( *this > other ) return true;

		return false;
	}

	bool operator >  ( const RSCID& other ) const
	{
		if( v[0] > other.v[0] ) return true;
		if( v[0] < other.v[0] ) return false;

		if( v[1] > other.v[1] ) return true;

		return false;
	}

	inline bool IsNonZero() { return( v[0] || v[1] ); }

	inline bool	HasComponent(const RSCID& lhs)	{	return operator&(lhs).IsNonZero(); }

	inline bool	HasAllComponent(const RSCID& lhs)
	{
		return operator==(operator|(lhs));
	}

	std::string ToString( int radix )
	{
		char buff[MAX_PATH];

		_ui64toa_s( v[0], buff, MAX_PATH, radix );
		std::string str(buff);

		_ui64toa_s( v[1], buff, MAX_PATH, radix );
		str += buff;

		return str;
	}

	static const int ELEMENT_BIT_SIZE = 64; // DWORD64
	static const int ELEMENTS_NUM = 2;
	static const int MAX_BITS = ELEMENT_BIT_SIZE * ELEMENTS_NUM;
	DWORD64 v[ELEMENTS_NUM]; // 128∫Ò∆Æ
};

};