#ifdef NWZ_CRYPTO_LIB
#else
#define NWZ_CRYPTO_LIB extern "C" __declspec(dllimport)
#endif

//BLOWFISH=0,DES=1

// 유니코드를 사용하는 경우
// encryptedBuffer = NULL , encryptedBufferSize = 0 을 입력하면 필요한 encryptedBuffer 크기를 리턴
NWZ_CRYPTO_LIB int encryptPwdUC(const TCHAR * plain, const int plainSize, TCHAR * encryptedBuffer,int encryptedBufferSize, int alg=0);
// plainBuffer = NULL , plainBufferSize = 0 을 입력하면 필요한 plainBuffer 크기를 리턴
NWZ_CRYPTO_LIB int decryptPwdUC(const TCHAR * encrypted, const int encryptedSize, TCHAR * plainBuffer, int plainBufferSize, int alg=0);

// ANSI char를 사용하는 경우
// encrypted = NULL , encryptedSize = 0 을 입력하면 필요한 encrypted 크기를 리턴
NWZ_CRYPTO_LIB int encryptPwd(const char * plain, const int plainSize, char * encrypted, int encryptedSize, int alg=0);
// plain = NULL , plainSize = 0 을 입력하면 필요한 plain 크기를 리턴
NWZ_CRYPTO_LIB int decryptPwd(const char * encrypted, const int encryptedSize, char * plain, int plainSize, int alg=0);