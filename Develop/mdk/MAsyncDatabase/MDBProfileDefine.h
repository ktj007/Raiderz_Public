#pragma once


namespace mdb
{
#define MDBR_DWORD_ADD_COUNT(x) if (0xfffffff0 < (++x)) { x = 0; }
}