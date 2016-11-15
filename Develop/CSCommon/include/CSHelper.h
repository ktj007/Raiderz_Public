#pragma once

#include "MUID.h"
#include "CSCommonLib.h"

template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

#define STRING_MATCH(TYPE, VAR, STRING) \
	for (int i = 0; i < arraysize(TYPE ## _STR); i++) \
	{ \
		if (!strcmp(TYPE ## _STR[i], STRING)) \
		{ \
			VAR = (TYPE)i; \
			break; \
		} \
	}