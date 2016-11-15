#pragma once

template<typename T>
class GObjectPool
{
public:
	GObjectPool();
	~GObjectPool();

	static GObjectPool<T>& Instance();

	void Init(size_t nMaxSize);
	void Fini();
	T* New();
	void Drop(T* pItem);

	size_t GetUnusedObjectCount() const;
	size_t GetPoolSize() const;
private:
	vector<T*>	m_vecUnuseds;
	size_t		m_nPoolSize;
};


//////////////////////////////////////////////////////////////////////////
//
// Implements
//
//////////////////////////////////////////////////////////////////////////


template<typename T>
GObjectPool<T>::GObjectPool()
: m_nPoolSize(0)
{
}

template<typename T>
GObjectPool<T>::~GObjectPool()
{
	Fini();
}

template<typename T>
void GObjectPool<T>::Init( size_t nMaxSize )
{
	m_vecUnuseds.resize(nMaxSize);
	for (size_t i=0; i<nMaxSize; ++i)
	{
		m_vecUnuseds[i] = new T();
	}
	m_nPoolSize = nMaxSize;
}

template<typename T>
void GObjectPool<T>::Fini()
{
	for each (T* each in m_vecUnuseds)
		SAFE_DELETE(each);
	m_vecUnuseds.clear();
	m_nPoolSize = 0;
}

template<typename T>
T* GObjectPool<T>::New()
{
	T* pItem = NULL;
	if (m_vecUnuseds.empty())
	{
		pItem = new T;
		++m_nPoolSize;
	}
	else
	{
		pItem = m_vecUnuseds.back();
		m_vecUnuseds.pop_back();
	}

	DCHECK(pItem);
	pItem->Create();

	return pItem;
}

template<typename T>
void GObjectPool<T>::Drop( T* pItem )
{
	DCHECK(pItem);
	pItem->Destroy();
	m_vecUnuseds.push_back(pItem);
}

template<typename T>
GObjectPool<T>& GObjectPool<T>::Instance()
{
	static GObjectPool<T> inst;
	return inst;
}

template<typename T>
size_t GObjectPool<T>::GetUnusedObjectCount() const
{
	return m_vecUnuseds.size();
}


template<typename T>
GObjectPool<T>& GetPool() 
{
	return GObjectPool<T>::Instance();
}

template<typename T>
size_t GObjectPool<T>::GetPoolSize() const
{
	return m_nPoolSize;
}
