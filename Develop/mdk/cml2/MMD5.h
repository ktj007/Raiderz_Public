// MMD5.h
#ifndef _MMD5_H_
#define _MMD5_H_

#include "RLib.h"

typedef struct md5
{
	unsigned long total[2];     /*!< number of bytes processed  */
	unsigned long state[4];     /*!< intermediate digest state  */
	unsigned char buffer[64];   /*!< data block being processed */
} md5_context;

class CML2_API MMD5
{
public:
	MMD5() { }
	virtual ~MMD5() { }

	static void md5_string(unsigned char* szInput, int nInputLen, unsigned char szOutput[16]);
	static bool md5_file(const char* szFilePath, unsigned char szOutput[16]);

private:
	static void md5_starts(md5_context *ctx);
	static void md5_update(md5_context *ctx, unsigned char* szInput, int nInputLen);
	static void md5_finish(md5_context *ctx, unsigned char szOutput[16]);
};

#endif