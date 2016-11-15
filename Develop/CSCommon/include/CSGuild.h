#ifndef _CS_GUILD_H_
#define _CS_GUILD_H_

enum GUILD_MEMBER_GRADE : unsigned char
{
	GMG_NONE = 0,
	GMG_MASTER = 1,
	GMG_NORMAL = 100,

	GMG_MAX
};

#define MAX_GUILD_MEMBER_SIZE 50

#endif