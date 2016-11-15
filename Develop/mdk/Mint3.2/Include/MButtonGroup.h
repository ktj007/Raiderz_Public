#pragma once

namespace mint3 {

class MButton;

// 버튼 그룹을 구현하는데 필요한 클래스
class MButtonGroup {
	friend MButton;
public:
	MButtonGroup();
	~MButtonGroup();

protected:
	int			m_nCount;
	MButton*	m_pPrevious;
};

class MButtonGroupManager : map<string, MButtonGroup*>
{
private:
public:
	MButtonGroupManager()
	{

	}
	~MButtonGroupManager()
	{
		while(empty()==false)
		{
			MButtonGroup* pButtonGroup = (*begin()).second;
			delete pButtonGroup;
			erase(begin());
		}
	}
	void Add(string strName, MButtonGroup* pButtonGroup)
	{
		insert(value_type(strName, pButtonGroup));
	}
	MButtonGroup* Get(string& strName)
	{
		iterator itor = find(strName);
		if (itor != end())
		{
			return (*itor).second;
		}
		return NULL;
	}
};

} //namespace mint3