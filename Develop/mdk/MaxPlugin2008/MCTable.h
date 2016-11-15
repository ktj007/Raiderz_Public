#pragma once

#include <map>
#include "RTypes.h"

/////////////////////////////////////////////////////
// MCUniqueTable : 중복이 제거되는 벡터 템플릿
//
template < class _TYPE, class _COMPARE = less<_TYPE> >
class MCUniqueTable : public vector<_TYPE>
{
	map<_TYPE,int,_COMPARE>	m_setVectors;

public:
	using vector<_TYPE>::size;
	using vector<_TYPE>::empty;

	const_reference operator[](size_t index) const { return this->at(index); }

	MCUniqueTable() {}
	~MCUniqueTable() {}

	int IsExist(const _TYPE& v)	{

		map<_TYPE,int,_COMPARE>::iterator itr = m_setVectors.find(v);
		if(itr!=m_setVectors.end())
			return itr->second;
		return -1;
	}

	int Add(const _TYPE& v) {

		// 같은것이 없다면..
		int nPos = IsExist(v);
		if(  nPos != -1 ) // 이미있다면..
			return nPos;

		push_back(v);
		m_setVectors.insert( map<_TYPE,int,_COMPARE>::value_type(v,size()-1) );

		return size()-1;
	}

	int Add(const _TYPE& v,bool& bExist) {

		// 같은것이 없다면..
		int nPos = IsExist(v);

		bExist = false;
		if(  nPos != -1 ) { // 이미있다면..
			bExist = true;
			return nPos;
		}

		push_back(v);
		m_setVectors.insert( map<_TYPE,int,_COMPARE>::value_type(v,size()-1) );

		return size()-1;
	}

};


/////////////////////////////////////////////////



typedef MCUniqueTable<int> MCIntTable;

struct MCVectorTableLess
{
	bool operator() (const RVector & a, const RVector & b) const
	{
		if(a.x!=b.x) return (a.x<b.x);
		if(a.y!=b.y) return (a.y<b.y);
		return (a.z<b.z);
	}
};
typedef MCUniqueTable<RVector,MCVectorTableLess> MCVectorTable;

struct MCVector4TableLess
{
	bool operator() (const RVector4 & a, const RVector4 & b) const
	{
		if(a.x!=b.x) return (a.x<b.x);
		if(a.y!=b.y) return (a.y<b.y);
		if(a.z!=b.z) return (a.z<b.z);
		return (a.w<b.w);
	}
};
typedef MCUniqueTable<RVector4,MCVector4TableLess> MCVector4Table;

struct MCVertexIndexTableLess
{
	// p,n,uv 까지를 키로 쓴다
	bool operator() (const RVertexIndex & a, const RVertexIndex & b) const
	{
		if(a.p!=b.p) return (a.p<b.p);
		if(a.n!=b.n) return (a.n<b.n);
		//if(a.n_tan!=b.n_tan) return (a.n_tan<b.n_tan);
		if(a.uv2 != b.uv2) return (a.uv2<b.uv2);
		return (a.uv<b.uv);
	}
};
typedef MCUniqueTable<RVertexIndex,MCVertexIndexTableLess> MCVertexIndexTable;
