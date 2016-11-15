#pragma once

#include <string>
using namespace std;



class XCheckSumEncrypt
{
protected:


public:
	XCheckSumEncrypt();
	virtual ~XCheckSumEncrypt();


	bool Encrypt( const char* _in_data, char* _out_data, size_t _size);
	bool Decrypt( const char* _in_data, char* _out_data, size_t _size);
};
