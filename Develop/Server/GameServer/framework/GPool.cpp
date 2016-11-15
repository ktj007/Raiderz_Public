#include "stdafx.h"
#include "GPool.h"

void GPoolNode::Use()
{
	m_bUsed = true;

	OnUse();
}
void GPoolNode::Drop()
{
	OnDrop();

	m_bUsed = false;
}

