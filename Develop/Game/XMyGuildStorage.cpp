#include "StdAfx.h"
#include "XMyGuildStorage.h"

XMyGuildStorage::XMyGuildStorage(void)
: XItemContainer(MAX_GUILDSTORAGE_SIZE)
, m_bExpended(false)
{
}

XMyGuildStorage::~XMyGuildStorage(void)
{
}
