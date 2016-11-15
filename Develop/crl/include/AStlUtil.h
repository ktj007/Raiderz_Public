#pragma once

#include <deque>

struct DeleteObject
{
	template<typename T>
	void operator() (const T* ptr) const
	{
		SAFE_DELETE(ptr);
	}
};

typedef struct DeleteObject DO;

struct DeletePairSecondObject
{
	template<typename T1, typename T2>
	void operator() (std::pair<T1, T2>& pr) const
	{
		SAFE_DELETE(pr.second);
	}
};

typedef struct DeletePairSecondObject DPSO;

struct DestroyAndDeletePairSecondObject
{
	template<typename T1, typename T2>
	void operator() (std::pair<T1, T2>& pr) const
	{
		pr.second->Destroy();
		SAFE_DELETE(pr.second);
	}
};

template<typename T>
vector<typename T> SetToVector(const set<typename T>& setVal)
{
	vector<typename T> vecVal;
	
	vecVal.insert(vecVal.end(), setVal.begin(), setVal.end());

	return vecVal;
};

template<typename T>
set<typename T> VectorToSet(const vector<typename T>& vecVal)
{
	set<typename T> setVal;

	for each (const T& val in vecVal)
	{
		setVal.insert(val);
	}

	return setVal;
};

template<typename T>
bool IsExist(const vector<typename T>& vecVal, const T& keyVal)
{
	for each (const T& val in vecVal)
	{
		if (keyVal == val)
		{
			return true;
		}
	}

	return false;
};

template<typename T>
bool IsNotExist(const vector<typename T>& vecVal, const T& keyVal)
{
	if (true == IsExist(vecVal, keyVal)) return false;

	return true;
};

template<typename T>
bool IsExist(const set<typename T>& setVal, const T& keyVal)
{
	if (setVal.end() == setVal.find(keyVal)) return false;

	return true;
};

template<typename T>
bool IsNotExist(const set<typename T>& setVal, const T& keyVal)
{
	if (true == IsExist(setVal, keyVal)) return false;

	return true;
};


template<typename MapType>
void MapSecondVectorInserter(MapType& rmap, typename MapType::key_type& key, typename const MapType::mapped_type::value_type& element)
{
	MapType::iterator it = rmap.find(key);
	if (it == rmap.end())
	{
		rmap.insert(make_pair(key, vector<MapType::mapped_type::value_type>()));
		it = rmap.find(key);
	}

	it->second.push_back(element);
}

template<typename MapType>
void MapSecondSetInserter(MapType& rmap, typename MapType::key_type& key, typename const MapType::mapped_type::value_type& element)
{
	MapType::iterator it = rmap.find(key);
	if (it == rmap.end())
	{
		rmap.insert(make_pair(key, set<MapType::mapped_type::value_type>()));
		it = rmap.find(key);
	}

	it->second.insert(element);
}

#define SAFE_DELETE_VECTOR(container)	{ for_each((container).begin(), (container).end(), DO()); (container).clear();	}
#define SAFE_DELETE_MAP(container)	{ for_each((container).begin(), (container).end(), DPSO()); (container).clear();	}
#define SAFE_DELETE_MAP_OP(container, option)	{ for_each((container).begin(), (container).end(), option()); (container).clear();	}

static deque<string> Tokenize(const string& str,const string& delimiters)
{
	deque<string> tokens;
	string::size_type delimPos = 0, tokenPos = 0, pos = 0;

	if(str.length()<1)  return tokens;
	while(1){
		delimPos = str.find_first_of(delimiters, pos);
		tokenPos = str.find_first_not_of(delimiters, pos);

		if(string::npos != delimPos){
			if(string::npos != tokenPos){
				if(tokenPos<delimPos){
					tokens.push_back(str.substr(pos,delimPos-pos));
				}else{
					tokens.push_back("");
				}
			}else{
				tokens.push_back("");
			}
			pos = delimPos+1;
		} else {
			if(string::npos != tokenPos){
				tokens.push_back(str.substr(pos));
			} else {
				tokens.push_back("");
			}
			break;
		}
	}
	return tokens;
}