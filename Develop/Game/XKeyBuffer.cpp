#include "stdafx.h"
#include "XKeyBuffer.h"

XKeyBuffer::XKeyBuffer() : m_nBegin(0), m_nEnd(0)
{
	m_Buffer.resize(MAX_KEY_BUFFER+1);
}

XKeyBuffer::~XKeyBuffer()
{

}

void XKeyBuffer::Push(KEYNODE& key_node)
{
	m_Buffer[m_nEnd] = key_node;

	m_nEnd++;
	if (m_nEnd >= MAX_KEY_BUFFER) m_nEnd = 0;
	if (m_nBegin == m_nEnd)
	{
		m_nBegin++;
		if (m_nBegin >= MAX_KEY_BUFFER) m_nBegin = 0;
	}
}


void XKeyBuffer::Push2(KEYNODE& key_node)
{
	m_Buffer[m_nEnd] = key_node;

	m_nEnd++;
	if (m_nEnd >= MAX_KEY_BUFFER) m_nEnd = 0;
	if (m_nBegin == m_nEnd)
	{
		m_nBegin++;
		if (m_nBegin >= MAX_KEY_BUFFER) m_nBegin = 0;
	}

	int nEndPrev = m_nEnd - 1;
	if (nEndPrev < 0) nEndPrev += MAX_KEY_BUFFER;

	swap(m_Buffer[m_nEnd], m_Buffer[nEndPrev]);
}

void XKeyBuffer::Pop()
{
	if (IsEmpty()) return;

	m_nBegin++;
	if (m_nBegin >= MAX_KEY_BUFFER) m_nBegin = 0;
}

const KEYNODE& XKeyBuffer::Get(int index) const
{
	int real_index = m_nBegin + index;
	if (real_index >= MAX_KEY_BUFFER)
	{
		real_index = real_index - MAX_KEY_BUFFER;
	}

	return m_Buffer[real_index];
}

int XKeyBuffer::GetSize()
{
	if (m_nEnd >= m_nBegin) return m_nEnd - m_nBegin;
	return (MAX_KEY_BUFFER - m_nBegin + m_nEnd);
}

bool XKeyBuffer::IsEmpty()
{
	return (m_nEnd == m_nBegin);
}

void XKeyBuffer::AddFilter(XVirtualKey nKey)
{
	m_Filter.insert(nKey);
}

bool XKeyBuffer::CheckFilter(XVirtualKey nKey)
{
	if (m_Filter.find(nKey) == m_Filter.end()) return false;
	return true;
}

void XKeyBuffer::Clear()
{
	m_nBegin = 0;
	m_nEnd = 0;
}

void XKeyBuffer::GetDebugString( wchar_t* pszOut, int nOutLen )
{
	wcscpy_s(pszOut, nOutLen, L"");

	int size = GetSize();
	for (int i =0; i < size; i++)
	{
		const KEYNODE& node = Get(i);
		wchar_t szNode[256];
		wchar_t szState[16];
		switch (node.nState)
		{
		case KEYNODE::PRESSONCE: swprintf_s(szState, L"[ONCE]"); break;
		case KEYNODE::PRESSED: swprintf_s(szState, L"[PRESS]"); break;
		case KEYNODE::RELEASED: swprintf_s(szState, L"[RELEASE]"); break;
		}

		swprintf_s(szNode, L"[%d:%s] ", node.nKeycode, szState);
		wcscat_s(pszOut, nOutLen, szNode);
	}
}

void XKeyBuffer::Remove( XVirtualKey nKey )
{
	vector<KEYNODE> vecNodes;

	int size = GetSize();
	bool exist_key = false;

	for (int i = 0; i < size; i++)
	{
		KEYNODE node = Get(i);
		if (nKey == node.nKeycode)
			exist_key = true;
		else
			vecNodes.push_back(node);
	}

	if (exist_key == false) return;

	Clear();
	for (size_t i = 0; i < vecNodes.size(); i++)
	{
		Push(vecNodes[i]);
	}
}

bool XKeyBuffer::IsExistPressedKeyNode( XVirtualKey nVirtualKey )
{
	if (IsEmpty()) return false;

	int size = GetSize();

	for (int i = size-1; i >= 0; i--)
	{
		const KEYNODE& node = Get(i);

		if ((node.nKeycode == nVirtualKey) && (node.Pressed())) return true;
	}
	return false;
}