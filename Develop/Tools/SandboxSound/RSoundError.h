#ifndef _RSOUNDERROR
#define _RSOUNDERROR


bool _ERROR( int _err);

void _ERRORMSG( int _err, const char* _str =NULL);

void _MESSAGE( const char* szFormat, ...);

#endif